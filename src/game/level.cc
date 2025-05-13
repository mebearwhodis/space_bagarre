#include "game/level.h"

#include <engine/window.h>

#include "body.h"
#include "collider.h"
#include "shape.h"
#include "vec2.h"

namespace spacebagarre
{
    void CreateLevel(crackitos_physics::physics::PhysicsWorld* world)
    {
        //Screen borders
        //Lower
        crackitos_core::math::Vec2f half_size = {(kWindowWidth + 100.f) / 2.0f, 15.0f};
        crackitos_core::math::Vec2f position = {kWindowWidth / 2.0f, kWindowHeight};
        CreateStaticAABB(world, half_size, position, 0.0f);

        //Upper
        half_size = {(kWindowWidth + 100.f) / 2.0f, 25.0f};
        position = {kWindowWidth / 2.0f, 0.0f - half_size.y};
        CreateStaticAABB(world, half_size, position, 0.0f);

        //Left
        half_size = {25.0f, kWindowHeight / 2.0f};
        position = {0.0f - half_size.x, kWindowHeight / 2.0f};
        CreateStaticAABB(world, half_size, position, 0.0f);

        //Right
        half_size = {kWindowWidth + 25.0f, kWindowHeight / 2.0f};
        position = {kWindowWidth + half_size.x, kWindowHeight / 2.0f};
        CreateStaticAABB(world, half_size, position, 0.0f);

        //Platforms
        for (int i = 0; i < platforms_bounds_.size(); i = i + 2)
        {
            CreateStaticAABBFromCorners(world, platforms_bounds_[i], platforms_bounds_[i+1], 0.0f);
        }
    }

    void CreateStaticAABBFromCorners(crackitos_physics::physics::PhysicsWorld* world,
                                  crackitos_core::math::Vec2f upper_left,
                                  crackitos_core::math::Vec2f bottom_right,
                                  fp friction)
    {
        crackitos_core::math::Vec2f position = (upper_left + bottom_right) / 2.0f;

        crackitos_physics::physics::Body body_def;
        body_def.set_position(position);
        body_def.set_type(crackitos_physics::physics::BodyType::Static);
        crackitos_physics::physics::BodyHandle body_ = world->CreateBody(body_def);

        crackitos_core::math::AABB aabb(upper_left, bottom_right);
        crackitos_physics::physics::Collider collider_def(aabb, 0.0f, friction, false, body_);
        world->CreateCollider(body_, collider_def);
    }


    void CreateStaticAABB(crackitos_physics::physics::PhysicsWorld* world, crackitos_core::math::Vec2f half_size,
                          crackitos_core::math::Vec2f position, fp friction)
    {
        crackitos_physics::physics::Body body_def;
        body_def.set_position(position);
        body_def.set_type(crackitos_physics::physics::BodyType::Static);
        crackitos_physics::physics::BodyHandle body_ = world->CreateBody(body_def);

        crackitos_core::math::AABB aabb(
            crackitos_core::math::Vec2f(position.x - half_size.x, position.y - half_size.y),
            crackitos_core::math::Vec2f(position.x + half_size.x, position.y + half_size.y)
        );
        crackitos_physics::physics::Collider collider_def(aabb, 0.0f, friction, false, body_);
        world->CreateCollider(body_, collider_def);
    }
} // spacebagarre
