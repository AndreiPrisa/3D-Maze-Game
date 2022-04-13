#pragma once

#include <glm/ext/vector_float2.hpp>
#include "threeDEntity.h"
#include "utils/math_utils.h"
#include<iostream>
#include "Timer.h"
using namespace std;


class Player {
public:
	glm::vec3 position;
	threeDEntity::Cube head, torso, leg, arm, hand;
	threeDEntity::Sphere leftArmJoint, rightArmJoint, leftLegJoint, rightLegJoint;
	threeDEntity::Cube collisionBox;
	float OYRotation, legArmOXRotation, armOZRotation;
	float health;
	Timer stamina, jumpCoolDown, jumpingTime;

	Player(glm::vec3 position, float scaleFactor, glm::vec3 skinColor, glm::vec3 shirtColor, 
		glm::vec3 pantsColor, float health = 100, float stamina = 100, float jumpCoolDown = 10) {
		
		float radiusBase = 0.1f;
		float resizeFactor = 0.2f;
		this->health = health;
		this->stamina = Timer(stamina);
		this->position = position;
		this->position.y *= resizeFactor;

		// timer setter
		this->jumpCoolDown = Timer(jumpCoolDown);
		this->jumpingTime = Timer(0.6f);

		// absolute position
		glm::vec3 torsoSize = resizeFactor * glm::vec3(scaleFactor / 4, scaleFactor / 3, scaleFactor / 10);
		this->torso = threeDEntity::Cube(position, torsoSize, shirtColor);

		// relative positions to torso
		glm::vec3 headSize = resizeFactor * glm::vec3(scaleFactor / 6);
		this->head = threeDEntity::Cube(glm::vec3(0, (torsoSize.y + headSize.y) / 2, 0), headSize, skinColor);

		glm::vec3 legSize = glm::vec3(this->torso.dimensions.x * 0.4f, this->torso.dimensions.y, this->torso.dimensions.z * 1.1f);
		this->leftLegJoint = threeDEntity::Sphere(0.5f * glm::vec3(-torsoSize.x + legSize.x, -torsoSize.y, 0), radiusBase, glm::vec3(0));
		this->rightLegJoint = threeDEntity::Sphere(0.5f * glm::vec3(torsoSize.x - legSize.x, -torsoSize.y, 0), radiusBase, glm::vec3(0));

		this->leg = threeDEntity::Cube(0.5f * glm::vec3(0, -legSize.y, 0), legSize, pantsColor);

		glm::vec3 armSize = glm::vec3(this->torso.dimensions.x * 0.4f, this->torso.dimensions.y * 0.8f, this->torso.dimensions.z * 1.1f);
		glm::vec3 handSize =  0.6f * headSize;
		this->leftArmJoint = threeDEntity::Sphere(0.5f * glm::vec3(-torsoSize.x - armSize.x, torsoSize.y, 0), radiusBase, glm::vec3(0));
		this->rightArmJoint = threeDEntity::Sphere(0.5f * glm::vec3(torsoSize.x + armSize.x, torsoSize.y, 0), radiusBase, glm::vec3(0));

		this->arm = threeDEntity::Cube(0.5f * glm::vec3(0, -armSize.y, 0), armSize, shirtColor);
		this->hand = threeDEntity::Cube(0.5f * glm::vec3(0, -armSize.y -handSize.y, 0), handSize, skinColor);

		// rotations
		legArmOXRotation = 0;
		OYRotation = 0;
		armOZRotation = 10.0f;

		// collision box
		float width = torsoSize.x + 2 * armSize.x;
		float height = legSize.y + torsoSize.y + headSize.y;
		float depth = max({legSize.z, torsoSize.z, headSize.z, legSize.z});
		this->collisionBox = threeDEntity::Cube(glm::vec3(0), glm::vec3(width, height, depth), glm::vec3(0));
	}

	void UpdatePosition(glm::vec3 newPosition) {
		this->position = newPosition;
		this->torso.center = newPosition;
		this->collisionBox.center = newPosition;
	}

	void UpdateOYRotation(float degrees) {
		this->OYRotation += degrees;
		float width = this->torso.dimensions.x + 2 * this->arm.dimensions.x;
		float depth = max({ this->leg.dimensions.z, this->torso.dimensions.z, this->head.dimensions.z});
		this->collisionBox.dimensions.x = max(depth, abs(cos(RADIANS(this->OYRotation)) * width));
		this->collisionBox.dimensions.z = max(depth, abs(sin(RADIANS(this->OYRotation)) * width));
	}

	void Animate(bool moving, float deltaTime, float animationTime) {
		static float timer = 0;
		static int direction = 1;
		float inclinationMax = 45.0f;
		float step = direction * deltaTime / animationTime * 3 * inclinationMax;

		if ((this->legArmOXRotation == 0 && moving) || this->legArmOXRotation != 0) {
			timer += deltaTime;
			if (abs(this->legArmOXRotation + step) >= inclinationMax) {
				this->legArmOXRotation = direction * inclinationMax;
				direction *= -1;
			}
			else if (!moving && ((this->legArmOXRotation + step) * this->legArmOXRotation < 0)) {
				this->legArmOXRotation = 0;
			}
			else {
				this->legArmOXRotation += step;
			}
		}
	}
};