#pragma once
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
#include <assimp/Importer.hpp> //wczytuje plik
#include <assimp/scene.h> //scene to reprezentacja calego pliku
#include <assimp/postprocess.h> //to co mozna zrobic z plikiem
class myModel3D
{
	public:
		myModel3D() {
		};
		~myModel3D(){
		}
		//odczytanie tekstur
		GLuint readTexture(const char* filename);//odczytanie jakiejs tekstury

		//?adowanie modelu 
		std::vector<glm::vec4> verts;
		std::vector<glm::vec4> norms;
		std::vector<glm::vec2> texs;
		std::vector<unsigned int> indices;
		int numberOfVerts;
		int numberOfIndex;
		int numberOfMeshes;
		int numberOfTexs;
		glm::vec4 lightposition1;
		glm::vec4 lightposition2;
		void loadModelAssimp(std::string path);

		//funckcje z initOpenGLProgram
		//uchwyty zwiazane z modelem
		GLuint tex0; //Uchwyt ? deklaracja globalna
		GLuint tex1; //Uchwyt ? deklaracja globalna, na druga teksture
		GLuint bufVertex;//identyfiakator bufora z wierzcholkami
		GLuint bufNormal;//identyfiakator bufora z normalnymi
		GLuint bufTexCoord;//identyfiakator bufora z wspTeksturowania
		GLuint bufIndex;//identyfiakator bufora z indeksami
		ShaderProgram* sp;
		void initModel(const char* pathObj, const char* pathTex0, const char* pathTex1);
		void freeModel();

		//funckje rysujace
		//zmienne i macierze dla wszystkich te same
		float aspectRatio = 1;
		
		//zmienne od animacji
		glm::mat4 matrixM;
		float coord_x = 0;
		float coord_y = 0;
		float coord_z = 0;

		float angle_x=0;
		float angle_y=0;
		float angle_z=0;

		float scale_x = 1.0f;
		float scale_y = 1.0f;
		float scale_z = 1.0f;

		//zmienne od obliczen, detekcji i prawdziwych danych o obiekcie
		float world_coord_x = 0;
		float world_coord_y = 0;
		float world_coord_z = 0;

		void setupModel(float moveX = 0.f, float moveY = 0.f, float moveZ = 0.f,
			float rotationX = 0.f, float rotationY = 0.f, float rotationZ = 0.f,
			float rescaleX = 1.0f, float rescaleY = 1.0f, float rescaleZ = 1.0f );
		void setLightYellow(glm::vec3 yellow);
		void setLightRed(glm::vec3 red);

		void drawModel(glm::mat4 V, glm::mat4 P, glm::mat4 M, float moveX=0.f, float moveY = 0.f, float moveZ = 0.f,
			float rotationX=0.f, float rotationY=0.f, float rotationZ = 0.f,
			float rescaleX=1.0f, float rescaleY = 1.0f, float rescaleZ = 1.0f );
};

