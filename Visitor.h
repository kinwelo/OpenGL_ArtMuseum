#include <Object3D.h>
#include <math.h>
#include <Object.h>
#include <Exhibit.h>

#pragma once

class Visitor : public Object
{
	
	float speed;
	bool stop[3]{ false, false, false };
	int waitClk = 0;
	int obj = 0;
	int pos = 0;

public:
	Visitor(Object3D *m, float position_x, float position_y, float position_z, float s)
	{
		model = m;
		position[0] = position_x;
		position[1] = position_y;
		position[2] = position_z;
		speed = s;
		waitClk = rand() % 500 + 200;
	}
private:
	int chceckWaitClk() {
		waitClk--;
		return waitClk;
	}

	void getNewWaitClk() {
		waitClk = rand() % 500 + 200;
	}

	void resetMove() {
		stop[0] = false;
		stop[1] = false;
		stop[2] = false;
	}

	bool canMove() {
		if (stop[0] && stop[1] && stop[2])
			return true;
		else
			return false;
	}

	float lookAt(
		float position_x,
		float position_z
	) {
		if (position_z > position[2]) {
			return atan((position_x - position[0]) / (position_z - position[2])) * 180 / PI;
		}
		else {
			return 180 + atan((position_x - position[0]) / (position_z - position[2])) * 180 / PI;
		}
		
	}

	int getFreePosition(Exhibit * exhibit) {
		for (int i = 0; i < exhibit->maxVisitors(); i++) {
			if (exhibit->placeToWatch[i][0] == 0) {
				exhibit->placeToWatch[i][0] = 1;
				return i;
			}
		}
	}
public:
	int * moveTo(
		float mainSpeed,
		ShaderProgram* sp,
		glm::mat4 P,
		glm::mat4 V,
		glm::mat4 M,
		glm::vec4 lightSource[],
		float scaleX,
		float scaleY,
		float scaleZ,
		Exhibit exhibit[],
		int exhibitCount
	)
	{
		float move = 1.0;
		float look = 0;
		int takePlace = 1;

		if (this->canMove()) {
			if (this->chceckWaitClk() <= 0) {
				obj = rand() % exhibitCount;
				this->resetMove();
				pos = this->getFreePosition(&exhibit[obj]);
				this->getNewWaitClk();
			}
			else {
				takePlace = 0;
			}
		}

		float position_x = exhibit[obj].placeToWatch[pos][1];
		float position_y = exhibit[obj].placeToWatch[pos][2];
		float position_z = exhibit[obj].placeToWatch[pos][3];
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

		if (stop[0] && stop[1] && stop[2]) {
			look = (lookAt(exhibit[obj].getPositionX(), exhibit[obj].getPositionZ()));
		}
		else
		{
			look = lookAt(position_x, position_z);
		}

		model->drawModel(sp, P, V, M, lightSource, position[0], position[1], position[2], look, scaleX, scaleY, scaleZ);
		int returnList[]{ pos, obj, takePlace };

		return returnList;
	}
};