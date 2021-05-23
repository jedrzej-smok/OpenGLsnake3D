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

		//³adowanie modelu 
		std::vector<glm::vec4> verts;
		std::vector<glm::vec4> norms;
		std::vector<glm::vec2> texs;
		std::vector<unsigned int> indices;
		int numberOfVerts;
		int numberOfIndex;
		void loadModelAssimp(std::string path);

		//funckcje z initOpenGLProgram
		//uchwyty zwiazane z modelem
		GLuint tex0; //Uchwyt – deklaracja globalna
		GLuint tex1; //Uchwyt – deklaracja globalna, na druga teksture
		GLuint bufVertex;//identyfiakator bufora z wierzcholkami
		GLuint bufNormal;//identyfiakator bufora z normalnymi
		GLuint bufTexCoord;//identyfiakator bufora z wspTeksturowania
		GLuint bufIndex;//identyfiakator bufora z indeksami
		ShaderProgram* sp;
		void initModel();
		void freeModel();

		//funckje rysujace
		//zmienne i macierze dla wszystkich te same
		float aspectRatio = 1;
		glm::mat4 V = glm::lookAt( glm::vec3(0, 0, -12.5), glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz widoku
		glm::mat4 P = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 0.01f, 50.0f); //Wylicz macierz rzutowania
		glm::mat4 M = glm::mat4(1.0f);
	
		//zmienne od animacji
		float coord_x = 0;
		float coord_y = 0;
		float coord_z = 0;

		float angle_x=0;
		float angle_y=0;
		float angle_z=0;

		float scale_x = 1.0f;
		float scale_y = 1.0f;
		float scale_z = 1.0f;



		void drawModel( float moveX=0.f, float moveY = 0.f, float moveZ = 0.f,
			float rotationX=0.f, float rotationY=0.f, float rotationZ = 0.f,
			float rescaleX=1.0f, float rescaleY = 1.0f, float rescaleZ = 1.0f );
};

