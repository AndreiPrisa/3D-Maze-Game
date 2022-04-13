#pragma once

#include <glm/ext/vector_float3.hpp>
#include "Constants.h"
#include "glm/glm.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
namespace threeDEntity {
    struct Cube
    {
        Cube() : center(0), dimensions(0), color(0) {}
        Cube(glm::vec3 center, glm::vec3 dimensions, glm::vec3 color)
            : center(center), dimensions(dimensions), color(color) {}
        glm::vec3 center;
        glm::vec3 dimensions;
        glm::vec3 color;
    };

    struct Sphere
    {
        Sphere() : center(0), radius(1), color(0) {};
        Sphere(glm::vec3 center, float radius, glm::vec3 color)
            : center(center), radius(radius), color(color) {}
        glm::vec3 center;
        float radius;
        glm::vec3 color;
    };

    class Bullet {
    public:
        Sphere body;
        glm::vec3 position;
        glm::vec3 direction;
        float distanceTravelled, maxDistance;
        float damage, speed;
        int type;

        Bullet(glm::vec3 position = glm::vec3(0), glm::vec3 direction = glm::vec3(0), float radius = 0,
            float maxDistance = 0, int type = EntityConstants::FRIENDLY, float speed = 4, float damage = 10, glm::vec3 color = glm::vec3(0)) {
            this->position = position;
            this->direction = direction;
            this->body = Sphere(position, radius, color);

            this->maxDistance = maxDistance;
            this->distanceTravelled = 0;

            this->damage = damage;
            this->speed = speed;

            this->type = type;
        }

        void move(float deltaTime) {
            this->distanceTravelled += speed * deltaTime;
            this->position += speed * deltaTime * this->direction;
            this->body.center = this->position;
        }
    };

    bool checkCollision(Cube cube1, Cube cube2);
    bool checkCollision(Sphere s1, Sphere s2);
    bool checkCollision(Sphere s, Cube c);
    Cube  *canMove(Cube collisionBox, std::vector<Cube*> obstacles, glm::vec3 displacement = glm::vec3(0));
}