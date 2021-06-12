
/*
Projekt Zaliczeniowy Grafika Komputerowa i Wizualizacja 2021
Mateusz Olewnik i Mariusz Nowak
Prowadzący: dr inż. Witold Andrzejewski 
*/

/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myCube.h"
#include "Object3D.h"
#include <Visitor.h>
#include <OBJ_Loader.h>
#include <firstMethodDrawing.h>
#include <MainDrawingMethod.h>
#include <SecondMethodDrawing.h>
#include <RoomDrawingMethod.h>
#include <SkyDrawingMethod.h>
#include <LionDrawingMethod.h>
#include <EnvmapDrawingMethod.h>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>


//Speed+window parameters
float speed_x = 0;
float speed_y = 0;
float visitor_speed = 1;
float aspectRatio = 1;
float walk_speed = 0;

//FPS free fly
//WASD - obroty kamerą
//Strzalka gora dol = latanie do przodu/tylu

//Polozenie poczatkowe: pozycja gracza i źródeł światła
glm::vec3 pos = glm::vec3(-4, 2.8,-11);
glm::vec4 zrSwiatla = glm::vec4(-4, 3.0, -10, 1);
glm::vec4 zrSwiatla2 = glm::vec4(-4, 3.0, 50, 1);
glm::vec4 zrSwiatla3 = glm::vec4(-2.0f, 2.0f, 29.0f, 1);
//glm::vec4 zrSwiatla = glm::vec4(pos, 1);
glm::vec4 sources[3] = {zrSwiatla,zrSwiatla2,zrSwiatla3};

//All Shaders
ShaderProgram* sp;
ShaderProgram* sp_l;
ShaderProgram* sp_main;
ShaderProgram* sp_envmap;

//All models
MainDrawingMethod blackBear("assets/statues/BlackBear.obj");
MainDrawingMethod cer("assets/statues/cer.obj");
RoomDrawingMethod room("assets/gallery/Museum.obj");
SkyDrawingMethod sky("assets/scene/Egg.obj");

//Room1
MainDrawingMethod  painting("assets/paintings/canvas.obj"), frame("assets/paintings/frame.obj");
MainDrawingMethod  painting2("assets/paintings/canvas.obj"), exitSign("assets/paintings/canvas.obj");
MainDrawingMethod  painting3("assets/paintings/canvas.obj");
MainDrawingMethod  painting4("assets/paintings/canvas.obj");
MainDrawingMethod  painting5("assets/paintings/canvas.obj");
//Room2
MainDrawingMethod  painting2_1("assets/paintings/canvas.obj"), painting2_2("assets/paintings/canvas.obj");

RoomDrawingMethod corridor("assets/gallery/corridor.obj");
RoomDrawingMethod  transition("assets/gallery/transition.obj");
MainDrawingMethod parquetry("assets/paintings/canvas.obj");
MainDrawingMethod postument("assets/gallery/postument.obj");
MainDrawingMethod door("assets/gallery/door.obj");
MainDrawingMethod visitor1("assets/scene/character.obj");
LionDrawingMethod lion("assets/statues/lion.obj"), brain("assets/statues/brain.obj"), 
  frameB("assets/paintings/fancyframe.obj"), frameB2("assets/paintings/fancyframe.obj");

glm::vec3 calcDir(float kat_x, float kat_y) {
	glm::vec4 dir = glm::vec4(0, 0, 1, 0);
	glm::mat4 M = glm::rotate(glm::mat4(1.0f), kat_y, glm::vec3(0, 1, 0));
	M = glm::rotate(M, kat_x, glm::vec3(1, 0, 0));
	dir = M * dir;
	return glm::vec3(dir); //wlasnosc wektora 3 liczbowego ucina wspolrzedne w z czteroliczbowych
}

GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Wczytanie do pamięci komputera
	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
	//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import do pamięci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
	//Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}

void keyCallback(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mod
) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_A) speed_y = 1;
		if (key == GLFW_KEY_D) speed_y = -1;
		if (key == GLFW_KEY_S) speed_x = 1;
		if (key == GLFW_KEY_W) speed_x = -1;
		if (key == GLFW_KEY_UP) walk_speed = 10;
		if (key == GLFW_KEY_DOWN) walk_speed = -10;

	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_A) speed_y = 0;
		if (key == GLFW_KEY_D) speed_y = 0;
		if (key == GLFW_KEY_S) speed_x = 0;
		if (key == GLFW_KEY_W) speed_x = 0;
		if (key == GLFW_KEY_UP) walk_speed = 0;
		if (key == GLFW_KEY_DOWN) walk_speed = 0;
	}
}

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	if (height == 0) return;
	aspectRatio = (float)width / (float)height;
	glViewport(0, 0, width, height);
}


