#pragma once

#include "threeDEntity.h"

bool threeDEntity::checkCollision(Cube cube1, Cube cube2) {
	float min1X, min2X;
	float min1Y, min2Y;
	float min1Z, min2Z;

	min1X = cube1.center.x - cube1.dimensions.x / 2;
	min2X = cube2.center.x - cube2.dimensions.x / 2;
	min1Y = cube1.center.y - cube1.dimensions.y / 2;
	min2Y = cube2.center.y - cube2.dimensions.y / 2;
	min1Z = cube1.center.z - cube1.dimensions.z / 2;
	min2Z = cube2.center.z - cube2.dimensions.z / 2;

	float max1X, max2X;
	float max1Y, max2Y;
	float max1Z, max2Z;

	max1X = cube1.center.x + cube1.dimensions.x / 2;
	max2X = cube2.center.x + cube2.dimensions.x / 2;
	max1Y = cube1.center.y + cube1.dimensions.y / 2;
	max2Y = cube2.center.y + cube2.dimensions.y / 2;
	max1Z = cube1.center.z + cube1.dimensions.z / 2;
	max2Z = cube2.center.z + cube2.dimensions.z / 2;

	return (min1X <= max2X && max1X >= min2X) &&
		(min1Y <= max2Y && max1Y >= min2Y) &&
		(min1Z <= max2Z && max1Z >= min2Z);
}

bool threeDEntity::checkCollision(Sphere s1, Sphere s2) {
	return glm::distance(s1.center, s2.center) < s1.radius + s2.radius;
}

bool threeDEntity::checkCollision(Sphere s, Cube c)
{
	glm::vec3 cubeMin, cubeMax;
	cubeMin = c.center - 0.5f * c.dimensions;
	cubeMax = c.center + 0.5f * c.dimensions;

	glm::vec3 interestPoint;
	interestPoint.x = std::max(cubeMin.x, std::min(s.center.x, cubeMax.x));
	interestPoint.y = std::max(cubeMin.y, std::min(s.center.y, cubeMax.y));
	interestPoint.z = std::max(cubeMin.z, std::min(s.center.z, cubeMax.z));
	return glm::distance(interestPoint, s.center) < s.radius;
}

threeDEntity::Cube *threeDEntity::canMove(Cube collisionBox, std::vector<Cube*> obstacles, glm::vec3 displacement)
{
	collisionBox.center += displacement;
	for (int i = 0; i < obstacles.size(); i++) {
		if (checkCollision(collisionBox, *obstacles[i])) {
			//std::cout << "collision with " << obstacles[i].center.x << " " << obstacles[i].center.z << std::endl;
			return obstacles[i];
		}
	}
	return nullptr;
}