cmake_minimum_required(VERSION 3.10)
project(EventSystemTests)

set(CMAKE_CXX_STANDARD 20)

# Add your source files
set(SOURCE_FILES
    EventHub.cpp
    IListener.cpp
    Logger.cpp
    # Add other source files here
)

# Create your main library
add_library(EventSystem ${SOURCE_FILES})

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)

# Create the test executable
add_executable(RunTests event_system_tests.cpp)

# Link the test executable with your library and Google Test
target_link_libraries(RunTests EventSystem gtest_main)

# Include directories
target_include_directories(RunTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Add the test
include(GoogleTest)
gtest_discover_tests(RunTests)