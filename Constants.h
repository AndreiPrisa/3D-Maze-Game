#pragma once
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float2.hpp>

class MazeConstants {
public:
	static const int CLEAR_PATH;
	static const int WALL_SQUARE;
	static const int ENEMY_SQUARE;
	static const int PLAYER_SQUARE;
	static const int EXIT_SQUARE;
	static const int BOSS_SQUARE;
	static const int HEALTHPACK_SQUARE;
};

class Colors {
public:
	static const glm::vec3 beige;
	static const glm::vec3 cyan;
	static const glm::vec3 lightBlue;
	static const glm::vec3 blue;
	static const glm::vec3 darkGreen;
	static const glm::vec3 lightGreen;
	static const glm::vec3 red;
	static const glm::vec3 yellow;
	static const glm::vec3 lightOrange;
	static const glm::vec3 lilla;
};

class EntityConstants {
public:
	static const int FRIENDLY;
	static const int HOSTILE;
	static const int NORMAL_TYPE;
	static const int GUNNER_TYPE;
	static const int BOSS_TYPE;
};

class Directions {
public:
	static const glm::vec3 zDir;
	static const glm::vec3 xDir;
	static const glm::vec3 yDir;
};

class DancerConst {
public:
	static const glm::vec3 dimensions;
};

class TimeValues {
public:
	static const glm::vec2 changeMotionInterval;
	static const glm::vec2 changeRotationInterval;
	static const glm::vec2 changeColorInterval;
	static const glm::vec2 rotationAngleInterval;
	static const glm::vec2 trackPointInterval;
	static const float colorChangeTime;

	static float getRandomVal(glm::vec2 interval) {
		return interval.x + (float)rand() / (RAND_MAX / (interval.y - interval.x));
	}
};

class Constants {
public:
	static const float minimalSpacing;
};

class Positions {
public:
	static const glm::vec3 HUDPos;
};