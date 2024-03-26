cmake_minimum_required(VERSION 3.25)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 23)

Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.4.0
)

FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

add_executable(tests_run RepetitionTest.cpp
        HashTest.cpp
        UtilTest.cpp
        TranspositionTest.cpp)
target_link_libraries(tests_run PRIVATE src)
target_link_libraries(tests_run PRIVATE Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(tests_run)