void allDrawInOnePlace(glm::mat4 P, glm::mat4 V, glm::mat4 M) {
	//Museum parts+scene
	room.drawModel(sp_l, P, V, M, sources, 3.0f, 1.0f, -15.0f, 180.0f, 0.002f, 0.0045f, 0.004f);
	room.drawModel(sp_l, P, V, M, sources, -9.0f, 1.0f, 58.0f, 360.0f, 0.002f, 0.0045f, 0.003f);
	sky.drawModel(sp_l, P, V, M, sources, -3.0f, -40.0f, 20.0f, 360.0f, 1.0f, 1.6, 1.6);
	corridor.drawModel(sp_main, P, V, M, sources, -3.0f, 1.0f, 26.5f, 0.0f, 0.51f, 0.47f, 0.5f);
	transition.drawModel(sp_l, P, V, M, sources,-8.4f, 3.25f, 4.9f, -90.0f, 0.6f, 0.34f,0.36f);
	transition.drawModel(sp_l, P, V, M, sources, 2.4f, 3.25f, 43.0f, 90.0f, 0.6f, 0.34f, 0.36f);
	parquetry.drawModel(sp_main, P, V, M, sources, -3.0f, 1.0f,25.0f, 0.0f, 12.0f, 0.1f, 80.7f);
	door.drawModel(sp_l, P, V, M, sources, -3.6f, 1.05f, -14.90f, 0.0f, 0.0071f, 0.0071f, 0.0071f);
	door.drawModel(sp_l, P, V, M, sources, -2.47f, 1.05f, -14.965f, -180.0f, 0.0071f, 0.0071f, 0.0071f);
	door.drawModel(sp_l, P, V, M, sources, 2.95f, 1.05f, 55.0f, 270.0f, 0.0071f, 0.0071f, 0.0071f);
	exitSign.drawModel(sp_l, P, V, M, sources, 2.98f, 2.7f, 53.6f, 270.0f, 0.7f,0.7f, 0.003f);
	//Visitors with simple "AI"
	visitor1.drawModel(sp_l, P, V, M, sources, 0.0f, 1.0f, -10.0f, 20.0f, 0.13f, 0.13f, 0.13f);

	//Museum statues and postuments
	//Statue1Room1
	cer.drawModel(sp_main, P, V, M, sources, -3.0f, 2.0f, -5.0f, 180.0f, 0.2f, 0.2f, 0.2f);
	postument.drawModel(sp_main, P, V, M, sources, -3.0f, 1.1f, -5.0f, 0.0f, 0.4f, 0.3f, 0.4f);
	//Statue2Room1
	brain.drawModel(sp_main, P, V, M, sources, -3.0f, 2.0f, 0.0f,90.0f, 0.035f, 0.035f, 0.035f);
	postument.drawModel(sp_main, P, V, M, sources, -3.0f, 1.1f, 0.0f, 0.0f, 0.4f, 0.3f, 0.4f);
	
	//Statue1Room2
	lion.drawModel(sp_envmap, P, V, M, sources, 1.8f, 1.0f, 3.7f, 180.0f, 0.1f, 0.1f, 0.1f);
	lion.drawModel(sp_envmap, P, V, M, sources, -7.8f, 1.0f, 3.7f, 180.0f, 0.1f, 0.1f, 0.1f);

	blackBear.drawModel(sp_main, P, V, M, sources, -2.4f, 1.0f, 27.0f, 180.0f, 0.1f, 0.1f, 0.1f);
	
	//Museum paintings+frames
	//Painting1Room1
	painting.drawModel(sp_l, P, V, M, sources, 2.68f, 3.0f, -10.0f, 90.0f, 1.0f, 1.0f, 0.003f);
	frame.drawModel(sp_main, P, V, M, sources, 2.8f, 3.0f, -10.0f, 90.0f, 0.5f, 0.5f, 0.5f);
	//Painting2Room1
	painting2.drawModel(sp_l, P, V, M, sources, 2.68f, 3.0f, -6.0f, 90.0f, 1.4f, 1.4f, 0.003f);
	frame.drawModel(sp_main, P, V, M, sources, 2.8f, 3.0f, -6.0f, 90.0f, 0.7f, 0.7f, 0.7f);
	//Painting3Room1
	painting3.drawModel(sp_l, P, V, M, sources, 2.68f, 3.0f, -2.0f, 90.0f, 1.0f, 1.0f, 0.003f);
	frame.drawModel(sp_main, P, V, M, sources, 2.8f, 3.0f, -2.0f, 90.0f, 0.5f, 0.5f, 0.5f);
	//Painting4Room1
	painting4.drawModel(sp_l, P, V, M, sources, -8.78f, 3.1f, -8.0f, 270.0f, 2.6f, 1.5f, 0.003f);
	frame.drawModel(sp_main, P, V, M, sources, -8.9f, 3.1f, -8.0f, 270.0f, 1.2f, 0.7f, 0.7f);
	//Painting5Room1
	painting5.drawModel(sp_l, P, V, M, sources, -8.78f, 3.1f, -2.0f, 270.0f, 2.6f, 1.5f, 0.003f);
	frame.drawModel(sp_main, P, V, M, sources, -8.9f, 3.1f, -2.0f, 270.0f, 1.2f, 0.7f, 0.7f);
	//Painting1Room2
	painting2_1.drawModel(sp_l, P, V, M, sources, 2.77f, 3.2f, 11.0f, 270.0f, 2.3f, 1.8f, 0.003f);
	frameB.drawModel(sp_main, P, V, M, sources, 2.8f, 1.4f, 11.0f, -90.0f, 0.19f, 0.17f, 0.1f);
	//Painting2Room2
	painting2_2.drawModel(sp_l, P, V, M, sources, -8.87f, 3.2f, 11.0f, 270.0f, 2.3f, 1.8f, 0.003f);
	frameB.drawModel(sp_main, P, V, M, sources, -8.9f, 1.4f, 11.0f, 90.0f, 0.19f, 0.17f, 0.1f);
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0.533, 0.921, 0.964, 1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);

	sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
	sp_l = new ShaderProgram("v_lambert.glsl", NULL, "f_lambert.glsl");
	sp_main = new ShaderProgram("v_manysources.glsl", NULL, "f_manysources.glsl");
	sp_envmap = new ShaderProgram("v_envmap.glsl", NULL, "f_envmap.glsl");

	GLuint steelTex = readTexture("assets/materials/steel.png");
	GLuint wallTex = readTexture("assets/materials/wallwhite.png");
	GLuint skyTex = readTexture("assets/materials/clearsky.png");
	GLuint paintingTex1 = readTexture("assets/paintings/patterns/abstract1.png");
	GLuint paintingTex2 = readTexture("assets/paintings/patterns/painting1.png");
	GLuint paintingTex3 = readTexture("assets/paintings/patterns/abstract2.png");
	GLuint paintingTex4 = readTexture("assets/paintings/patterns/abstract3.png");
	GLuint paintingTex5 = readTexture("assets/paintings/patterns/abstract4.png");
	GLuint paintingTex2_1 = readTexture("assets/paintings/patterns/animal2.png");
	GLuint paintingTex2_2 = readTexture("assets/paintings/patterns/animal1.png");

	GLuint floorTex = readTexture("assets/materials/floor.png");
	GLuint refTex = readTexture("assets/materials/sky.png");
	GLuint frameTex = readTexture("assets/paintings/patterns/goldframe.png");
	GLuint fancyframeTex = readTexture("assets/materials/fancyframeDiffuse.png");
	GLuint postumentTex = readTexture("assets/materials/pedestal.png");
	GLuint doorTex = readTexture("assets/materials/door.png");
	GLuint signTex = readTexture("assets/materials/sign.png");

	GLuint visitorTex1 = readTexture("assets/materials/skins/skin1.png");
	GLuint visitorTex2 = readTexture("assets/materials/skins/skin2.png");
	GLuint visitorTex3 = readTexture("assets/materials/skins/skin5.png");
	GLuint visitorTex4 = readTexture("assets/materials/skins/skin6.png");


	blackBear.texture = steelTex;
	cer.texture = steelTex;
	//cer.texture_refl = refTex;
	room.texture = wallTex;
	//room2ndpart.texture = wallTex;
	sky.texture = skyTex;
	//Room1
	painting.texture = paintingTex1;
	frame.texture = frameTex;
	painting2.texture = paintingTex2;
	painting3.texture = paintingTex3;
	painting4.texture = paintingTex4;
	painting5.texture = paintingTex5;
	//Room2
	painting2_1.texture = paintingTex2_1;
	frameB.texture = fancyframeTex;
	painting2_2.texture = paintingTex2_2;
	frameB2.texture = fancyframeTex;

	corridor.texture = wallTex;
	transition.texture = wallTex;
	parquetry.texture = floorTex;
	postument.texture = postumentTex;
	door.texture = doorTex;
	visitor1.texture = visitorTex1;
	lion.texture = postumentTex;
	lion.texture_refl = refTex;
	brain.texture = frameTex;
	brain.texture_refl = refTex;
	exitSign.texture = signTex;
}

//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************
	delete sp;
	delete sp_l;
	delete sp_main;
	delete sp_envmap;
}

//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float kat_x, float kat_y) {
	//Kat x - kat gora gol jak podnosimy glowe ,kat y - jak skrecimy glowe lewo prawo
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V = glm::lookAt(pos, pos + calcDir(kat_x, kat_y), glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz widoku

	glm::mat4 P = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 0.01f, 300.0f); //Wylicz macierz rzutowania
	glm::mat4 M = glm::mat4(1.0f);
	
	allDrawInOnePlace(P, V, M);

	glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni
}


int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(1000, 1000, "Art Museum", NULL, NULL);  //Utwórz okno  i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	//Główna pętla
	float angle = 0; //zadeklaruj zmienną przechowującą aktualny kąt obrotu
	float kat_x = 0;
	float kat_y = 0;
	glfwSetTime(0); //Zeruj timer
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		
		kat_x += speed_x * glfwGetTime();
		kat_y += speed_y * glfwGetTime();
		pos += (float)(walk_speed * glfwGetTime()) * calcDir(kat_x, kat_y);//wylaczenie latania: zamiast katu X dac 0
		glfwSetTime(0); //Zeruj timer
		drawScene(window, kat_x, kat_y);

		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
