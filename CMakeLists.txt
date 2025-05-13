cmake_minimum_required(VERSION 3.18)
project(SpaceBagarre)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Options ----
option(ENABLE_PROFILING "Enable Tracy Profiling" OFF)
if (ENABLE_PROFILING)
    list(APPEND VCPKG_MANIFEST_FEATURES "profiling")
    find_package(Tracy CONFIG REQUIRED)
endif()

# ---- Externals ----
add_subdirectory(externals/crackitos_physics)
find_package(imgui CONFIG REQUIRED)
find_package(sdl3 CONFIG REQUIRED)
find_package(sdl3_image CONFIG REQUIRED)

# ---- ImGui ----
set(IMGUI_BACKENDS_DIR ${CMAKE_SOURCE_DIR}/externals/imgui/backends)
set(IMGUI_BACKENDS_SRC
        ${IMGUI_BACKENDS_DIR}/imgui_impl_sdl3.cpp
        ${IMGUI_BACKENDS_DIR}/imgui_impl_sdlrenderer3.cpp
)

# ---- Photon Libraries ----
set(PHOTON_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/externals/photon/include
        ${CMAKE_SOURCE_DIR}/externals/photon/include/Common-cpp
        ${CMAKE_SOURCE_DIR}/externals/photon/include/LoadBalancing-cpp
        ${CMAKE_SOURCE_DIR}/externals/photon/include/Photon-cpp
)

set(PHOTON_LIB_DIR ${CMAKE_SOURCE_DIR}/externals/photon/libs/windows)

set(PHOTON_LIBS
        debug ${PHOTON_LIB_DIR}/Common-cpp_vc17_debug_windows_md_x64.lib
        debug ${PHOTON_LIB_DIR}/LoadBalancing-cpp_vc17_debug_windows_md_x64.lib
        debug ${PHOTON_LIB_DIR}/Photon-cpp_vc17_debug_windows_md_x64.lib
        optimized ${PHOTON_LIB_DIR}/Common-cpp_vc17_release_windows_md_x64.lib
        optimized ${PHOTON_LIB_DIR}/LoadBalancing-cpp_vc17_release_windows_md_x64.lib
        optimized ${PHOTON_LIB_DIR}/Photon-cpp_vc17_release_windows_md_x64.lib
)

# ---- Source Files ----
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cc)
file(GLOB MAIN_FILES CONFIGURE_DEPENDS main/*.cc)

# ---- Include Directories ----
include_directories(${CMAKE_SOURCE_DIR}/include)

# ---- Create executable for each main/ file ----
foreach(MAIN_FILE ${MAIN_FILES})
    get_filename_component(EXE_NAME ${MAIN_FILE} NAME_WE)
    add_executable(${EXE_NAME}
            ${MAIN_FILE}
            ${SRC_FILES}
            ${IMGUI_BACKENDS_SRC}
    )

    # ---- Link Libraries ----
    target_link_libraries(${EXE_NAME} PRIVATE
            crackitos_core
            crackitos_physics
            imgui::imgui
            SDL3::SDL3
            SDL3_image::SDL3_image
            ${PHOTON_LIBS}
    )

    if (ENABLE_PROFILING)
        target_link_libraries(${EXE_NAME} PRIVATE Tracy::TracyClient)
    endif()

    target_include_directories(${EXE_NAME} PRIVATE
            ${CMAKE_SOURCE_DIR}/include
            ${PHOTON_INCLUDE_DIR}
            ${IMGUI_BACKENDS_DIR}
    )

    # Copy data/ folder next to executable
    add_custom_command(TARGET ${EXE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:${EXE_NAME}>/data
    )
endforeach()