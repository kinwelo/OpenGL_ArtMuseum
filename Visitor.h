#include <Object3D.h>
#include <firstMethodDrawing.h>
#include <math.h>
#include <Object.h>

#pragma once

class Visitor : public Object
{
	
	float speed;
	bool stop[3]{ false, false, false };

public:
	Visitor(Object3D *m, float position_x, float position_y, float position_z, float s)
	{
		model = m;
		position[0] = position_x;
		position[1] = position_y;
		position[2] = position_z;
		speed = s;
	}

	void resetMove() {
		stop[0] = false;
		stop[1] = false;
		stop[2] = false;
	}

	float lookAt(
		float position_x,
		float position_z
	) {
		return atan(abs(position[0] - position_x) / abs(position[2] - position_z)) * 180 / PI;
	}

	void moveTo(
		float position_x, 
		float position_y, 
		float position_z, 
		float mainSpeed,
		ShaderProgram* sp,
		glm::mat4 P,
		glm::mat4 V,
		glm::mat4 M,
		glm::vec4 lightSource,
		float rotate
	)
	{
		float move = 1.0;
		float side_A_to_B = abs(position[0] - position_x) / abs(position[2] - position_z);
		if (!stop[0]) {
			if ((position[0] - position_x) > 0) {
				move = -1.0;
			}
			else {
				move = 1.0;
			}
			position[0] = position[0] + (mainSpeed * speed * move);
			if (move == -1 && (position[0] - position_x) < 0) {
				stop[0] = true;
			}
			if (move == 1 && (position[0] - position_x) > 0) {
				stop[0] = true;
			}
		}
		
		if (!stop[1]) {
			if ((position[1] - position_y) > 0) {
				move = -1.0;
			}
			else {
				move = 1.0;
			}
			position[1] = position[1] + (mainSpeed * speed * move);
			if (move == -1 && (position[1] - position_y) < 0) {
				stop[1] = true;
			}
			if (move == 1 && (position[1] - position_y) > 0) {
				stop[1] = true;
			}
		}

		if (!stop[2]) {
			if ((position[2] - position_z) > 0) {
				move = -1.0;
			}
			else {
				move = 1.0;
			}
			position[2] = position[2] + (mainSpeed * speed * move / side_A_to_B);
			if (move == -1 && (position[2] - position_z) < 0) {
				stop[2] = true;
			}
			if (move == 1 && (position[2] - position_z) > 0) {
				stop[2] = true;
			}
		}
		model->drawModel(sp, P, V, M, lightSource, position[0], position[1], position[2], lookAt(position_x, position_z));
	}
};