
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
#include "tt.h"
#include "Object3D.h"
#include <OBJ_Loader.h>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

float speed_x = 0;
float speed_y = 0;
float aspectRatio = 1;

//FPS free fly
//WASD - obroty kamerą
//Strzalka gora dol = latanie do przodu/tylu
glm::vec3 pos = glm::vec3(0, 2, -15);
float walk_speed = 0;

glm::vec3 calcDir(float kat_x, float kat_y) {
	glm::vec4 dir = glm::vec4(0, 0, 1, 0);
	glm::mat4 M = glm::rotate(glm::mat4(1.0f), kat_y, glm::vec3(0, 1, 0));
	M = glm::rotate(M, kat_x, glm::vec3(1, 0, 0));
	dir = M * dir;
	return glm::vec3(dir); //wlasnosc wektora 3 liczbowego ucina wspolrzedne w z czteroliczbowych
}


ShaderProgram* sp;


//Odkomentuj, żeby rysować kostkę
float* vertices = myCubeVertices;
float* normals = myCubeNormals;
float* texCoords = myCubeTexCoords;
int vertexCount = myCubeVertexCount;




////Odkomentuj, żeby rysować czajnik
//float* vertices = myTeapotVertices;
///* float* normals = myTeapotNormals;
//float* normals = myTeapotVertexNormals;//w tej formie mozna czesto wyeksportowac wektory normalnie w blenderze
//float* texCoords = myTeapotTexCoords;
//float* colors = myTeapotColors;
////int vertexCount = myTeapotVertexCount;
//

//Test na konsultacje model.obj proba przerobienia na odpowiednie tablice

//float* vertices = myVerts1;
//float* normals = myVertNormals1;
//float* texCoords = mytexCoords1;
//int vertexCount =VertCount1;

GLuint tex0;

Object3D blackBear, cer;


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
		if (key == GLFW_KEY_UP) walk_speed = 2;
		if (key == GLFW_KEY_DOWN) walk_speed = -2;

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

void drawTestObject(glm::mat4 P,glm::mat4 V, glm::mat4 M, glm::vec4 lightSource) {
	
	M = glm::translate(M, glm::vec3(0.0f, 1.0f, -3.0f));
	M = glm::rotate(M, 180*PI/180, glm::vec3(0.0f, 1.0f, 0.0f));
	sp->use();//Aktywacja programu cieniującego
	//Przeslij parametry programu cieniującego do karty graficznej
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));


	glUniform4fv(sp->u("lp"), 1, glm::value_ptr(lightSource));

	glEnableVertexAttribArray(sp->a("vertex"));  //Włącz przesyłanie danych do atrybutu vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, blackBear.verts.data()); //Wskaż tablicę z danymi dla atrybutu vertex

	glEnableVertexAttribArray(sp->a("normal"));  //Włącz przesyłanie danych do atrybutu normal
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, blackBear.norms.data()); //Wskaż tablicę z danymi dla atrybutu normal
															// Dlaczego przez blackBear.getNorms().data() nie działa?

	glEnableVertexAttribArray(sp->a("texCoord0"));  //Włącz przesyłanie danych do atrybutu normal
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, blackBear.textureCoords.data()); //Wskaż tablicę z danymi dla atrybutu normal

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);

	//glDrawArrays(GL_TRIANGLES, 0, vertexCount); //Narysuj obiekt
	glDrawElements(GL_TRIANGLES, blackBear.indices.size(), GL_UNSIGNED_INT, blackBear.indices.data());

	glDisableVertexAttribArray(sp->a("vertex"));  //Wyłącz przesyłanie danych do atrybutu vertex
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord0"));


	M = glm::translate(M, glm::vec3(7.0f, 0.0f, 5.0f));
	M = glm::rotate(M, 230 * PI / 180, glm::vec3(0.0f, 1.0f, 0.0f));
	sp->use();//Aktywacja programu cieniującego
	//Przeslij parametry programu cieniującego do karty graficznej
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));


	glUniform4fv(sp->u("lp"), 1, glm::value_ptr(lightSource));

	glEnableVertexAttribArray(sp->a("vertex"));  //Włącz przesyłanie danych do atrybutu vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, cer.verts.data()); //Wskaż tablicę z danymi dla atrybutu vertex

	glEnableVertexAttribArray(sp->a("normal"));  //Włącz przesyłanie danych do atrybutu normal
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, cer.norms.data()); //Wskaż tablicę z danymi dla atrybutu normal
															// Dlaczego przez blackBear.getNorms().data()

	glEnableVertexAttribArray(sp->a("texCoord0"));  //Włącz przesyłanie danych do atrybutu normal
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, cer.textureCoords.data()); //Wskaż tablicę z danymi dla atrybutu normal

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);

	//glDrawArrays(GL_TRIANGLES, 0, vertexCount); //Narysuj obiekt
	glDrawElements(GL_TRIANGLES, cer.indices.size(), GL_UNSIGNED_INT, cer.indices.data());

	glDisableVertexAttribArray(sp->a("vertex"));  //Wyłącz przesyłanie danych do atrybutu vertex
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord0"));

}


//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0.533, 0.921, 0.964, 1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);

	sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
	tex0 = readTexture("assets/materials/steel.png");
	
	blackBear.loadModel(std::string("assets/BlackBear/BlackBear.obj"));
	cer.loadModel(std::string("assets/cer/cer.obj"));
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************

	delete sp;
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

	glm::mat4 P = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 0.01f, 50.0f); //Wylicz macierz rzutowania
	glm::vec4 zrSwiatla = glm::vec4(0, 20, -20, 1);
	glm::mat4 M = glm::mat4(1.0f);


	drawTestObject(P,V,M,zrSwiatla);
	

	glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni
}


int main(void)
{
	//objl::Loader Loader;
	//// Load .obj File
	//bool loadout = Loader.LoadFile("model.obj");

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
//drawScene(window, angle_x, angle_y); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
