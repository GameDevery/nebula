#pragma once
//------------------------------------------------------------------------------
/**
    @class PhysicsFeature::PhysicsFeatureUnit

    Sets up and interfaces with the physics subsystem.
    
    @copyright
    (C) 2019-2020 Individual contributors, see AUTHORS file
*/
#include "game/featureunit.h"
#include "core/cvar.h"

#define TIMESOURCE_PHYSICS 'PHTS'

//------------------------------------------------------------------------------
namespace PhysicsFeature
{

class PhysicsFeatureUnit : public Game::FeatureUnit
{
    __DeclareClass(PhysicsFeatureUnit)
    __DeclareSingleton(PhysicsFeatureUnit)

public:

    /// constructor
    PhysicsFeatureUnit();
    /// destructor
    ~PhysicsFeatureUnit();

    void OnAttach() override;

    /// Called upon activation of feature unit
    void OnActivate();
    /// Called upon deactivation of feature unit
    void OnDeactivate();
    /// called from within GameServer::NotifyBeforeCleanup()
    void OnBeforeCleanup(Game::World* world);

    /// called on begin of frame
    virtual void OnBeginFrame();
    /// called at the end of the feature trigger cycle
    virtual void OnDecay();
    /// called when game debug visualization is on
    virtual void OnRenderDebug();

private:
    Util::Dictionary<Game::World*, IndexT> physicsWorlds;
    bool simulating;
    int debugDrawPhysicsLastValue = 0;

    Core::CVar* cl_debug_draw_physics;
};

} // namespace PhysicsFeature
//------------------------------------------------------------------------------
