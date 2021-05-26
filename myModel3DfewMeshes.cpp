#include "myModel3DfewMeshes.h"



void myModel3DfewMeshes::loadModelAssimp(std::string path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
	std::cout << import.GetErrorString() << std::endl;
	if (scene->HasMeshes()) {
		std::cout << "scena ma jakies meshe, cos wczytane\n";
		numberOfMeshes = scene->mNumMeshes;
		std::cout << "scena ma ile meshy:" << numberOfMeshes << std::endl;
		std::cout << "scena ma ile wbudowanych textur:" << scene->mNumTextures << std::endl;
		std::cout << "scena ma ile zrodel swiatla:" << scene->mNumLights << std::endl;
		std::cout << "scena ma ile materialow:" << scene->mNumMaterials << std::endl;
	}
	else { std::cout << "scena nie ma meshy, niepoprawne wczytanie modelu.\n"; }


	aiMesh* mesh = scene->mMeshes[0];//mamy jeden model
	
	for (int m = 0; m < numberOfMeshes; m++) {
		mesh = scene->mMeshes[m];
		std::cout << "Mesh nr:" << m << "\n";
		unsigned int liczba_zest = mesh->GetNumUVChannels();
		std::cout << "liczba zestawow tekstur dla mesha:" << liczba_zest << std::endl;
		for (int t = 0; t < 8; t++) {
			if (mesh->HasTextureCoords(t))
			{
				std::cout << "mesha ma zestaw teksturowania nr:" << t << ", ten zestaw ma wymiarow:" << mesh->mNumUVComponents[t] << "\n";
			}
		}
		//wczytanie mesha==================================================================================================================
		myMesh myMeshtmp(spModel);
		for (int i = 0; i < mesh->mNumVertices; i++) {
			aiVector3D vertex = mesh->mVertices[i];
			myMeshtmp.verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));//bo wierzchoelk

			aiVector3D normal = mesh->mNormals[i];
			myMeshtmp.norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));//bo wektor

			aiVector3D texCoord = mesh->mTextureCoords[0][i];
			myMeshtmp.texs.push_back(glm::vec2(texCoord.x, texCoord.y));
		}
		//mesh skalada sie z wielokatow, face to ten wielokat
		for (int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (int j = 0; j < face.mNumIndices; j++) {//numIndicies to liczba wierzchokow tworzacych jeden wielokat
				myMeshtmp.indices.push_back(face.mIndices[j]);
			}
		}
		std::cout << "mesha ma faces:" << mesh->mNumFaces << std::endl;
		std::cout << "size verts:" << myMeshtmp.verts.size() << std::endl;
		std::cout << "size norms:" << myMeshtmp.norms.size() << std::endl;
		std::cout << "size texs:" << myMeshtmp.texs.size() << std::endl;
		std::cout << "size indicies:" << myMeshtmp.indices.size() << std::endl;
		meshes.push_back(myMeshtmp);
	}



	
}

void myModel3DfewMeshes::initModel(const char* pathObj) {

	//NAJPIERW CZYTAJ OBIEKT XDDDDDDDDDDD
	//assimp
	loadModelAssimp(std::string(pathObj));
	manualSetupTexture();

}
void myModel3DfewMeshes::freeModel() {
	//************Tutaj umieszczaj kod, który nale¿y wykonaæ po zakoñczeniu pêtli g³ównej************
	//Usuniêcie tekstury z pamiêci karty graficznej 
	for (int m = 0; m < numberOfMeshes; m++) {
		meshes[m].freeMesh();
	}
}
void myModel3DfewMeshes::drawModel3DfewMeshes(glm::mat4 V, glm::mat4 P, glm::mat4 M, float moveX, float moveY, float moveZ,
	float rotationX, float rotationY, float rotationZ,
	float rescaleX, float rescaleY, float rescaleZ) {
	for (int m = 0; m < numberOfMeshes; m++) {
		meshes[m].drawMesh(V,P,M,moveX,moveY,moveZ,rotationX,rotationY,rotationZ,rescaleX,rescaleY,rescaleZ);
	}

}
void myModel3DfewMeshes::manualSetupTexture() {
	//meshes[1].setupMesh("a.png", "sky.png");
	meshes[0].setupMesh("modele/aroy.png", "modele/sky.png");

}