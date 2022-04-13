#pragma once

#include "components/simple_scene.h"
#include "lab_m1/lab5/lab_camera.h"
#include "lab_m1/lab3/object2D.h"
#include "lab_m1/Tema1/Entity.h"
#include "threeDEntity.h"
#include "Player.h"
#include "Enemy.h"
#include "maze.h"
#include "Timer.h"
#include "components/text_renderer.h"


class Tema2 : public gfxc::SimpleScene
{
public:
    Tema2();
    ~Tema2();

    struct ViewportSpace
    {
        ViewportSpace() : x(0), y(0), width(1), height(1) {}
        ViewportSpace(int x, int y, int width, int height)
            : x(x), y(y), width(width), height(height) {}
        int x;
        int y;
        int width;
        int height;
    };

    struct LogicSpace
    {
        LogicSpace() : x(0), y(0), width(1), height(1) {}
        LogicSpace(float x, float y, float width, float height)
            : x(x), y(y), width(width), height(height) {}
        float x;
        float y;
        float width;
        float height;
    };

    void Init() override;

private:
    void FrameStart() override;
    void Update(float deltaTimeSeconds) override;
    void FrameEnd() override;

    void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;

    void OnInputUpdate(float deltaTime, int mods) override;
    void OnKeyPress(int key, int mods) override;
    void OnKeyRelease(int key, int mods) override;
    void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
    void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
    void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
    void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
    void OnWindowResize(int width, int height) override;


    void drawMaze();
    void drawPlayer();
    void drawMinimap(glm::mat3 vm);
    glm::mat4 drawCube(threeDEntity::Cube cube, glm::mat4 startModelMatrix, glm::vec3 rotation = glm::vec3(0));
    void initializeMaze();
    void MyRenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color = glm::vec3(1),
        const float hitTimeValue = 0, const float health = 0, const float fullHealth = 0);
    std::vector<threeDEntity::Cube*> getNeighbours(glm::vec2 position);
    void drawEnemies(float deltaTime, Player* player, std::list<Hostiles::Enemy*> *listOfEnemies);
    void drawBullets(float deltaTime);
    float randomFloatValue(float minVal, float maxVal);
    void rechargeStamina(float deltaTime);
    bool jump(float deltaTime, bool noObstacle = true);

    // HUD functions
    void MyRenderMesh2D(Mesh* mesh, Shader* shader, const glm::mat3& modelMatrix, glm::mat3& visMatrix);
    void drawHUD(glm::mat3 vm, LogicSpace logicSpace, Player player);
    glm::vec2 getBarCenter(Entity::Rectangle bar, LogicSpace logicSpace);
    void writeInBarCenter(string text, Entity::Rectangle bar, LogicSpace logicSpace, glm::vec3 color = glm::vec3(1));
    void SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear);
    glm::mat3 VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);
    void UpdateDiff();
    void UpdateMinimapCamPos();

protected:
    struct KeyHash {
        size_t operator()(const glm::ivec2& v) const
        {
            return hash<int>() (v.x) ^ hash<int>()(v.y);
        }
    };

    struct KeyEquals {
        bool operator()(const glm::ivec2& a, const glm::ivec2& b) const
        {
            return a.x == b.x && a.y == b.y;
        }
    };

    struct MiniMap {
        MiniMap() : corner(0), dimensions(0) {}
        MiniMap(glm::vec2 corner, glm::vec2 dimensions) : corner(corner), dimensions(dimensions) {}
       glm::vec2 corner;
       glm::vec2 dimensions;
    };


    implemented::Camera* mainCamera;
    std::vector<implemented::Camera*> cameras;
    int order;

    glm::mat4 projectionMatrix;
    bool renderCameraTarget;

    // TODO(student): If you need any other class variables, define them here.
    float fov;
    float width, height;
    float zNear, zFar;
    bool perspective;

    // maze
    Maze::MazeStruct maze;
    typedef std::unordered_map<glm::ivec2, threeDEntity::Cube*, KeyHash, KeyEquals> UMapIvec2;
    std::list<Hostiles::Enemy*> enemies;
    std::list<Hostiles::Enemy*> bossGroup;
    threeDEntity::Cube exitCube;
    float enemyRadius;
    float minSpeedEnemy, maxSpeedEnemy;
    float spikeLength;
    bool defeatedBoss;
    bool godMode;
    
    UMapIvec2 walls;

    // units
    float squareUnit;
    float sphereUnit;
    float planeUnit;
    float scaleFactor;
    float moveFactor;

    // player
    Player *player;
    float fullHealthPlayer, fullStaminaPlayer;

    // bullet
    std::list<threeDEntity::Bullet> bullets;
    float bulletSize;
    float maxDistance;

    // light and material properties
    glm::vec3 lightPosition;
    unsigned int materialShininess;
    float materialKd;
    float lightIntensity;
    float materialKs;

    // camera
    float implicitDistance;
    float shootingDistance;
    float OXMinFirstPerson, OXMaxFirstPerson;
    float OXMinThirdPerson, OXMaxThirdPerson;

    // resolution
    glm::vec2 implicitResolution;
    glm::vec2 fullScreenResolution;

    // toggle
    bool toggleCursor;
    bool toggleFullscreen;
    bool firstPerson;
    bool toggleWireFrame;

    // debug
    GLenum polygonMode;

    // physics
    float fallSpeed;
    float cameraSpeed;
    float speedAmplifier;
    float jumpVelocity;
    glm::vec3 jumpingDir;
    bool moving, sprinting, jumping;

    // HUD
    implemented::Camera* HUDCamera;
    ViewportSpace viewSpace;
    LogicSpace logicSpace;
    glm::vec2 difference;
    float barHeight, barLength;
    float barDistance;
    Entity::Rectangle maxHealthBar, maxTimeBar;
    glm::mat3 projMatrix2D;

    // minimap
    MiniMap miniMap;
    glm::vec3 minimapCamPos;
    glm::vec2 numberOfSeenSpaces;
    float squareSide, scaleFactor2D;
    LogicSpace minimapLogicSpace;
    std::list<Entity::Circle> enemies2D;
    std::vector<Entity::Rectangle> walls2D;
    Entity::Circle player2D;


    // time
    Timer timeToLeaveMaze;
    Timer fireTime;

    private:
    // text
        gfxc::TextRenderer* textRenderer;
        glm::vec2 textScaleFactor;
        float textFontSize;
};