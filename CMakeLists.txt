cmake_minimum_required(VERSION 3.31)
project(MyGLFWProject)



set(CMAKE_CXX_STANDARD 20)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME}
        glfw
        ${OPENGL_LIBRARIES}
)
target_include_directories(${PROJECT_NAME} PRIVATE
        ${OPENGL_INCLUDE_DIR}
)