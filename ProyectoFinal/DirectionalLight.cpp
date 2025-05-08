#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Light()
{
    direction = glm::vec3(0.0f, -1.0f, 0.0f);
}

DirectionalLight::DirectionalLight(GLfloat red, GLfloat green, GLfloat blue,
    GLfloat aIntensity, GLfloat dIntensity,
    GLfloat xDir, GLfloat yDir, GLfloat zDir) : Light(red, green, blue, aIntensity, dIntensity)
{
    direction = glm::vec3(xDir, yDir, zDir);
}

void DirectionalLight::UseLight(GLfloat ambientIntensityLocation, GLfloat ambientcolorLocation,
    GLfloat diffuseIntensityLocation, GLfloat directionLocation)
{
    glUniform3f(ambientcolorLocation, color.x, color.y, color.z);
    glUniform1f(ambientIntensityLocation, ambientIntensity);

    glUniform3f(directionLocation, direction.x, direction.y, direction.z);
    glUniform1f(diffuseIntensityLocation, diffuseIntensity);
}

// Cambiar la dirección de la luz
void DirectionalLight::SetDirection(GLfloat x, GLfloat y, GLfloat z)
{
    direction = glm::vec3(x, y, z);
}

// Cambiar la intensidad de la luz
void DirectionalLight::SetIntensity(GLfloat ambientIntensity, GLfloat diffuseIntensity)
{
    this->ambientIntensity = ambientIntensity;
    this->diffuseIntensity = diffuseIntensity;
}

DirectionalLight::~DirectionalLight()
{
}