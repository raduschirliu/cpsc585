#include "engine/physics/PhysicsService.h"

#include <glm/glm.hpp>
#include <optional>

#include "RaycastData.h"
#include "VehicleCommands.h"
#include "engine/App.h"
#include "engine/asset/AssetService.h"
#include "engine/core/debug/Log.h"
#include "engine/core/math/Physx.h"
#include "engine/gui/GuiService.h"
#include "engine/input/InputService.h"
#include "engine/render/RenderService.h"
#include "engine/scene/Entity.h"
#include "engine/service/ServiceProvider.h"

using snippetvehicle2::BaseVehicle;
using std::string;
using std::string_view;
using std::vector;
using namespace physx;
using namespace physx::vehicle2;

static constexpr bool kPhysxRecordAllocations = true;
static constexpr uint32_t kPhysxCpuThreads = 2;
static constexpr const char* kPvdHost = "127.0.0.1";
static constexpr int kPvdPort = 5425;
static constexpr uint32_t kPvdTimeoutMillis = 10;
static const Timestep kPhysxTimestep = Timestep::Seconds(1.0f / 60.0f);
static const OnPhysicsUpdateEvent kPhysicsUpdateEventData{.step =
                                                              kPhysxTimestep};

static const PxVec3 kGravity(0.0f, -98.1f, 0.0f);
// Typically speed tolerance should be gravity acceleration * 1 sec
static const PxTolerancesScale kDefaultTolerancesScale(1.0f, 98.1f);
static const PxCookingParams kDefaultPxCookingParams =
    PxCookingParams(kDefaultTolerancesScale);

/* ---------- from Service ---------- */
void PhysicsService::OnInit()
{
    GetEventBus().Subscribe<OnGuiEvent>(this);

    prev_time_ = 0.0;
    tick_rate_ = 0;
    tick_count_ = 0;

    time_accumulator_.SetSeconds(0.0f);
    InitPhysX();
}

void PhysicsService::OnStart(ServiceProvider& service_provider)
{
    asset_service_ = &service_provider.GetService<AssetService>();
    input_service_ = &service_provider.GetService<InputService>();
    render_service_ = &service_provider.GetService<RenderService>();
}

void PhysicsService::OnSceneLoaded(Scene& scene)
{
    if (kScene_)
    {
        PX_RELEASE(kScene_);
    }

    time_accumulator_.SetSeconds(0.0f);

    PxSceneDesc scene_desc(kPhysics_->getTolerancesScale());
    scene_desc.gravity = kGravity;
    scene_desc.cpuDispatcher = kDispatcher_;
    scene_desc.filterShader = physx::PxDefaultSimulationFilterShader;

    kScene_ = kPhysics_->createScene(scene_desc);
    kScene_->setSimulationEventCallback(this);

    PxPvdSceneClient* pvd_client = kScene_->getScenePvdClient();
    if (pvd_client)
    {
        pvd_client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS,
                                    true);
        pvd_client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvd_client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES,
                                    true);
    }

    debug_draw_scene_ = false;
    kScene_->setVisualizationParameter(PxVisualizationParameter::eSCALE, 0.0f);
    kScene_->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES,
                                       1.0f);
    kScene_->setVisualizationParameter(
        PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

    vehicle_context_.gravity = kGravity;
    vehicle_context_.physxScene = kScene_;
}

void PhysicsService::OnUpdate()
{
    const Timestep& delta = GetApp().GetDeltaTime();
    time_accumulator_ += delta;

    StepPhysics();

    if (input_service_->IsKeyPressed(GLFW_KEY_F3))
    {
        show_debug_menu_ = !show_debug_menu_;
    }

    if (debug_draw_scene_)
    {
        const auto& render_buffer = kScene_->getRenderBuffer();
        const PxDebugLine* lines = render_buffer.getLines();
        const size_t num_lines = render_buffer.getNbLines();

        DebugDrawList& draw_list = render_service_->GetDebugDrawList();

        for (size_t i = 0; i < num_lines; i++)
        {
            const PxDebugLine& line = lines[i];

            const LineVertex start(PxToGlm(line.pos0),
                                   PxColorToVec(line.color0));
            const LineVertex end(PxToGlm(line.pos1), PxColorToVec(line.color1));
            draw_list.AddLine(start, end);
        }
    }
}

