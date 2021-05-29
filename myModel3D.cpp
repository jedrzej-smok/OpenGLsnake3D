#include "myModel3D.h"

GLuint myModel3D::readTexture(const char* filename) {
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

void myModel3D::loadModelAssimp(std::string path)
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


	 
	std::cout << "\t\tnumberofMeshes:" << numberOfMeshes << std::endl;
	//std::cout << "size verts:" << verts.size() << std::endl;
	for (int m = 0; m < numberOfMeshes; m++) {
		std::cout << "poczatek size verts:" << verts.size() << std::endl;
		aiMesh* mesh  = scene->mMeshes[m];
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
		std::cout << "koniec size verts:" << verts.size() << std::endl;
		std::cout << "mesha ma faces:" << mesh->mNumFaces << std::endl;
		std::cout << "size verts:" << verts.size() << std::endl;
		std::cout << "size norms:" << norms.size() << std::endl;
		std::cout << "size texs:" << texs.size() << std::endl;
		std::cout << "size indicies:" << indices.size() << std::endl<<std::endl;
		
	}


	
	std::cout << "koniec meshy\n";
	//std::cout << "mesha ma faces:" << mesh->mNumFaces << std::endl;
	std::cout << "size verts:" << verts.size() << std::endl;
	std::cout << "size norms:" << norms.size() << std::endl;
	std::cout << "size texs:" << texs.size() << std::endl;
	std::cout << "size indicies:" << indices.size() << std::endl << std::endl;
}

void myModel3D::initModel(const char* pathObj, const char* pathTex0, const char* pathTex1) {

	//NAJPIERW CZYTAJ OBIEKT XDDDDDDDDDDD
	//assimp
	loadModelAssimp(std::string(pathObj));

	//tekstury
	//Wczytanie i import obrazka – z initOpenGLProgram
	tex0 = readTexture(pathTex0);
	tex1 = readTexture(pathTex1);//druga tekstura
	//program cieniujacy
	sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
	//VBO
	numberOfVerts = verts.size();
	numberOfIndex = indices.size();
	glGenBuffers(1, &bufVertex);//liczba uchwytow do wygenerowania,wskaznik na dokad te uchwyty maja byc wygenerowane
	glBindBuffer(GL_ARRAY_BUFFER, bufVertex);//jakis typ, uchwyt ktory uaktywaniamy
	glBufferData(GL_ARRAY_BUFFER, numberOfVerts * sizeof(float) * 4, verts.data(), GL_STATIC_DRAW);//liczba bajtow,tablica z danymi, spsoob dostepu danyych

	glGenBuffers(1, &bufNormal);//liczba uchwytow do wygenerowania,wskaznik na dokad te uchwyty maja byc wygenerowane
	glBindBuffer(GL_ARRAY_BUFFER, bufNormal);//jakis typ, uchwyt ktory uaktywaniamy
	glBufferData(GL_ARRAY_BUFFER, numberOfVerts * sizeof(float) * 4, norms.data(), GL_STATIC_DRAW);//liczba bajtow,tablica z danymi, spsoob dostepu danyych

	glGenBuffers(1, &bufTexCoord);//liczba uchwytow do wygenerowania,wskaznik na dokad te uchwyty maja byc wygenerowane
	glBindBuffer(GL_ARRAY_BUFFER, bufTexCoord);//jakis typ, uchwyt ktory uaktywaniamy
	glBufferData(GL_ARRAY_BUFFER, numberOfVerts * sizeof(float) * 2, texs.data(), GL_STATIC_DRAW);//liczba bajtow,tablica z danymi, spsoob dostepu danyych
	glBindBuffer(GL_ARRAY_BUFFER, 0);//sprzatanie

	glGenBuffers(1, &bufIndex);//liczba uchwytow do wygenerowania,wskaznik na dokad te uchwyty maja byc wygenerowane
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIndex);//jakis typ, uchwyt ktory uaktywaniamy
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numberOfIndex * sizeof(int), indices.data(), GL_STATIC_DRAW);//liczba bajtow,tablica z danymi, spsoob dostepu danyych
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);//sprzatanie
	//koniec VBO
}

void myModel3D::setupModel(float moveX , float moveY,  float moveZ,
	float rotationX, float rotationY, float rotationZ,
	float rescaleX , float rescaleY , float rescaleZ ) {
	coord_x = moveX;//przesuñ model
	coord_y = moveY;
	coord_z = moveZ;

	angle_x = rotationX;//przekrec model
	angle_y = rotationY;
	angle_z = rotationZ;

	scale_x = rescaleX;
	scale_y = rescaleY;
	scale_z = rescaleZ;
}

