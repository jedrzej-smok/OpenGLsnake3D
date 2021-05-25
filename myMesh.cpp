#include "myMesh.h"

GLuint myMesh::readTexture(const char* filename) {
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



void myMesh::setupMesh( const char* pathTex0, const char* pathTex1) {

	
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
void myMesh::freeMesh() {
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
void myMesh::drawMesh(glm::mat4 V, glm::mat4 P, glm::mat4 M, float moveX, float moveY, float moveZ,
	float rotationX, float rotationY, float rotationZ,
	float rescaleX, float rescaleY, float rescaleZ) {

	coord_x += moveX;//przesuñ model
	coord_y += moveY;
	coord_z += moveZ;

	angle_x += rotationX;//przekrec model
	//angle_y += rotationY;
	angle_z += rotationZ;
	//==========================================
	coord_x += sin(angle_x)*rotationY;//rotatation to spped_y
	coord_z += cos(angle_x) * rotationY;
	//==========================================

	scale_x *= rescaleX;
	scale_y *= rescaleY;
	scale_z *= rescaleZ;

	M = glm::translate(M, glm::vec3(coord_x, coord_y, coord_z));

	M = glm::rotate(M, angle_x, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	M = glm::rotate(M, angle_y, glm::vec3(1.0f, 0.0f, 0.0f)); //Wylicz macierz modelu
	M = glm::rotate(M, angle_z, glm::vec3(0.0f, 0.0f, 1.0f)); //Wylicz macierz modelu

	M = glm::scale(M, glm::vec3(rescaleX, rescaleY, rescaleZ));





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