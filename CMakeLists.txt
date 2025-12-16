cmake_minimum_required(VERSION 3.20)
project(Silnik3D)

set(CMAKE_CXX_STANDARD 20)

# Ścieżki
set(GLFW_PATH "${CMAKE_SOURCE_DIR}/libs/glfw")
set(GLM_PATH "${CMAKE_SOURCE_DIR}/libs/glm")
set(GLEW_PATH "${CMAKE_SOURCE_DIR}/libs/glew")

# Sprawdź czy biblioteki istnieją
if(NOT EXISTS "${GLFW_PATH}")
    message(FATAL_ERROR "GLFW not found at: ${GLFW_PATH}")
endif()

if(NOT EXISTS "${GLM_PATH}")
    message(FATAL_ERROR "GLM not found at: ${GLM_PATH}")
endif()

if(NOT EXISTS "${GLEW_PATH}")
    message(FATAL_ERROR "GLEW not found at: ${GLEW_PATH}")
endif()

message(STATUS "GLFW path: ${GLFW_PATH}")
message(STATUS "GLM path: ${GLM_PATH}")
message(STATUS "GLEW path: ${GLEW_PATH}")

# Wybierz bibliotekę GLFW
if(EXISTS "${GLFW_PATH}/lib-mingw-w64/libglfw3.a")
    set(GLFW_LIBRARY "${GLFW_PATH}/lib-mingw-w64/libglfw3.a")
elseif(EXISTS "${GLFW_PATH}/lib-mingw-w64/libglfw3dll.a")
    set(GLFW_LIBRARY "${GLFW_PATH}/lib-mingw-w64/libglfw3dll.a")
else()
    message(FATAL_ERROR "No GLFW library found")
endif()

# Pliki źródłowe
add_executable(${PROJECT_NAME}
        main.cpp
        Engine.cpp
        GeometryRenderer.cpp
        Camera.cpp
        Camera.hpp
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        "${GLFW_PATH}/include"
        "${GLM_PATH}"
        "${GLEW_PATH}/include"
)

# Linkowanie
target_link_libraries(${PROJECT_NAME}
        ${GLFW_LIBRARY}
        "${GLEW_PATH}/lib/Release/x64/glew32s.lib"  # Statyczna wersja GLEW
        opengl32
        gdi32
)

# Definicje
if(WIN32)
    add_definitions(-D_USE_MATH_DEFINES)
    add_definitions(-DGLEW_STATIC)  # Ważne dla statycznej wersji GLEW
endif()

add_definitions(-DGLM_FORCE_RADIANS)
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

message(STATUS "Project configuration complete!")