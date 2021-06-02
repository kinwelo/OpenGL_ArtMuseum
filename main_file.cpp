
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
#include "myTeapot.h"
#include "Object3D.h"
#include <SecondMethodDrawing.h>
#include <RoomMethodDrawing.h>
#include <SkyDrawingMethod.h>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>


//Speed+window parameters
float speed_x = 0;
float speed_y = 0;
float aspectRatio = 1;
float walk_speed = 0;

//FPS free fly
//WASD - obroty kamerą
//Strzalka gora dol = latanie do przodu/tylu

//Polozenie poczatkowe: pozycja gracza i źródeł światła
glm::vec3 pos = glm::vec3(0, 2, -0);
glm::vec4 zrSwiatla = glm::vec4(-7, 10, 0, 1);
glm::vec4 zrSwiatla2 = glm::vec4(2, 3, 30.0, 1);
//glm::vec4 zrSwiatla = glm::vec4(pos, 1);





//All Shaders
ShaderProgram* sp;
ShaderProgram* sp_l;
ShaderProgram* sp_main;



//Odkomentuj, żeby rysować kostkę
float* vertices = myCubeVertices;
float* normals = myCubeNormals;
float* texCoords = myCubeTexCoords;
int vertexCount = myCubeVertexCount;


//All Textures
GLuint tex0;



//All models
SecondMethodDrawing blackBear("assets/BlackBear/BlackBear.obj"),
	cer("assets/cer/cer.obj");
RoomMethodDrawing room("assets/gallery/Museum.obj"), room2ndpart("assets/gallery/Museum.obj");
SkyDrawingMethod sky("assets/scene/Egg.obj");
SecondMethodDrawing painting("assets/paintings/canvas.obj");


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
	room2ndpart.drawModel(sp_l, P, V, M, zrSwiatla, -9.0f, 1.0f, 58.0f, 360.0f, 0.002f, 0.003f, 0.003f);
	blackBear.drawModel(sp, P, V, M, zrSwiatla, zrSwiatla2, -2.4f, 1.0f, 27.0f, 180.0f, 0.1f, 0.1f, 0.1f);
	cer.drawModel(sp, P, V, M, zrSwiatla, zrSwiatla2, -7.0f, 1.0f, 10.0f, 50.0f, 0.3f, 0.3f, 0.3f);
	room.drawModel(sp_l, P, V, M, zrSwiatla, 3.0f, 1.0f, -5.0f, 180.0f, 0.002f, 0.003f, 0.003f);
	sky.drawModel(sp_l, P, V, M, zrSwiatla, -3.0f, -40.0f, 20.0f, 360.0f, 1.0f, 1.6, 1.6);
	painting.drawModel(sp, P, V, M, zrSwiatla, zrSwiatla2, 2.8f, 2.5f, 5.0f, 90.0f, 1.0f, 1.0f, 0.001f);

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

	GLuint steelTex = readTexture("assets/materials/steel.png");
	GLuint wallTex = readTexture("assets/materials/wallwhite.png");
	GLuint skyTex = readTexture("assets/materials/clearsky.png");
	GLuint paintingTex1 = readTexture("assets/paintings/patterns/test.png");
	blackBear.texture = steelTex;
	cer.texture = steelTex;
	room.texture = wallTex;
	room2ndpart.texture = wallTex;
	sky.texture = skyTex;
	painting.texture = paintingTex1;
}



//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************

	delete sp;
	delete sp_l;
}




//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float kat_x, float kat_y) {
	//Kat x - kat gora gol jak podnosimy glowe ,kat y - jak skrecimy glowe lewo prawo
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V = glm::lookAt(pos, pos + calcDir(kat_x, kat_y), glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz widoku
		//glm::vec3(0, 0, -10),//na z oddalanie od obiektu
		//glm::vec3(0, 0, 0),
		//glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz widoku

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

	window = glfwCreateWindow(1000, 1000, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

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
	//float angle_x = 0; //Aktualny kąt obrotu obiektu
	//float angle_y = 0; //Aktualny kąt obrotu obiektu
	glfwSetTime(0); //Zeruj timer
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		//angle_x += speed_x * glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
		//angle_y += speed_y * glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
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
