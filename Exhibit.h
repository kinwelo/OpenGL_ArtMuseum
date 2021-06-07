#pragma once
#include "Object.h"

enum canStay { LEFT, AROUND, RIGHT};

class Exhibit :
    public Object
{
	int visitors = 0;
	int capacity;
	int id;
	float placeToWatch[4][4];
	Exhibit(int id, Object3D* m, float position_x, float position_y, float position_z, int max)
	{
		this->id = id;
		model = m;
		position[0] = position_x;
		position[1] = position_y;
		position[2] = position_z;
		capacity = max;

		placeToWatch[0][0] = 0;
		placeToWatch[0][1] = 0;
		placeToWatch[0][2] = 0;
		placeToWatch[0][3] = 0;
		switch (canStay::AROUND)
		{
		case LEFT:
			placeToWatch[0][1] = position[0] - 2;
			placeToWatch[0][2] = position[1];
			placeToWatch[0][3] = position[2] + 2;

			placeToWatch[1][1] = position[0] - 2;
			placeToWatch[1][2] = position[1];
			placeToWatch[1][3] = position[2];

			placeToWatch[2][1] = position[0] - 2;
			placeToWatch[2][2] = position[1];
			placeToWatch[2][3] = position[2] - 2;
			break;
		case AROUND:
			placeToWatch[0][1] = position[0] - 2;
			placeToWatch[0][2] = position[1];
			placeToWatch[0][3] = position[2] + 2;

			placeToWatch[1][1] = position[0] - 2;
			placeToWatch[1][2] = position[1];
			placeToWatch[1][3] = position[2] - 2;

			placeToWatch[2][1] = position[0] + 2;
			placeToWatch[2][2] = position[1];
			placeToWatch[2][3] = position[2] + 2;

			placeToWatch[3][1] = position[0] + 2;
			placeToWatch[3][2] = position[1];
			placeToWatch[3][3] = position[2] - 2;
			break;
		case RIGHT:
			placeToWatch[0][1] = position[0] + 2;
			placeToWatch[0][2] = position[1];
			placeToWatch[0][3] = position[2] - 2;

			placeToWatch[1][1] = position[0] + 2;
			placeToWatch[1][2] = position[1];
			placeToWatch[1][3] = position[2];

			placeToWatch[2][1] = position[0] + 2;
			placeToWatch[2][2] = position[1];
			placeToWatch[2][3] = position[2] + 2;
			break;
		default:
			break;
		}
	}
};