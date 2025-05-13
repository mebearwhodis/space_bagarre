#ifndef SPACEBAGARRE_GAME_LEVEL_H_
#define SPACEBAGARRE_GAME_LEVEL_H_
#include "physics_world.h"
#include "commons/commons.h"

namespace spacebagarre
{
    void CreateLevel(crackitos_physics::physics::PhysicsWorld* world);
    void CreateStaticAABBFromCorners(crackitos_physics::physics::PhysicsWorld* world,
                                     crackitos_core::math::Vec2f upper_left, crackitos_core::math::Vec2f bottom_right,
                                     fp friction);
    void CreateStaticAABB(crackitos_physics::physics::PhysicsWorld* world, crackitos_core::math::Vec2f half_size,
                          crackitos_core::math::Vec2f position, fp friction);

    //TODO check array size
    static constexpr std::array<crackitos_core::math::Vec2f, 60> platforms_bounds_ = {
        {
            {876.0f, 1044.0f},
            {1043.0f, 1064.0f},
            {918.0f, 1023.0f},
            {1001.0f, 1043.0f},
            {813.0f, 981.0f},
            {854.0f, 991.0f},
            {1065.0f, 981.0f},
            {1106.0f, 991.0f},
            {1065.0f, 897.0f},
            {1106.0f, 907.0f},
            {1254.0f, 876.0f},
            {1295.0f, 886.0f},
            {1380.0f, 855.0f},
            {1421.0f, 865.0f},
            {351.0f, 645.0f},
            {371.0f, 655.0f},
            {435.0f, 603.0f},
            {455.0f, 613.0f},
            {393.0f, 708.0f},
            {413.0f, 718.0f},
            {498.0f, 687.0f},
            {518.0f, 697.0f},
            {582.0f, 729.0f},
            {602.0f, 739.0f},
            {687.0f, 582.0f},
            {707.0f, 592.0f},
            {750.0f, 519.0f},
            {770.0f, 529.0f},
            {834.0f, 519.0f},
            {854.0f, 529.0f},
            {918.0f, 519.0f},
            {938.0f, 529.0f},
            {876.0f, 582.0f},
            {896.0f, 592.0f},
            {1002.0f, 477.0f},
            {1022.0f, 487.0f},
            {792.0f, 456.0f},
            {812.0f, 466.0f},
            {792.0f, 351.0f},
            {812.0f, 361.0f},
            {876.0f, 351.0f},
            {896.0f, 361.0f},
            {834.0f, 393.0f},
            {854.0f, 403.0f},
            {876.0f, 456.0f},
            {896.0f, 466.0f},
            {771.0f, 603.0f},
            {833.0f, 613.0f},
            {561.0f, 603.0f},
            {602.0f, 613.0f},
            {624.0f, 792.0f},
            {665.0f, 802.0f},
            {729.0f, 834.0f},
            {770.0f, 844.0f},
            {813.0f, 876.0f},
            {854.0f, 886.0f},
            {897.0f, 918.0f},
            {1022.0f, 928.0f},
            {918.0f, 309.0f},
            {1001.0f, 329.0f}
        }
    };
} // spacebagarre

#endif //SPACEBAGARRE_GAME_LEVEL_H_
