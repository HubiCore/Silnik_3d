cmake_minimum_required(VERSION 3.31)
project(MyGLFWProject)

set(CMAKE_CXX_STANDARD 20)

# Find packages
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# Add executable
add_executable(${PROJECT_NAME} main.cpp)

# Link libraries
target_link_libraries(${PROJECT_NAME}
        glfw
        ${OPENGL_LIBRARIES}
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
        ${OPENGL_INCLUDE_DIR}
)