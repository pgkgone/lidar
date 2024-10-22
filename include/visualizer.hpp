#pragma once

#include "frame.hpp"
#include "camera.hpp"

#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <cstdint>
#include <iostream>
#include <optional>
#include <thread>

const uint16_t ToggleTimeout = 100;
const float CameraSpeed = 0.5f;
const double TargetFps = 60.0;

class TFrameVisualizer {
private:
    GLFWwindow* _Window;
    TCamera _Camera;

    const TFramesList& _Frames;

    uint64_t _CurrentFrame = 0;
    double _LastFrameRenderTime = 0.0;

    std::optional<glm::dvec2> _MousePos;

    uint64_t _ToggleTimeout = 0;
    bool _IsPaused = false;
    bool _IsSegmentationEnabled = false;

    const double _FrameDurationMs = 1.0 / TargetFps;

public:
    TFrameVisualizer(const TFramesList& frames)
        : _Frames(frames) 
    {
    }

    bool InitOpenGL() {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW\n";
            return false;
        }

        _Window = glfwCreateWindow(1600, 1200, "Lidar Visualizer", nullptr, nullptr);
        if (!_Window) {
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(_Window);

        glEnable(GL_DEPTH_TEST);
        glPointSize(3.0f);

        return true;
    }

    void Run() {
        _LastFrameRenderTime = glfwGetTime();
        while (!glfwWindowShouldClose(_Window)) {
            double currentTime = glfwGetTime();
            double deltaTime = currentTime - _LastFrameRenderTime;

            glfwPollEvents();
            ProcessInput(_Window, deltaTime);

            if (_FrameDurationMs <= deltaTime) { 
                _LastFrameRenderTime = currentTime;
                Render();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        glfwTerminate();
    }

private:

    void ProcessInput(GLFWwindow *window, float deltaTime) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            _Camera.ProcessKeyboard(TCamera::ECameraMovement::FORWARD, deltaTime * CameraSpeed);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            _Camera.ProcessKeyboard(TCamera::ECameraMovement::BACKWARD, deltaTime * CameraSpeed);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            _Camera.ProcessKeyboard(TCamera::ECameraMovement::LEFT, deltaTime * CameraSpeed);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            _Camera.ProcessKeyboard(TCamera::ECameraMovement::RIGHT, deltaTime * CameraSpeed);
        }
        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && _ToggleTimeout == 0) {
            _IsSegmentationEnabled = !_IsSegmentationEnabled;
            _ToggleTimeout = ToggleTimeout;
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && _ToggleTimeout == 0) {
            _IsPaused = !_IsPaused;
            _ToggleTimeout = ToggleTimeout;
        }

        if (_ToggleTimeout != 0) {
            --_ToggleTimeout;
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            glm::dvec2 pos;
            glfwGetCursorPos(window, &pos.x, &pos.y);
            if (_MousePos) {
                _Camera.ProcessMouseMovement(
                    _MousePos.value_or(glm::vec2()) - pos
                );
            }
            _MousePos = pos;
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
            _MousePos = std::nullopt;
        }
    }

    void Render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 proj = glm::perspective(
            glm::radians(45.0f),
            4.0f / 3.0f,
            0.1f,
            2000.0f
        );
        glm::mat4 view = _Camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        glm::mat4 mvp = proj * view * model;

        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(&proj[0][0]);

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(&view[0][0]);

        if (0 < _Frames.size()) {
            glBegin(GL_POINTS);
            for (const TPoint& point : _Frames[_CurrentFrame % _Frames.size()].Points) {
                glm::vec3 color = _IsSegmentationEnabled ? GetColorMap(point.Type) : GetColorMap(0);
                glColor3f(color.r, color.g, color.b);
                glVertex3f(point.X, point.Y, point.Z);
            }
            glEnd();
        }

        glfwSwapBuffers(_Window);

        if (!_IsPaused) {
            ++_CurrentFrame;
        }
    }
};