void PhysicsService::OnCleanup()
{
    PxCloseVehicleExtension();

    PX_RELEASE(kScene_);
    PX_RELEASE(kMaterial_);
    PX_RELEASE(kDispatcher_);
    PX_RELEASE(cooking_);
    PX_RELEASE(kPhysics_);

    if (kPvd_)
    {
        PxPvdTransport* transport = kPvd_->getTransport();
        kPvd_->release();
        transport->release();
    }

    PX_RELEASE(kFoundation_);
}

void PhysicsService::OnGui()
{
    if (!show_debug_menu_)
    {
        return;
    }

    if (!ImGui::Begin("PhysicsService Debug", &show_debug_menu_))
    {
        ImGui::End();
        return;
    }

    // Simulation stats
    ImGui::Text("Simulation stats");
    ImGui::Spacing();

    PxSimulationStatistics stats;
    kScene_->getSimulationStatistics(stats);

    ImGui::Text("Tick rate: %d", tick_rate_);
    ImGui::Text("Static Bodies: %u", stats.nbStaticBodies);
    ImGui::Text("Dynamic Bodies: %u", stats.nbDynamicBodies);
    ImGui::Text("Active Dynamic Bodies: %u", stats.nbActiveDynamicBodies);
    ImGui::Text("Vehicles: %u", vehicles_.size());

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Visualization
    ImGui::Text("Visualization");
    ImGui::Spacing();

    ImGui::Checkbox("Draw Raycasts", &debug_draw_raycast_);

    if (ImGui::Checkbox("Scene Debug Drawing", &debug_draw_scene_))
    {
        kScene_->setVisualizationParameter(PxVisualizationParameter::eSCALE,
                                           debug_draw_scene_ ? 1.0f : 0.0f);
    }

    if (!debug_draw_scene_)
    {
        ImGui::BeginDisabled();
    }

    ImGui::Indent(6.0f);
    DrawDebugParamWidget("Collision shapes",
                         PxVisualizationParameter::eCOLLISION_SHAPES);
    DrawDebugParamWidget("Actor axes", PxVisualizationParameter::eACTOR_AXES);
    ImGui::Unindent(6.0f);

    if (!debug_draw_scene_)
    {
        ImGui::EndDisabled();
    }

    ImGui::End();
}

string_view PhysicsService::GetName() const
{
    return "PhysicsService";
}

PxRigidStatic* PhysicsService::CreatePlaneRigidStatic(const PxPlane& dimensions)
{
    return physx::PxCreatePlane(*kPhysics_, dimensions, *kMaterial_);
}

PxTriangleMesh* PhysicsService::CreateTriangleMesh(const string& mesh_name)
{
    // Converting our Mesh to a Px Mesh description
    const Mesh& mesh = asset_service_->GetMesh(mesh_name);

    PxTriangleMeshDesc mesh_desc;
    mesh_desc.setToDefault();

    vector<PxVec3> vertices;

    for (auto& vertex : mesh.vertices)
    {
        vertices.push_back(GlmToPx(vertex.position));
    }

    mesh_desc.triangles.count = static_cast<PxU32>(mesh.indices.size()) / 3;
    mesh_desc.triangles.data = mesh.indices.data();
    mesh_desc.triangles.stride = sizeof(uint32_t) * 3;

    mesh_desc.points.count = static_cast<PxU32>(vertices.size());
    mesh_desc.points.data = vertices.data();
    mesh_desc.points.stride = sizeof(PxVec3);

    // Cook and build a TriangleMesh
    PxDefaultMemoryOutputStream cooking_out_buffer;
    PxTriangleMeshCookingResult::Enum result;

    const bool status =
        cooking_->cookTriangleMesh(mesh_desc, cooking_out_buffer, &result);
    ASSERT_MSG(status, "Mesh cooking must succeeed");
    ASSERT_MSG(result != PxTriangleMeshCookingResult::Enum::eFAILURE,
               "Mesh cooking must succeed");

    if (result == PxTriangleMeshCookingResult::Enum::eLARGE_TRIANGLE)
    {
        debug::LogWarn("Mesh '{}' is too large for cooking, may cause issues",
                       mesh_name);
    }

    PxDefaultMemoryInputData mesh_in_buffer(cooking_out_buffer.getData(),
                                            cooking_out_buffer.getSize());
    PxTriangleMesh* triangle_mesh =
        kPhysics_->createTriangleMesh(mesh_in_buffer);
    ASSERT_MSG(triangle_mesh, "Mesh creation must succeed");

    return triangle_mesh;
}

