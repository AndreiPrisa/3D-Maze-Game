#include "lab_m1/Tema2/Constants.h"
#include <glm/ext/vector_float2.hpp>

namespace Maze {
    struct MazeStruct
    {
        MazeStruct() : M(0), N(0), matrix(0), startPoint(0) {}
        MazeStruct(int M, int N, int **matrix, glm::vec2 startPoint) :
            M(M), N(N), matrix(matrix), startPoint(startPoint) {}
        int M;
        int N;
        int **matrix;
        glm::vec2 startPoint;
    };

	MazeStruct generateMaze(int lines, int rows, float enemySpawnChance);
    glm::vec2 getMazePos(MazeStruct maze, float resizeFactor, glm::vec3 pos);
    glm::vec2 getMinimapPos(float resizeFactor, glm::vec3 pos, glm::vec2 displacement = glm::vec2(0));
	void printMaze(MazeStruct maze);
}