//
// Created by Hubi_Core on 12/9/25.
//
#include <GLFW/glfw3.h>
#include <iostream>
#ifndef ENGINE_CUH
#define ENGINE_CUH

class Engine {
    int resX, reY, fps;
    public:
    Engine(int resX, int resY, int fps) {
        this->resX = resX;
        this->reY = resY;
        this->fps = fps;
        GLFWwindow* window = glfwCreateWindow(resX, resY, "Silnik 3d", nullptr, nullptr);
    };
    void Close();





};
#endif //ENGINE_CUH
