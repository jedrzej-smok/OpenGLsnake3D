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

float speed_x=0;
float speed_y=0;
float aspectRatio=1;
GLuint tex0; //Uchwyt � deklaracja globalna
GLuint tex1; //Uchwyt � deklaracja globalna, na druga teksture
GLuint bufVertex;//identyfiakator bufora z wierzcholkami
GLuint bufNormal;//identyfiakator bufora z normalnymi
GLuint bufTexCoord;//identyfiakator bufora z wspTeksturowania
int numberOfVerts;

ShaderProgram *sp;
std::vector<glm::vec4> verts;
std::vector<glm::vec4> norms;
std::vector<glm::vec2> texs;
std::vector<unsigned int> indices;

void loadModelAssimp(std::string path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
	std::cout << import.GetErrorString() << std::endl;
	if (scene->HasMeshes()) {
		std::cout << "scena ma meshes\n";
		std::cout << "scena ma textury:" << scene->mNumTextures << std::endl;
		for (int m = 0; m < scene->mNumMeshes; m++) {
			std::cout << "Mesh nr:" << m << ", ";
		}
	}
	else { std::cout << "scena nie ma meshy"; }
	

	aiMesh* mesh = scene->mMeshes[0];//mamy jeden model
	if(mesh->HasTextureCoords(0))
	{
		std::cout << "hasTexturecoords(0)";
	}
	unsigned int liczba_zest = mesh->GetNumUVChannels();
	unsigned int wymiar_wsp_tex = mesh->mNumUVComponents[0];
	std::cout << "liczba zestawow tekstur dla mesha:" << liczba_zest << std::endl;
	std::cout << "wymiar dla zestawu 0:" << wymiar_wsp_tex << std::endl;

	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i];
		verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));//bo wierzchoelk

		aiVector3D normal = mesh->mNormals[i];
		norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));//bo wektor


		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		texs.push_back(glm::vec2(texCoord.x, texCoord.y));
	}
	//mesh skalada sie z wielokatow, face to ten wielokat
	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++) {//numIndicies to liczba wierzchokow tworzacych jeden wielokat
			indices.push_back(face.mIndices[j]);
		}
	}
	std::cout << "mesha ma faces:" << mesh->mNumFaces << std:: endl;
	std::cout << "size verts:" << verts.size() << std:: endl;
	std::cout << "size norms:" << norms.size() << std:: endl;
	std::cout << "size texs:" << texs.size() << std:: endl;
	std::cout << "size indicies:" << indices.size() << std:: endl;
}


GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Wczytanie do pami�ci komputera
	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do kt�rych wczytamy wymiary obrazka
	//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import do pami�ci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
	//Wczytaj obrazek do pami�ci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}

//Odkomentuj, �eby rysowa� kostk�
//float* vertices = myCubeVertices;
//float* normals = myCubeNormals;
//float* texCoords = myCubeTexCoords;
//float* colors = myCubeColors;
//int vertexCount = myCubeVertexCount;



//Procedura obs�ugi b��d�w
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}


void keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods) {
    if (action==GLFW_PRESS) {
        if (key==GLFW_KEY_LEFT) speed_x=-PI/2;
        if (key==GLFW_KEY_RIGHT) speed_x=PI/2;
        if (key==GLFW_KEY_UP) speed_y=PI/2;
        if (key==GLFW_KEY_DOWN) speed_y=-PI/2;
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

//Procedura inicjuj�ca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, kt�ry nale�y wykona� raz, na pocz�tku programu************
	glClearColor(0,0,0,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	glfwSetKeyCallback(window,keyCallback);
	//Wczytanie i import obrazka � w initOpenGLProgram
	tex0 = readTexture("metal.png");
	tex1 = readTexture("sky.png");//druga tekstura

	sp=new ShaderProgram("v_simplest.glsl",NULL,"f_simplest.glsl");
	
	//NAJPIERW CZYTAJ OBIEKT XDDDDDDDDDDD
	//assimp
	loadModelAssimp(std::string("anvil.obj"));

	//VBO
	numberOfVerts = verts.size();
	glGenBuffers(1, &bufVertex);//liczba uchwytow do wygenerowania,wskaznik na dokad te uchwyty maja byc wygenerowane
	glBindBuffer(GL_ARRAY_BUFFER, bufVertex);//jakis typ, uchwyt ktory uaktywaniamy
	glBufferData(GL_ARRAY_BUFFER, numberOfVerts*sizeof(float) * 4, verts.data(), GL_STATIC_DRAW);//liczba bajtow,tablica z danymi, spsoob dostepu danyych

	glGenBuffers(1, &bufNormal);//liczba uchwytow do wygenerowania,wskaznik na dokad te uchwyty maja byc wygenerowane
	glBindBuffer(GL_ARRAY_BUFFER, bufNormal);//jakis typ, uchwyt ktory uaktywaniamy
	glBufferData(GL_ARRAY_BUFFER, numberOfVerts * sizeof(float) * 4, norms.data(), GL_STATIC_DRAW);//liczba bajtow,tablica z danymi, spsoob dostepu danyych

	glGenBuffers(1, &bufTexCoord);//liczba uchwytow do wygenerowania,wskaznik na dokad te uchwyty maja byc wygenerowane
	glBindBuffer(GL_ARRAY_BUFFER, bufTexCoord);//jakis typ, uchwyt ktory uaktywaniamy
	glBufferData(GL_ARRAY_BUFFER, numberOfVerts * sizeof(float) * 2, texs.data(), GL_STATIC_DRAW);//liczba bajtow,tablica z danymi, spsoob dostepu danyych
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);//sprzatanie
	
	
}


//Zwolnienie zasob�w zaj�tych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    //************Tutaj umieszczaj kod, kt�ry nale�y wykona� po zako�czeniu p�tli g��wnej************
	//Usuni�cie tekstury z pami�ci karty graficznej � w freeOpenGLProgram
	glDeleteTextures(1, &tex0);
	glDeleteTextures(1, &tex1);
    delete sp;
}




