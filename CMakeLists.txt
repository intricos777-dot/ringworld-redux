cmake_minimum_required(VERSION 3.20)
project(te-halo-inspired VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(/home/sin/Projects/twilight-elysium ${CMAKE_BINARY_DIR}/twilight-elysium)

# Core game library
add_library(tehi-core STATIC
    src/core/game.cpp
    src/core/world.cpp
)
target_link_libraries(tehi-core PUBLIC te-engine te-renderer te-asset)
target_include_directories(tehi-core PUBLIC include)

# Player controller
add_library(tehi-player STATIC
    src/player/controller.cpp
    src/player/camera.cpp
)
target_link_libraries(tehi-player PRIVATE tehi-core)

# Weapons
add_library(tehi-weapons STATIC
    src/weapons/weapon.cpp
    src/weapons/inventory.cpp
)
target_link_libraries(tehi-weapons PRIVATE tehi-core)

# Vehicles
add_library(tehi-vehicles STATIC
    src/vehicles/vehicle.cpp
)
target_link_libraries(tehi-vehicles PRIVATE tehi-core)

# AI
add_library(tehi-ai STATIC
    src/ai/bot.cpp
    src/ai/navmesh.cpp
)
target_link_libraries(tehi-ai PRIVATE tehi-core)

# Campaign / Co-op
add_library(tehi-campaign STATIC
    src/campaign/mission.cpp
)
target_link_libraries(tehi-campaign PRIVATE tehi-core)

add_library(tehi-multiplayer STATIC
    src/multiplayer/coop.cpp
)
target_link_libraries(tehi-multiplayer PRIVATE tehi-core tehi-campaign)

# World/maps
add_library(tehi-world STATIC
    src/world/map.cpp
    src/world/spawner.cpp
)
target_link_libraries(tehi-world PRIVATE tehi-core tehi-ai tehi-weapons)

# UI
add_library(tehi-ui STATIC
    src/ui/hud.cpp
)
target_link_libraries(tehi-ui PRIVATE tehi-core)

# Test runner
enable_testing()
add_executable(tehi-test src/core/main.cpp)
target_link_libraries(tehi-test PRIVATE tehi-core tehi-player tehi-weapons tehi-world tehi-ai)
add_test(NAME tehi-core COMMAND tehi-test)
