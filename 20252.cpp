/*---------------------------------------------------------*/
/* --------------------------------------------------------*/
/*-----------------    2025-2   ---------------------------*/
/*----------- Alumno: Luna Velazquez Said Josue -----------*/
/*------------- No. Cuenta    318128308     ---------------*/

#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h>						//main
#include <stdlib.h>		
#include <glm/glm.hpp>					//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>					//Texture
#include <cmath>

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>
#include <mmsystem.h>



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor* monitors;

GLuint VBO[3], VAO[3], EBO[3];

//Camera
Camera camera(glm::vec3(0.0f, 10.0f, 190.0f));
float MovementSpeed = 0.1f;
GLfloat lastX = SCR_WIDTH / 2.0f,
		lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//Timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
lastFrame = 0.0f;

void getResolution(void);
void myData(void);							// De la practica 4
void LoadTextures(void);					// De la práctica 6
unsigned int generateTextures(char*, bool, bool);	// De la práctica 6

//For Keyboard
float	movX = 0.0f,
movY = 0.0f,
movZ = -5.0f,
rotX = 0.0f;

//Texture
unsigned int	t_smile,
t_toalla,
t_unam,
t_white,
t_agua,
t_mosaico_alberca,
t_pared_alberca,
t_asta,
t_gradasjpg,
t_Hungria,
t_Australia,
t_USA,
t_boya_roja,
t_asientos,
t_blanco,
t_rojo_pelota,
t_baseplaforma,
t_baseplataforma,
t_oro,
t_plata,
t_cobre,
t_podio,
t_Black,
t_Cuadro_blanco,
//t_Solid_red,
t_Solid_white_bordered,
t_ladrillos;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

//// Light
glm::vec3 lightColor = glm::vec3(0.7f);
glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);

// ------------------- LUCES PULSANTES -------------------
bool pulseLights = true;
float pulseMin = 0.35f;
float pulseMax = 1.00f;
float pulseSpeed = 0.003f; // ajusta


// posiciones
float	movAuto_x = 0.0f,
movAuto_z = 0.0f,
orienta = 90.0f;
bool	animacion = false,
recorrido1 = true,
recorrido2 = false,
recorrido3 = false,
recorrido4 = false;


//Keyframes (Manipulación y dibujo)
float	posX = 0.0f,					//VARIABLES DE DIBUJO
		posY = 0.0f,
		posZ = 0.0f,
		rotRodIzq = 0.0f,
		giroMonito = 0.0f,				//	CAMBIO EN EL CODIGO
		giroCabeza = 0.0f,				//	CAMBIO EN EL CODIGO
		giroBrazoDerecho = 0.0f;		//	CAMBIO EN EL CODIGO
float	incX = 0.0f,					//	VARIABLE DE CALCULO
		incY = 0.0f,
		incZ = 0.0f,
		rotRodIzqInc = 0.0f,
		giroMonitoInc = 0.0f,			//	CAMBIO EN EL CODIGO
	    giroCabezaInc = 0.0f,			//	CAMBIO EN EL CODIGO
		giroBrazoDerechoInc = 0.0f;		//	CAMBIO EN EL CODIGO

#define MAX_FRAMES 18					//Número de KeyFrames		
int i_max_steps = 60;
int i_curr_steps = 0;					
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float rotRodIzq;
	float giroMonito;
	float giroCabeza;				//	CAMBIO EN EL CODIGO
	float giroBrazoDerecho;			//	CAMBIO EN EL CODIGO

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 6;			//introducir número en caso de tener Key guardados //	SIRVE PARA YA HACER LAS ANIMACIONES CON SOLO COMPILAR
bool play = false;
int playIndex = 0;

// ------------------- ANIMACION PELOTA: PERIMETRO + REBOTE + GIRO -------------------
bool ballFollowPerimeter = true;

// Posición pelota
float ballX = 0.0f, ballY = 0.0f, ballZ = 0.0f;

// Rotación pelota (grados)
float ballRot = 0.0f;

// Parámetros
float ballSpeed = 35.0f;      // unidades/seg (ajusta)
float ballRadius = 1.2f;      // radio "visual" del modelo (ajusta si rota raro)
float bounceAmp = 1.0f;       // amplitud del rebote
float bounceFreq = 2.0f;      // rebotes por segundo aprox (ajusta)
float bouncePhase = 0.0f;     // fase acumulada

// Perímetro 
float poolMinX = -55.0f;
float poolMaxX = 55.0f;
float poolMinZ = -25.0f;
float poolMaxZ = 25.0f;

// Altura base 
float ballBaseY = 23.0f;

// 0:+X, 1:+Z, 2:-X, 3:-Z
int ballEdge = 0;

// ------------------- ANIMACION POKEBALL + PODIUM -------------------
enum PBState { PB_IDLE = 0, PB_MOVE, PB_OPENING, PB_SUCK, PB_CLOSING, PB_DONE };
PBState pbState = PB_IDLE;
bool pbSequence = false;


