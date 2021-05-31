/*
Niniejszy program jest wolnym oprogramowaniem; mo¿esz go
rozprowadzaæ dalej i / lub modyfikowaæ na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundacjê Wolnego
Oprogramowania - wed³ug wersji 2 tej Licencji lub(wed³ug twojego
wyboru) którejœ z póŸniejszych wersji.

Niniejszy program rozpowszechniany jest z nadziej¹, i¿ bêdzie on
u¿yteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyœlnej
gwarancji PRZYDATNOŒCI HANDLOWEJ albo PRZYDATNOŒCI DO OKREŒLONYCH
ZASTOSOWAÑ.W celu uzyskania bli¿szych informacji siêgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnoœci¹ wraz z niniejszym programem otrzyma³eœ te¿ egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeœli nie - napisz do Free Software Foundation, Inc., 59 Temple
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
#include <iostream>
#include <stdio.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myCube.h"
#include "myTeapot.h"
#include <assimp/Importer.hpp> //wczytuje plik
#include <assimp/scene.h> //scene to reprezentacja calego pliku
#include <assimp/postprocess.h> //to co mozna zrobic z plikiem
#include <myModel3D.h>
#include <myModel3DfewMeshes.h>
#include <Windows.h>


ShaderProgram* sp;

float aspectRatio = 1;
double frameTime=0;
float speed_x = 0;
float speed_y = 0;
float speed_z = 0;
glm::mat4 Vglobal = glm::lookAt(glm::vec3(0, 25, -22.5), glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz widoku
glm::mat4 Pglobal = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 0.01f, 50.0f); //Wylicz macierz rzutowania
glm::mat4 Mglobal = glm::mat4(1.0f);
bool collisionWithApple = false;
float ballSize = 2.0f;

//modele============================================================
myModel3D head;
myModel3D ball;
myModel3D tail;
std::vector<std::vector<float>> tailCoords;
std::vector<std::vector<float>> flexions;


//Procedura obs³ugi b³êdów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods) {
    if (action==GLFW_PRESS) {
        if (key==GLFW_KEY_LEFT) speed_x=PI/2;
        if (key==GLFW_KEY_RIGHT) speed_x=-PI/2;
        if (key==GLFW_KEY_UP) speed_y=1.f;
        if (key==GLFW_KEY_DOWN) speed_y=-1.f;
        if (key==GLFW_KEY_A) collisionWithApple=true;
    }
    if (action==GLFW_RELEASE) {
        if (key==GLFW_KEY_LEFT) speed_x=0;
        if (key==GLFW_KEY_RIGHT) speed_x=0;
        if (key==GLFW_KEY_UP) speed_y=0;
        if (key==GLFW_KEY_DOWN) speed_y=0;
    }
}

void windowResizeCallback(GLFWwindow* window,int width,int height) {
    if (height==0) return;
    aspectRatio=(float)width/(float)height;
    glViewport(0,0,width,height);
}

//Procedura inicjuj¹ca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który nale¿y wykonaæ raz, na pocz¹tku programu************
	glClearColor(0,0,0,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	glfwSetKeyCallback(window,keyCallback);
	sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
	sp->use();
	//firstModel.initModel("anvil.obj", "metal.png","sky.png");
	//firstModel.initModel("modele/ball.obj", "modele/zielony.png", "modele/sky.png");
	//tmpModel.initModel("modele/dinoTest.obj", "modele/aroy.png", "modele/sky.png");
	
	ball.initModel("modele/ball.obj", "modele/zielony.png", "modele/sky.png");
	ball.setupModel(0, 0, PI, 0, 0, 0, 0.25f, 0.25f, 0.25f);

	head.initModel("modele/minidragon.obj", "modele/minidragon.png", "modele/sky.png");
	head.setupModel(0,2.f, 2.f, PI, 0, 0, 0.8f, 0.8f, 0.8f);
	
	tail.setupModel(0, 0, 0, 0, 0, 0, 1, 1, 1);
	tail.initModel("modele/ball.obj", "modele/zielony.png", "modele/sky.png");

}


//Zwolnienie zasobów zajêtych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    //************Tutaj umieszczaj kod, który nale¿y wykonaæ po zakoñczeniu pêtli g³ównej************
	
	ball.freeModel();
	head.freeModel();
	tail.freeModel();
	//===========
	delete sp;
}




//Procedura rysuj¹ca zawartoœæ sceny
void drawScene(GLFWwindow* window) {
	//************Tutaj umieszczaj kod rysuj¹cy obraz******************
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//wlasciwa czesc ============================================================
	
	ball.drawModel(Vglobal, Pglobal, Mglobal, (sin(ball.angle_x))*speed_y,0, (cos(ball.angle_x))*speed_y, speed_x*frameTime, 0, 0, 1, 1, 1);
	head.drawModel(Vglobal, Pglobal, ball.matrixM, 0,0,0,0, 0, 0, 1,1,1);
	
	
	
	//zapisz miejsce ball
	flexions.insert(flexions.begin(), std::vector<float>{ball.coord_x, ball.coord_y,ball.coord_z, ball.angle_x});
	//std::cout << "coord_x:" << ball.coord_x << ", coord_z:" << ball.coord_z << "\n";
	std::cout << tailCoords.size()<<"\n";
	for (int t = 0; t < tailCoords.size(); t++) {
		if (flexions.size() >= t) {
			//std::cout <<"t: "<<t<<"; "<< flexions[t][0] << "," << flexions[t][1] << "," << flexions[t][2] << "," << flexions[t][3] << "\n";
			tail.setupModel(flexions[t][0], flexions[t][1], flexions[t][2], flexions[t][3], 0, 0, 1.f, 1.f, 1.f);
			tail.drawModel(Vglobal, Pglobal, Mglobal,0,0,0,0, 0, 0, 1, 1, 1);
		}
	}
	
	//std::cout << "balls[0].angle_x: " << balls[0].angle_x << "\n";
	//std::cout << "-sin(balls[0].angle_x)*ballSize:" << -sin(balls[0].angle_x) * ballSize << "\n";
	//std::cout << "-cos(balls[0].angle_x)*ballSize:" << -cos(balls[0].angle_x) * ballSize << "\n\n\n";



//reszta=====================================================================
    glfwSwapBuffers(window); //Przerzuæ tylny bufor na przedni
}
void addball() {
	if (collisionWithApple) {
		for (int o = 0; o < 1; o++) {
			tailCoords.push_back(std::vector<float>{ 0, 0, 0 });
		}
	}
	collisionWithApple = false;
	
}

int main(void)
{
	GLFWwindow* window; //WskaŸnik na obiekt reprezentuj¹cy oknom

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurê obs³ugi b³êdów

	if (!glfwInit()) { //Zainicjuj bibliotekê GLFW
		fprintf(stderr, "Nie mo¿na zainicjowaæ GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(800, 800, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Je¿eli okna nie uda³o siê utworzyæ, to zamknij program
	{
		fprintf(stderr, "Nie mo¿na utworzyæ okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje siê aktywny i polecenia OpenGL bêd¹ dotyczyæ w³aœnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekê GLEW
		fprintf(stderr, "Nie mo¿na zainicjowaæ GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjuj¹ce

	//G³ówna pêtla
	glfwSetTime(0); //Zeruj timer
	while (!glfwWindowShouldClose(window)) //Tak d³ugo jak okno nie powinno zostaæ zamkniête
	{
		frameTime = glfwGetTime();
		glfwSetTime(0); //Zeruj timer
		drawScene(window); //Wykonaj procedurê rysuj¹c¹
		glfwPollEvents(); //Wykonaj procedury callback w zaleznoœci od zdarzeñ jakie zasz³y.
		
		//=======================================
		//najpierw detect potem
		addball();
		Sleep(3000.f / 60.f);
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuñ kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajête przez GLFW
	exit(EXIT_SUCCESS);
}

