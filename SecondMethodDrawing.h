#pragma once
#include "Object3D.h"
#include <stdlib.h>
#include <stdio.h>

class SecondMethodDrawing :
    public Object3D
{
public:
    SecondMethodDrawing(std::string plik) : Object3D(plik)
    {
        ;
    }

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
	)
	{
		std::cout << "Second dwowing metchod" << std::endl;
	}
};