// Posiciones 
glm::vec3 podiumPos = glm::vec3(0.0f, 0.0f, 0.0f);    // donde está tu podium (mundo)
glm::vec3 pbStartPos = glm::vec3(10.0f, 60.0f, 10.0f);    // donde inicia la pokeball
glm::vec3 pbPos = pbStartPos;

// Escalas base
float pbScaleWorld = 10.0f;   // tu escala actual de pokeball
float podiumScaleWorld = 10.0f;   // tu escala actual de podium

// Movimiento pokeball
float pbMoveSpeed = 45.0f;       // unidades/seg
float pbStopDist = 8.0f;        // distancia para detenerse "frente" al podio

// Apertura tapa
float pbOpenAngle = 0.0f;        // grados
float pbOpenMax = 120.0f;      // grados apertura
float pbOpenSpeed = 180.0f;      // grados/seg

// Succión del podio
bool  podiumVisible = true;
glm::vec3 podiumAnimPos = podiumPos;
float podiumAnimScale = podiumScaleWorld;
float podiumShrinkSpeed = 1.2f;  // escala/seg (reduce rápido)
float podiumPullSpeed = 60.0f; // unidades/seg (jalar hacia pokeball)

// Centro de "captura" (AJUSTA Y según tu pokeball)
glm::vec3 pbCaptureLocal = glm::vec3(0.0f, 2.5f, 0.0f); // punto dentro de la pokeball

glm::vec3 pbTopHingeLocal = glm::vec3(0.0f, 2.2f, 0.0f);  // punto bisagra
glm::vec3 pbTopPivotAxis = glm::vec3(1.0f, 0.0f, 0.0f);  // eje de giro tapa


// ------------------- ANIMACION GRADAS: ROTAR + SUBIR + DESAPARECER -------------------
bool  gradesAnim = true;
float gradesRotY = 0.0f;
float gradesScale01 = 1.0f;
float gradesLiftY = 0.0f;

float gradesRotSpeed = 45.0f;
float gradesFadeSpeed = 0.15f;
float gradesLiftSpeed = 12.0f;
float gradesLiftMax = 120.0f;


// ------------------- AGUA PROCEDURAL (GLOBALS) -------------------
float waterOffsetY = 0.0f;
float waterTime = 0.0f;
float waterAmp = 0.30f;      // altura ola
float waterSpeed = 2.0f;     // velocidad ola

// ------------------- BANDERAS SWING -------------------
bool flagsSwing = true;
float flagAmp = 10.0f;      // grados
float flagSpeed = 0.004f;   // ajusta



void saveFrame(void)
{
	//printf("frameindex %d\n", FrameIndex);
	std::cout << "Frame Index = " << FrameIndex << std::endl;

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	KeyFrame[FrameIndex].giroMonito = giroMonito;
	KeyFrame[FrameIndex].giroCabeza = giroCabeza;								//	CAMBIO EN EL CODIGO
	KeyFrame[FrameIndex].giroBrazoDerecho = giroBrazoDerecho;					//	CAMBIO EN EL CODIGO

	std::cout << "posX = " << posX << std::endl;								//CAMBIO EN EL CODIGO
	std::cout << "posZ = " << posZ << std::endl;								//CAMBIO EN EL CODIGO
	std::cout << "Giro = " << giroMonito << std::endl;							//CAMBIO EN EL CODIGO	
	std::cout << "Cabeza = " << giroCabeza << std::endl;						//CAMBIO EN EL CODIGO	

	FrameIndex++;

}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	giroMonito = KeyFrame[0].giroMonito;
	giroCabeza = KeyFrame[0].giroCabeza;								//	CAMBIO EN EL CODIGO
	giroBrazoDerecho = KeyFrame[0].giroBrazoDerecho;					//	CAMBIO EN EL CODIGO	
}

void interpolation(void)
{
	incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	rotRodIzqInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	giroMonitoInc = (KeyFrame[playIndex + 1].giroMonito - KeyFrame[playIndex].giroMonito) / i_max_steps;
	giroCabezaInc = (KeyFrame[playIndex + 1].giroCabeza - KeyFrame[playIndex].giroCabeza) / i_max_steps;						//	CAMBIO EN EL CODIGO
	giroBrazoDerechoInc = (KeyFrame[playIndex + 1].giroBrazoDerecho - KeyFrame[playIndex].giroBrazoDerecho) / i_max_steps;		//	CAMBIO EN EL CODIGO

}

unsigned int generateTextures(const char* filename, bool alfa, bool isPrimitive)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;

	if (isPrimitive) stbi_set_flip_vertically_on_load(true);
	else            stbi_set_flip_vertically_on_load(false);

	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (!data) {
		std::cout << "Failed to load texture: " << filename << std::endl;
		return 100;
	}

	if (alfa)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	return textureID;
}

