#pragma once

#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

class TCamera
{
public:
    enum ECameraMovement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

private:
    glm::vec3 _WorldUp;
    // camera Attributes
    glm::vec3 _Position;
    glm::vec3 _Front;
    glm::vec3 _Up;
    glm::vec3 _Right;
    // euler Angles
    float _Yaw;
    float _Pitch;
    // camera options
    float _MovementSpeed;
    float _MouseSensitivity;
    float _Zoom;

public:
    TCamera(
        glm::vec3 position = glm::vec3(-136.0f, 14.0f, 115.0f),
        glm::vec3 worldUp = glm::vec3(0.0f, 0.0f, 1.0f),
        float yaw = -270.0f,
        float pitch = -23.0f
    ) : 
        _Front(glm::vec3(1.0f, 0.0f, -0.4f)),
        _MovementSpeed(250.0f),
        _MouseSensitivity(0.1f),
        _Zoom(45.0f)
    {
        _Position = position;
        _WorldUp = worldUp;
        _Yaw = yaw;
        _Pitch = pitch;
        UpdateCameraVectors();
    }

    glm::mat4 GetViewMatrix() {
        return glm::lookAt(_Position, _Position + _Front, _Up);
    }

    void ProcessKeyboard(ECameraMovement direction, float deltaTime) {
        float velocity = _MovementSpeed * deltaTime;
        if (direction == FORWARD) {
            _Position += _Front * velocity;
        }
        if (direction == BACKWARD) {
            _Position -= _Front * velocity;
        }
        if (direction == LEFT) {
            _Position -= _Right * velocity;
        }
        if (direction == RIGHT) {
            _Position += _Right * velocity;
        }
        UpdateCameraVectors();
    }

    void ProcessMouseMovement(glm::vec2 offset) {
        _Yaw   -= offset.x * _MouseSensitivity;
        _Pitch += offset.y * _MouseSensitivity;

        if (_Pitch > 89.0f)
            _Pitch = 89.0f;
        if (_Pitch < -89.0f)
            _Pitch = -89.0f;

        UpdateCameraVectors();
    }

private:

    void UpdateCameraVectors() {
        glm::vec3 front;
        front.x = sin(glm::radians(_Yaw)) * cos(glm::radians(_Pitch));
        front.y = cos(glm::radians(_Yaw)) * cos(glm::radians(_Pitch));
        front.z = sin(glm::radians(_Pitch));

        _Front = glm::normalize(front);
        _Right = glm::normalize(glm::cross(_Front, _WorldUp));
        _Up    = glm::normalize(glm::cross(_Right, _Front));
    }
};
