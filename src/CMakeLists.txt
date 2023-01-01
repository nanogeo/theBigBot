# The MIT License (MIT)
#
# Copyright (c) 2021-2022 Alexander Kurbatov

set(bot_sources
    main.cpp
    TossBot.cpp)

add_executable(TossBot ${bot_sources})

if (BUILD_FOR_LADDER)
    target_compile_definitions(TossBot PRIVATE BUILD_FOR_LADDER)
endif ()

if (MSVC)
    target_compile_options(TossBot PRIVATE /W4 /EHsc)
else ()
    target_compile_options(TossBot PRIVATE -Wall -Wextra -pedantic)
endif ()

target_link_libraries(TossBot PRIVATE cpp_sc2)

if (MINGW)
    target_link_libraries(TossBot PRIVATE ssp)
elseif (APPLE)
    target_link_libraries(TossBot PRIVATE "-framework Carbon")
# Building on Linux
elseif (UNIX AND NOT APPLE)
    target_link_libraries(TossBot PRIVATE pthread dl)
endif ()