void LoadTextures()
{

	t_smile = generateTextures("Texturas/awesomeface.png", 1, true);
	t_toalla = generateTextures("Texturas/toalla.tga", 0, true);
	t_unam = generateTextures("Texturas/escudo_unam.jpg", 0, true);
	t_ladrillos = generateTextures("Texturas/bricks.jpg", 0, true);
	t_agua = generateTextures("Texturas/agua.jpg", 0, true);
	t_mosaico_alberca = generateTextures("Texturas/mosaico_alberca.jpg", 0, true);
	t_pared_alberca = generateTextures("Texturas/pared_alberca.jpg", 0, true);
	t_asta = generateTextures("Texturas/asta.jpg", 0, true);
	t_gradasjpg = generateTextures("Texturas/gradasjpg.jpg", 0, true);
	t_Hungria = generateTextures("Textures/Hungria.png", 1, true);
	t_Australia = generateTextures("Textures/Australia.png", 1, true);
	t_USA = generateTextures("Textures/USA.png", 1, true);
	t_boya_roja = generateTextures("Textures/boya_roja.jpg", 0, true);
	t_asientos = generateTextures("Textures/asientos.jpg", 0, true);
	t_blanco = generateTextures("Textures/blanco.jpg", 0, true);
	t_rojo_pelota = generateTextures("Textures/rojo_pelota.jpg", 0, true);
	t_baseplaforma = generateTextures("Textures/baseplataforma.jpg", 0, true);
	t_baseplataforma = generateTextures("Textures/baseplaforma.jpg", 0, true);
	t_cobre = generateTextures("Textures/cobre.jpg", 0, true);
	t_oro = generateTextures("Textures/oro.jpg", 0, true);
	t_plata = generateTextures("Textures/plata.jpg", 0, true);
	t_podio = generateTextures("Textures/podio.jpg", 0, true);
	//t_Solid_Red = generateTextures("Textures/Solid_red.png", 1, true);
	t_Solid_white_bordered = generateTextures("Textures/Solid_white_bordered.png", 1, true);
	t_Cuadro_blanco = generateTextures("Textures/Cuadro_blanco.png", 1, true);
	t_Black = generateTextures("Textures/Black.png", 1, true);
	//This must be the last
	t_white = generateTextures("Texturas/white.jpg", 0, false);
}



