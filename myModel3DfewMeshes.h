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
#include <myMesh.h>
class myModel3DfewMeshes
{
public:
	ShaderProgram* spModel;
	myModel3DfewMeshes() {};
	~myModel3DfewMeshes() {
	}
	
	void setSp(ShaderProgram* a) {
		spModel = a;
	};
	//³adowanie modelu 
	int numberOfMeshes=0;
	std::vector<myMesh> meshes;
	
	void loadModelAssimp(std::string path);

	
	
	void initModel(const char* pathObj);
	void manualSetupTexture();
	void freeModel();

	//funckje rysujace
	//zmienne i macierze dla wszystkich te same
	float aspectRatio = 1;
	

	void drawModel3DfewMeshes(glm::mat4 V, glm::mat4 P, glm::mat4 M, float moveX = 0.f, float moveY = 0.f, float moveZ = 0.f,
		float rotationX = 0.f, float rotationY = 0.f, float rotationZ = 0.f,
		float rescaleX = 1.0f, float rescaleY = 1.0f, float rescaleZ = 1.0f);
};

