#ifndef SPACEBAGARRE_GAME_LEVEL_H_
#define SPACEBAGARRE_GAME_LEVEL_H_
#include "game_contact_listener.h"
#include "physics_world.h"
#include "commons/commons.h"

namespace spacebagarre
{
    void CreateLevel(crackitos_physics::physics::PhysicsWorld* world, GameContactListener* listener);
    void CreateStaticAABBFromCorners(crackitos_physics::physics::PhysicsWorld* world,
                                     crackitos_core::math::Vec2f upper_left,
                                     crackitos_core::math::Vec2f bottom_right,
                                     fp friction,
                                     GameContactListener* listener);
    void CreateOuterWall(crackitos_physics::physics::PhysicsWorld* world,
                         crackitos_core::math::Vec2f half_size,
                         crackitos_core::math::Vec2f position,
                         fp friction,
                         GameContactListener* listener);

    //TODO check array size
    static constexpr std::array<crackitos_core::math::Vec2f, 60> platforms_bounds_ = {
        {

        }
    };
} // spacebagarre

#endif //SPACEBAGARRE_GAME_LEVEL_H_