//Procedura rysuj�ca zawarto�� sceny
void drawScene(GLFWwindow* window,float angle_x,float angle_y) {
	//************Tutaj umieszczaj kod rysuj�cy obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V=glm::lookAt(
         glm::vec3(0, 0, -12.5),
         glm::vec3(0,0,0),
         glm::vec3(0.0f,1.0f,0.0f)); //Wylicz macierz widoku

    glm::mat4 P=glm::perspective(50.0f*PI/180.0f, aspectRatio, 0.01f, 50.0f); //Wylicz macierz rzutowania

    glm::mat4 M=glm::mat4(1.0f);
	M=glm::rotate(M,angle_y,glm::vec3(1.0f,0.0f,0.0f)); //Wylicz macierz modelu
	M=glm::rotate(M,angle_x,glm::vec3(0.0f,1.0f,0.0f)); //Wylicz macierz modelu

    sp->use();//Aktywacja programu cieniuj�cego
    //Przeslij parametry programu cieniuj�cego do karty graficznej
    glUniformMatrix4fv(sp->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"),1,false,glm::value_ptr(V));
    glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(M));

    glEnableVertexAttribArray(sp->a("vertex"));  //W��cz przesy�anie danych do atrybutu vertex
	glBindBuffer(GL_ARRAY_BUFFER, bufVertex);//chyba ten bufor aktywny
	glVertexAttribPointer(sp->a("vertex"),4,GL_FLOAT,false,0,NULL); //Wska� tablic� z danymi dla atrybutu vertex; zmiana na VBO

	glEnableVertexAttribArray(sp->a("normal"));  //W��cz przesy�anie danych do atrybutu normal
	glBindBuffer(GL_ARRAY_BUFFER, bufNormal);//chyba ten bufor aktywny
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, NULL); //Wska� tablic� z danymi dla atrybutu normal

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glBindBuffer(GL_ARRAY_BUFFER, bufTexCoord);//chyba ten bufor aktywny
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, NULL);//odpowiednia tablica

	glBindBuffer(GL_ARRAY_BUFFER, 0);//sprzatanie

	//CIENIOWANIE
	//Powi�zanie zmiennej typu sampler2D z jednostk� teksturuj�ca, tu zerowa jednostka teksturujaca z fs
	glUniform1i(sp->u("textureMap0"), 0); //drawScene
	
	//Przed glDrawArrays w drawScene Przypisanie tekstury do jednostki teksturuj�ce
	glActiveTexture(GL_TEXTURE0);//aktywacja jeddnostki teksturujacej
	glBindTexture(GL_TEXTURE_2D, tex0);//przypiasanie tekstury do jednostki

	glUniform1i(sp->u("textureMap1"), 1); //drawScene
	glActiveTexture(GL_TEXTURE1);//aktywacja jeddnostki teksturujacej
	glBindTexture(GL_TEXTURE_2D, tex1);//przypiasanie tekstury do jednostki

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //glDrawArrays(GL_TRIANGLES,0,numberOfVerts ); //Narysuj obiekt/
	//do Assimp
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data()); //PO ZMIANIA na VBO



    glDisableVertexAttribArray(sp->a("vertex"));  //Wy��cz przesy�anie danych do atrybutu vertex
	glDisableVertexAttribArray(sp->a("normal"));  //Wy��cz przesy�anie danych do atrybutu normal
	glDisableVertexAttribArray(sp->a("texCoord0"));

    glfwSwapBuffers(window); //Przerzu� tylny bufor na przedni
}


int main(void)
{
	GLFWwindow* window; //Wska�nik na obiekt reprezentuj�cy okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedur� obs�ugi b��d�w

	if (!glfwInit()) { //Zainicjuj bibliotek� GLFW
		fprintf(stderr, "Nie mo�na zainicjowa� GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Utw�rz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

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
	float angle_x=0; //Aktualny k�t obrotu obiektu
	float angle_y=0; //Aktualny k�t obrotu obiektu
	glfwSetTime(0); //Zeruj timer
	while (!glfwWindowShouldClose(window)) //Tak d�ugo jak okno nie powinno zosta� zamkni�te
	{
        angle_x+=speed_x*glfwGetTime(); //Zwi�ksz/zmniejsz k�t obrotu na podstawie pr�dko�ci i czasu jaki up�yna� od poprzedniej klatki
        angle_y+=speed_y*glfwGetTime(); //Zwi�ksz/zmniejsz k�t obrotu na podstawie pr�dko�ci i czasu jaki up�yna� od poprzedniej klatki
        glfwSetTime(0); //Zeruj timer
		drawScene(window,angle_x,angle_y); //Wykonaj procedur� rysuj�c�
		glfwPollEvents(); //Wykonaj procedury callback w zalezno�ci od zdarze� jakie zasz�y.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usu� kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zaj�te przez GLFW
	exit(EXIT_SUCCESS);
}

