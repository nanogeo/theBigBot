# The MIT License (MIT)
#
# Copyright (c) 2021-2022 Alexander Kurbatov

file(GLOB_RECURSE bot_sources *.cpp)
file(GLOB_RECURSE bot_headers *.h)

add_executable(theBigBot ${bot_sources} ${bot_headers})

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