void animate(void)								//	ES UN CICLO EN SI MISMO
{
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				std::cout << "Animation ended" << std::endl;
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
				interpolation();
			}
		}
		else
		{
			posX += incX;
			posY += incY;
			posZ += incZ;

			rotRodIzq += rotRodIzqInc;
			giroMonito += giroMonitoInc;
			giroCabeza += giroCabezaInc;
			giroBrazoDerecho += giroBrazoDerechoInc;

			i_curr_steps++;
		}
	}

	//Vehículo
	if (animacion)
	{
		movAuto_x += 3.0f;
	}

	// ------------------- Pelota: movimiento perímetro + rebote + giro -------------------
	if (ballFollowPerimeter)
	{
		
		float dt = (float)deltaTime / 1000.0f;
		if (dt > 0.05f) dt = 0.05f; // clamp para evitar saltos cuando hay lag

		// Rebote vertical (solo hacia arriba)
		bouncePhase += bounceFreq * dt;
		float bounceY = bounceAmp * fabsf(sinf(bouncePhase * 2.0f * 3.1415926f));

		// Avance
		float step = ballSpeed * dt;

		switch (ballEdge)
		{
		case 0: // +X (sobre poolMinZ)
			ballX += step;
			if (ballX >= poolMaxX) { ballX = poolMaxX; ballEdge = 1; }
			break;

		case 1: // +Z (sobre poolMaxX)
			ballZ += step;
			if (ballZ >= poolMaxZ) { ballZ = poolMaxZ; ballEdge = 2; }
			break;

		case 2: // -X (sobre poolMaxZ)
			ballX -= step;
			if (ballX <= poolMinX) { ballX = poolMinX; ballEdge = 3; }
			break;

		case 3: // -Z (sobre poolMinX)
			ballZ -= step;
			if (ballZ <= poolMinZ) { ballZ = poolMinZ; ballEdge = 0; }
			break;
		}

		// Altura final
		ballY = ballBaseY + bounceY;

		// Rotación realista por distancia recorrida:
		// gradosPorUnidad = 360 / (2*pi*R)
		float degreesPerUnit = 360.0f / (2.0f * 3.1415926f * ballRadius);
		ballRot += step * degreesPerUnit;
		if (ballRot > 360.0f) ballRot -= 360.0f;
	}

	// ------------------- SECUENCIA POKEBALL: mover->abrir->meter podium->cerrar -------------------
	{
		float dt = (float)deltaTime / 1000.0f;
		if (dt > 0.05f) dt = 0.05f;

		if (pbSequence && pbState == PB_IDLE) {
			// Reset de animación al arrancar
			pbPos = pbStartPos;
			pbOpenAngle = 0.0f;

			podiumVisible = true;
			podiumAnimPos = podiumPos;
			podiumAnimScale = podiumScaleWorld;

			pbState = PB_MOVE;
		}

		if (!pbSequence && pbState != PB_IDLE && pbState != PB_DONE) {
			// 
		}

		switch (pbState)
		{
		case PB_IDLE:
			break;

		case PB_MOVE:
		{
			glm::vec3 target = podiumPos + glm::vec3(0.0f, 0.0f, -pbStopDist);
			target.y = podiumPos.y + 28.0f; // offset arriba del podio


			glm::vec3 dir = target - pbPos;
			float dist = glm::length(dir);

			if (dist < 0.5f) {
				pbPos = target;
				pbState = PB_OPENING;
			}
			else {
				dir = dir / dist;
				pbPos += dir * pbMoveSpeed * dt;
			}
		}
		break;

		case PB_OPENING:
			pbOpenAngle += pbOpenSpeed * dt;
			if (pbOpenAngle >= pbOpenMax) {
				pbOpenAngle = pbOpenMax;
				pbState = PB_SUCK;
			}
			break;

		case PB_SUCK:
		{
			// Punto de captura en mundo
			glm::vec3 captureWorld = pbPos + pbCaptureLocal; // 

			// Mover podio hacia la pokeball
			glm::vec3 dir = captureWorld - podiumAnimPos;
			float dist = glm::length(dir);
			if (dist > 0.001f) dir /= dist;

			podiumAnimPos += dir * podiumPullSpeed * dt;

			// Reducir escala
			podiumAnimScale -= podiumShrinkSpeed * dt;
			if (podiumAnimScale < 0.05f) podiumAnimScale = 0.05f;

			// Cuando ya esté casi dentro
			if (dist < 1.5f && podiumAnimScale <= 0.08f) {
				podiumVisible = false;
				pbState = PB_CLOSING;
			}
		}
		break;

		case PB_CLOSING:
			pbOpenAngle -= pbOpenSpeed * dt;
			if (pbOpenAngle <= 0.0f) {
				pbOpenAngle = 0.0f;
				pbState = PB_DONE;
				pbSequence = false; // termina secuencia
			}
			break;

		case PB_DONE:
			// Se queda cerrada
			break;
		}
	}
	// ------------------- GRADAS : rotar + subir + desaparecer -------------------
	{
		float dt = (float)deltaTime / 1000.0f;
		if (dt > 0.05f) dt = 0.05f;

		if (gradesAnim && gradesScale01 > 0.0f)
		{
			gradesRotY += gradesRotSpeed * dt;
			if (gradesRotY > 360.0f) gradesRotY -= 360.0f;

			gradesLiftY += gradesLiftSpeed * dt;
			if (gradesLiftY > gradesLiftMax) gradesLiftY = gradesLiftMax;

			gradesScale01 -= gradesFadeSpeed * dt;
			if (gradesScale01 < 0.0f) gradesScale01 = 0.0f;
		}
	}

// ------------------- Agua: ola procedural -------------------
{
	float dt = (float)deltaTime / 1000.0f;
	if (dt > 0.05f) dt = 0.05f;

	waterTime += dt * waterSpeed;
	waterOffsetY = sinf(waterTime) * waterAmp;
}

}

void getResolution() {
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}

