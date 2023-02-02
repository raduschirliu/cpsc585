// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (c) 2008-2022 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#pragma once

#include "vehicle2/PxVehicleAPI.h"
#include "../physxintegration/PhysXIntegration.h"

namespace snippetvehicle2
{

using namespace physx;
using namespace physx::vehicle2;

struct DirectDrivetrainParams
{
	PxVehicleDirectDriveThrottleCommandResponseParams directDriveThrottleResponseParams;

	DirectDrivetrainParams transformAndScale(
		const PxVehicleFrame& srcFrame, const PxVehicleFrame& trgFrame, const PxVehicleScale& srcScale, const PxVehicleScale& trgScale) const;

	PX_FORCE_INLINE bool isValid(const PxVehicleAxleDescription& axleDesc) const
	{
		if (!directDriveThrottleResponseParams.isValid(axleDesc))
			return false;

		return true;
	}
};

struct DirectDrivetrainState
{
	PxReal directDriveThrottleResponseStates[PxVehicleLimits::eMAX_NB_WHEELS];

	PX_FORCE_INLINE void setToDefault()
	{
		PxMemZero(this, sizeof(DirectDrivetrainState));
	}
};


//
//This class holds the parameters, state and logic needed to implement a vehicle that
//is using a direct drivetrain.
//
//See BaseVehicle for more details on the snippet code design.
//
class DirectDriveVehicle
	: public PhysXActorVehicle
	, public PxVehicleDirectDriveCommandResponseComponent
	, public PxVehicleDirectDriveActuationStateComponent
	, public PxVehicleDirectDrivetrainComponent
{
public:
    bool initialize(PxPhysics& physics, const PxCookingParams& params, PxMaterial& defaultMaterial, bool addPhysXBeginEndComponents = true)
    {
        mTransmissionCommandState.setToDefault();

        if (!PhysXActorVehicle::initialize(physics, params, defaultMaterial))
            return false;

        if (!mDirectDriveParams.isValid(mBaseParams.axleDescription))
            return false;

        //Set the drivetrain state to default.
        mDirectDriveState.setToDefault();

        //Add all the components in sequence that will simulate a vehicle with a direct drive drivetrain.
        initComponentSequence(addPhysXBeginEndComponents);

        return true;

    }
	virtual void destroy();

    virtual void initComponentSequence(bool addPhysXBeginEndComponents)
    {
        //Wake up the associated PxRigidBody if it is asleep and the vehicle commands signal an
    //intent to change state.
    //Read from the physx actor and write the state (position, velocity etc) to the vehicle.
        if (addPhysXBeginEndComponents)
            mComponentSequence.add(static_cast<PxVehiclePhysXActorBeginComponent*>(this));

        //Read the input commands (throttle, brake etc) and forward them as torques and angles to the wheels on each axle.
        mComponentSequence.add(static_cast<PxVehicleDirectDriveCommandResponseComponent*>(this));

        //Work out which wheels have a non-zero drive torque and non-zero brake torque.
        //This is used to determine if any tire is to enter the "sticky" regime that will bring the 
        //vehicle to rest.
        mComponentSequence.add(static_cast<PxVehicleDirectDriveActuationStateComponent*>(this));

        //Perform a scene query against the physx scene to determine the plane and friction under each wheel.
        mComponentSequence.add(static_cast<PxVehiclePhysXRoadGeometrySceneQueryComponent*>(this));

        //Start a substep group that can be ticked multiple times per update.
        //In this example, we update the suspensions, tires and wheels 3 times without recalculating 
        //the plane underneath the wheel.  This is useful for stability at low forward speeds and is
        //computationally cheaper than simulating the whole pipeline at a smaller timestep.
        mComponentSequenceSubstepGroupHandle = mComponentSequence.beginSubstepGroup(3);

        //Update the suspension compression given the plane under each wheel.
        //Update the kinematic compliance from the compression state of each suspension.
        //Convert suspension state to suspension force and torque.
        mComponentSequence.add(static_cast<PxVehicleSuspensionComponent*>(this));

        //Compute the load on the tire, the friction experienced by the tire 
        //and the lateral/longitudinal slip angles.
        //Convert load/friction/slip to tire force and torque.
        //If the vehicle is to come rest then compute the "sticky" velocity constraints to apply to the
        //vehicle.
        mComponentSequence.add(static_cast<PxVehicleTireComponent*>(this));

        //Apply any velocity constraints to a data buffer that will be consumed by the physx scene
        //during the next physx scene update.
        mComponentSequence.add(static_cast<PxVehiclePhysXConstraintComponent*>(this));

        //Apply the tire force, brake force and drive force to each wheel and
        //forward integrate the rotation speed of each wheel.
        mComponentSequence.add(static_cast<PxVehicleDirectDrivetrainComponent*>(this));

        //Apply the suspension and tire forces to the vehicle's rigid body and forward 
        //integrate the state of the rigid body.
        mComponentSequence.add(static_cast<PxVehicleRigidBodyComponent*>(this));

        //Mark the end of the substep group.
        mComponentSequence.endSubstepGroup();

        //Update the rotation angle of the wheel by forwarding integrating the rotational
        //speed of each wheel.
        //Compute the local pose of the wheel in the rigid body frame after accounting 
        //suspension compression and compliance.
        mComponentSequence.add(static_cast<PxVehicleWheelComponent*>(this));

        //Write the local poses of each wheel to the corresponding shapes on the physx actor.
        //Write the momentum change applied to the vehicle's rigid body to the physx actor.
        //The physx scene can now try to apply that change to the physx actor.
        //The physx scene will account for collisions and constraints to be applied to the vehicle 
        //that occur by applying the change.
        if (addPhysXBeginEndComponents)
            mComponentSequence.add(static_cast<PxVehiclePhysXActorEndComponent*>(this));
    }

	void getDataForDirectDriveCommandResponseComponent(
		const PxVehicleAxleDescription*& axleDescription,
		PxVehicleSizedArrayData<const PxVehicleBrakeCommandResponseParams>& brakeResponseParams,
		const PxVehicleDirectDriveThrottleCommandResponseParams*& throttleResponseParams,
		const PxVehicleSteerCommandResponseParams*& steerResponseParams,
		PxVehicleSizedArrayData<const PxVehicleAckermannParams>& ackermannParams,
		const PxVehicleCommandState*& commands, const PxVehicleDirectDriveTransmissionCommandState*& transmissionCommands,
		const PxVehicleRigidBodyState*& rigidBodyState,
		PxVehicleArrayData<PxReal>& brakeResponseStates, PxVehicleArrayData<PxReal>& throttleResponseStates, 
		PxVehicleArrayData<PxReal>& steerResponseStates)
	{
		axleDescription = &mBaseParams.axleDescription;
		brakeResponseParams.setDataAndCount(mBaseParams.brakeResponseParams, sizeof(mBaseParams.brakeResponseParams) / sizeof(PxVehicleBrakeCommandResponseParams));
		throttleResponseParams = &mDirectDriveParams.directDriveThrottleResponseParams;
		steerResponseParams = &mBaseParams.steerResponseParams;
		ackermannParams.setDataAndCount(mBaseParams.ackermannParams, sizeof(mBaseParams.ackermannParams)/sizeof(PxVehicleAckermannParams));
		commands = &mCommandState;
		transmissionCommands = &mTransmissionCommandState;
		rigidBodyState = &mBaseState.rigidBodyState;
		brakeResponseStates.setData(mBaseState.brakeCommandResponseStates);
		throttleResponseStates.setData(mDirectDriveState.directDriveThrottleResponseStates);
		steerResponseStates.setData(mBaseState.steerCommandResponseStates);
	}

	virtual void getDataForDirectDriveActuationStateComponent(
		const PxVehicleAxleDescription*& axleDescription,
		PxVehicleArrayData<const PxReal>& brakeResponseStates,
		PxVehicleArrayData<const PxReal>& throttleResponseStates,
		PxVehicleArrayData<PxVehicleWheelActuationState>& actuationStates)
	{
		axleDescription = &mBaseParams.axleDescription;
		brakeResponseStates.setData(mBaseState.brakeCommandResponseStates);
		throttleResponseStates.setData(mDirectDriveState.directDriveThrottleResponseStates);
		actuationStates.setData(mBaseState.actuationStates);
	}

	virtual void getDataForDirectDrivetrainComponent(
		const PxVehicleAxleDescription*& axleDescription,
		PxVehicleArrayData<const PxReal>& brakeResponseStates,
		PxVehicleArrayData<const PxReal>& throttleResponseStates,
		PxVehicleArrayData<const PxVehicleWheelParams>& wheelParams,
		PxVehicleArrayData<const PxVehicleWheelActuationState>& actuationStates,
		PxVehicleArrayData<const PxVehicleTireForce>& tireForces,
		PxVehicleArrayData<PxVehicleWheelRigidBody1dState>& wheelRigidBody1dStates)
	{
		axleDescription = &mBaseParams.axleDescription;
		brakeResponseStates.setData(mBaseState.brakeCommandResponseStates);
		throttleResponseStates.setData(mDirectDriveState.directDriveThrottleResponseStates);
		wheelParams.setData(mBaseParams.wheelParams);
		actuationStates.setData(mBaseState.actuationStates);
		tireForces.setData(mBaseState.tireForces);
		wheelRigidBody1dStates.setData(mBaseState.wheelRigidBody1dStates);
	}


	//Parameters and states of the vehicle's direct drivetrain.
	DirectDrivetrainParams mDirectDriveParams;
	DirectDrivetrainState mDirectDriveState;

	//The commands that will control the vehicle's transmission
	PxVehicleDirectDriveTransmissionCommandState mTransmissionCommandState;
};

}//namespace snippetvehicle2
