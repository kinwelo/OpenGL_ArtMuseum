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
#include "tt.h"
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

	std::vector<glm::vec4> getVerts() {
		return verts;
	}

	std::vector<glm::vec4> getNorms() {
		return norms;
	}

	std::vector<glm::vec2> getTextureCoords() {
		return textureCoords;
	}

	std::vector<unsigned int> getIndices() {
		return indices;
	}

};

#endif