PxRigidDynamic* PhysicsService::CreateRigidDynamic(const glm::vec3& position,
                                                   const glm::quat& orientation)
{
    const PxTransform pose = CreatePxTransform(position, orientation);
    PxRigidDynamic* rigid_dynamic = kPhysics_->createRigidDynamic(pose);
    ASSERT(rigid_dynamic);

    return rigid_dynamic;
}

PxRigidStatic* PhysicsService::CreateRigidStatic(const glm::vec3& position,
                                                 const glm::quat& orientation)
{
    const PxTransform pose = CreatePxTransform(position, orientation);
    PxRigidStatic* rigid_static = kPhysics_->createRigidStatic(pose);
    ASSERT(rigid_static);

    return rigid_static;
}

void PhysicsService::RegisterActor(PxActor* actor, Entity* entity)
{
    ASSERT_MSG(actor, "Actor must be valid");
    kScene_->addActor(*actor);
    actors_.insert({actor, entity});
}

void PhysicsService::RegisterVehicle(BaseVehicle* vehicle, Entity* entity)
{
    ASSERT_MSG(vehicle, "Vehicle must be valid");
    vehicles_.insert({vehicle, entity});
}

void PhysicsService::UnregisterActor(PxActor* actor, Entity* entity)
{
    ASSERT_MSG(actor, "Actor must be valid");
    kScene_->removeActor(*actor);
    for (auto pair = actors_.begin(), next_pair = pair; pair != actors_.end();
         pair = next_pair)
    {
        next_pair++;
        if (pair->first == actor)
        {
            actors_.erase(actor);
            return;
        }
    }
}

void PhysicsService::UnregisterVehicle(BaseVehicle* vehicle, Entity* entity)
{
    ASSERT_MSG(vehicle, "Vehicle must be valid");

    for (auto pair = vehicles_.begin(), next_pair = pair;
         pair != vehicles_.end(); pair = next_pair)
    {
        next_pair++;
        if (pair->first == vehicle)
        {
            vehicles_.erase(vehicle);
            return;
        }
    }

    /* auto iter = vehicles_.begin(); */
    /*  */
    /* while (iter != vehicles_.end()) */
    /* { */
    /*     // TODO: This may be an issue if the address of a BaseVehicle ever */
    /*     // changes */
    /*     if (*iter == vehicle) */
    /*     { */
    /*         vehicles_.erase(iter); */
    /*         return; */
    /*     } */
    /*  */
    /*     iter++; */
    /* } */
}

PxShape* PhysicsService::CreateShape(const physx::PxGeometry& geometry)
{
    PxShape* shape = kPhysics_->createShape(geometry, *kMaterial_, true);
    ASSERT(shape);

    return shape;
}

void PhysicsService::DrawDebugParamWidget(
    const string& name, PxVisualizationParameter::Enum parameter)
{
    const float value = kScene_->getVisualizationParameter(parameter);
    bool state = value != 0.0f;

    if (ImGui::Checkbox(name.c_str(), &state))
    {
        kScene_->setVisualizationParameter(parameter, state ? 1.0f : 0.0f);
    }
}