void myData() {
	float vertices[] = {
		// positions          // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	float verticesPiso[] = {
		// positions          // texture coords
		 10.5f,  10.5f, 0.0f,   4.0f, 4.0f, // top right
		 10.5f, -10.5f, 0.0f,   4.0f, 0.0f, // bottom right
		-10.5f, -10.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-10.5f,  10.5f, 0.0f,   0.0f, 4.0f  // top left 
	};
	unsigned int indicesPiso[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	GLfloat verticesCubo[] = {
		//Position				//texture coords
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,	//V0 - Frontal
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,	//V1
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,	//V5
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,	//V0
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,	//V5

		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,	//V2 - Trasera
		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f,	//V3
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V7
		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,	//V2
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V7

		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4 - Izq
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V7
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,	//V3
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,	//V3
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		-0.5f, -0.5f, 0.5f,		0.0f, 1.0f,	//V0

		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,	//V5 - Der
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,	//V1
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,	//V2
		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,	//V5
		0.5f, 0.5f, -0.5f,		1.0f, 0.0f,	//V6
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,	//V2

		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4 - Sup
		0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V5
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V7
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6

		-0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V0 - Inf
		-0.5f, -0.5f, -0.5f,	1.0f, 1.0f,	//V3
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f,	//V2
		-0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V0
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f,	//V2
		0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V1
	};

	glGenVertexArrays(3, VAO);
	glGenBuffers(3, VBO);
	glGenBuffers(3, EBO);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Para Piso
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPiso), verticesPiso, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPiso), indicesPiso, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	//PARA CUBO
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCubo), verticesCubo, GL_STATIC_DRAW);

	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int main() {
	// glfw: initialize and configure
	glfwInit();

	// glfw window creation
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Pratica X 2025-2", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	//Mis funciones
	//Datos a utilizar
	LoadTextures();
	myData();
	glEnable(GL_DEPTH_TEST);

	

	// build and compile shaders
	// -------------------------
	Shader myShader("shaders/shader_texture_color.vs", "shaders/shader_texture_color.fs"); //To use with primitives
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights_mod.fs");	//To use with static models
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");	//To use with skybox
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");	//To use with animated models 
	
	vector<std::string> faces{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// load models
	// -----------

	
	///////////////////////////////////
	Model agua("resources/objects/agua/agua.obj");
	Model basealberca("resources/objects/basealberca/basealberca.obj");
	Model asta("resources/objects/asta/astas.obj");
	Model bancaderecha("resources/objects/bancaderecha/bancaderecha.obj");
	Model bancafrente("resources/objects/bancafrente/bancafrente.obj");
	Model bancaizq("resources/objects/bancaizq/bancaizq.obj");
	Model bandehungria("resources/objects/bandehungria/bandehungria.obj");
	Model banderaaustralia("resources/objects/banderaaustralia/banderaaustralia.obj");
	Model banderausa("resources/objects/banderausa/banderausa.obj");
	Model baseladrillo("resources/objects/baseladrillo/baseladrillo.obj");
	Model boyas("resources/objects/boyas/boyas.obj");
	Model butacaderecha("resources/objects/butacaderecha/butacaderecha.obj");
	Model butacafrente("resources/objects/butacafrente/butacafrente.obj");
	Model butacaizq("resources/objects/butacaizq/butacaizq.obj");
	Model pelota("resources/objects/pelota/pelota.obj");
	Model plataforma("resources/objects/plataforma/plataforma.obj");
	Model podium("resources/objects/podium/podium.obj");
	Model base_pokeball("resources/objects/Pokeball/base_pokeball.obj");
	Model pivote_pokeball("resources/objects/Pokeball/pivote_pokeball.obj");
	Model tapa_pokeball("resources/objects/Pokeball/tapa_pokeball.obj");



	ModelAnim animacionPersonaje("resources/objects/Personaje1/Arm.dae");
	animacionPersonaje.initShaders(animShader.ID);
	ModelAnim remyCaminando("resources/objects/Remy/Remy.dae");////
	remyCaminando.initShaders(animShader.ID);/////
///


	//Para objetos con animacion

	//Inicialización de KeyFrames
	/*/for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].giroMonito = 0;
	}*/

	KeyFrame[0].posX = 0;							//	CAMBIO EN EL CODIGO
	KeyFrame[0].posY = 0;							//	CAMBIO EN EL CODIGO	
	KeyFrame[0].posZ = 0;							//	CAMBIO EN EL CODIGO
	KeyFrame[0].rotRodIzq = 0;						//	CAMBIO EN EL CODIGO	
	KeyFrame[0].giroMonito = 0;						//	CAMBIO EN EL CODIGO	
	KeyFrame[0].giroBrazoDerecho = 0;				//	CAMBIO EN EL CODIGO

	KeyFrame[1].posX = 20.0f;						//	CAMBIO EN EL CODIGO
	KeyFrame[1].posY = 15.0f;						//	CAMBIO EN EL CODIGO
	KeyFrame[1].posZ = 0;							//	CAMBIO EN EL CODIGO
	KeyFrame[1].rotRodIzq = 0;						//	CAMBIO EN EL CODIGO
	KeyFrame[1].giroMonito = 0;						//	CAMBIO EN EL CODIGO
	KeyFrame[1].giroBrazoDerecho = 0;				//	CAMBIO EN EL CODIGO

	KeyFrame[2].posX = 20.0f;						//	CAMBIO EN EL CODIGO
	KeyFrame[2].posY = 28.0f;						//	CAMBIO EN EL CODIGO
	KeyFrame[2].posZ = 10.0f;						//	CAMBIO EN EL CODIGO
	KeyFrame[2].rotRodIzq = 5.0f;					//	CAMBIO EN EL CODIGO
	KeyFrame[2].giroMonito = 90.0f;					//	CAMBIO EN EL CODIGO
	KeyFrame[2].giroBrazoDerecho = 75.0f;			//	CAMBIO EN EL CODIGO

	KeyFrame[3].posX = 0.0f;						//	CAMBIO EN EL CODIGO
	KeyFrame[3].posY = 0.0f;						//	CAMBIO EN EL CODIGO	
	KeyFrame[3].posZ = 0.0f;						//	CAMBIO EN EL CODIGO
	KeyFrame[3].rotRodIzq = 0.0f;					//	CAMBIO EN EL CODIGO
	KeyFrame[3].giroMonito = 0.0f;					//	CAMBIO EN EL CODIGO
	KeyFrame[3].giroBrazoDerecho = -90.0f;			//	CAMBIO EN EL CODIGO

	KeyFrame[4].posX = -20.0f;
	KeyFrame[4].posY = 28.0f;
	KeyFrame[4].posZ = -10.0f;
	KeyFrame[4].rotRodIzq = -15.0f;					//	CAMBIO EN EL CODIGO
	KeyFrame[4].giroMonito = -90.0f;				//	CAMBIO EN EL CODIGO
	KeyFrame[4].giroBrazoDerecho = 75.0f;			//	CAMBIO EN EL CODIGO

	KeyFrame[5].posX = 0;							//	CAMBIO EN EL CODIGO
	KeyFrame[5].posY = 0;							//	CAMBIO EN EL CODIGO
	KeyFrame[5].posZ = 0;							//	CAMBIO EN EL CODIGO
	KeyFrame[5].rotRodIzq = 0;						//	CAMBIO EN EL CODIGO
	KeyFrame[5].giroMonito = 0;						//	CAMBIO EN EL CODIGO
	KeyFrame[5].giroBrazoDerecho = -90.0f;			//	CAMBIO EN EL CODIGO

	// create transformations and Projection
	glm::mat4 modelOp = glm::mat4(1.0f);		// initialize Matrix, Use this matrix for individual models
	glm::mat4 viewOp = glm::mat4(1.0f);		//Use this matrix for ALL models
	glm::mat4 projectionOp = glm::mat4(1.0f);	//This matrix is for Projection

	glm::mat4 tmpCuerpoAquaman = glm::mat4(1.0f);		//////

	// ------------------- INIT PELOTA -------------------
	ballX = poolMinX;
	ballZ = poolMinZ;
	ballY = ballBaseY;
	ballEdge = 0;
	ballRot = 0.0f;
	bouncePhase = 0.0f;


	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);

		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		animate();

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		//Setup shader for static models
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);

		if (pulseLights)
		{
			float t = (sinf(SDL_GetTicks() * pulseSpeed) + 1.0f) * 0.5f; // 0..1
			float I = pulseMin + t * (pulseMax - pulseMin);

			staticShader.setVec3("dirLight.ambient", ambientColor * I);
			staticShader.setVec3("dirLight.diffuse", diffuseColor * I);
		}
		else
		{
			staticShader.setVec3("dirLight.ambient", ambientColor);
			staticShader.setVec3("dirLight.diffuse", diffuseColor);
		}

		staticShader.setVec3("dirLight.specular", glm::vec3(0.6f, 0.6f, 0.6f));


		staticShader.setVec3("pointLight[0].position", lightPosition);
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.08f);
		staticShader.setFloat("pointLight[0].linear", 0.009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.032f);

		staticShader.setVec3("pointLight[1].position", glm::vec3(-80.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.032f);

		staticShader.setVec3("spotLight[0].position", glm::vec3(0.0f, 20.0f, 10.0f));
		staticShader.setVec3("spotLight[0].direction", glm::vec3(0.0f, -1.0f, 0.0f));
		staticShader.setVec3("spotLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(10.0f)));
		staticShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(60.0f)));
		staticShader.setFloat("spotLight[0].constant", 1.0f);
		staticShader.setFloat("spotLight[0].linear", 0.0009f);
		staticShader.setFloat("spotLight[0].quadratic", 0.0005f);

		staticShader.setFloat("material_shininess", 32.0f);

		//glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		// view/projection transformations
		//glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		viewOp = camera.GetViewMatrix();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);

		//Setup shader for primitives
		myShader.use();
		// view/projection transformations
		//projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 400.0f);
		viewOp = camera.GetViewMatrix();
		// pass them to the shaders
		//myShader.setMat4("model", modelOp);
		myShader.setMat4("view", viewOp);
		// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		myShader.setMat4("projection", projectionOp);
		/**********/


		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
		animShader.use(); //encargado de dibujar modelos  7777
		animShader.setMat4("projection", projectionOp);
		animShader.setMat4("view", viewOp);

		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", ambientColor);
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 20.0f, 262.3f)); // translate it down so it's at the center of the scene
		modelOp = glm::scale(modelOp, glm::vec3(0.06f));	// it's a bit too big for our scene, so scale it down
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		animShader.setMat4("model", modelOp);
		animacionPersonaje.Draw(animShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Segundo Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(140.0f, 23.0f, 40.0f));  ///
		modelOp = glm::scale(modelOp, glm::vec3(0.06f));                           ///
		animShader.setMat4("model", modelOp);                                      ///
		remyCaminando.Draw(animShader);                                             ///


		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario Primitivas
		// -------------------------------------------------------------------------------------------------------------------------
		myShader.use();

		//Tener Piso como referencia
		glBindVertexArray(VAO[2]);
		glBindVertexArray(0);
		// ------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario Primitivas
		// -------------------------------------------------------------------------------------------------------------------------

		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);


		///////////////////////

		modelOp = glm::mat4(1.0f);
		modelOp = glm::translate(modelOp, glm::vec3(0.0f, waterOffsetY, 0.0f)); // <-- AQUÍ
		modelOp = glm::scale(modelOp, glm::vec3(10.0f));
		staticShader.setMat4("model", modelOp);
		agua.Draw(staticShader);




		modelOp = glm::mat4(1.0f);
		modelOp = glm::scale(modelOp, glm::vec3(10.0f));
		staticShader.setMat4("model", modelOp);
		basealberca.Draw(staticShader);

		modelOp = glm::mat4(1.0f);
		modelOp = glm::scale(modelOp, glm::vec3(10.0f));
		staticShader.setMat4("model", modelOp);
		asta.Draw(staticShader);

		float flagAng = 0.0f;
		if (flagsSwing)
			flagAng = sinf(SDL_GetTicks() * flagSpeed) * flagAmp;

		// Hungría
		modelOp = glm::mat4(1.0f);
		modelOp = glm::rotate(modelOp, glm::radians(flagAng), glm::vec3(0, 0, 1));
		modelOp = glm::scale(modelOp, glm::vec3(10.0f));
		staticShader.setMat4("model", modelOp);
		bandehungria.Draw(staticShader);

		// Australia
		modelOp = glm::mat4(1.0f);
		modelOp = glm::rotate(modelOp, glm::radians(flagAng), glm::vec3(0, 0, 1));
		modelOp = glm::scale(modelOp, glm::vec3(10.0f));
		staticShader.setMat4("model", modelOp);
		banderaaustralia.Draw(staticShader);

		// USA
		modelOp = glm::mat4(1.0f);
		modelOp = glm::rotate(modelOp, glm::radians(flagAng), glm::vec3(0, 0, 1));
		modelOp = glm::scale(modelOp, glm::vec3(10.0f));
		staticShader.setMat4("model", modelOp);
		banderausa.Draw(staticShader);



		modelOp = glm::mat4(1.0f);
		modelOp = glm::scale(modelOp, glm::vec3(10.0f));
		staticShader.setMat4("model", modelOp);
		baseladrillo.Draw(staticShader);

		modelOp = glm::mat4(1.0f);
		modelOp = glm::scale(modelOp, glm::vec3(10.0f));
		staticShader.setMat4("model", modelOp);
		boyas.Draw(staticShader);

		// ------------------- DIBUJAR GRADAS (bancas + butacas) -------------------
		if (gradesScale01 > 0.0f)
		{
			float s = 10.0f * gradesScale01;

			// Banca derecha
			modelOp = glm::mat4(1.0f);
			modelOp = glm::translate(modelOp, glm::vec3(0.0f, gradesLiftY, 0.0f));
			modelOp = glm::rotate(modelOp, glm::radians(gradesRotY), glm::vec3(0.0f, 1.0f, 0.0f));
			modelOp = glm::scale(modelOp, glm::vec3(s));
			staticShader.setMat4("model", modelOp);
			bancaderecha.Draw(staticShader);

			// Banca frente
			modelOp = glm::mat4(1.0f);
			modelOp = glm::translate(modelOp, glm::vec3(0.0f, gradesLiftY, 0.0f));
			modelOp = glm::rotate(modelOp, glm::radians(gradesRotY), glm::vec3(0.0f, 1.0f, 0.0f));
			modelOp = glm::scale(modelOp, glm::vec3(s));
			staticShader.setMat4("model", modelOp);
			bancafrente.Draw(staticShader);

			// Banca izquierda
			modelOp = glm::mat4(1.0f);
			modelOp = glm::translate(modelOp, glm::vec3(0.0f, gradesLiftY, 0.0f));
			modelOp = glm::rotate(modelOp, glm::radians(gradesRotY), glm::vec3(0.0f, 1.0f, 0.0f));
			modelOp = glm::scale(modelOp, glm::vec3(s));
			staticShader.setMat4("model", modelOp);
			bancaizq.Draw(staticShader);

			// Butaca derecha
			modelOp = glm::mat4(1.0f);
			modelOp = glm::translate(modelOp, glm::vec3(0.0f, gradesLiftY, 0.0f));
			modelOp = glm::rotate(modelOp, glm::radians(gradesRotY), glm::vec3(0.0f, 1.0f, 0.0f));
			modelOp = glm::scale(modelOp, glm::vec3(s));
			staticShader.setMat4("model", modelOp);
			butacaderecha.Draw(staticShader);

			// Butaca frente
			modelOp = glm::mat4(1.0f);
			modelOp = glm::translate(modelOp, glm::vec3(0.0f, gradesLiftY, 0.0f));
			modelOp = glm::rotate(modelOp, glm::radians(gradesRotY), glm::vec3(0.0f, 1.0f, 0.0f));
			modelOp = glm::scale(modelOp, glm::vec3(s));
			staticShader.setMat4("model", modelOp);
			//butacafrente.Draw(staticShader);

			// Butaca izquierda
			modelOp = glm::mat4(1.0f);
			modelOp = glm::translate(modelOp, glm::vec3(0.0f, gradesLiftY, 0.0f));
			modelOp = glm::rotate(modelOp, glm::radians(gradesRotY), glm::vec3(0.0f, 1.0f, 0.0f));
			modelOp = glm::scale(modelOp, glm::vec3(s));
			staticShader.setMat4("model", modelOp);
			butacaizq.Draw(staticShader);
		}



		// ------------------- Pelota animada -------------------
		modelOp = glm::mat4(1.0f);
		modelOp = glm::translate(modelOp, glm::vec3(ballX, ballY, ballZ));

		// Giro coherente con dirección: si va por X gira sobre Z, si va por Z gira sobre X
		if (ballEdge == 0 || ballEdge == 2) {
			modelOp = glm::rotate(modelOp, glm::radians(ballRot), glm::vec3(0.0f, 0.0f, 1.0f));
		}
		else {
			modelOp = glm::rotate(modelOp, glm::radians(ballRot), glm::vec3(1.0f, 0.0f, 0.0f));
		}

		modelOp = glm::scale(modelOp, glm::vec3(10.0f));
		staticShader.setMat4("model", modelOp);
		pelota.Draw(staticShader);


		modelOp = glm::mat4(1.0f);
		modelOp = glm::scale(modelOp, glm::vec3(10.0f));
		staticShader.setMat4("model", modelOp);
		plataforma.Draw(staticShader);

		// ------------------- PODIUM (animado / se mete a pokeball) -------------------
		if (podiumVisible)
		{
			modelOp = glm::mat4(1.0f);
			modelOp = glm::translate(modelOp, podiumAnimPos);
			modelOp = glm::scale(modelOp, glm::vec3(podiumAnimScale));
			staticShader.setMat4("model", modelOp);
			podium.Draw(staticShader);
		}

		// ------------------- POKEBALL (animada: se mueve y abre/cierra) -------------------
		glm::mat4 pbWorld = glm::mat4(1.0f);
		pbWorld = glm::translate(pbWorld, pbPos);
		pbWorld = glm::scale(pbWorld, glm::vec3(pbScaleWorld));

		// Base
		modelOp = pbWorld;
		staticShader.setMat4("model", modelOp);
		base_pokeball.Draw(staticShader);

		// Pivote (si tu modelo realmente es el botón/centro)
		modelOp = pbWorld;
		staticShader.setMat4("model", modelOp);
		pivote_pokeball.Draw(staticShader);

		// Tapa: rotación alrededor de una bisagra local
		modelOp = pbWorld;
		// llevar al punto bisagra, rotar, regresar
		modelOp = glm::translate(modelOp, pbTopHingeLocal);
		modelOp = glm::rotate(modelOp, glm::radians(pbOpenAngle), pbTopPivotAxis);
		modelOp = glm::translate(modelOp, -pbTopHingeLocal);

		staticShader.setMat4("model", modelOp);
		tapa_pokeball.Draw(staticShader);


		////////////////////////////

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(250.0f, 0.0f, -10.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		//casaDoll.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.75f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.2f));
		staticShader.setMat4("model", modelOp);
		//piso.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -70.0f));
		modelOp = glm::scale(modelOp, glm::vec3(5.0f));
		staticShader.setMat4("model", modelOp);
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
		//casaVieja.Draw(staticShader);

		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------
		skyboxShader.use();
		skybox.Draw(skyboxShader, viewOp, projectionOp, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	//skybox.Terminate();
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow* window, int key, int scancode, int action, int mode) 
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);


	//To play KeyFrame animation 
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			std::cout << "Play animation" << std::endl;
			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
			std::cout << "Not enough Key Frames" << std::endl;
		}
	}

	//To Save a KeyFrame
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}
	// ------------------- Controles Pelota -------------------
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
		ballFollowPerimeter = !ballFollowPerimeter;

	if (key == GLFW_KEY_KP_ADD && action == GLFW_PRESS)   // keypad +
		ballSpeed += 5.0f;

	if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS) // keypad -
		ballSpeed = (ballSpeed > 5.0f) ? (ballSpeed - 5.0f) : 5.0f;

	// Iniciar secuencia pokeball -> podium
	if (key == GLFW_KEY_O && action == GLFW_PRESS)  // O = iniciar
	{
		pbSequence = true;
		pbState = PB_IDLE; // para que reinicie bien
	}

	// R = reiniciar animación gradas
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		gradesRotY = 0.0f;
		gradesScale01 = 1.0f;
		gradesLiftY = 0.0f;
		gradesAnim = true;
	}

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos) 
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}