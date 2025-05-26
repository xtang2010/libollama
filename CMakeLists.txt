# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(libollama LANGUAGES C)

# Set the build type to Debug
set(CMAKE_BUILD_TYPE Debug)

# Find all lib sources
file(GLOB LIB_SOURCES "lib/*.c")

# Include directories
include_directories(lib/public)
include_directories(/usr/local/include)

# Add the library
add_library(ollama SHARED ${LIB_SOURCES})

# Add test cases
enable_testing()

# add customer target "test_all"
add_custom_target(test_all)
add_dependencies(test_all ollama)


# find all test sources
file(GLOB TEST_SOURCES "test/*.c")

# for libcurl
link_directories(/usr/local/lib)

# test executables
foreach(SOURCE ${TEST_SOURCES})
    get_filename_component(BASE_NAME ${SOURCE} NAME_WE)
    add_executable(${BASE_NAME} EXCLUDE_FROM_ALL ${SOURCE})
    add_dependencies(test_all ${BASE_NAME})
    target_link_libraries(${BASE_NAME} ollama libcurl.so libcjson.so)
    add_test(NAME ${BASE_NAME} COMMAND ${BASE_NAME})
endforeach()

# installation
set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation directory prefix")
install(FILES lib/public/ollama/ollama.h DESTINATION include/ollama)
install(TARGETS ollama ARCHIVE DESTINATION lib)
