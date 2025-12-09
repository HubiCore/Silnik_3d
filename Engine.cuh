//
// Created by Hubi_Core on 12/9/25.
//
#include <GLFW/glfw3.h>
#include <iostream>
#include <functional>
#ifndef ENGINE_CUH
#define ENGINE_CUH

class Engine {
    int resX, reY, fps;
    bool is_fullscreen;
    std::function<void(GLFWwindow*, int, int, int, int)> keyCallback;
    std::function<void(GLFWwindow*, double, double)> mouseCallback;
    std::function<void(GLFWwindow*, int, int, int)> mouseButtonCallback;
    public:
    Engine(int resX, int resY, int fps) {
        this->resX = resX;
        this->reY = resY;
        this->fps = fps;
        GLFWwindow* window = glfwCreateWindow(resX, resY, "Silnik 3d", nullptr, nullptr);
    };
    void Close();
    void set_fullscreen(bool fullscreen);
    ~Engine();



};
#endif //ENGINE_CUH
