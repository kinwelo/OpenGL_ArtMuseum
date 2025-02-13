#pragma once
#include "Object3D.h"
#include <stdlib.h>
#include <stdio.h>

class EnvmapDrawingMethod :
	public Object3D
{
public:
	EnvmapDrawingMethod(std::string plik) : Object3D(plik)
	{
		;
	}

	void drawModel(
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
	)
	{
		M = glm::translate(M, glm::vec3(xAxis, yAxis, zAxis));
		M = glm::rotate(M, rotate * PI / 180, glm::vec3(0.0f, 1.0f, 0.0f));
		M = glm::scale(M, glm::vec3(xScale, yScale, zScale));
		sp->use();//Aktywacja programu cieniuj�cego
		//Przeslij parametry programu cieniuj�cego do karty graficznej
		glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
		glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
		glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

		glUniform4fv(sp->u("lp[0]"), 1, glm::value_ptr(lightSource[0]));
		glUniform4fv(sp->u("lp[1]"), 1, glm::value_ptr(lightSource[1]));

		glEnableVertexAttribArray(sp->a("vertex"));  //W��cz przesy�anie danych do atrybutu vertex
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, verts.data()); //Wska� tablic� z danymi dla atrybutu vertex

		glEnableVertexAttribArray(sp->a("normal"));  //W��cz przesy�anie danych do atrybutu normal
		glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, norms.data()); //Wska� tablic� z danymi dla atrybutu normal

		glEnableVertexAttribArray(sp->a("texCoord0"));  //W��cz przesy�anie danych do atrybutu normal
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, textureCoords.data()); //Wska� tablic� z danymi dla atrybutu normal

		glUniform1i(sp->u("textureMap0"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glUniform1i(sp->u("textureMap1"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture_refl);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

		glDisableVertexAttribArray(sp->a("vertex"));  //Wy��cz przesy�anie danych do atrybutu vertex
		glDisableVertexAttribArray(sp->a("normal"));
		glDisableVertexAttribArray(sp->a("texCoord0"));
	}
};



