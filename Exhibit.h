#pragma once
#include "Object.h"

enum canStay { LEFT, AROUND, RIGHT};

class Exhibit :
    public Object
{
public:
	canStay type;
	float placeToWatch[4][4];

	int maxVisitors() {
		switch (canStay::AROUND)
		{
		case LEFT:
			return 3;
		case AROUND:
			return 4;
		case RIGHT:
			return 3;
		default:
			break;
		}
	}

	Exhibit( float position_x, float position_y, float position_z, canStay stay, float distance)
	{
		position[0] = position_x;
		position[1] = position_y;
		position[2] = position_z;
		type = stay;
		placeToWatch[0][0] = 0;
		placeToWatch[0][1] = 0;
		placeToWatch[0][2] = 0;
		placeToWatch[0][3] = 0;
		switch (canStay::AROUND)
		{
		case LEFT:
			placeToWatch[0][1] = position[0] - distance;
			placeToWatch[0][2] = 1;
			placeToWatch[0][3] = position[2] + distance;

			placeToWatch[1][1] = position[0] - distance;
			placeToWatch[1][2] = 1;
			placeToWatch[1][3] = position[2];

			placeToWatch[2][1] = position[0] - distance;
			placeToWatch[2][2] = 1;
			placeToWatch[2][3] = position[2] - distance;
			break;
		case AROUND:
			placeToWatch[0][1] = position[0] - distance;
			placeToWatch[0][2] = 1;
			placeToWatch[0][3] = position[2] + distance;

			placeToWatch[1][1] = position[0] - distance;
			placeToWatch[1][2] = 1;
			placeToWatch[1][3] = position[2] - distance;

			placeToWatch[2][1] = position[0] + distance;
			placeToWatch[2][2] = 1;
			placeToWatch[2][3] = position[2] + distance;

			placeToWatch[3][1] = position[0] + distance;
			placeToWatch[3][2] = 1;
			placeToWatch[3][3] = position[2] - distance;
			break;
		case RIGHT:
			placeToWatch[0][1] = position[0] + distance;
			placeToWatch[0][2] = 1;
			placeToWatch[0][3] = position[2] - distance;

			placeToWatch[1][1] = position[0] + distance;
			placeToWatch[1][2] = 1;
			placeToWatch[1][3] = position[2];

			placeToWatch[2][1] = position[0] + distance;
			placeToWatch[2][2] = 1;
			placeToWatch[2][3] = position[2] + distance;
			break;
		default:
			break;
		}
	}

	float getPositionX() {
		return position[0];
	}

	float getPositionY() {
		return position[1];
	}

	float getPositionZ() {
		return position[2];
	}
};