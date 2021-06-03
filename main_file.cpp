/*
Niniejszy program jest wolnym oprogramowaniem; mo�esz go
rozprowadza� dalej i / lub modyfikowa� na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundacj� Wolnego
Oprogramowania - wed�ug wersji 2 tej Licencji lub(wed�ug twojego
wyboru) kt�rej� z p�niejszych wersji.

Niniejszy program rozpowszechniany jest z nadziej�, i� b�dzie on
u�yteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domy�lnej
gwarancji PRZYDATNO�CI HANDLOWEJ albo PRZYDATNO�CI DO OKRE�LONYCH
ZASTOSOWA�.W celu uzyskania bli�szych informacji si�gnij do
Powszechnej Licencji Publicznej GNU.

Z pewno�ci� wraz z niniejszym programem otrzyma�e� te� egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
je�li nie - napisz do Free Software Foundation, Inc., 59 Temple
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
glm::mat4 Vglobal = glm::lookAt(glm::vec3(0, 30, -32.5), glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.5f)); //Wylicz macierz widoku
glm::mat4 Pglobal = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 0.01f, 200.0f); //Wylicz macierz rzutowania
glm::mat4 Mglobal = glm::mat4(1.0f);
bool collisionWithApple = false;
float distCollisionHead = 3.0f;
float distCollisionApple = 2.0f;
float distRandApple = 4.0f;
bool existingApple = false;
int weight = 800;
int height = 800;
int cnt = 0;
float abc = 0.f;
glm::vec3 posCamera = glm::vec3(0, 2, -31);

//modele============================================================
myModel3D head;
myModel3D ball;
myModel3D tail;
myModel3D apple;
myModel3D ground;

float cameraSpeedX = 0.f;
float cameraSpeedY = 0.f;

float cameraWalkSpeed = 0.f;
float cameraAngleX = 0.f;
float cameraAngleY = 0.f;

//wyskosc kamery
float cameraSpeedHeight = 0.f;
float cameraHeight = 2.0f;

glm::vec3 toRed(-20,18,11);
glm::vec3 toYellow(20,18,11);

std::vector<std::vector<float>> tailCoords;
std::vector<std::vector<float>> flexions;


//Procedura obs�ugi b��d�w
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods) {
    if (action==GLFW_PRESS) {
        if (key==GLFW_KEY_LEFT) speed_x=PI/2;
        if (key==GLFW_KEY_RIGHT) speed_x=-PI/2;
        if (key==GLFW_KEY_UP) speed_y=1.f;
        if (key==GLFW_KEY_DOWN) speed_y=-1.f;
        if (key==GLFW_KEY_Q) collisionWithApple=true;
		
		//kamera
		 
		//obrot lewo prawo
		if (key == GLFW_KEY_A) cameraSpeedY = 0.5f;
		if (key == GLFW_KEY_D) cameraSpeedY = -0.5f;
		//przesuwanie kamery w plaszczyznie xz
		if (key == GLFW_KEY_W) cameraWalkSpeed = 50.f;
		if (key == GLFW_KEY_S) cameraWalkSpeed = -50.f;
		//obrot kamery gora dol
		if (key == GLFW_KEY_PAGE_DOWN) cameraSpeedX = 0.5f;
		if (key == GLFW_KEY_PAGE_UP) cameraSpeedX = -0.5f;
		//wysokosc kamery
		if (key == GLFW_KEY_Z) cameraSpeedHeight = 5.f;
		if (key == GLFW_KEY_X) cameraSpeedHeight = -5.f;
    }
    if (action==GLFW_RELEASE) {
        if (key==GLFW_KEY_LEFT) speed_x=0;
        if (key==GLFW_KEY_RIGHT) speed_x=0;
        if (key==GLFW_KEY_UP) speed_y=0;
        if (key==GLFW_KEY_DOWN) speed_y=0;

		//kamera
		if (key == GLFW_KEY_A) cameraSpeedY = 0;
		if (key == GLFW_KEY_D) cameraSpeedY = 0;

		if (key == GLFW_KEY_W) cameraWalkSpeed = 0;
		if (key == GLFW_KEY_S) cameraWalkSpeed = 0;

		if (key == GLFW_KEY_PAGE_DOWN) cameraSpeedX = 0;
		if (key == GLFW_KEY_PAGE_UP) cameraSpeedX = 0;

		if (key == GLFW_KEY_Z) cameraSpeedHeight = 0;
		if (key == GLFW_KEY_X) cameraSpeedHeight = 0;
	}
}

glm::vec3 calcDir(float kat_x, float kat_y) {
	glm::vec4 dir = glm::vec4(0, 0, 1, 0);
	glm::mat4 M = glm::rotate(glm::mat4(1.0f), kat_y, glm::vec3(0, 1, 0));
	M = glm::rotate(M, kat_x, glm::vec3(1, 0, 0));
	dir = M * dir;
	return glm::vec3(dir);
}

void moveCamera(float time) {
	cameraAngleX += cameraSpeedX * time;
	cameraAngleY += cameraSpeedY * time;
	cameraHeight += cameraSpeedHeight * time;
	posCamera += cameraWalkSpeed * time * calcDir(cameraSpeedX, cameraAngleY);
	posCamera.y = cameraHeight;
	Vglobal = glm::lookAt(posCamera, posCamera+calcDir(cameraAngleX, cameraAngleY), glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz widoku
}
void windowResizeCallback(GLFWwindow* window,int width,int height) {
    if (height==0) return;
    aspectRatio=(float)width/(float)height;
    glViewport(0,0,width,height);
}

//Procedura inicjuj�ca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, kt�ry nale�y wykona� raz, na pocz�tku programu************
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
	head.setupModel(0,5.f, 2.f, PI, 0, 0, 0.8f, 0.8f, 0.8f);
	
	tail.setupModel(0, 0, 0, 0, 0, 0, 1, 1, 1);
	tail.initModel("modele/ball.obj", "modele/zielony.png", "modele/sky.png");

	apple.initModel("modele/apple3.obj", "modele/apple3.png", "modele/sky.png");
	apple.setupModel(-10, 0, 20, PI / 2, 0, 0, 2.f, 2.f, 2.f);

	ground.initModel("modele/sand/sand.obj", "modele/sand/Sand_color.png", "modele/sand/Sand_rough.png");
	ground.setupModel(0, -3.7f, 0, 0, 0, 0, 50.f, 1.f, 50.f);

}


//Zwolnienie zasob�w zaj�tych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    //************Tutaj umieszczaj kod, kt�ry nale�y wykona� po zako�czeniu p�tli g��wnej************
	
	ball.freeModel();
	head.freeModel();
	tail.freeModel();
	apple.freeModel();
	ground.freeModel();
	//===========
	delete sp;
}

bool bendSnake() {
	float dist;
	dist = sqrt((apple.world_coord_x - head.world_coord_x) * (apple.world_coord_x - head.world_coord_x) +
		(apple.world_coord_y - head.world_coord_y) * (apple.world_coord_y - head.world_coord_y) +
		(apple.world_coord_z - head.world_coord_z) * (apple.world_coord_z - head.world_coord_z));
	//zderzy sie ze soba
	if (dist < distCollisionApple) {
		//std::cout << "\n bedzie jablko\n";
		return true;
	}
	return false;
}

void A(float* a) {

	
	if (cnt%40<20) {
		*a += 0.05f;
	}
	else {
		*a -= 0.05f;
	}
}

//Procedura rysuj�ca zawarto�� sceny
void drawScene(GLFWwindow* window) {
	//************Tutaj umieszczaj kod rysuj�cy obraz******************
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//wlasciwa czesc ============================================================
	ball.setLightRed(posCamera + toRed);
	head.setLightRed(posCamera + toRed);
	tail.setLightRed(posCamera + toRed);
	ground.setLightRed(posCamera + toRed);

	ball.setLightYellow(posCamera + toYellow);
	head.setLightYellow(posCamera + toYellow);
	tail.setLightYellow(posCamera + toYellow);
	ground.setLightYellow(posCamera + toYellow);



	ball.drawModel(Vglobal, Pglobal, Mglobal, (sin(ball.angle_x))*speed_y,0, (cos(ball.angle_x))*speed_y, speed_x*frameTime, 0, 0, 1, 1, 1);
	if (bendSnake()&&cnt>10) {
		head.angle_y = -PI / 4;
	}
	head.drawModel(Vglobal, Pglobal, ball.matrixM, 0, 0, 0, 0, 0, 0, 1, 1, 1);
	if (bendSnake() && cnt > 10) {
		head.angle_y = 0;
	}
	apple.coord_y = abc;
	apple.drawModel(Vglobal, Pglobal, Mglobal, 0, 0, 0, PI/90, 0, 0, 1, 1, 1);
	
	ground.drawModel(Vglobal, Pglobal, Mglobal, 0, 0, 0, 0, 0, 0, 1, 1, 1);
	
	
	//zapisz miejsce ball
	flexions.insert(flexions.begin(), std::vector<float>{ball.coord_x, ball.coord_y,ball.coord_z, ball.angle_x});
	//std::cout << "coord_x:" << ball.coord_x << ", coord_z:" << ball.coord_z << "\n";
	//std::cout <<"tail size:"<< tailCoords.size()<<"\n";
	
	for (int t = 0; t < tailCoords.size(); t++) {
		if (flexions.size() >= t) {
			//std::cout <<"t: "<<t<<"; "<< flexions[t][0] << "," << flexions[t][1] << "," << flexions[t][2] << "," << flexions[t][3] << "\n";
			tail.setupModel(flexions[t][0], flexions[t][1], flexions[t][2], flexions[t][3], 0, 0, 1.f, 1.f, 1.f);
			tail.drawModel(Vglobal, Pglobal, Mglobal,0,0,0,0, 0, 0, 1, 1, 1);
			//zapamietaj wspl segmentow
			tailCoords[t] = std::vector<float>{tail.world_coord_x, tail.world_coord_y, tail.world_coord_z};
			//std::cout << tailCoords[t][0]<<"\n";
		}
	}
	
	
	
//reszta=====================================================================
    glfwSwapBuffers(window); //Przerzu� tylny bufor na przedni
}
void addball() {
	if (collisionWithApple) {
		for (int o = 0; o < 1; o++) {
			tailCoords.push_back(std::vector<float>{ 1000000000, 1000000000, 1000000000 });
		}
	}
	collisionWithApple = false;
	
}
void detectCollisionItself() {
	float d;
	for (int t = 6; t < tailCoords.size(); t++) {
		d = sqrt((tailCoords[t][0] - head.world_coord_x)* (tailCoords[t][0] - head.world_coord_x) + 
			(tailCoords[t][1] - head.world_coord_y) * (tailCoords[t][1] - head.world_coord_y) +
			(tailCoords[t][2] - head.world_coord_z) * (tailCoords[t][2] - head.world_coord_z));
		//zderzy sie ze soba
		if (d < distCollisionHead) {
			std::cout << "zderzenie ze soba head:";
			std::cout << head.world_coord_x << ", " << head.coord_z << ", blok: x:" << tailCoords[t][0] << ", " << tailCoords[t][2] << "\n";
			std::cout << " odleglosc: " << d << ";\t";
			std::cout << "\nt:" << t << std::endl;
			Sleep(1000);
			exit(0);
		}
	}
}

void randApple() {
	if (collisionWithApple == true) {
		//zostalo zjedzonee
		int x=rand()%20-10, z=rand()%20-10;
		float dist;
		bool out = false;
		do {
			out = false;
			x = rand() % 20 - 10;
			z = rand() % 200;

			dist = sqrt((head.world_coord_x - x) * (head.world_coord_x - x) +
				(head.world_coord_z - z) * (head.world_coord_z - z));
			//zderzy sie ze soba
			if (dist < distRandApple) {
				out = true;
			}
			dist = sqrt((ball.world_coord_x - x) * (ball.world_coord_x - x) +
				(ball.world_coord_z - z) * (ball.world_coord_z - z));
			//zderzy sie ze soba
			if (dist < distRandApple) {
				out = true;
			}

			for (int t = 0; t < tailCoords.size(); t++) {
				dist = sqrt((tailCoords[t][0] - x) * (tailCoords[t][0] - x) +
					(tailCoords[t][2] - z) * (tailCoords[t][2] - z));
				//zderzy sie ze soba
				if (dist < distRandApple) {
					break;
				}
			}
		} while (dist < distRandApple || out);//zeby nie bylo na wezu
		
		
		apple.setupModel((float)x, 0, (float)z, 0, 0, 0, 2.f, 2.f, 2.f);


	}
}

void detectCollisionApple() {
	float dist;
	dist = sqrt((apple.world_coord_x - head.world_coord_x) * (apple.world_coord_x - head.world_coord_x) +
			(apple.world_coord_y - head.world_coord_y) * (apple.world_coord_y - head.world_coord_y) +
			(apple.world_coord_z - head.world_coord_z) * (apple.world_coord_z - head.world_coord_z));
		//zderzy sie ze soba
		if (dist < distCollisionApple) {
			std::cout << "\n najechane jablko\n";
			
			collisionWithApple = true;//powieksz weza
			
		}
		randApple();
}



int main(void)
{
	GLFWwindow* window; //Wska�nik na obiekt reprezentuj�cy oknom

	glfwSetErrorCallback(error_callback);//Zarejestruj procedur� obs�ugi b��d�w

	if (!glfwInit()) { //Zainicjuj bibliotek� GLFW
		fprintf(stderr, "Nie mo�na zainicjowa� GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(weight, height, "OpenGL", NULL, NULL);  //Utw�rz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Je�eli okna nie uda�o si� utworzy�, to zamknij program
	{
		fprintf(stderr, "Nie mo�na utworzy� okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje si� aktywny i polecenia OpenGL b�d� dotyczy� w�a�nie jego.
	glfwSwapInterval(1); //Czekaj na 1 powr�t plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotek� GLEW
		fprintf(stderr, "Nie mo�na zainicjowa� GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjuj�ce

	//G��wna p�tla
	glfwSetTime(0); //Zeruj timer
	while (!glfwWindowShouldClose(window)) //Tak d�ugo jak okno nie powinno zosta� zamkni�te
	{
		frameTime = glfwGetTime();
		glfwSetTime(0); //Zeruj timer
		moveCamera((float)frameTime);
		drawScene(window); //Wykonaj procedur� rysuj�c�
		glfwPollEvents(); //Wykonaj procedury callback w zalezno�ci od zdarze� jakie zasz�y.
		
		
		//=======================================
		//najpierw detect apple potem
		
		
		detectCollisionApple();
		addball();
		
		//czy zderzy sie ze soba
		
		detectCollisionItself();
		

		Sleep(3000.f / 60.f);
		cnt++;
		A(&abc);
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usu� kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zaj�te przez GLFW
	exit(EXIT_SUCCESS);
}