/* ---------- raycasting ---------- */
std::optional<RaycastData> PhysicsService::RaycastDynamic(
    const glm::vec3& origin, const glm::vec3& unit_dir,
    float max_distance /* default = 100000 */)
{
    // convert coordinates to PhysX units
    PxVec3 px_origin = GlmToPx(origin);
    PxVec3 px_unit_dir = GlmToPx(unit_dir);

    // get filter data
    PxHitFlags hit_flags = PxHitFlag::eDEFAULT;  // get first hit only
    PxQueryFilterData filter_data(PxQueryFlag::eDYNAMIC);

    PxRaycastBuffer raycast_result;
    kScene_->raycast(px_origin, px_unit_dir, max_distance, raycast_result,
                     hit_flags, filter_data);

    if (debug_draw_raycast_)
    {
        LineVertex start(PxToGlm(px_origin), Color4u(255, 0, 0, 255));
        LineVertex end(PxToGlm(px_origin + px_unit_dir * max_distance),
                       Color4u(255, 0, 0, 255));
        render_service_->GetDebugDrawList().AddLine(start, end);
    }

    // check if hit successful
    if (!raycast_result.hasBlock)
    {
        return std::nullopt;
    }

    // check if the actor hit even exists
    PxActor* target_actor = raycast_result.block.actor;
    if (actors_.count(target_actor) == 0)
    {
        return std::nullopt;
    }
    Entity* target_entity = actors_[target_actor];

    // bundle all the data to send
    RaycastData result(raycast_result, target_entity);

    return result;
}

std::optional<RaycastData> PhysicsService::RaycastStatic(
    const glm::vec3& origin, const glm::vec3& unit_dir,
    float max_distance /* default = 100000 */)
{
    // convert coordinates to PhysX units
    PxVec3 px_origin = GlmToPx(origin);
    PxVec3 px_unit_dir = GlmToPx(unit_dir);

    // get filter data
    PxHitFlags hit_flags = PxHitFlag::eDEFAULT;  // get first hit only
    PxQueryFilterData filter_data(PxQueryFlag::eSTATIC);

    PxRaycastBuffer raycast_result;
    kScene_->raycast(px_origin, px_unit_dir, max_distance, raycast_result,
                     hit_flags, filter_data);

    if (debug_draw_raycast_)
    {
        LineVertex start(PxToGlm(px_origin), Color4u(255, 0, 0, 255));
        LineVertex end(PxToGlm(px_origin + px_unit_dir * max_distance),
                       Color4u(0, 255, 0, 255));
        render_service_->GetDebugDrawList().AddLine(start, end);
    }

    // check if hit successful
    if (!raycast_result.hasBlock)
    {
        return std::nullopt;
    }

    // check if the actor hit even exists
    PxActor* target_actor = raycast_result.block.actor;
    if (actors_.count(target_actor) == 0)
    {
        return std::nullopt;
    }
    Entity* target_entity = actors_[target_actor];

    // bundle all the data to send
    RaycastData result(raycast_result, target_entity);

    return result;
}

/* ---------- PhysX ----------*/
void PhysicsService::InitPhysX()
{
    kFoundation_ = PxCreateFoundation(PX_PHYSICS_VERSION, kDefaultAllocator_,
                                      kDefaultErrorCallback_);
    ASSERT_MSG(kFoundation_, "PhysX must be initialized");

    kDispatcher_ = physx::PxDefaultCpuDispatcherCreate(kPhysxCpuThreads);
    ASSERT(kDispatcher_);

    //// For debugging purposes, initializing the physx visual debugger
    ///(download: https://developer.nvidia.com/gameworksdownload#)
    kPvd_ = PxCreatePvd(*kFoundation_);  // create the instance of pvd
    ASSERT_MSG(kPvd_, "Error initializing PhysX Visual Debugger");

    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(
        kPvdHost, kPvdPort, kPvdTimeoutMillis);
    ASSERT_MSG(transport, "Error creating PvdTransport Socket");

    const bool pvd_status =
        kPvd_->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
    if (pvd_status)
    {
        debug::LogInfo("PhysX SDK connected to PVD");
    }

    // Physics initlaization
    kPhysics_ = PxCreatePhysics(PX_PHYSICS_VERSION, *kFoundation_,
                                kDefaultTolerancesScale,
                                kPhysxRecordAllocations, kPvd_);
    ASSERT(kPhysics_);

    // create default material
    kMaterial_ = kPhysics_->createMaterial(0.5f, 0.5f, 0.6f);
    ASSERT(kMaterial_);

    cooking_ = PxCreateCooking(PX_PHYSICS_VERSION, *kFoundation_,
                               kDefaultPxCookingParams);
    ASSERT(cooking_);

    // setting up the vehicle physics
    const bool vehicle_init_status = PxInitVehicleExtension(*kFoundation_);
    ASSERT(vehicle_init_status);

    // Set up the simulation context.
    // The snippet is set up with
    // a) z as the longitudinal axis
    // b) x as the lateral axis
    // c) y as the vertical axis.
    // d) metres  as the lengthscale.
    vehicle_context_.setToDefault();
    vehicle_context_.frame.lngAxis = PxVehicleAxes::ePosZ;  // 4
    vehicle_context_.frame.latAxis = PxVehicleAxes::ePosX;  // 0
    vehicle_context_.frame.vrtAxis = PxVehicleAxes::ePosY;  // 2
    vehicle_context_.scale.scale = 1.0f;
    vehicle_context_.physxActorUpdateMode =
        PxVehiclePhysXActorUpdateMode::eAPPLY_ACCELERATION;
}

