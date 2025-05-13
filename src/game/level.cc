#include "game/level.h"

#include <engine/window.h>

#include "body.h"
#include "collider.h"
#include "shape.h"
#include "vec2.h"

namespace spacebagarre
{
    void CreateLevel(crackitos_physics::physics::PhysicsWorld* world, GameContactListener* listener)
    {
        //Screen borders
        //Lower
        crackitos_core::math::Vec2f half_size = {(kWindowWidth + 100.f) / 2.0f, 27.0f};
        crackitos_core::math::Vec2f position = {kWindowWidth / 2.0f, kWindowHeight};
        CreateOuterWall(world, half_size, position, 0.0f, listener);

        //Upper
        half_size = {(kWindowWidth + 100.f) / 2.0f, 27.0f};
        position = {kWindowWidth / 2.0f, 0.0f};
        CreateOuterWall(world, half_size, position, 0.0f, listener);

        //Left
        half_size = {27.0f, kWindowHeight / 2.0f};
        position = {0.0f, kWindowHeight / 2.0f};
        CreateOuterWall(world, half_size, position, 0.0f, listener);

        //Right
        half_size = {27.0f, kWindowHeight / 2.0f};
        position = {kWindowWidth, kWindowHeight / 2.0f};
        CreateOuterWall(world, half_size, position, 0.0f, listener);
    }

    void CreateStaticAABBFromCorners(crackitos_physics::physics::PhysicsWorld* world,
                                     crackitos_core::math::Vec2f upper_left,
                                     crackitos_core::math::Vec2f bottom_right,
                                     fp friction,
                                     GameContactListener* listener)
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


    void CreateOuterWall(crackitos_physics::physics::PhysicsWorld* world,
                         crackitos_core::math::Vec2f half_size,
                         crackitos_core::math::Vec2f position,
                         fp friction,
                         GameContactListener* listener)
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
        const auto collider_handle = world->CreateCollider(body_, collider_def);
        if (listener)
        {
            listener->RegisterToListenerMap(ObjectType::kWall, collider_handle);
        }
    }
} // spacebagarre