void myModel3D::freeModel() {
	//************Tutaj umieszczaj kod, który nale¿y wykonaæ po zakoñczeniu pêtli g³ównej************
	//Usuniêcie tekstury z pamiêci karty graficznej 
	glDeleteTextures(1, &tex0);
	glDeleteTextures(1, &tex1);
	//usuniecie VBO z GPU
	glDeleteBuffers(1, &bufVertex);
	glDeleteBuffers(1, &bufNormal);
	glDeleteBuffers(1, &bufTexCoord);
	glDeleteBuffers(1, &bufIndex);
	delete sp;
}
void myModel3D:: drawModel(glm::mat4 V, glm::mat4 P, glm::mat4 M, float moveX, float moveY, float moveZ,
	float rotationX, float rotationY, float rotationZ,
	float rescaleX, float rescaleY, float rescaleZ){

	coord_x += moveX;//przesuñ model
	coord_y += moveY;
	coord_z += moveZ;

	angle_x += rotationX;//przekrec model
	angle_y += rotationY;
	angle_z += rotationZ;

	scale_x *= rescaleX;
	scale_y *= rescaleY;
	scale_z *= rescaleZ;

	M = glm::translate(M, glm::vec3(coord_x, coord_y, coord_z));//Wylicz macierz modelu
	
	M = glm::rotate(M, angle_x, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	M = glm::rotate(M, angle_y, glm::vec3(1.0f, 0.0f, 0.0f)); //Wylicz macierz modelu
	M = glm::rotate(M, angle_z, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu

	M = glm::scale(M, glm::vec3(scale_x, scale_y, scale_z)); //Wylicz macierz modelu

	//std::cout << M[0].x << "," << M[0].y << "," << M[0].z << ","<< M[0].w  << std::endl;
	//std::cout << M[1].x << "," << M[1].y << "," << M[1].z << ","<< M[1].w  << std::endl;
	//std::cout << M[2].x << "," << M[2].y << "," << M[2].z << ","<< M[2].w  << std::endl;
	//std::cout << M[3].x << "," << M[3].y << "," << M[3].z << ","<< M[3].w  << std::endl;//zmienia wspolrzedna xDDDDDDDDDDDDDD
	//std::cout << "\n\n\n";

	//STANDARDOWE URUCHAMIANIE=====================================================================================================
	sp->use();//Aktywacja programu cieniuj¹cego
	//Przeslij parametry programu cieniuj¹cego do karty graficznej
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

	glEnableVertexAttribArray(sp->a("vertex"));  //W³¹cz przesy³anie danych do atrybutu vertex
	glBindBuffer(GL_ARRAY_BUFFER, bufVertex);//chyba ten bufor aktywny
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, NULL); //Wska¿ tablicê z danymi dla atrybutu vertex; zmiana na VBO

	glEnableVertexAttribArray(sp->a("normal"));  //W³¹cz przesy³anie danych do atrybutu normal
	glBindBuffer(GL_ARRAY_BUFFER, bufNormal);//chyba ten bufor aktywny
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, NULL); //Wska¿ tablicê z danymi dla atrybutu normal

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glBindBuffer(GL_ARRAY_BUFFER, bufTexCoord);//chyba ten bufor aktywny
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, NULL);//odpowiednia tablica
	glBindBuffer(GL_ARRAY_BUFFER, 0);//sprzatanie


	//CIENIOWANIE
	//Powi¹zanie zmiennej typu sampler2D z jednostk¹ teksturuj¹ca, tu zerowa jednostka teksturujaca z fs
	glUniform1i(sp->u("textureMap0"), 0); //drawScene

	//Przed glDrawArrays w drawScene Przypisanie tekstury do jednostki teksturuj¹ce
	glActiveTexture(GL_TEXTURE0);//aktywacja jeddnostki teksturujacej
	glBindTexture(GL_TEXTURE_2D, tex0);//przypiasanie tekstury do jednostki

	glUniform1i(sp->u("textureMap1"), 1); //drawScene
	glActiveTexture(GL_TEXTURE1);//aktywacja jeddnostki teksturujacej
	glBindTexture(GL_TEXTURE_2D, tex1);//przypiasanie tekstury do jednostki

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//glDrawArrays(GL_TRIANGLES,0,numberOfVerts ); //Narysuj obiekt/
	//do Assimp
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIndex);//chyba ten bufor aktywny
	glDrawElements(GL_TRIANGLES, numberOfIndex, GL_UNSIGNED_INT, NULL); //PO ZMIANIA na VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);//sprzatanie

	glDisableVertexAttribArray(sp->a("vertex"));  //Wy³¹cz przesy³anie danych do atrybutu vertex
	glDisableVertexAttribArray(sp->a("normal"));  //Wy³¹cz przesy³anie danych do atrybutu normal
	glDisableVertexAttribArray(sp->a("texCoord0"));
}