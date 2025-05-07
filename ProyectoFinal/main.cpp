/*
Práctica 7: Iluminación 1 
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;
float angulovaria = 0.0f;
float caballo1RotationX = 90.0f; // Ángulo de rotación inicial para el caballo 1
float caballo2RotationX = 135.0f; // Ángulo de rotación inicial para el caballo 1
float angleCaballo1 = 0.0f;
float angleCaballo2 = 0.0f;
float angleBalloons = 0.0f;
float angleBaseball = 0.0f;
int index = 0;

float handAnimationTime = 0.0f; // Tiempo acumulado para la animación de las manos
bool handIncreasing = true;    // Indica si las manos están aumentando el ángulo
float handMinAngle = 85.0f;    // Ángulo mínimo de las manos
float handMaxAngle = 95.0f;    // Ángulo máximo de las manos
float handCurrentAngle = 95.0f; // Ángulo actual de las manos
float handAnimationDuration = 30.0f; // Duración de la animación (en segundos)
bool isHandAnimationActive = false; // Indica si la animación de las manos está activa

float brazoAnimationTime = 0.0f; // Tiempo acumulado para la animación de los brazos
bool brazoIncreasing = true;    // Indica si los brazos están aumentando el ángulo
float brazoMinAngle = 175.0f;   // Ángulo mínimo de los brazos
float brazoMaxAngle = 185.0f;   // Ángulo máximo de los brazos
float brazoCurrentAngle = 175.0f; // Ángulo actual de los brazos
float brazoAnimationDuration = 30.0f; // Duración de la animación (en segundos)

float jumpHeight = 0.5f;       // Altura máxima del salto
float jumpSpeed = 0.05f;        // Velocidad del salto (unidades por segundo)
float characterVerticalPosition = 0.0f; // Posición vertical actual del personaje
bool isJumpingUp = true;       // Indica si el personaje está subiendo
bool isJumping = false;        // Indica si el personaje está en el aire

enum CameraMode {
    FIRST_PERSON,
    THIRD_PERSON,
    OVERHEAD,
    STATIC_CAMERA
};

// Índices para las luces de las atracciones en el arreglo pointLights
const int IDX_BOLICHE = 6;
const int IDX_CARRUSEL = 7;
const int IDX_RUEDA = 8;
const int IDX_TOPO = 9;
const int IDX_BEISBOL = 10;
const int IDX_DARDO = 11;

float dayNightTimer = 0.0f; // Temporizador para alternar entre día y noche

// Estados y temporizadores para las luces de discoteca
bool discoLightActive[6] = { false, false, false, false, false, false };
float discoLightTimer[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
int discoLightColorIndex[6] = { 0, 0, 0, 0, 0, 0 };

glm::vec3 positionAux;
float rotationAux;

// Colores de discoteca
glm::vec3 discoColors[3] = {
    glm::vec3(1.0f, 0.0f, 0.0f), // Rojo
    glm::vec3(0.0f, 0.0f, 1.0f), // Azul
    glm::vec3(0.0f, 1.0f, 0.0f)  // Verde
};

glm::vec3 positionsGames[6] = {
    glm::vec3(12.0f, 0.0f, -0.6f), // Carousel
    glm::vec3(19.5f, 1.0f, 9.7f), // Rueda de la fortuna
    glm::vec3(-5.0f, 2.0f, 15.0f), // Topo
    glm::vec3(-6.0f, 2.0f, -5.0f), // Baseball
    glm::vec3(-7.0f, 3.0f, -15.0f), // Linea de boliche
    glm::vec3(17.0f, 0.0f, 15.0f), // Lanzamiento de dardo
};

float valueYawGames[6] = {
    {-90.0f},
    {0.0f},
    {0.0f},
    {0.0f},
    {0.0f},
    {-180.0}
};

float valuePitchGames[6] = {
    {0.0f},
    {15.0f},
    {-30.0f},
    {-25.0f},
    {-45.0f},
    {0.0f}
};

glm::vec3 translateWhileAnimationIsActive[6]{
    glm::vec3(11.5f, -0.1f, -2.2),
    glm::vec3(27.5f, -0.1f, 9.7f),
    glm::vec3(-0.5f, -0.3f, 15.0f),
    glm::vec3(-3.0f, -0.1f, -5.0f),
    glm::vec3(-4.6f, -0.1f, -15.0f),
    glm::vec3(13.5f, -0.1f, 15.5f),
};

float rotateWhileAnimationIsActive[6]{
    {90.0f},
    {0.0f},
    {0.0f},
    {0.0f},
    {0.0f},
    {180.0f},
};

CameraMode currentCameraMode = FIRST_PERSON;

float topo1PositionY = -0.5f; // Posición inicial en Y del Topo 1
int topo1VerticalState = 0;

float topo2PositionY = -0.5f; // Posición inicial en Y del Topo 1
int topo2VerticalState = 0;

float topo1RotationY = 0.0f;       // Ángulo actual de rotación del Topo 1
float topo1TimeAccumulator = 0.0f; // Tiempo acumulado en el estado actual
int topo1State = 0;

float topo2RotationY = 0.0f;       // Ángulo actual de rotación del Topo 1
float topo2TimeAccumulator = 0.0f; // Tiempo acumulado en el estado actual
int topo2State = 0;

float baseballPositionX = 0.0f; // Posición inicial en X del modelo Baseball
bool baseballMovingForward = true;

bool isAnimatingCarousel = false; // Indica si la animación del Carousel está activa
int carouselState = 0;            // Estado actual de la animación (0: inicial, 1: girando, 2: regreso al estado inicial)
float carouselRotationY = 90.0f;   // Ángulo de rotación actual del tubo y los caballos
float carouselSpeed = 0.5f;      // Velocidad de rotación (grados por segundo)
float carouselAnimationTime = 0.0f; // Tiempo acumulado para la animación
float carouselAnimationDuration = 4.0f; // Duración de la animación completa (en segundos)

bool isAnimatingBaseball = false;
int baseballState = 0;
float batRotationY = 0;
float baseballSpeed = 2.0f;
float baseballAnimationTime = 0.0f;
float baseballAnimationDuration = 4.0f;

bool isAnimatingTopo = false;
int topoState = 0;
float topoRotation = 0;

float asientosRotationX = 0.0f; // Ángulo de rotación actual de los asientos en el eje X
int asientosDirection = 1;      // Dirección de la rotación (1: aumentando, -1: disminuyendo)
float asientosSpeed = 0.05f;    // Velocidad de rotación de los asientos (grados por segundo)

float characterRotationY = 0.0f; // Ángulo de rotación del personaje en el eje Y

bool isAnimatingMedusa = true; // Indica si la animación está activa
int medusaState = 0;           // Estado actual de la animación (0: inicial, 1: moviendo en Z negativo, 2: regresando)
float medusaPositionZ = 0.0f;  // Posición actual en Z de la medusa
float medusaSpeed = 1.0f;      // Velocidad de movimiento (unidades por segundo)
float medusaAnimationTime = 0.0f; // Tiempo acumulado para la animación
float medusaAnimationDuration = 300.0f; // Duración de cada traslado (en segundos)

float medusaRotationY = 0.0f; // Ángulo actual de rotación en el eje Y
bool medusaRotationDirection = true; // Dirección de la rotación (true: aumentando, false: disminuyendo)
bool isMedusaLightActive = false;

float proximityThreshold = 7.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;


// Personajes

Model Cuerpo_M;
Model BrazoD_M;
Model BrazoI_M;
Model PieD_M;
Model PieI_M;

// Utils

Model Coin_M;
Model Medusa_M;
Model Lampara_M;

// Atracciones
Model BaseRueda_M;
Model AsentosRueda_M;
Model RuedaFortuna_M;
Model GolpeaTopo_M;
Model Globos_M;
Model Carousel_M;
Model MonanhaRusa_M;
Model Cancha_M;
Model Baseball_M;
Model Bat_M;
Model Caballo_M;
Model Tubos_M;
Model TopoToy_M;
Model LineBowling_M;
Model Bowling_M;
Model Pine_M;
Model PuestoGlobos_M;
Model DardoGlobos_M;
Model Globo_M;

// Puestos de servicio
Model HotDogs_M;
Model Pizza_M;
Model Hotdogs2_M;
Model Palomitas_M;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


// KEYFRAMES 

float dayLightIntensity = 0.6f; // Intensidad de la luz durante el día
float nightLightIntensity = 0.3f; // Intensidad de la luz durante la noche

bool isDartAnimating = false; // Indica si la animación del dardo está activa
float dartPositionX = 0.0f;   // Posición actual del dardo en el eje Xs
float dartSpeed = 0.01f;       // Velocidad del dardo (unidades por segundo)
bool areBalloonsVisible = true; // Indica si los globos están visibles
int dartState = 0;

float brazoD_RotationX = 0.0f;
float brazoI_RotationX = 0.0f;
float pieD_RotationX = 0.0f;
float pieI_RotationX = 0.0f;
float walkAnimationTime = 0.0f; // Acumulador de tiempo para la animación
bool isWalking = false;

bool isAnimatingBowling = false; // Indica si la animación está activa
int bowlingState = 0;            // Estado actual de la animación (0: inicial, 1: avanzando, 2: regresando)
float bowlingPositionX = -4.5f;  // Posición inicial en X de la bola de boliche
float bowlingSpeed = 0.05f;

bool pinsKnockedOver = false; // Indica si los pinos ya han sido derribados
float pinsRotationX = 0.0f;   // Ángulo de rotación actual de los pinos

float pinsAnimationDuration = 1.0f; // Duración de la animación (en segundos)
float pinsAnimationTime = 0.0f;     // Tiempo acumulado para la animación
bool pinsReturningToNormal = false;

bool isAnimatingRueda = false; // Indica si la animación de la rueda está activa
int ruedaState = 0;            // Estado actual de la animación (0: inicial, 1: girando, 2: regreso al estado inicial)
float ruedaRotationX = 0.0f;   // Ángulo de rotación actual de la rueda en el eje X
float ruedaSpeed = 0.5f;

bool isDay = true; // Estado inicial: día
bool tKeyPressed = false;

bool isAnimationActive = false;

float changeValueVCarousel(float value) {
    if (isAnimatingCarousel) {
        return value;
    } return -0.8f;
}

void UpdateMedusaAnimation(float deltaTime) {
    float moveDuration = medusaAnimationDuration; // Duración del traslado en segundos
    switch (medusaState) {
    case 0: // Estado inicial
        medusaPositionZ = 0.0f; // Asegurar posición inicial en Z
        if (isAnimatingMedusa) {
            medusaState = 1; // Cambiar al estado "moviendo en Z negativo"
        }
        break;

    case 1: // Moviendo en Z negativo
        medusaAnimationTime += deltaTime; // Incrementar el tiempo acumulado
        medusaPositionZ = glm::mix(0.0f, -4.0f, medusaAnimationTime / moveDuration); // Interpolación lineal entre 0 y -4 unidades

        if (medusaAnimationTime >= moveDuration) { // Si se completa la duración
            medusaPositionZ = -4.0f; // Asegurarse de que no exceda el límite
            medusaAnimationTime = 0.0f; // Reiniciar el tiempo acumulado
            medusaState = 2; // Cambiar al estado "regresando"
        }
        break;

    case 2: // Regresando a la posición inicial
        medusaAnimationTime += deltaTime; // Incrementar el tiempo acumulado
        medusaPositionZ = glm::mix(-4.0f, 0.0f, medusaAnimationTime / moveDuration); // Interpolación lineal hacia 0 unidades

        if (medusaAnimationTime >= moveDuration) { // Si se completa la duración
            medusaPositionZ = 0.0f; // Asegurarse de que no sea menor que el límite
            medusaAnimationTime = 0.0f; // Reiniciar el tiempo acumulado
            medusaState = 0; // Volver al estado inicial
        }
        break;
    }
}

void UpdateJumpAnimation(float deltaTime) {
    if (isAnimatingBowling || isAnimatingRueda || isAnimatingCarousel || isDartAnimating) { // Solo realizar el salto si hay una animación activa
        if (isJumpingUp) {
            // Subir al personaje
            characterVerticalPosition += jumpSpeed * deltaTime;
            if (characterVerticalPosition >= jumpHeight) {
                characterVerticalPosition = jumpHeight; // Limitar la altura máxima
                isJumpingUp = false; // Cambiar a la fase de descenso
            }
        }
        else {
            // Bajar al personaje
            characterVerticalPosition -= jumpSpeed * deltaTime;
            if (characterVerticalPosition <= 0.0f) {
                characterVerticalPosition = 0.0f; // Limitar la posición mínima
                isJumpingUp = true; // Cambiar a la fase de ascenso
                isJumping = false; // Finalizar el salto
            }
        }
    }
}

void UpdateMedusaRotation(float deltaTime) {
    float rotationSpeed = 0.1f; // Velocidad de rotación en grados por segundo
    float maxRotation = 10.0f; // Límite superior de rotación
    float minRotation = -10.0f; // Límite inferior de rotación

    // Actualizar el ángulo de rotación según la dirección
    if (medusaRotationDirection) {
        medusaRotationY += rotationSpeed * deltaTime;
        if (medusaRotationY >= maxRotation) {
            medusaRotationY = maxRotation;
            medusaRotationDirection = false; // Cambiar dirección
        }
    }
    else {
        medusaRotationY -= rotationSpeed * deltaTime;
        if (medusaRotationY <= minRotation) {
            medusaRotationY = minRotation;
            medusaRotationDirection = true; // Cambiar dirección
        }
    }
}

void updateCameraAndCharacter(Camera& camera, glm::vec3& characterPosition, bool* keys, GLfloat deltaTime) {
    // Límites del terreno escalado
    const float minX = -36.0f;
    const float maxX = 36.0f;
    const float minZ = -20.0f;
    const float maxZ = 20.0f;

    static bool vKeyPressed = false;

    switch (currentCameraMode) {
    case FIRST_PERSON:
        // La cámara está en la posición del personaje
        camera.setPosition(characterPosition + glm::vec3(0.0f, 0.3f, 0.25f)); // Altura de los ojos
        camera.update();

        // Movimiento del personaje
        if (keys[GLFW_KEY_W]) characterPosition += glm::vec3(camera.getFront().x, 0.0f, camera.getFront().z) * deltaTime * 0.1f; // Adelante
        if (keys[GLFW_KEY_S]) characterPosition -= glm::vec3(camera.getFront().x, 0.0f, camera.getFront().z) * deltaTime * 0.1f; // Atrás
        if (keys[GLFW_KEY_A]) characterPosition -= glm::vec3(camera.getRight().x, 0.0f, camera.getRight().z) * deltaTime * 0.1f; // Izquierda
        if (keys[GLFW_KEY_D]) characterPosition += glm::vec3(camera.getRight().x, 0.0f, camera.getRight().z) * deltaTime * 0.1f; // Derecha

        // Restringir la posición dentro de los límites
        characterPosition.x = glm::clamp(characterPosition.x, minX, maxX);
        characterPosition.z = glm::clamp(characterPosition.z, minZ, maxZ);

        // Fijar la posición en el eje Y
        characterPosition.y = 0.0f;
        break;

    case THIRD_PERSON:
        // Posiciona la cámara detrás y arriba del personaje
        camera.setPosition(characterPosition - glm::vec3(3.0f, 0.0f, 0.0f) + glm::vec3(0.0f, 2.0f, 0.0f));
        camera.setFront(glm::normalize(characterPosition - camera.getPosition()));
        camera.update();

        // Movimiento del personaje y ajuste de rotación
        if (keys[GLFW_KEY_W]) {
            characterPosition += glm::vec3(1.0f, 0.0f, 0.0f) * deltaTime * 0.1f; // Adelante
            characterRotationY = 0.0f; // Apunta hacia el lado X positivo
        }
        if (keys[GLFW_KEY_S]) {
            characterPosition -= glm::vec3(1.0f, 0.0f, 0.0f) * deltaTime * 0.1f; // Atrás
            characterRotationY = 180.0f; // Apunta hacia el lado X negativo
        }
        if (keys[GLFW_KEY_A]) {
            characterPosition -= glm::vec3(0.0f, 0.0f, 1.0f) * deltaTime * 0.1f; // Izquierda
            characterRotationY = 90.0f; // Apunta hacia el lado Z negativo
        }
        if (keys[GLFW_KEY_D]) {
            characterPosition += glm::vec3(0.0f, 0.0f, 1.0f) * deltaTime * 0.1f; // Derecha
            characterRotationY = -90.0f; // Apunta hacia el lado Z positivo
        }

        // Restringir la posición dentro de los límites
        characterPosition.x = glm::clamp(characterPosition.x, minX, maxX);
        characterPosition.z = glm::clamp(characterPosition.z, minZ, maxZ);

        // Fijar la posición en el eje Y
        characterPosition.y = 0.0f;
        break;

    case OVERHEAD:
        // La cámara está fija en una posición elevada
        camera.setPosition(glm::vec3(0.0f, 60.0f, 0.0f)); // Vista desde arriba
        camera.setYaw(-90.0f); // Mirando hacia abajo
        camera.setPitch(-89.0f);
        camera.update();

        // Bloquear movimiento del personaje
        break;

    case STATIC_CAMERA: {
        static bool vKeyPressed = false; // Detectar una sola pulsación de la tecla V
        static bool firstPress = true;  // Controlar si es la primera vez que se presiona V

        if (keys[GLFW_KEY_V]) {
            if (!vKeyPressed) { // Solo ejecutar una vez por pulsación
                vKeyPressed = true;

                if (!firstPress) {
                    // Apagar las luces del juego anterior
                    discoLightActive[index] = false;

                    // Incrementar el índice y reiniciar a 0 si llega al final
                    index = (index + 1) % 6;
                }
                else {
                    firstPress = false; // Desactivar el control de primera pulsación
                }

                // Encender las luces del juego actual
                discoLightActive[index] = true;
            }
        }
        else {
            vKeyPressed = false; // Reiniciar el estado de la tecla V
        }

        // Configurar la posición y orientación de la cámara según el índice actual
        camera.setPosition(positionsGames[index]);
        camera.setYaw(valueYawGames[index]);
        camera.setPitch(valuePitchGames[index]);
        camera.update();
        break;
    }
    }
}


void UpdateRuedaAnimation(float deltaTime, glm::vec3& characterPosition) {
    switch (ruedaState) {
    case 0: // Estado inicial
        ruedaRotationX = 0.0f; // Asegurar que la rotación esté en 0 grados
        asientosRotationX = 0.0f; // Reiniciar la rotación de los asientos
        if (isAnimatingRueda) {
            isAnimationActive = true; // Activar la bandera de animación
            index = 1;
			positionAux = translateWhileAnimationIsActive[index]; // Guardar la posición del personaje
			rotationAux = characterRotationY; // Guardar la rotación del personaje
			characterPosition = translateWhileAnimationIsActive[index]; // Actualizar la posición del personaje
			characterRotationY = rotateWhileAnimationIsActive[index]; // Actualizar la rotación del personaje
            currentCameraMode = STATIC_CAMERA; // Cambiar a cámara estática
            camera.setPosition(positionsGames[1]); // Posición del carrusel
            camera.setYaw(valueYawGames[1]); // Yaw del carrusel
            camera.setPitch(valuePitchGames[1]); // Pitch del carrusel
            camera.update(); // Actualizar la cámara
            ruedaState = 1; // Cambiar al estado "girando"
        }
        break;

    case 1: // Girando
        ruedaRotationX += ruedaSpeed * deltaTime; // Incrementar el ángulo de rotación de la rueda
        if (ruedaRotationX >= 360.0f) { // Si completa un giro completo
            ruedaRotationX = 360.0f; // Asegurarse de que no exceda el límite
            ruedaState = 2; // Cambiar al estado "regreso al estado inicial"
        }

        // Actualizar la rotación de los asientos
        asientosRotationX += asientosDirection * asientosSpeed * deltaTime;
        if (asientosRotationX >= 5.0f) { // Si alcanza el límite superior
            asientosRotationX = 5.0f;
            asientosDirection = -1; // Cambiar la dirección
        }
        else if (asientosRotationX <= -5.0f) { // Si alcanza el límite inferior
            asientosRotationX = -5.0f;
            asientosDirection = 1; // Cambiar la dirección
        }
        break;

    case 2: // Regreso al estado inicial
        ruedaRotationX = 0.0f; // Reiniciar el ángulo de rotación de la rueda
        asientosRotationX = 0.0f; // Reiniciar la rotación de los asientos
        isAnimatingRueda = false; // Detener la animación
        ruedaState = 0; // Volver al estado inicial
		discoLightActive[2] = false; // Desactivar la luz de discoteca de la rueda
		isAnimationActive = false; // Desactivar la bandera de animación
		characterPosition = positionAux; // Volver a la posición original del personaje
		characterRotationY = rotationAux; // Volver a la rotación original del personaje
		currentCameraMode = FIRST_PERSON; // Cambiar a cámara en primera persona
        break;
    }
}

void UpdateCarouselAnimation(float deltaTime, glm::vec3& characterPosition) {
    switch (carouselState) {
    case 0: // Estado inicial
        if (isAnimatingCarousel) {
            positionAux = characterPosition;
			rotationAux = characterRotationY;
            isAnimationActive = true; // Activar la bandera de animación
            currentCameraMode = STATIC_CAMERA; // Cambiar a cámara estática
            index = 0; // Índice del carrusel
            camera.setPosition(positionsGames[index]); // Posición del carrusel
            camera.setYaw(valueYawGames[index]); // Yaw del carrusel
            camera.setPitch(valuePitchGames[index]); // Pitch del carrusel
            camera.update(); // Actualizar la cámara
            characterPosition = translateWhileAnimationIsActive[index]; // Actualizar la posición del personaje
			characterRotationY = rotateWhileAnimationIsActive[index]; // Actualizar la rotación del personaje
            carouselState = 1; // Cambiar al estado "girando"
            carouselRotationY = 0.0f; // Reiniciar el ángulo de rotación
        }
        break;

    case 1: // Girando
        carouselRotationY += 0.5f * deltaTime; // Incrementar el ángulo de rotación
        if (carouselRotationY >= 360.0f) { // Si completa un giro completo
            carouselRotationY = 360.0f; // Asegurarse de que no exceda el límite
            carouselState = 2; // Cambiar al estado "regreso al estado inicial"
        }
        break;

    case 2: // Regreso al estado inicial
        carouselRotationY = 0.0f; // Reiniciar el ángulo de rotación
        isAnimatingCarousel = false; // Detener la animación
        isAnimationActive = false; // Desactivar la bandera de animación
        currentCameraMode = FIRST_PERSON; // Cambiar a cámara en primera persona
        discoLightActive[1] = false; // Desactivar la luz de discoteca del carrusel
		characterPosition = positionAux; // Volver a la posición original del personaje
		characterRotationY = rotationAux; // Volver a la rotación original del personaje
        carouselState = 0; // Volver al estado inicial
        break;
    }
}

void UpdateBowlingAnimation(float deltaTime, glm::vec3& characterPosition) {
    switch (bowlingState) {
    case 0: // Estado inicial
        if (isAnimatingBowling) {
            isAnimationActive = true; // Activar la bandera de animación
            currentCameraMode = STATIC_CAMERA; // Cambiar a cámara estática
            index = 4; // Índice del boliche
            camera.setPosition(positionsGames[index]); // Posición del boliche
            camera.setYaw(valueYawGames[index]); // Yaw del boliche
            camera.setPitch(valuePitchGames[index]); // Pitch del boliche
            camera.update(); // Actualizar la cámara
			positionAux = characterPosition;
			rotationAux = characterRotationY; // Guardar la rotación del personaje
			characterPosition = translateWhileAnimationIsActive[index]; // Actualizar la posición del personaje
			characterRotationY = rotateWhileAnimationIsActive[index]; // Actualizar la rotación del personaje

            bowlingState = 1; // Cambiar al estado "avanzando"
        }
        break;

    case 1: // Avanzando
        bowlingPositionX += bowlingSpeed * deltaTime; // Incrementar la posición en X
        if (bowlingPositionX >= 1.5f) { // Si alcanza el límite
            bowlingPositionX = 1.5f; // Asegurarse de que no exceda el límite
            bowlingState = 2; // Cambiar al estado "regresando"
        }
        break;

    case 2: // Regresando
        bowlingPositionX -= bowlingSpeed * deltaTime; // Decrementar la posición en X
        if (bowlingPositionX <= -4.5f) { // Si regresa a la posición inicial
            bowlingPositionX = -4.5f; // Asegurarse de que no sea menor que el límite
            bowlingState = 0; // Volver al estado inicial
            isAnimatingBowling = false; // Detener la animación
            isAnimationActive = false; // Desactivar la bandera de animación
            currentCameraMode = FIRST_PERSON; // Cambiar a cámara en primera persona
			characterPosition = positionAux; // Volver a la posición original del personaje
			characterRotationY = rotationAux; // Volver a la rotación original del personaje
            discoLightActive[0] = false; // Desactivar la luz de discoteca del boliche
        }
        break;
    }
}

void UpdateDartAndBalloonsAnimation(float deltaTime, glm::vec3& characterPosition) {
    if (isDartAnimating) {
        switch (dartState) {
        case 0: // Estado inicial
            isAnimationActive = true;
            currentCameraMode = STATIC_CAMERA; // Cambiar a cámara estática
            index = 5;
            camera.setPosition(positionsGames[index]); // Posición del dardo
            camera.setYaw(valueYawGames[index]); // Yaw del dardo
            camera.setPitch(valuePitchGames[index]); // Pitch del dardo
            camera.update(); // Actualizar la cámara

            positionAux = characterPosition; // Guardar la posición del personaje
            rotationAux = characterRotationY; // Guardar la rotación del personaje
            characterPosition = translateWhileAnimationIsActive[index]; // Actualizar la posición del personaje
            characterRotationY = rotateWhileAnimationIsActive[index]; // Actualizar la rotación del personaje

            dartPositionX = 0.0f; // Asegurar posición inicial
            areBalloonsVisible = true; // Asegurar que los globos sean visibles
            discoLightActive[5] = true; // Activar la luz del dardo
            dartState = 1; // Cambiar al estado "avanzando"
            break;

        case 1: // Avanzando
            dartPositionX -= dartSpeed * deltaTime; // Mover el dardo hacia adelante
            if (dartPositionX <= -2.0f) { // Si el dardo alcanza las 2 unidades
                dartPositionX = -2.0f; // Limitar la posición
                areBalloonsVisible = false; // Ocultar los globos
                dartState = 2; // Cambiar al estado "regresando"
            }
            break;

        case 2: // Regresando
            dartPositionX += dartSpeed * deltaTime; // Mover el dardo hacia atrás
            if (dartPositionX >= 0.0f) { // Si el dardo regresa a su posición inicial
                dartPositionX = 0.0f; // Asegurar posición inicial
                dartState = 3; // Cambiar al estado "finalizando"
            }
            break;

        case 3: // Finalizando
            areBalloonsVisible = true; // Mostrar los globos
            characterPosition = positionAux; // Volver a la posición original del personaje
            characterRotationY = rotationAux; // Volver a la rotación original del personaje
            currentCameraMode = FIRST_PERSON; // Cambiar a cámara en primera persona
            discoLightActive[5] = false;
            isAnimationActive = false;
            isDartAnimating = false; // Finalizar la animación
            dartState = 0; // Volver al estado inicial
            break;
        }
    }
}

void UpdateBaseballAnimation(float deltaTime, glm::vec3& characterPosition) {
    float moveDuration = 60.0f; // Duración del traslado en segundos
    switch (baseballState) {
    case 0: // Estado inicial
        if (isAnimatingBaseball) {
            index = 3;
            currentCameraMode = STATIC_CAMERA; // Cambiar a cámara estática
            camera.setPosition(positionsGames[3]); // Posición del carrusel
            camera.setYaw(valueYawGames[3]); // Yaw del carrusel
            camera.setPitch(valuePitchGames[3]); // Pitch del carrusel
			camera.update(); // Actualizar la cámara
			positionAux = characterPosition; // Guardar la posición del personaje
			rotationAux = characterRotationY; // Guardar la rotación del personaje
			characterPosition = translateWhileAnimationIsActive[index]; // Actualizar la posición del personaje
			characterRotationY = rotateWhileAnimationIsActive[index]; // Actualizar la rotación del personaje
            baseballState = 1; // Cambiar al estado "girando el bat"
        }
        break;

    case 1: // Girando el bat
        batRotationY += baseballSpeed * deltaTime; // Incrementar el ángulo del bat
        if (batRotationY >= 90.0f) { // Si el bat alcanza 90 grados
            batRotationY = 90.0f; // Asegurarse de que no exceda el límite
            baseballState = 2; // Cambiar al estado "moviendo la pelota"
            baseballAnimationTime = 0.0f; // Reiniciar el tiempo acumulado
        }
        break;

    case 2: { // Moviendo la pelota hacia adelante
        baseballAnimationTime += deltaTime; // Incrementar el tiempo acumulado
        float t = baseballAnimationTime / moveDuration; // Progreso de la animación (0.0 a 1.0)
        baseballPositionX = glm::mix(0.0f, 12.0f, t); // Interpolación lineal entre 0 y 12 unidades

        // Si la pelota alcanza 5 o 12 unidades en X
        if (baseballPositionX >= 12.0f) {
            baseballPositionX = 12.0f; // Asegurarse de que no exceda el límite
            baseballState = 3; // Cambiar al estado "regresando la pelota"

            baseballAnimationTime = 0.0f; // Reiniciar el tiempo acumulado
        }
        break;
    }

    case 3: { // Regresando la pelota a su posición inicial
        baseballAnimationTime += deltaTime; // Incrementar el tiempo acumulado
        float t = baseballAnimationTime / moveDuration; // Progreso de la animación (0.0 a 1.0)
        baseballPositionX = glm::mix(baseballPositionX, 0.0f, t); // Interpolación lineal hacia 0 unidades
        if (baseballAnimationTime >= moveDuration) { // Si se completa la duración
            baseballPositionX = 0.0f; // Asegurarse de que no sea menor que el límite
            baseballState = 4; // Cambiar al estado "regresando el bat"
        }
        break;
    }

    case 4: // Regresando el bat a su posición inicial
        batRotationY -= baseballSpeed * deltaTime; // Decrementar el ángulo del bat
        if (batRotationY <= 0.0f) { // Si el bat regresa a su posición inicial
            batRotationY = 0.0f; // Asegurarse de que no sea menor que el límite
            baseballState = 0; // Volver al estado inicial
            isAnimatingBaseball = false; // Detener la animación
            characterPosition = positionAux; // Volver a la posición original del personaje
            characterRotationY = rotationAux; // Volver a la rotación original del personaje
        }
        isAnimationActive = false; // Desactivar la bandera de animación
        currentCameraMode = FIRST_PERSON; // Cambiar a cámara en primera persona
		discoLightActive[4] = false; // Desactivar la luz de discoteca del beisbol
        break;
    }
}

//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void UpdateTopo1Animation(float deltaTime, glm::vec3& characterPosition) {
    float moveSpeed = 0.001f; // Velocidad de movimiento en Y (unidades por segundo)
    float rotationSpeed = 2.0f; // Velocidad de rotación (grados por segundo)
    float maxHeight = -0.4f; // Altura máxima (Estado 2)
    float minHeight = -0.5f; // Altura mínima (Estado 1)

    switch (topo1State) {
    case 0: // Estado 1: Inicial
        topo1PositionY = minHeight; // Asegurar posición inicial en Y
        topo1RotationY = 0.0f; // Asegurar rotación inicial
        if (isAnimatingTopo) { // Si la animación está 
            index = 2;
            currentCameraMode = STATIC_CAMERA; // Cambiar a cámara estática
            camera.setPosition(positionsGames[2]); // Posición del carrusel
            camera.setYaw(valueYawGames[2]); // Yaw del carrusel
            camera.setPitch(valuePitchGames[2]); // Pitch del carrusel
			camera.update(); // Actualizar la cámara
			positionAux = characterPosition; // Guardar la posición del personaje
			rotationAux = characterRotationY; // Guardar la rotación del personaje
			characterPosition = translateWhileAnimationIsActive[index]; // Actualizar la posición del personaje
			characterRotationY = rotateWhileAnimationIsActive[index]; // Actualizar la rotación del personaje
            isAnimationActive = true;

            topo1State = 1; // Cambiar al Estado 2
        }
        break;

    case 1: // Estado 2: Subiendo y rotando
        topo1PositionY += moveSpeed * deltaTime; // Incrementar posición en Y
        topo1RotationY += rotationSpeed * deltaTime; // Incrementar rotación en Y

        // Limitar posición y rotación
        if (topo1PositionY >= maxHeight) topo1PositionY = maxHeight;
        if (topo1RotationY >= 180.0f) topo1RotationY = 180.0f;

        // Si alcanza la posición y rotación deseadas
        if (topo1PositionY == maxHeight && topo1RotationY == 180.0f) {
            topo1TimeAccumulator = 0.0f; // Reiniciar el tiempo acumulado
            topo1State = 2; // Cambiar al Estado 3
        }
        break;

    case 2: // Estado 3: Pausa en la posición máxima
        topo1TimeAccumulator += deltaTime; // Acumular tiempo
        if (topo1TimeAccumulator >= 1.0f) { // Si ha pasado 1 segundo
            topo1TimeAccumulator = 0.0f; // Reiniciar el tiempo acumulado
            topo1State = 3; // Cambiar al Estado 4 (regresando al estado inicial)
        }
        break;

    case 3: // Estado 4: Regresando al estado inicial
        topo1PositionY -= moveSpeed * deltaTime; // Decrementar posición en Y
        topo1RotationY -= rotationSpeed * deltaTime; // Decrementar rotación en Y

        // Limitar posición y rotación
        if (topo1PositionY <= minHeight) topo1PositionY = minHeight;
        if (topo1RotationY <= 0.0f) topo1RotationY = 0.0f;

        // Si regresa a la posición y rotación inicial
        if (topo1PositionY == minHeight && topo1RotationY == 0.0f) {
            topo1State = 0; // Volver al Estado 1
            isAnimationActive = false; // Desactivar la bandera de animación
            currentCameraMode = FIRST_PERSON; // Cambiar a cámara en primera persona
			characterPosition = positionAux; // Volver a la posición original del personaje
            characterRotationY = rotationAux; // Volver a la rotación original del personaje
            isAnimatingTopo = false; // Detener la animación
        }
        discoLightActive[3] = false;

        break;
    }
}

void UpdateTopo2Animation(float deltaTime) {
    float moveSpeed = 0.004f; // Velocidad de movimiento en Y (unidades por segundo)
    float rotationSpeed = 2.0f; // Velocidad de rotación (grados por segundo)
    float maxHeight = -0.5f; // Altura máxima (Estado 2)
    float minHeight = -0.6f; // Altura mínima (Estado 1)

    switch (topo2State) {
    case 0: // Estado 1: Inicial
        topo2PositionY = minHeight; // Asegurar posición inicial en Y
        topo2RotationY = 0.0f; // Asegurar rotación inicial
        if (isAnimatingTopo) { // Si la animación está activa
            topo2State = 1; // Cambiar al Estado 2
        }
        break;

    case 1: // Estado 2: Subiendo y rotando
        topo2PositionY += moveSpeed * deltaTime; // Incrementar posición en Y
        topo2RotationY += rotationSpeed * deltaTime; // Incrementar rotación en Y

        // Limitar posición y rotación
        if (topo2PositionY >= maxHeight) topo2PositionY = maxHeight;
        if (topo2RotationY >= 180.0f) topo2RotationY = 180.0f;

        // Si alcanza la posición y rotación deseadas
        if (topo2PositionY == maxHeight && topo2RotationY == 180.0f) {
            topo2TimeAccumulator = 0.0f; // Reiniciar el tiempo acumulado
            topo2State = 2; // Cambiar al Estado 3
        }
        break;

    case 2: // Estado 3: Pausa en la posición máxima
        topo2TimeAccumulator += deltaTime; // Acumular tiempo
        if (topo2TimeAccumulator >= 1.0f) { // Si ha pasado 1 segundo
            topo2TimeAccumulator = 0.0f; // Reiniciar el tiempo acumulado
            topo2State = 3; // Cambiar al Estado 4 (regresando al estado inicial)
        }
        break;

    case 3: // Estado 4: Regresando al estado inicial
        topo2PositionY -= moveSpeed * deltaTime; // Decrementar posición en Y
        topo2RotationY -= rotationSpeed * deltaTime; // Decrementar rotación en Y

        // Limitar posición y rotación
        if (topo2PositionY <= minHeight) topo2PositionY = minHeight;
        if (topo2RotationY <= 0.0f) topo2RotationY = 0.0f;

        // Si regresa a la posición y rotación inicial
        if (topo2PositionY == minHeight && topo2RotationY == 0.0f) {
            topo2State = 0; // Volver al Estado 1
            isAnimatingTopo = false; // Detener la animación
        }
        break;
    }
}

void UpdateDiscoAttractionLights(float deltaTime) {
    float colorChangeInterval = 15.0f; // Cambia de color cada 1 segundo

    for (int i = 0; i < 6; ++i) {
        if (discoLightActive[i]) {
            discoLightTimer[i] += deltaTime;
            if (discoLightTimer[i] >= colorChangeInterval) {
                discoLightColorIndex[i] = (discoLightColorIndex[i] + 1) % 3;
                discoLightTimer[i] = 0.0f;
            }
            // Cambia el color de la luz en el arreglo pointLights
            int idx = IDX_BOLICHE + i;
            pointLights[idx].SetColor(
                discoColors[discoLightColorIndex[i]].r,
                discoColors[discoLightColorIndex[i]].g,
                discoColors[discoLightColorIndex[i]].b
            );
        }
    }
}

void UpdateBrazoAnimation(float deltaTime) {
    if (isAnimatingCarousel || isAnimatingRueda || isDartAnimating) { // Solo animar si hay una animación activa
        brazoAnimationTime += deltaTime;

        // Calcular el progreso de la animación (0.0 a 1.0)
        float t = brazoAnimationTime / brazoAnimationDuration;

        if (brazoIncreasing) {
            // Interpolar entre el ángulo mínimo y máximo
            brazoCurrentAngle = glm::mix(brazoMinAngle, brazoMaxAngle, t);
            if (brazoAnimationTime >= brazoAnimationDuration) {
                brazoAnimationTime = 0.0f; // Reiniciar el tiempo acumulado
                brazoIncreasing = false;  // Cambiar la dirección
            }
        }
        else {
            // Interpolar entre el ángulo máximo y mínimo
            brazoCurrentAngle = glm::mix(brazoMaxAngle, brazoMinAngle, t);
            if (brazoAnimationTime >= brazoAnimationDuration) {
                brazoAnimationTime = 0.0f; // Reiniciar el tiempo acumulado
                brazoIncreasing = true;   // Cambiar la dirección
            }
        }
    }
}

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

    GLfloat floorVertices[] = {
     -6.0f, 0.0f, -5.0f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f, // Esquina inferior izquierda
      6.0f, 0.0f, -5.0f,  10.0f, 0.0f,  0.0f, -1.0f, 0.0f, // Esquina inferior derecha
     -6.0f, 0.0f,  5.0f,  0.0f, 10.0f,  0.0f, -1.0f, 0.0f, // Esquina superior izquierda
      6.0f, 0.0f,  5.0f,  10.0f, 10.0f,  0.0f, -1.0f, 0.0f  // Esquina superior derecha
    };

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

bool IsNear(glm::vec3 position1, glm::vec3 position2, float threshold) {
    return glm::distance(position1, position2) < threshold;
}

void UpdateHandAnimation(float deltaTime) {
    if (isAnimatingBowling ||  isAnimatingBaseball || isAnimatingTopo) { // Solo animar si la animación está activa
        handAnimationTime += deltaTime;

        // Calcular el progreso de la animación (0.0 a 1.0)
        float t = handAnimationTime / handAnimationDuration;

        if (handIncreasing) {
            // Interpolar entre el ángulo mínimo y máximo
            handCurrentAngle = glm::mix(handMinAngle, handMaxAngle, t);
            if (handAnimationTime >= handAnimationDuration) {
                handAnimationTime = 0.0f; // Reiniciar el tiempo acumulado
                handIncreasing = false;  // Cambiar la dirección
            }
        }
        else {
            // Interpolar entre el ángulo máximo y mínimo
            handCurrentAngle = glm::mix(handMaxAngle, handMinAngle, t);
            if (handAnimationTime >= handAnimationDuration) {
                handAnimationTime = 0.0f; // Reiniciar el tiempo acumulado
                handIncreasing = true;   // Cambiar la dirección
            }
        }
    }
}

int main()
{
    mainWindow = Window(1280, 1024); // 1280, 1024 or 1024, 768
    mainWindow.Initialise();

    CreateObjects();
    CreateShaders();

    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.1f, 0.5f);

    brickTexture = Texture("Textures/brick.png");
    brickTexture.LoadTextureA();
    dirtTexture = Texture("Textures/dirt.png");
    dirtTexture.LoadTextureA();
    plainTexture = Texture("Textures/plain.png");
    plainTexture.LoadTextureA();
    pisoTexture = Texture("Textures/cesped.jpg");
    pisoTexture.LoadTextureA();

    // Personajes

    Cuerpo_M = Model();
    Cuerpo_M.LoadModel("Models/cuerpo.obj");

    BrazoD_M = Model();
    BrazoD_M.LoadModel("Models/brazoderecho.obj");

    BrazoI_M = Model();
    BrazoI_M.LoadModel("Models/brazoizquierdo.obj");

    PieD_M = Model();
    PieD_M.LoadModel("Models/piederecho.obj");

    PieI_M = Model();
    PieI_M.LoadModel("Models/pieizquierdo.obj");

    // Utils

    Coin_M = Model();
    Coin_M.LoadModel("Models/coin.obj");

    Lampara_M = Model();
    Lampara_M.LoadModel("Models/lampara.obj");

    Medusa_M = Model();
    Medusa_M.LoadModel("Models/medusa.obj");

    // Atracciones

    RuedaFortuna_M = Model();
    RuedaFortuna_M.LoadModel("Models/wheel.obj");

    BaseRueda_M = Model();
    BaseRueda_M.LoadModel("Models/base.obj");

    AsentosRueda_M = Model();
    AsentosRueda_M.LoadModel("Models/seatings.obj");

    GolpeaTopo_M = Model();
    GolpeaTopo_M.LoadModel("Models/topo.obj");

    Carousel_M = Model();
    Carousel_M.LoadModel("Models/carousel.obj");

    MonanhaRusa_M = Model();
    MonanhaRusa_M.LoadModel("Models/cancha.obj");

    Cancha_M = Model();
    Cancha_M.LoadModel("Models/saha.obj");

    Baseball_M = Model();
    Baseball_M.LoadModel("Models/baseball.obj");

    Bat_M = Model();
    Bat_M.LoadModel("Models/bat.obj");

    Caballo_M = Model();
    Caballo_M.LoadModel("Models/caballoprimario.obj");

    Tubos_M = Model();
    Tubos_M.LoadModel("Models/tubos.obj");

    TopoToy_M = Model();
    TopoToy_M.LoadModel("Models/topotoy.obj");

    LineBowling_M = Model();
    LineBowling_M.LoadModel("Models/Bowling-Alley.obj");

    Bowling_M = Model();
    Bowling_M.LoadModel("Models/bowling.obj");

    Pine_M = Model();
    Pine_M.LoadModel("Models/pine.obj");

	PuestoGlobos_M = Model();
	PuestoGlobos_M.LoadModel("Models/PuestoGlobos.obj");

	DardoGlobos_M = Model();
	DardoGlobos_M.LoadModel("Models/dardo.obj");

	Globo_M = Model();
	Globo_M.LoadModel("Models/GarfieldGlobo.obj");

    // Puestos de servicio

    HotDogs_M = Model();
    HotDogs_M.LoadModel("Models/hotdogs.obj");

    Pizza_M = Model();
    Pizza_M.LoadModel("Models/puesto2.obj");

    Hotdogs2_M = Model();
    Hotdogs2_M.LoadModel("Models/hotdogs2.obj");

    Palomitas_M = Model();
    Palomitas_M.LoadModel("Models/palomitas.obj");

    std::vector<std::string> dayFaces = {
        "Textures/Skybox/dia/cupertin-lake_rt.tga",
        "Textures/Skybox/dia/cupertin-lake_lf.tga",
        "Textures/Skybox/dia/cupertin-lake_dn.tga",
        "Textures/Skybox/dia/cupertin-lake_up.tga",
        "Textures/Skybox/dia/cupertin-lake_bk.tga",
        "Textures/Skybox/dia/cupertin-lake_ft.tga"
    };

    std::vector<std::string> nightFaces = {
        "Textures/Skybox/noche/cupertin-lake-night_rt.tga",
        "Textures/Skybox/noche/cupertin-lake-night_lf.tga",
        "Textures/Skybox/noche/cupertin-lake-night_dn.tga",
        "Textures/Skybox/noche/cupertin-lake-night_up.tga",
        "Textures/Skybox/noche/cupertin-lake-night_bk.tga",
        "Textures/Skybox/noche/cupertin-lake-night_ft.tga"
    };

    skybox = Skybox(dayFaces);

    Material_brillante = Material(4.0f, 256);
    Material_opaco = Material(0.3f, 4);

    // luz direccional, sólo 1 y siempre debe de existir
    mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
        dayLightIntensity, dayLightIntensity,
        0.0f, -1.0f, 0.0f);
    // contador de luces puntuales
    unsigned int pointLightCount = 0;
    
    // Inicializar las PointLights para las lámparas
    pointLights[0] = PointLight(1.0f, 1.0f, 0.8f,  // Color cálido
        0.0f, 1.0f,       // Intensidad ambiental y difusa
        11.0f, 4.0f, 9.0f, // Posición de la lámpara 1
        1.0f, 0.09f, 0.032f); // Atenuación
    pointLightCount++;

    pointLights[1] = PointLight(1.0f, 1.0f, 0.8f,  // Color cálido
        0.0f, 1.0f,       // Intensidad ambiental y difusa
        -1.0f, 4.0f, 5.0f, // Posición de la lámpara 2
        1.0f, 0.09f, 0.032f); // Atenuación

    pointLightCount++;

    pointLights[2] = PointLight(1.0f, 1.0f, 0.8f,  // Color cálido
        0.0f, 1.0f,       // Intensidad ambiental y difusa
        -20.0f, 4.0f, -6.0f, // Posición de la lámpara 3
        1.0f, 0.09f, 0.032f); // Atenuación

    pointLightCount++;

    pointLights[3] = PointLight(1.0f, 1.0f, 0.8f,  // Color cálido
        0.0f, 1.0f,       // Intensidad ambiental y difusa
        17.0f, 4.0f, -6.5f, // Posición de la lámpara 4
        1.0f, 0.09f, 0.032f); // Atenuación

    pointLightCount++;

    pointLights[4] = PointLight(
        1.0f, 0.0f, 1.0f,  // Color púrpura (RGB)
        0.0f, 1.0f,        // Intensidad ambiental y difusa
        17.0f, 0.1f, 11.0f, // Posición inicial (coincide con la medusa)
        1.0f, 0.09f, 0.032f // Atenuación
    );

    pointLightCount++;


    pointLights[5] = PointLight(
        1.0f, 0.0f, 1.0f,  // Color púrpura (RGB)
        0.0f, 1.0f,        // Intensidad ambiental y difusa
        -12.0f, 0.1f, -0.1f, // Posición inicial (coincide con la medusa)
        1.0f, 0.09f, 0.032f // Atenuación
    );

    pointLightCount++;

    // Boliche
    pointLights[IDX_BOLICHE] = PointLight(
        1.0f, 0.0f, 0.0f, // Color inicial: rojo
        0.0f, 1.0f,       // Intensidad ambiental y difusa
        0.0f, 1.0f, -15.0f, // Posición sobre el boliche
        1.0f, 0.09f, 0.032f // Atenuación
    );
    // Carrusel
    pointLights[IDX_CARRUSEL] = PointLight(
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f,
        12.0f, 0.0f, -5.0f,
        1.0f, 0.09f, 0.032f
    );
    // Rueda de la fortuna
    pointLights[IDX_RUEDA] = PointLight(
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f,
        30.0f, 7.0f, 10.0f,
        1.0f, 0.09f, 0.032f
    );

    // Luz para el topo
    pointLights[IDX_TOPO] = PointLight(
        1.0f, 0.0f, 0.0f, // Color inicial: rojo
        0.0f, 1.0f,       // Intensidad ambiental y difusa
        0.0f, 0.0f, 15.0f, // Posición sobre el topo
        1.0f, 0.09f, 0.032f // Atenuación
    );

    // Luz para el béisbol
    pointLights[IDX_BEISBOL] = PointLight(
        1.0f, 0.0f, 0.0f, // Color inicial: rojo
        0.0f, 1.0f,       // Intensidad ambiental y difusa
        -2.0f, 0.0f, -5.0f, // Posición sobre el béisbol
        1.0f, 0.09f, 0.032f // Atenuación
    );

    // Luz para el béisbol
    pointLights[IDX_DARDO] = PointLight(
        1.0f, 0.0f, 0.0f, // Color inicial: rojo
        0.0f, 1.0f,       // Intensidad ambiental y difusa
        12.0f, 0.0f, 15.0f, // Posición sobre el béisbol
        1.0f, 0.09f, 0.032f // Atenuación
    );

    
    // se crean mas luces puntuales y spotlight 

    GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
        uniformSpecularIntensity = 0, uniformShininess = 0;
    GLuint uniformColor = 0;
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

    glm::vec3 characterPosition = glm::vec3(0.0f, 0.0f, 0.0f); // Posición inicial del personaje

    ////Loop mientras no se cierra la ventana
    while (!mainWindow.getShouldClose())
    {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        deltaTime += (now - lastTime) / limitFPS;
        lastTime = now;

        angleCaballo1 += deltaTime;
        angleCaballo2 += deltaTime;
        angulovaria += deltaTime;


        if (isAnimationActive) {
            currentCameraMode = STATIC_CAMERA;
        }
        else {
            if (mainWindow.getsKeys()[GLFW_KEY_B]) {
                currentCameraMode = FIRST_PERSON;
                index = 0;
            }
            if (mainWindow.getsKeys()[GLFW_KEY_N]) {
                currentCameraMode = THIRD_PERSON;
                index = 0;
            }
            if (mainWindow.getsKeys()[GLFW_KEY_M]) {
                currentCameraMode = OVERHEAD;
                index = 0;
            }
            if (mainWindow.getsKeys()[GLFW_KEY_V]) {
                currentCameraMode = STATIC_CAMERA;
            }
        }

        // Detectar si el personaje está caminando
        isWalking = mainWindow.getsKeys()[GLFW_KEY_W] || mainWindow.getsKeys()[GLFW_KEY_A] ||
            mainWindow.getsKeys()[GLFW_KEY_S] || mainWindow.getsKeys()[GLFW_KEY_D];

        if (isWalking) {
            walkAnimationTime += deltaTime * 0.1f; // Incrementa el tiempo de animación (ajusta la velocidad con el factor 5.0f)

            // Oscilar los ángulos de rotación entre -3 y 3 grados
            brazoD_RotationX = 10.0f * sin(walkAnimationTime);
            brazoI_RotationX = -10.0f * sin(walkAnimationTime);
            pieD_RotationX = -10.0f * sin(walkAnimationTime);
            pieI_RotationX = 10.0f * sin(walkAnimationTime);
        }

        if (IsNear(characterPosition, glm::vec3(-5.5f, 0.0f, -15.0f), proximityThreshold)) {
            if (mainWindow.getsKeys()[GLFW_KEY_L]) {
                if (!isAnimatingBowling) { // Solo activar si no está animando
                    isAnimatingBowling = true;
					discoLightActive[0] = true; // Activar la luz del boliche
                    bowlingState = 0; // Iniciar desde el estado inicial
                }
            }
        }

        if (IsNear(characterPosition, glm::vec3(12.0f, 0.0f, -5.0f), proximityThreshold)) {
            if (mainWindow.getsKeys()[GLFW_KEY_K]) {
                if (!isAnimatingCarousel) { // Solo activar si no está animando
                    isAnimatingCarousel = true;
					discoLightActive[1] = true; // Activar la luz del carrusel
                    carouselState = 0; // Iniciar desde el estado inicial
                }
            }
        }


        if (IsNear(characterPosition, glm::vec3(-2.3f, 0.0f, -5.0f), proximityThreshold)) {
            if (mainWindow.getsKeys()[GLFW_KEY_J]) {
                if (!isAnimatingBaseball) { // Solo activar si no está animando
                    isAnimatingBaseball = true;
                    baseballState = 0; // Iniciar desde el estado inicial
                    discoLightActive[4] = true;
                }
            }

        }

        if (IsNear(characterPosition, glm::vec3(-0.1, 0.0f, 15.13f), proximityThreshold)) {
            if (mainWindow.getsKeys()[GLFW_KEY_H]) { 
                if (!isAnimatingTopo) {
                    isAnimatingTopo = true;
                    topo1State = 0; // Iniciar desde el estado inicial
                    topo2State = 0; // Iniciar desde el estado inicial
					discoLightActive[3] = true; // Activar la luz del topo
                }
            }
        }


        if (IsNear(characterPosition, glm::vec3(25.0f, 0.0f, 10.0f), proximityThreshold)) {
            if (mainWindow.getsKeys()[GLFW_KEY_G]) { // Presionar 'G' para activar la animación
                if (!isAnimatingRueda) { // Solo activar si no está animando
                    isAnimatingRueda = true;
					discoLightActive[2] = true; // Activar la luz de la rueda de la fortuna
                    ruedaState = 0; // Iniciar desde el estado inicial
                }
            }
        }

        if (IsNear(characterPosition, glm::vec3(15.0f, 0.0f, 15.0f), proximityThreshold)) {
            if (mainWindow.getsKeys()[GLFW_KEY_F]) { // Presionar 'G' para activar la animación
                if (!isDartAnimating) { // Solo activar si no está animando
                    isDartAnimating = true;
                    discoLightActive[5] = true; // Activar la luz de la rueda de la fortuna
                    dartState = 0; // Iniciar desde el estado inicial
                }
            }
        }

        // Incrementar el temporizador
        dayNightTimer += deltaTime;

        // Alternar entre día y noche cada 60 segundos
        if (dayNightTimer >= 3600.0f) {
            dayNightTimer = 0.0f; // Reiniciar el temporizador
            isDay = !isDay; // Alternar entre día y noche

            // Cambiar las caras del skybox y la luz direccional
            if (isDay) {
                skybox = Skybox(dayFaces); // Cambiar a las caras del día
                mainLight.SetDirection(0.0f, -1.0f, 0.0f); // Luz desde arriba hacia abajo
                mainLight.SetIntensity(0.3, 0.6); // Luz intensa
            }
            else {
                skybox = Skybox(nightFaces); // Cambiar a las caras de la noche
                mainLight.SetDirection(0.0f, -1.0f, 0.0f); // Mantener la dirección
                mainLight.SetIntensity(0.1, 0.3); // Luz tenue
            }
        }

        // Detectar si la bola de boliche alcanza la posición 20.0f en X
        if (bowlingPositionX >= -0.1f && !pinsKnockedOver) {
            pinsKnockedOver = true; // Marcar los pinos como derribados
            pinsAnimationTime = 0.0f; // Reiniciar el tiempo de animación
        }

        // Detectar si la bola de boliche regresa a su posición inicial
        if (bowlingPositionX <= -4.5f && pinsKnockedOver) {
            pinsKnockedOver = false; // Marcar los pinos como no derribados
            pinsReturningToNormal = true; // Iniciar el regreso a 0 grados
            pinsAnimationTime = 0.0f; // Reiniciar el tiempo de animación
        }

        // Animar los pinos de 0 a -90 grados
        if (pinsKnockedOver && pinsAnimationTime < pinsAnimationDuration) {
            pinsAnimationTime += deltaTime; // Incrementar el tiempo acumulado
            float t = pinsAnimationTime / pinsAnimationDuration; // Progreso de la animación (0.0 a 1.0)
            pinsRotationX = glm::mix(0.0f, -90.0f, t); // Interpolación lineal entre 0 y -90 grados
        }

        // Animar los pinos de -90 a 0 grados
        if (pinsReturningToNormal && pinsAnimationTime < pinsAnimationDuration) {
            pinsAnimationTime += deltaTime; // Incrementar el tiempo acumulado
            float t = pinsAnimationTime / pinsAnimationDuration; // Progreso de la animación (0.0 a 1.0)
            pinsRotationX = glm::mix(-90.0f, 0.0f, t); // Interpolación lineal entre -90 y 0 grados
            if (pinsAnimationTime >= pinsAnimationDuration) {
                pinsReturningToNormal = false; // Finalizar el regreso a 0 grados
            }
        }
        
        // Recibir eventos del usuario
        glfwPollEvents();
        camera.keyControl(mainWindow.getsKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

        // Clear the window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
        shaderList[0].UseShader();
        uniformModel = shaderList[0].GetModelLocation();
        uniformProjection = shaderList[0].GetProjectionLocation();
        uniformView = shaderList[0].GetViewLocation();
        uniformEyePosition = shaderList[0].GetEyePositionLocation();
        uniformColor = shaderList[0].getColorLocation();

        // información en el shader de intensidad especular y brillo
        uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
        uniformShininess = shaderList[0].GetShininessLocation();

        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

        // luz ligada a la cámara de tipo flash
        // sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
        glm::vec3 lowerLight = camera.getCameraPosition();
        lowerLight.y -= 0.3f;
        spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

        // información al shader de fuentes de iluminación
        shaderList[0].SetDirectionalLight(&mainLight);

        glm::mat4 model(1.0);
        glm::mat4 modelaux(1.0);
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(5.5f, 1.0f, 4.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));

        pisoTexture.UseTexture();
        Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

        meshList[2]->RenderMesh();

        // Actualizar cámara y personaje
        updateCameraAndCharacter(camera, characterPosition, mainWindow.getsKeys(), deltaTime);

        if (currentCameraMode != FIRST_PERSON) {
            // Renderizar el modelo de Rigby excepto en primera persona
                glm::mat4 model;

                // Renderizar el cuerpo
                model = glm::mat4(1.0f);
                model = glm::translate(model, characterPosition + glm::vec3(0.0f, characterVerticalPosition, 0.0f)); // Incluir la posición vertical
                model = glm::rotate(model, glm::radians(characterRotationY), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación en el eje Y
                glUniformMatrix4fv(shaderList[0].GetModelLocation(), 1, GL_FALSE, glm::value_ptr(model));
                Cuerpo_M.RenderModel();

                // Renderizar el brazo derecho
                model = glm::mat4(1.0f);
                model = glm::translate(model, characterPosition + glm::vec3(0.0f, characterVerticalPosition, 0.0f));
                model = glm::rotate(model, glm::radians(characterRotationY), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación general en Y
                model = glm::rotate(model, glm::radians(brazoD_RotationX), glm::vec3(0.0f, 0.0f, 1.0f)); // Animación en Z
                if (isAnimatingBaseball || isAnimatingTopo || isAnimatingBowling) {
                    model = glm::rotate(model, glm::radians(handCurrentAngle), glm::vec3(0.0f, 0.0f, 1.0f)); // Animación en Z
                }
                if (isAnimatingRueda || isAnimatingCarousel) {
                    model = glm::rotate(model, glm::radians(brazoCurrentAngle), glm::vec3(1.0f, 0.0f, 0.0f)); // Animación en X
                }
                glUniformMatrix4fv(shaderList[0].GetModelLocation(), 1, GL_FALSE, glm::value_ptr(model));
                BrazoD_M.RenderModel();

                // Renderizar el brazo izquierdo
                model = glm::mat4(1.0f);
                model = glm::translate(model, characterPosition + glm::vec3(0.0f, characterVerticalPosition, 0.0f));
                model = glm::rotate(model, glm::radians(characterRotationY), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación general en Y
                model = glm::rotate(model, glm::radians(brazoI_RotationX), glm::vec3(0.0f, 0.0f, 1.0f)); // Animación en Z
                if (isAnimatingBaseball || isAnimatingTopo || isAnimatingBowling) {
                    model = glm::rotate(model, glm::radians(handCurrentAngle), glm::vec3(0.0f, 0.0f, 1.0f)); // Animación en Z
                }
                if (isAnimatingRueda || isAnimatingCarousel ) {
                    model = glm::rotate(model, glm::radians(brazoCurrentAngle), glm::vec3(1.0f, 0.0f, 0.0f)); // Animación en X
                }
                glUniformMatrix4fv(shaderList[0].GetModelLocation(), 1, GL_FALSE, glm::value_ptr(model));
                BrazoI_M.RenderModel();

                // Renderizar el pie derecho
                model = glm::mat4(1.0f);
                model = glm::translate(model, characterPosition + glm::vec3(0.0f, characterVerticalPosition, 0.0f));
                model = glm::rotate(model, glm::radians(characterRotationY), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación general en Y
                model = glm::rotate(model, glm::radians(pieD_RotationX), glm::vec3(0.0f, 0.0f, 1.0f)); // Animación en Z
                glUniformMatrix4fv(shaderList[0].GetModelLocation(), 1, GL_FALSE, glm::value_ptr(model));
                PieD_M.RenderModel();

                // Renderizar el pie izquierdo
                model = glm::mat4(1.0f);
                model = glm::translate(model, characterPosition + glm::vec3(0.0f, characterVerticalPosition, 0.0f));
                model = glm::rotate(model, glm::radians(characterRotationY), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación general en Y
                model = glm::rotate(model, glm::radians(pieI_RotationX), glm::vec3(0.0f, 0.0f, 1.0f)); // Animación en Z
                glUniformMatrix4fv(shaderList[0].GetModelLocation(), 1, GL_FALSE, glm::value_ptr(model));
                PieI_M.RenderModel();
        }

        /*---------------------LLAMADA A FUNCIONES----------------------*/

        UpdateTopo1Animation(deltaTime, characterPosition);
        UpdateTopo2Animation(deltaTime);
        UpdateRuedaAnimation(deltaTime, characterPosition);
        UpdateCarouselAnimation(deltaTime, characterPosition);
        UpdateMedusaAnimation(deltaTime);
        UpdateMedusaRotation(deltaTime);
        UpdateBowlingAnimation(deltaTime, characterPosition);
        UpdateBaseballAnimation(deltaTime, characterPosition);
        UpdateDiscoAttractionLights(deltaTime);
        UpdateBrazoAnimation(deltaTime);
        UpdateJumpAnimation(deltaTime);
        UpdateHandAnimation(deltaTime);
		UpdateDartAndBalloonsAnimation(deltaTime, characterPosition);

        /*----------------RENDERIZADO DE LUCES DE JUEGOS MECANICOS--------------------*/

        unsigned int totalLights = 0; // Contador de luces activas
        if(isDay) {
            for (int i = 0; i < 6; ++i) {
                if (discoLightActive[i]) {
                    totalLights = std::max(totalLights, (unsigned int)(IDX_BOLICHE + i + 1));
                }
            }
            shaderList[0].SetPointLights(pointLights, totalLights);

        }
        else {
            shaderList[0].SetPointLights(pointLights, pointLightCount + totalLights);
        }

        /*---------------------Rueda de la fortuna-------------------------*/

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(30.0f, 6.0f, 10.0f)); // Posición de la rueda
        model = glm::rotate(model, glm::radians(ruedaRotationX), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación en el eje X
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        RuedaFortuna_M.RenderModel();

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(30.0f, 6.0f, 10.0f)); // Posición de los asientos
        model = glm::rotate(model, glm::radians(asientosRotationX), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación en el eje X
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        AsentosRueda_M.RenderModel();

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(30.0f, 6.0f, 10.0));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        BaseRueda_M.RenderModel();

        model = glm::translate(model, glm::vec3(-5.0f, -5.7f + sin(glm::radians(angulovaria)), 0.0f));
        Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Coin_M.RenderModel();

        /*------------------ NPC's -------------------------*/

        pointLights[4].setPosition(17.0f, 0.1f + sin(glm::radians(angulovaria)), 9.5f + medusaPositionZ);
        pointLights[5].setPosition(-12.0f, 0.1f + sin(glm::radians(angulovaria)), -2.5f + medusaPositionZ);

        model = glm::mat4(1.0);

        model = glm::translate(model, glm::vec3(17.0f, 0.1f + sin(glm::radians(angulovaria)), 11.0f + medusaPositionZ));
        model = glm::rotate(model, glm::radians(medusaRotationY), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación en el eje Y
        Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Medusa_M.RenderModel();

        model = glm::translate(model, glm::vec3(0.15f, 0.0f, -1.5f));
        Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Medusa_M.RenderModel();
        
        model = glm::mat4(1.0);

        model = glm::translate(model, glm::vec3(-12.0f, 0.1f + sin(glm::radians(angulovaria)), -1.5f + medusaPositionZ));
        model = glm::rotate(model, glm::radians(medusaRotationY), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación en el eje Y
        Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Medusa_M.RenderModel();

        model = glm::translate(model, glm::vec3(0.15f, 0.0f, -1.5f));
        Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Medusa_M.RenderModel();


        /*------------------ Puestos comida -------------------------*/

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-20.0f, 0.2f, -3.5f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Hotdogs2_M.RenderModel();

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(17.0f, -0.15f, -6.5f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Palomitas_M.RenderModel();

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 7.0f));
        model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f)); 
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Pizza_M.RenderModel();

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(12.0f, -0.5f, 7.0));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        HotDogs_M.RenderModel();

        /*------------------ GLOBOS CON DARDOS -------------------*/
        
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(12.0f, -0.5f, 15.0));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
        PuestoGlobos_M.RenderModel();

        if (isDartAnimating) {
            model = glm::mat4(1.0);
            model = glm::translate(model, glm::vec3(12.0f + dartPositionX, -0.5f, 15.0));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
            DardoGlobos_M.RenderModel();

            model = glm::mat4(1.0);
            model = glm::translate(model, glm::vec3(12.0f + dartPositionX, -0.5f, 15.6));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
            DardoGlobos_M.RenderModel();

            model = glm::mat4(1.0);
            model = glm::translate(model, glm::vec3(12.0f + dartPositionX, -0.5f, 14.4));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
            DardoGlobos_M.RenderModel();
        }

        if (areBalloonsVisible) {
            model = glm::mat4(1.0);
            model = glm::translate(model, glm::vec3(12.0f, -0.5f, 15.0f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
            Globo_M.RenderModel();

            model = glm::mat4(1.0);
            model = glm::translate(model, glm::vec3(12.0f, -0.5f, 14.3f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
            Globo_M.RenderModel();

            model = glm::mat4(1.0);
            model = glm::translate(model, glm::vec3(12.0f, -0.5f, 15.7f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
            Globo_M.RenderModel();

        }

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(15.0f, 1.5f + sin(glm::radians(angulovaria)), 15.0f));
        Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Coin_M.RenderModel();


        /*------------------ Lampararas -------------------------*/

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(12.0f, -1.0f, 9.0));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Lampara_M.RenderModel();

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 5.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Lampara_M.RenderModel();

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-20.0f, -1.0f, -7.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); 
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Lampara_M.RenderModel();

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(17.0f, -1.0f, -7.2f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Lampara_M.RenderModel();

        /*------------------ Golpea al topo ------------------------*/

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, -0.6f, 15.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        GolpeaTopo_M.RenderModel();

        // Topo 1 

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.03f, topo1PositionY, 15.18));
        model = glm::rotate(model, glm::radians(topo1RotationY), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación en el eje Y
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        TopoToy_M.RenderModel();

        // Topo 2
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.03f, 0.1 + topo2PositionY, 15.0));
        model = glm::rotate(model, glm::radians(topo2RotationY), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación en el eje Y
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        TopoToy_M.RenderModel();

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.03f, topo1PositionY, 14.83));
        model = glm::rotate(model, glm::radians(topo1RotationY), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación en el eje Y
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        TopoToy_M.RenderModel();

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-0.14f, topo2PositionY, 14.9));
        model = glm::rotate(model, glm::radians(topo2RotationY), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación en el eje Y
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        TopoToy_M.RenderModel();

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-0.14f, topo2PositionY, 15.13));
        model = glm::rotate(model, glm::radians(topo2RotationY), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación en el eje Y
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        TopoToy_M.RenderModel();

        model = glm::translate(model, glm::vec3(-0.1f, 1.5f + sin(glm::radians(angulovaria)), 0.0f));
        Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Coin_M.RenderModel();

        /*------------------Carousel-------------------------*/

        // Carousel

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(12.0f, -1.0f, -5.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Carousel_M.RenderModel();

        // Moneda de interaccion

        model = glm::translate(model, glm::vec3(-0.0f, 2.0f + sin(glm::radians(angulovaria)), 5.0f));
        Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Coin_M.RenderModel();

        // Tubo del Carousel
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(12.0f, -1.0f, -5.0f));
        model = glm::rotate(model, glm::radians(carouselRotationY), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación en el eje Y
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Tubos_M.RenderModel();

        // Caballo 1
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(12.0f, changeValueVCarousel(-0.8f + 0.1f * sin(glm::radians(angleCaballo1))), -5.0f));
        model = glm::rotate(model, glm::radians(carouselRotationY), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación en el eje Y
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Caballo_M.RenderModel();

        // Caballo 1
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(12.0f, changeValueVCarousel(-0.8f + 0.2f * sin(glm::radians(angleCaballo1))), -5.0f));
        model = glm::rotate(model, glm::radians(45.0f+carouselRotationY), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación en el eje Y
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Caballo_M.RenderModel();

        /*------------------Cancha de beisbol-------------------*/

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, 1.5f, -5.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Cancha_M.RenderModel();

        model = glm::translate(model, glm::vec3(-2.3f, -1.2f + sin(glm::radians(angulovaria)), 0.0f));
        Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Coin_M.RenderModel();


        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-2.0f + baseballPositionX, 0.0f + 0.1f * sin(glm::radians(angleBaseball)), -4.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Baseball_M.RenderModel();

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-2.2f, -0.6f, -5.0f)); // Posición del bat
        model = glm::rotate(model, glm::radians(batRotationY), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación en el eje Y
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Bat_M.RenderModel();

        /*----------------Bowling----------------------*/

        // Linea de boliche

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, -15.0f)); // Posición del bat
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        LineBowling_M.RenderModel();

        // Bola de boliche

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(bowlingPositionX, -0.85f, -15.0f)); 
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Bowling_M.RenderModel();

        		// Moneda de interaccion
		model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-4.7f, 0.3f + sin(glm::radians(angulovaria)), -15.0f));
        Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Coin_M.RenderModel();

        // Pinos
       
        // Pino 1
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, -0.9f, -14.9f)); // Posición del pino
        model = glm::rotate(model, glm::radians(pinsRotationX), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación en el eje X
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Pine_M.RenderModel();

        // Pino 2
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, -0.9f, -15.0f)); // Posición del pino
        model = glm::rotate(model, glm::radians(pinsRotationX), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación en el eje X
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Pine_M.RenderModel();

        // Pino 3
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, -0.9f, -15.1f)); // Posición del pino
        model = glm::rotate(model, glm::radians(pinsRotationX), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación en el eje X
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Pine_M.RenderModel();

        // Pino 4
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-0.1f, -0.9f, -14.95f)); // Posición del pino
        model = glm::rotate(model, glm::radians(pinsRotationX), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación en el eje X
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Pine_M.RenderModel();

        // Pino 5
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-0.1f, -0.9f, -15.05f)); // Posición del pino
        model = glm::rotate(model, glm::radians(pinsRotationX), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación en el eje X
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Pine_M.RenderModel();

        glUseProgram(0);

        mainWindow.swapBuffers();
    }

    return 0;
}