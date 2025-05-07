#pragma once
#include "Light.h"

class DirectionalLight : public Light
{
public:
    DirectionalLight();
    DirectionalLight(GLfloat red, GLfloat green, GLfloat blue,
        GLfloat aIntensity, GLfloat dIntensity,
        GLfloat xDir, GLfloat yDir, GLfloat zDir);

    void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientcolorLocation,
        GLfloat diffuseIntensityLocation, GLfloat directionLocation);

    // M�todos para cambiar la direcci�n e intensidad
    void SetDirection(GLfloat x, GLfloat y, GLfloat z);
    void SetIntensity(GLfloat ambientIntensity, GLfloat diffuseIntensity);

    ~DirectionalLight();

private:
    glm::vec3 direction; // Direcci�n de la luz
};