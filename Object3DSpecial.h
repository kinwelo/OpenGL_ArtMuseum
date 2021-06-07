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
#include "Object3D.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

// Math.h - STD math Library
#include <math.h>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#ifndef Object3DS_H
#define Object3DS_H

class Object3DSpecial
	//Alternatywna wersja klasy obiektu 3d dla specjalnych modeli jak:œciany muzeum czy sfera sceny o odwrotnych normalnych
{
protected:
	std::vector<glm::vec4> verts;
	std::vector<glm::vec4> norms;
	std::vector<glm::vec2> textureCoords;
	std::vector<unsigned int> indices;

public:
	GLuint texture;

	/*
		plik - œcie¿ka do pliku obiektu 3D
	*/
	Object3DSpecial(std::string plik) {
		loadModelAlt(plik);
	}
private:
	void loadModelAlt(std::string plik) {
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
			norms.push_back(glm::vec4(-normal.x, -normal.y, -normal.z, 0));

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

	virtual void drawModel(
		ShaderProgram* sp,
		glm::mat4 P,
		glm::mat4 V,
		glm::mat4 M,
		glm::vec4 lightSource[],
		float xAxis,
		float yAxis,
		float zAxis,
		float rotate,
		float xScale,
		float yScale,
		float zScale
	) = 0;

};

#endif