# The MIT License (MIT)
#
# Copyright (c) 2021-2022 Alexander Kurbatov

set(bot_sources
    action_manager.cpp
    army_group.cpp
    build_order_manager.cpp
    finite_state_machine.cpp
    fire_control
    locations.cpp
    locations_abyssal_reef.cpp
    locations_acropolis.cpp
    locations_automaton.cpp
    locations_ephemeron.cpp
    locations_equilibrium.cpp
    locations_goldenaura.cpp
    locations_gresvan.cpp
    locations_hard_lead.cpp
    locations_interloper.cpp
    locations_oceanborn.cpp
    locations_site_delta.cpp
    locations_thunderbird.cpp
    main.cpp
    nav_mesh_pathfinding.cpp
    path_manager.cpp
    pathfinding.cpp
    quartic.cpp
    theBigBot.cpp
    utility.cpp
    worker_manager.cpp)

add_executable(theBigBot ${bot_sources})

if (BUILD_FOR_LADDER)
    target_compile_definitions(theBigBot PRIVATE BUILD_FOR_LADDER)
endif ()

if (MSVC)
    target_compile_options(theBigBot PRIVATE /W4 /EHsc)
else ()
    target_compile_options(theBigBot PRIVATE -Wall -Wextra -pedantic)
endif ()

target_link_libraries(theBigBot PRIVATE cpp_sc2)

if (MINGW)
    target_link_libraries(theBigBot PRIVATE ssp)
elseif (APPLE)
    target_link_libraries(theBigBot PRIVATE "-framework Carbon")
# Building on Linux
elseif (UNIX AND NOT APPLE)
    target_link_libraries(theBigBot PRIVATE pthread dl)
endif ()
