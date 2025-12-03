cmake_minimum_required(VERSION 3.16)

project(solar_sim LANGUAGES C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Trim vendored GLFW build to only the core library
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

# GLFW
add_subdirectory(Libraries/glfw-3.4)

#GLM
add_subdirectory(Libraries/glm)


# OpenGL
find_package(OpenGL REQUIRED)

file(GLOB UTIL_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/Util/*.cpp
)

add_executable(solar_sim
    main.cpp
    Libraries/glad/src/glad.c
    Libraries/stb/src/stb.cpp
    ${UTIL_SOURCES}
)

target_include_directories(solar_sim PRIVATE
    ${CMAKE_SOURCE_DIR}/Libraries/glad/include
    ${CMAKE_SOURCE_DIR}/Libraries/stb/include
    ${CMAKE_SOURCE_DIR}/Util
)

# Link everything
target_link_libraries(solar_sim
    PRIVATE
        glfw
        glm
        OpenGL::GL
)

# GLAD on Linux uses dlopen/dlsym
if(UNIX AND NOT APPLE)
    target_link_libraries(solar_sim PRIVATE dl)
endif()