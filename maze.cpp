#pragma once

#include "maze.h"
#include <iostream>
#include <list>
#include <vector>

using namespace std;
using namespace Maze;

MazeStruct Maze::generateMaze(int rows, int cols, float enemySpawnChance)
{
	int** matrix;
	matrix = new int* [rows];
	for (int i = 0; i < rows; i++) {
		matrix[i] = new int[cols];
		for (int j = 0; j < cols; j++) {
			matrix[i][j] = MazeConstants::WALL_SQUARE;
		}
	}

	std::vector<glm::vec2> emptySpaces;

	// how many times to randomly pick a square to start the maze from
	int factor = rows * cols - rows - cols;
	int randomRow = 1 + rand() % (rows - 2);
	int randomCol = 1 + rand() % (cols - 2);

	glm::vec2 startPoint = glm::vec2(randomCol, randomRow);
	matrix[(int)startPoint.y][(int)startPoint.x] = MazeConstants::CLEAR_PATH;

	emptySpaces.push_back(startPoint);
	// generate emptySpaces
	for (int i = 0; i < factor; i++) {
		int randomIdx = rand() % emptySpaces.size();
		glm::vec2 randomPos = emptySpaces[randomIdx];
		while (1) {
			std::vector<glm::vec2> availablePos;
			availablePos.clear();
			for (int j = -1; j <= 1; j += 2) {
				for (int k = -1; k <= 1; k += 2) {
					int upDown = j == -1 ? 0 : 1;
					int leftRight = 1 - upDown;

					glm::vec2 testPos = glm::vec2(randomPos.x + k * upDown, randomPos.y + k * leftRight);
					bool flag;
					// boundaries
					flag = testPos.x > 0 && testPos.x < cols - 1;
					flag = flag && testPos.y > 0 && testPos.y < rows - 1;

					flag = flag && (randomPos.x == testPos.x - 1 && randomPos.y == testPos.y ? true : matrix[(int)testPos.y][(int)testPos.x - 1] == MazeConstants::WALL_SQUARE);
					flag = flag && (randomPos.x == testPos.x + 1 && randomPos.y == testPos.y ? true : matrix[(int)testPos.y][(int)testPos.x + 1] == MazeConstants::WALL_SQUARE);
					flag = flag && (randomPos.x == testPos.x && randomPos.y == testPos.y - 1 ? true : matrix[(int)testPos.y - 1][(int)testPos.x] == MazeConstants::WALL_SQUARE);
					flag = flag && (randomPos.x == testPos.x && randomPos.y == testPos.y + 1 ? true : matrix[(int)testPos.y + 1][(int)testPos.x] == MazeConstants::WALL_SQUARE);
					if (flag) {
						availablePos.push_back(testPos);
					}
				}
			}
			if (availablePos.size() == 0) {
				break;
			}
			else {
				randomPos = availablePos[rand() % availablePos.size()];
				matrix[(int)randomPos.y][(int)randomPos.x] = MazeConstants::CLEAR_PATH;
				emptySpaces.push_back(randomPos);
			}
		}
	}

	// generate enemy squares
	int freeSpaces = emptySpaces.size() - 1;
	int enemySpaces = 0;
	for (int i = 1; i < emptySpaces.size(); i++) {
		int chance = rand() % 100 + 1; // 1 to 100
		if (chance <= enemySpawnChance) {
			matrix[(int)emptySpaces[i].y][(int)emptySpaces[i].x] = MazeConstants::ENEMY_SQUARE;
			enemySpaces++;
		}
	}

	int x, y;
	enum walls { East, West, North, South, last} wall;
	wall = (walls) (rand() % last);
	x = 1 + rand() % (cols - 2);
	y = 1 + rand() % (rows - 2);

	int xMod, yMod;
	xMod = 0;
	yMod = 0;

	switch (wall) {
	case East:
		x = 0;
		xMod = 1;
		break;
	case West:
		x = cols - 1;
		xMod = -1;
		break;
	case North:
		y = 0;
		yMod = 1;
		break;
	case South:
		y = rows - 1;
		yMod = -1;
	}
	matrix[y][x] = MazeConstants::EXIT_SQUARE;
	int count = 1;
	while (matrix[y + count * yMod][x + count * xMod] == MazeConstants::WALL_SQUARE) { // crash!!!!!
		matrix[y + count * yMod][x + count * xMod] = MazeConstants::CLEAR_PATH;
		count++;
	}
	matrix[y + yMod][x + xMod] = MazeConstants::BOSS_SQUARE;

	matrix[(int)startPoint.y][(int)startPoint.x] = MazeConstants::PLAYER_SQUARE;
	return MazeStruct(rows, cols, matrix, startPoint);
}

void Maze::printMaze(MazeStruct maze)
{
	for (int i = 0; i < maze.M; i++) {
		for (int j = 0; j < maze.N; j++) {
			cout << maze.matrix[i][j] << " ";
		}
		cout << endl;
	}
}

glm::vec2 Maze::getMazePos(MazeStruct maze, float resizeFactor, glm::vec3 pos) {
	int row = maze.M - (pos.x + resizeFactor / 2) / resizeFactor;
	int col = (pos.z + resizeFactor / 2) / resizeFactor;
	return glm::vec2(row, col);
}

glm::vec2 Maze::getMinimapPos(float resizeFactor, glm::vec3 pos, glm::vec2 displacement) {
	return resizeFactor * (glm::vec2(pos.z, pos.x) + displacement);
}
