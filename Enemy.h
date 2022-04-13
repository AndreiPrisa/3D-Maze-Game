#include "Player.h"
#include "Timer.h"

namespace Hostiles {
	class Enemy {
	public:
		glm::vec3 position, forward;
		int type;
		threeDEntity::Sphere torso;
		threeDEntity::Sphere collisionBox;
		threeDEntity::Cube spawnArea;
		float speed;
		float health, fullHealth;
		float damage;
		bool playerDetected;
		Timer gotHitTimer;

		Enemy(int type, threeDEntity::Cube spawnArea, float radius, float speed, glm::vec3 color,
			glm::vec3 forward = Directions::xDir, float collisionDisplacement = 0, float health = 30, float damage = 25) {
			this->type = type;
			this->playerDetected = false;
			this->health = health;
			this->damage = damage;
			this->fullHealth = health;
			this->speed = speed;
			this->spawnArea = spawnArea;

			this->position = glm::vec3(this->spawnArea.center.x, radius + collisionDisplacement, this->spawnArea.center.z);

			this->torso = threeDEntity::Sphere(this->position, radius, color);
			this->collisionBox = threeDEntity::Sphere(this->position, radius + collisionDisplacement, glm::vec3(0));
			this->forward = forward;
			gotHitTimer = Timer(0.5f);
		}

		void updatePos(glm::vec3 newPosition) {
			this->position = newPosition;
			this->torso.center = newPosition;
			this->collisionBox.center = newPosition;
		}

		void patrol(float deltaTime) {
			glm::vec3 nextPos = this->position + this->speed * deltaTime * this->forward;
			nextPos.y = this->position.y;

			if (abs(nextPos.x - this->spawnArea.center.x) <= this->spawnArea.dimensions.x / 2 - this->collisionBox.radius &&
				abs(nextPos.z - this->spawnArea.center.z) <= this->spawnArea.dimensions.z / 2 - this->collisionBox.radius) {
				updatePos(nextPos);
			}
			else {
				float radius = this->collisionBox.radius;
				if (this->forward.x != 0) {
					nextPos = glm::vec3(this->spawnArea.center.x + this->forward.x * (this->spawnArea.dimensions.x / 2 - radius - Constants::minimalSpacing),
						this->position.y, this->position.z);
				}
				else {
					nextPos = glm::vec3(this->position.x, this->position.y,
						this->spawnArea.center.z + this->forward.z * (this->spawnArea.dimensions.z / 2 - radius - Constants::minimalSpacing));
				}
				updatePos(nextPos);
				this->forward = glm::vec3(-this->forward.z, 0, this->forward.x);
			}
		}

		glm::vec3 getDamageColor() {
			glm::vec3 color = this->torso.color;
			float coefficient = health / fullHealth;
			color.x +=  (1 - coefficient) * (1 - color.x);
			color.y = coefficient * color.y;
			color.z = coefficient * color.z;
			return color;
		}

		void followPlayer(glm::vec3 playerPos, float deltaTime, glm::vec3 dir) {
			if (dir == glm::vec3(0)) {
				this->forward = glm::normalize(playerPos - this->position);
				this->forward.y = 0;
			}
			else {
				this->forward = dir;
			}
			updatePos(this->position + this->speed * deltaTime * this->forward);
		}

		void move(Player* player, float deltaTime, glm::vec3 dir = glm::vec3(0)) {
			if (!playerDetected) {
				if (checkCollision(player->collisionBox, this->spawnArea)) {
					playerDetected = true;
				}
			}

			if (playerDetected) {
				followPlayer(player->position, deltaTime, dir);
			}
			else {
				patrol(deltaTime);
			}
		}
	};

	class Gunner : Enemy {
	public:
		Timer fireTimer;

		Gunner(int type, threeDEntity::Cube spawnArea, float radius, float speed, glm::vec3 color,
			glm::vec3 forward = Directions::xDir, float collisionDisplacement = 0, float health = 30, float damage = 25, float fireRate = 1) :
			Enemy(type, spawnArea, radius, speed, color, forward, collisionDisplacement, health, damage)
		{
			this->fireTimer = Timer(fireRate);
		}

		void shoot(float deltaTime, std::list<threeDEntity::Bullet> *bullets, glm::vec3 dir, float bulletSize, float maxDistance) {
			if (playerDetected) {
				fireTimer.update(deltaTime);
				if (fireTimer.isTimedOut()) {
					bullets->push_back(threeDEntity::Bullet(this->torso.center, dir, bulletSize, maxDistance, EntityConstants::HOSTILE));
					fireTimer.reset();
				}
			}
		}
	};
}