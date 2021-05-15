/**dsdsdsdsdsdsdsd
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
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

float speed_x=0;
float speed_y=0;
float aspectRatio=1;
GLuint tex0; //Uchwyt – deklaracja globalna
GLuint tex1; //Uchwyt – deklaracja globalna, na druga teksture
ShaderProgram *sp;
std::vector<glm::vec4> verts;
std::vector<glm::vec4> norms;
std::vector<glm::vec2> texs;
std::vector<unsigned int> indices;
void loadModel(std::string path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

	aiMesh* mesh = scene->mMeshes[0];//mamy jeden model
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i];
		verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));//bo wierzchoelk

		aiVector3D normal = mesh->mNormals[i];
		norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));//bo wektor


		//unsigned int liczba_zest = mesh->GetNumUVChannels();
		//unsigned int wymiar_wsp_tex = mesh->mNumUVComponents[0];
		
		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		texs.push_back(glm::vec2(texCoord.x, texCoord.y));
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

}


GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Wczytanie do pamiêci komputera
	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
	//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import do pamiêci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
	//Wczytaj obrazek do pamiêci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}

//Odkomentuj, ¿eby rysowaæ kostkê
//float* vertices = myCubeVertices;
//float* normals = myCubeNormals;
//float* texCoords = myCubeTexCoords;
//float* colors = myCubeColors;
//int vertexCount = myCubeVertexCount;


//Odkomentuj, ¿eby rysowaæ czajnik
float* vertices = myTeapotVertices;
float* normals = myTeapotVertexNormals;
float* texCoords = myTeapotTexCoords;
float* colors = myTeapotColors;
int vertexCount = myTeapotVertexCount;

//ASSIMP
/*auto vertices = verts.data();
auto normals = norms.data();
auto texCoords = texs.data();
float* colors = myTeapotColors;
int vertexCount = myTeapotVertexCount;*/



//Procedura obs³ugi b³êdów
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

//Procedura inicjuj¹ca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który nale¿y wykonaæ raz, na pocz¹tku programu************
	glClearColor(0,0,0,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	glfwSetKeyCallback(window,keyCallback);
	//Wczytanie i import obrazka – w initOpenGLProgram
	tex0 = readTexture("handgun_fire.png");
	tex1 = readTexture("sky.png");//druga tekstura

	sp=new ShaderProgram("v_simplest.glsl",NULL,"f_simplest.glsl");
	
	//assimp
	//loadModel(std::string("Handgun_obj.obj"));
}


//Zwolnienie zasobów zajêtych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    //************Tutaj umieszczaj kod, który nale¿y wykonaæ po zakoñczeniu pêtli g³ównej************
	//Usuniêcie tekstury z pamiêci karty graficznej – w freeOpenGLProgram
	glDeleteTextures(1, &tex0);
	glDeleteTextures(1, &tex1);
    delete sp;
}




//Procedura rysuj¹ca zawartoœæ sceny
void drawScene(GLFWwindow* window,float angle_x,float angle_y) {
	//************Tutaj umieszczaj kod rysuj¹cy obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V=glm::lookAt(
         glm::vec3(0, 0, -2.5),
         glm::vec3(0,0,0),
         glm::vec3(0.0f,1.0f,0.0f)); //Wylicz macierz widoku

    glm::mat4 P=glm::perspective(50.0f*PI/180.0f, aspectRatio, 0.01f, 50.0f); //Wylicz macierz rzutowania

    glm::mat4 M=glm::mat4(1.0f);
	M=glm::rotate(M,angle_y,glm::vec3(1.0f,0.0f,0.0f)); //Wylicz macierz modelu
	M=glm::rotate(M,angle_x,glm::vec3(0.0f,1.0f,0.0f)); //Wylicz macierz modelu

    sp->use();//Aktywacja programu cieniuj¹cego
    //Przeslij parametry programu cieniuj¹cego do karty graficznej
    glUniformMatrix4fv(sp->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"),1,false,glm::value_ptr(V));
    glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(M));

    glEnableVertexAttribArray(sp->a("vertex"));  //W³¹cz przesy³anie danych do atrybutu vertex
    glVertexAttribPointer(sp->a("vertex"),4,GL_FLOAT,false,0,vertices); //Wska¿ tablicê z danymi dla atrybutu vertex

	glEnableVertexAttribArray(sp->a("color"));  //W³¹cz przesy³anie danych do atrybutu color
	glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, colors); //Wska¿ tablicê z danymi dla atrybutu color

	glEnableVertexAttribArray(sp->a("normal"));  //W³¹cz przesy³anie danych do atrybutu normal
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals); //Wska¿ tablicê z danymi dla atrybutu normal

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords);//odpowiednia tablica

	//Powi¹zanie zmiennej typu sampler2D z jednostk¹ teksturuj¹ca, tu zerowa jednostka teksturujaca z fs
	glUniform1i(sp->u("textureMap0"), 0); //drawScene
	
	//Przed glDrawArrays w drawScene Przypisanie tekstury do jednostki teksturuj¹ce
	glActiveTexture(GL_TEXTURE0);//aktywacja jeddnostki teksturujacej
	glBindTexture(GL_TEXTURE_2D, tex0);//przypiasanie tekstury do jednostki

	glUniform1i(sp->u("textureMap1"), 1); //drawScene
	glActiveTexture(GL_TEXTURE1);//aktywacja jeddnostki teksturujacej
	glBindTexture(GL_TEXTURE_2D, tex1);//przypiasanie tekstury do jednostki


    //glDrawArrays(GL_TRIANGLES,0,vertexCount); //Narysuj obiekt
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

    glDisableVertexAttribArray(sp->a("vertex"));  //Wy³¹cz przesy³anie danych do atrybutu vertex
	glDisableVertexAttribArray(sp->a("color"));  //Wy³¹cz przesy³anie danych do atrybutu color
	glDisableVertexAttribArray(sp->a("normal"));  //Wy³¹cz przesy³anie danych do atrybutu normal
	glDisableVertexAttribArray(sp->a("texCoord0"));

    glfwSwapBuffers(window); //Przerzuæ tylny bufor na przedni
}


int main(void)
{
	GLFWwindow* window; //WskaŸnik na obiekt reprezentuj¹cy okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurê obs³ugi b³êdów

	if (!glfwInit()) { //Zainicjuj bibliotekê GLFW
		fprintf(stderr, "Nie mo¿na zainicjowaæ GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

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
	float angle_x=0; //Aktualny k¹t obrotu obiektu
	float angle_y=0; //Aktualny k¹t obrotu obiektu
	glfwSetTime(0); //Zeruj timer
	while (!glfwWindowShouldClose(window)) //Tak d³ugo jak okno nie powinno zostaæ zamkniête
	{
        angle_x+=speed_x*glfwGetTime(); //Zwiêksz/zmniejsz k¹t obrotu na podstawie prêdkoœci i czasu jaki up³yna³ od poprzedniej klatki
        angle_y+=speed_y*glfwGetTime(); //Zwiêksz/zmniejsz k¹t obrotu na podstawie prêdkoœci i czasu jaki up³yna³ od poprzedniej klatki
        glfwSetTime(0); //Zeruj timer
		drawScene(window,angle_x,angle_y); //Wykonaj procedurê rysuj¹c¹
		glfwPollEvents(); //Wykonaj procedury callback w zaleznoœci od zdarzeñ jakie zasz³y.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuñ kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajête przez GLFW
	exit(EXIT_SUCCESS);
}