void PhysicsService::StepPhysics()
{
    if (kScene_)
    {
        while (time_accumulator_ >= kPhysxTimestep)
        {
            const float timestep_sec =
                static_cast<float>(kPhysxTimestep.GetSeconds());

            GetEventBus().Publish<OnPhysicsUpdateEvent>(
                &kPhysicsUpdateEventData);

            // Update vehicles
            for (auto& vehicle_entity : vehicles_)
            {
                BaseVehicle* vehicle = vehicle_entity.first;
                vehicle->step(timestep_sec, vehicle_context_);
            }

            // Update scene
            kScene_->simulate(timestep_sec);
            kScene_->fetchResults(true);

            time_accumulator_ -= kPhysxTimestep;
            tick_count_ += 1;
        }
    }

    // Measure physics tick rate
    const double cur_time = glfwGetTime();
    if (cur_time - prev_time_ >= 1.0)
    {
        tick_rate_ = tick_count_;
        tick_count_ = 0;
        prev_time_ = cur_time;
    }
}

const PxVec3& PhysicsService::GetGravity() const
{
    return kGravity;
}

/* From PxSimulationEventCallback */
void PhysicsService::onConstraintBreak(PxConstraintInfo* constraints,
                                       PxU32 count)
{
}

void PhysicsService::onWake(PxActor** actors, PxU32 count)
{
}

void PhysicsService::onSleep(PxActor** actors, PxU32 count)
{
}

void PhysicsService::onContact(const PxContactPairHeader& pair_header,
                               const PxContactPair* pairs, PxU32 pairs_count)
{
}

void PhysicsService::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        PxTriggerPair& pair = pairs[i];

        Entity* trigger_entity =
            static_cast<Entity*>(pair.triggerActor->userData);
        Entity* other_entity = static_cast<Entity*>(pair.otherActor->userData);

        ASSERT_MSG(trigger_entity,
                   "PxActor userdata must be a valid entity pointer");
        ASSERT_MSG(other_entity,
                   "PxActor userdata must be a valid entity pointer");

        OnTriggerEvent event_data = {.other = other_entity};

        bool enter = pair.status == PxPairFlag::eNOTIFY_TOUCH_FOUND;

        if (enter)
        {
            for (auto& entry : trigger_entity->GetComponents())
            {
                entry.component->OnTriggerEnter(event_data);
            }
        }
        else
        {
            for (auto& entry : trigger_entity->GetComponents())
            {
                entry.component->OnTriggerExit(event_data);
            }
        }

        event_data = {.other = trigger_entity};

        if (enter)
        {
            for (auto& entry : other_entity->GetComponents())
            {
                entry.component->OnTriggerEnter(event_data);
            }
        }
        else
        {
            for (auto& entry : other_entity->GetComponents())
            {
                entry.component->OnTriggerExit(event_data);
            }
        }
    }
}

void PhysicsService::onAdvance(const PxRigidBody* const* body_buffer,
                               const PxTransform* pose_buffer,
                               const PxU32 count)
{
}
