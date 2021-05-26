#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myCube.h"
#include "myTeapot.h"
#include "tt.h"
#include "Object3D.h"
#include <OBJ_Loader.h>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#ifndef Object3D_H
#define Object3D_H

class Object3D
{
public:
	std::vector<glm::vec4> verts;
	std::vector<glm::vec4> norms;
	std::vector<glm::vec2> textureCoords;
	std::vector<unsigned int> indices;
	GLuint texture;

	/*
		plik - œcie¿ka do pliku obiektu 3D
	*/
	Object3D(std::string plik) {
		loadModel(plik);
	}
private:
	void loadModel(std::string plik) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(plik,
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals
		);
		std::cout << importer.GetErrorString() << std::endl;

		aiMesh* mesh = scene->mMeshes[0];
		for (int i = 0; i < mesh->mNumVertices; i++) {
			aiVector3D vertex = mesh->mVertices[i];
			verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

			aiVector3D normal = mesh->mNormals[i];
			norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

			//zalozenie ze mamy tylko zerowy zestaw tekstur(tylko 2d) 47 min co gdy nie

			aiVector3D texCoord = mesh->mTextureCoords[0][i];
			textureCoords.push_back(glm::vec2(texCoord.x, texCoord.y));

		}

		for (int i = 0; i < mesh->mNumFaces; i++) {
			aiFace& face = mesh->mFaces[i];

			for (int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}

	}

public:
	/*
		P, V, M, lightSource - parametry programu cieniuj¹cego,
		xAxis, yAxis, zAxis, - osie zmiany po³o¿enia
		rotate - oœ obrotu
	*/
	void drawModel(
		ShaderProgram* sp,
		glm::mat4 P,
		glm::mat4 V,
		glm::mat4 M,
		glm::vec4 lightSource,
		float xAxis,
		float yAxis,
		float zAxis,
		float rotate
	) {
		M = glm::translate(M, glm::vec3(xAxis, yAxis, zAxis));
		M = glm::rotate(M, rotate * PI / 180, glm::vec3(0.0f, 1.0f, 0.0f));
		sp->use();//Aktywacja programu cieniuj¹cego
		//Przeslij parametry programu cieniuj¹cego do karty graficznej
		glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
		glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
		glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));


		glUniform4fv(sp->u("lp"), 1, glm::value_ptr(lightSource));

		glEnableVertexAttribArray(sp->a("vertex"));  //W³¹cz przesy³anie danych do atrybutu vertex
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, verts.data()); //Wska¿ tablicê z danymi dla atrybutu vertex

		glEnableVertexAttribArray(sp->a("normal"));  //W³¹cz przesy³anie danych do atrybutu normal
		glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, norms.data()); //Wska¿ tablicê z danymi dla atrybutu normal
																// Dlaczego przez blackBear.getNorms().data() nie dzia³a?

		glEnableVertexAttribArray(sp->a("texCoord0"));  //W³¹cz przesy³anie danych do atrybutu normal
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, textureCoords.data()); //Wska¿ tablicê z danymi dla atrybutu normal

		glUniform1i(sp->u("textureMap0"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

		glDisableVertexAttribArray(sp->a("vertex"));  //Wy³¹cz przesy³anie danych do atrybutu vertex
		glDisableVertexAttribArray(sp->a("normal"));
		glDisableVertexAttribArray(sp->a("texCoord0"));
	}

};

#endif