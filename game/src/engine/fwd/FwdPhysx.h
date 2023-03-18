#pragma once

namespace physx
{

class PxRigidDynamic;
class PxRigidStatic;
class PxShape;

template <class T>
class PxVec3T;
typedef PxVec3T<float> PxVec3;
typedef PxVec3T<double> PxVec3d;

template<class T>	class PxTransformT;
typedef PxTransformT<float>	PxTransform;

}  // namespace physx