#pragma once

#include "Constants.h"

const int MazeConstants::CLEAR_PATH = 0;
const int MazeConstants::WALL_SQUARE = 1;
const int MazeConstants::ENEMY_SQUARE = 2;
const int MazeConstants::PLAYER_SQUARE = 3;
const int MazeConstants::EXIT_SQUARE = 4;
const int MazeConstants::HEALTHPACK_SQUARE = 5;
const int MazeConstants::BOSS_SQUARE = 6;

const glm::vec3 Colors::beige = glm::vec3(0.96f, 0.96f, 0.86f);
const glm::vec3 Colors::cyan = glm::vec3(0.41f, 0.96f, 0.96f);
const glm::vec3 Colors::lightBlue = glm::vec3(0, 0.36f, 0.9f);
const glm::vec3 Colors::blue = glm::vec3(0, 0, 0.9f);
const glm::vec3 Colors::darkGreen = glm::vec3(0, 0.7f, 0);
const glm::vec3 Colors::lightGreen = glm::vec3(0.55f, 0.91f, 0.2f);
const glm::vec3 Colors::red = glm::vec3(0.7f, 0, 0);
const glm::vec3 Colors::yellow = glm::vec3(0.8f, 0.8f, 0);
const glm::vec3 Colors::lightOrange = glm::vec3(0.95f, 0.68f, 0.25f);
const glm::vec3 Colors::lilla = glm::vec3(0.8f, 0.8f, 1);

const int EntityConstants::FRIENDLY = 0;
const int EntityConstants::HOSTILE = 1;
const int EntityConstants::NORMAL_TYPE = 2;
const int EntityConstants::GUNNER_TYPE = 3;
const int EntityConstants::BOSS_TYPE = 4;

const glm::vec3 Directions::xDir = glm::vec3(1, 0, 0);
const glm::vec3 Directions::yDir = glm::vec3(0, 1, 0);
const glm::vec3 Directions::zDir = glm::vec3(0, 0, 1);


const glm::vec2 TimeValues::changeMotionInterval = glm::vec2(0.5f, 1.5f);
const glm::vec2 TimeValues::changeRotationInterval = glm::vec2(5, 8);
const glm::vec2 TimeValues::rotationAngleInterval = glm::vec2(45, 90);
const glm::vec2 TimeValues::changeColorInterval = glm::vec2(4, 8);
const glm::vec2 TimeValues::trackPointInterval = glm::vec2(6, 10);
const float TimeValues::colorChangeTime = 1.5f;

const glm::vec3 DancerConst::dimensions = glm::vec3(0.5f, 1, 0.5f);

const float Constants::minimalSpacing = 0.000001f;

const glm::vec3 Positions::HUDPos = glm::vec3(0, 0, 50);