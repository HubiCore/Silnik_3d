cmake_minimum_required(VERSION 3.31)
project(MyGLFWProject)
set(CMAKE_CXX_STANDARD 20)

if (UNIX) # dla sigm
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW3 REQUIRED glfw3)
    find_package(OpenGL REQUIRED)
    find_package(glm REQUIRED)
    #glew to brainrot
    pkg_check_modules(GLEW REQUIRED glew)

    set(MY_INCLUDE_DIRS
            ${GLFW3_INCLUDE_DIRS}
            ${OPENGL_INCLUDE_DIR}
            ${GLM_INCLUDE_DIR}
            ${GLEW_INCLUDE_DIRS}
            ${stb_SOURCE_DIR}
    )

    set(MY_LIBRARIES
            ${GLFW3_LIBRARIES}
            ${OPENGL_LIBRARIES}
            ${GLEW_LIBRARIES}
    )

    set(MY_LINK_DIRECTORIES
            ${GLFW3_LIBRARY_DIRS}
            ${GLEW_LIBRARY_DIRS}
    )
endif (UNIX)
if (WIN32) # dla szpontów
    set(GLFW_PATH "${CMAKE_SOURCE_DIR}/libs/glfw")
    set(GLM_PATH "${CMAKE_SOURCE_DIR}/libs/glm")
    set(GLEW_PATH "${CMAKE_SOURCE_DIR}/libs/glew")
    set(STB_PATH "${CMAKE_SOURCE_DIR}/libs/stb-master")

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

    if(NOT EXISTS "${STB_PATH}")
        message(FATAL_ERROR "STB not found at: ${STB_PATH}")
    endif()

    message(STATUS "GLFW path: ${GLFW_PATH}")
    message(STATUS "GLM path: ${GLM_PATH}")
    message(STATUS "GLEW path: ${GLEW_PATH}")
    message(STATUS "STB path: ${STB_PATH}")

    # Wybierz bibliotekę GLFW
    if(EXISTS "${GLFW_PATH}/lib-mingw-w64/libglfw3.a")
        set(GLFW_LIBRARY "${GLFW_PATH}/lib-mingw-w64/libglfw3.a")
    elseif(EXISTS "${GLFW_PATH}/lib-mingw-w64/libglfw3dll.a")
        set(GLFW_LIBRARY "${GLFW_PATH}/lib-mingw-w64/libglfw3dll.a")
    else()
        message(FATAL_ERROR "No GLFW library found")
    endif()

    set(MY_INCLUDE_DIRS
            "${GLFW_PATH}/include"
            "${GLM_PATH}"
            "${GLEW_PATH}/include"
    )

    set(MY_LIBRARIES
            ${GLFW_LIBRARY}
            "${GLEW_PATH}/lib/Release/x64/glew32s.lib"
            opengl32
            gdi32
    )
endif(WIN32)

add_executable(${PROJECT_NAME}
        main.cpp
        Engine.cpp
        GeometryRenderer.cpp
        Camera/Camera.cpp
        Camera/Camera.hpp
        ComplexObject.hpp
        ComplexObject.cpp
        GeometryRenderer.cpp
        Transform/Transform.hpp
        Transform/Transform.cpp
        Transform/TransformableObject.hpp
        Transform/TransformableObject.cpp
        Transform/TransformableGeometry.hpp
        Transform/TransformableGeometry.cpp
        SceneManager/SceneManager.hpp
        SceneManager/SceneManager.cpp
)

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${MY_INCLUDE_DIRS})

# Add link directories (for Unix)
if (UNIX)
    target_link_directories(${PROJECT_NAME} PRIVATE ${MY_LINK_DIRECTORIES})
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME} ${MY_LIBRARIES})

if(WIN32)
    add_definitions(-D_USE_MATH_DEFINES)
    add_definitions(-DGLEW_STATIC)  # Ważne dla statycznej wersji GLEW (coklwiek to znaczy)
endif()

add_definitions(-DGLM_FORCE_RADIANS)
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)