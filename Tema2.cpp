#include "lab_m1/Tema2/Tema2.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    // generate seed
    srand(time(nullptr));
    renderCameraTarget = true;
    width = 10;
    height = 10;
    fov = RADIANS(60.0f);
    zNear = 0.01f;
    zFar = 200.0f;
    perspective = true;

    // toggles
    toggleCursor = false;
    toggleFullscreen = false;
    firstPerson = false;
    jumping = false;
    defeatedBoss = false;
    godMode = false;
    toggleWireFrame = false;


    // 2D objects
    glm::vec3 centre = glm::vec3(0.1f, 0.1f, 0);
    glm::vec3 corner = glm::vec3(0, 0, -300);
    squareSide = 1;

    // fara cursor
    window->DisablePointer();
    // dezactivam comenzile implicite ale camerei
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(Positions::HUDPos);
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    // get fullscreen dimensions and normal dimensions
    GLFWvidmode videoDisplay = *glfwGetVideoMode(glfwGetPrimaryMonitor());
    implicitResolution = glm::vec2(window->GetResolution().x, window->GetResolution().y);
    fullScreenResolution = glm::vec2(videoDisplay.width, videoDisplay.height);

    // Light & material properties
    {
        lightPosition = glm::vec3(0, 1, 1);
        materialShininess = 15;
        materialKd = 0.5;
        materialKs = 0.1;
        lightIntensity = 1;
    }

    // units initialize
    squareUnit = 1;
    sphereUnit = 0.33f;
    planeUnit = 0.02f;
    fallSpeed = 3.0f;
    cameraSpeed = 1.5f;
    speedAmplifier = 2;

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("healthPack");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "Tema2"), "healthPack.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }


    {
        Shader* shader = new Shader("MyShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader = new Shader("EnemyShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShaderEnemy.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // HUD
    Mesh* healthBarBorder = object2D::CreateSquare("healthBorder", corner, squareSide, Colors::red);
    AddMeshToList(healthBarBorder);

    Mesh* healthBarFull = object2D::CreateSquare("healthFull", corner, squareSide, Colors::red, true);
    AddMeshToList(healthBarFull);

    Mesh* timeBarBorder = object2D::CreateSquare("timeBorder", corner, squareSide, Colors::yellow);
    AddMeshToList(timeBarBorder);

    Mesh* timeBarFull = object2D::CreateSquare("timeFull", corner, squareSide, Colors::yellow, true);
    AddMeshToList(timeBarFull);

    Mesh* sprintBorder = object2D::CreateSquare("sprintBorder", corner, squareSide, Colors::lightBlue);
    AddMeshToList(sprintBorder);

    Mesh* sprintFull = object2D::CreateSquare("sprintFull", corner, squareSide, Colors::lightBlue, true);
    AddMeshToList(sprintFull);

    Mesh* jumpBorder = object2D::CreateSquare("jumpBorder", corner, squareSide, Colors::lightGreen);
    AddMeshToList(jumpBorder);

    Mesh* jumpFull = object2D::CreateSquare("jumpFull", corner, squareSide, Colors::lightGreen, true);
    AddMeshToList(jumpFull);

    // minimap
    Mesh* wall2D = object2D::CreateSquare("wall2D", corner, squareSide, Colors::lightBlue, true);
    AddMeshToList(wall2D);

    Mesh* exitCube2D = object2D::CreateSquare("exitCube2D", corner, squareSide, Colors::yellow, true);
    AddMeshToList(exitCube2D);

    Mesh* playerMesh2D = object2D::CreatePolygon("playerMesh2D", centre, squareSide, Colors::cyan, 120, true);
    AddMeshToList(playerMesh2D);

    Mesh* enemyMesh2D = object2D::CreatePolygon("enemyMesh2D", centre, squareSide, Colors::red, 5, true);
    AddMeshToList(enemyMesh2D);
    ///

    maze = Maze::generateMaze(15, 15, 35);
    Maze::printMaze(maze);
    
    // scale factors
    scaleFactor = 2;
    moveFactor = squareUnit * scaleFactor;

    glm::vec3 centerOfMaze = glm::vec3(maze.N * moveFactor / 2, 10 * moveFactor, maze.M * moveFactor / 2);

    // bullet
    maxDistance = moveFactor;
    bulletSize = 0.01f * scaleFactor * sphereUnit;

    // enemy
    enemyRadius = 0.35f * scaleFactor * sphereUnit;
    minSpeedEnemy = 0.5f;
    maxSpeedEnemy = 2;
    spikeLength = 0.2f;

    // player
    fullHealthPlayer = 100;
    fullStaminaPlayer = 100;

    // main camera
    implemented::Camera* camera1 = new implemented::Camera();  
    float worldX = (maze.M - 1 - maze.startPoint.y) * moveFactor;
    float worldZ = maze.startPoint.x * moveFactor;
    projectionMatrix = glm::perspective(fov, window->props.aspectRatio, zNear, zFar);

    // generate walls positions;
    initializeMaze();
    

    // camera parameters
    implicitDistance = moveFactor / 4;
    shootingDistance = -player->head.dimensions.z / 2 - 0.04f;
    OXMinFirstPerson = -65.0f;
    OXMaxFirstPerson = 45.0f;
    OXMinThirdPerson = -65.0f;
    OXMaxThirdPerson = 30.0f;

    // main camera
    camera1->Set(glm::vec3(worldX, moveFactor / 2, worldZ), glm::vec3(worldX, moveFactor / 2, worldZ + implicitDistance),
        glm::vec3(0, 1, 0));
    cameras.push_back(camera1);
    mainCamera = camera1;
    mainCamera->distanceToTarget = implicitDistance;
    mainCamera = camera1;
    order = 0;

    // debug camera
    implemented::Camera* camera2 = new implemented::Camera();
    camera2->Set(centerOfMaze, glm::vec3(10, 0, 10), glm::vec3(0, 1, 0));
    cameras.push_back(camera2);
    glEnable(GL_CULL_FACE);
    polygonMode = GL_FILL;

    {
        logicSpace.x = 0;       // logic x
        logicSpace.y = 0;     // logic y
        logicSpace.width = 16 * 1.1f;   // logic width
        logicSpace.height = 9 * 1.1f;  // logic height
    }

    // HUD
    {
        barLength = 5;
        barHeight = 0.8f;
        difference = glm::vec2(0);
        barDistance = 0.5f;
        maxHealthBar = Entity::Rectangle(glm::vec2(logicSpace.width - barDistance - barLength,
            logicSpace.height - barDistance - barHeight), barLength, barHeight);
        maxTimeBar = Entity::Rectangle(glm::vec2(barDistance, logicSpace.height - barDistance - barHeight), barLength, barHeight);

        // Create the text renderer object
        glm::ivec2 resolution = window->GetResolution();
        textRenderer = new gfxc::TextRenderer(window->props.selfDir, resolution.x, resolution.y);
        textScaleFactor.x = resolution.x / logicSpace.width;
        textScaleFactor.y = resolution.y / logicSpace.height;
        textFontSize = barHeight / 2;
        textRenderer->Load(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::FONTS, "Hack-Bold.ttf"), textScaleFactor.y * textFontSize);
    }

    // minimap
    {
        scaleFactor2D = squareSide;
        numberOfSeenSpaces = glm::vec2(5, 3);
        minimapCamPos = glm::vec3(0);
        minimapLogicSpace = LogicSpace(0, 0, numberOfSeenSpaces.x * scaleFactor2D, numberOfSeenSpaces.y* scaleFactor2D);
        miniMap = MiniMap(glm::vec2(0, -numberOfSeenSpaces.x * scaleFactor2D), glm::vec2(maze.M * scaleFactor2D, maze.N * scaleFactor2D));
    }

    // timers
    timeToLeaveMaze = Timer(300);
    fireTime = Timer(0.5f);
}

float Tema2::randomFloatValue(float  low, float high) {
    return  low + rand() / (RAND_MAX / (high - low));
}

void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

std::vector<threeDEntity::Cube*> Tema2::getNeighbours(glm::vec2 position) {
    std::vector<threeDEntity::Cube*> neighbours;
    std::vector<glm::vec2> pos = {  glm::vec2(position.x - 1, position.y),
                                    glm::vec2(position.x + 1, position.y), 
                                    glm::vec2(position.x, position.y - 1),
                                    glm::vec2(position.x, position.y + 1) };
    
    for (int i = 0; i < pos.size(); i++) {
        int row, col;
        row = pos[i].x;
        col = pos[i].y;
        if (row < maze.M && row >= 0 && col < maze.N && col >= 0) {
            threeDEntity::Cube* wall;
            if (walls.find(glm::ivec2(row, col)) != walls.end()) {
                wall = walls.at(glm::ivec2(row, col));
                neighbours.push_back(wall);
            }
        }
    }
    return neighbours;
}

void Tema2::initializeMaze() {
    for (int row = maze.M - 1; row >= 0; row--) {
        for (int col = 0; col < maze.N; col++) {

            float worldX = (maze.M - 1 - row) * moveFactor;
            float worldY = moveFactor / 2;
            float worldZ = col * moveFactor;

            //float worldX2D = miniMap.corner.x + (row + 1) * squareSide;
            //float worldY2D = miniMap.corner.y + col * squareSide;

            if (maze.matrix[row][col] == MazeConstants::WALL_SQUARE) {
                {
                    walls[glm::ivec2(row, col)] = new threeDEntity::Cube(glm::vec3(worldX, worldY, worldZ), glm::vec3(moveFactor), Colors::lightBlue);
                    //walls2D.push_back(Entity::Rectangle(glm::vec2(worldX2D, worldY2D), squareSide, squareSide));
                    //cout << worldX << " " << worldY << endl;
                    //cout << walls.back().center << endl;
                }
            }
            else if (maze.matrix[row][col] == MazeConstants::PLAYER_SQUARE) {
                player = new Player(glm::vec3(worldX, worldY, worldZ), moveFactor, Colors::beige, Colors::cyan, Colors::blue);
                //player2D = Entity::Circle(glm::vec2(worldX2D - 1 / 2 * squareSide,worldY2D + 1 / 2 * squareSide), squareSide / 2);
                glm::vec2 minimapPlayerPos = Maze::getMinimapPos(scaleFactor2D / moveFactor, player->position);
                float x = minimapPlayerPos.x - (numberOfSeenSpaces.x - 1) * scaleFactor2D;
                float y = minimapPlayerPos.y - (numberOfSeenSpaces.y - 1) * scaleFactor2D;
                minimapLogicSpace = LogicSpace(x, y,
                    numberOfSeenSpaces.y * scaleFactor2D,
                    numberOfSeenSpaces.x * scaleFactor2D);

                cout << glm::vec2(row, col) << endl;
            }
            else if (maze.matrix[row][col] == MazeConstants::ENEMY_SQUARE) {
                threeDEntity::Cube spawnArea = threeDEntity::Cube(glm::vec3(worldX, worldY, worldZ), glm::vec3(moveFactor), glm::vec3(0));
                enemies.push_back(new Hostiles::Enemy(EntityConstants::NORMAL_TYPE, spawnArea, enemyRadius, 
                    randomFloatValue(minSpeedEnemy, maxSpeedEnemy), Colors::lightGreen, Directions::xDir, spikeLength * sphereUnit));
            }
            else if (maze.matrix[row][col] == MazeConstants::EXIT_SQUARE) {
                cout << glm::vec2(row, col) << endl;
                exitCube = threeDEntity::Cube(glm::vec3(worldX, worldY, worldZ), glm::vec3(moveFactor), Colors::yellow);
                walls[glm::ivec2(row, col)] = &exitCube;
            }
            else if (maze.matrix[row][col] == MazeConstants::BOSS_SQUARE) {
                threeDEntity::Cube spawnArea = threeDEntity::Cube(glm::vec3(worldX, worldY, worldZ), glm::vec3(moveFactor), glm::vec3(0));
                int groupSpeed = randomFloatValue(1, 2);
                bossGroup.push_back((Hostiles::Enemy *) new Hostiles::Gunner(EntityConstants::GUNNER_TYPE, spawnArea, 0.7f * enemyRadius,
                    groupSpeed, Colors::lilla, Directions::xDir, spikeLength * sphereUnit));
                bossGroup.push_back((Hostiles::Enemy*) new Hostiles::Gunner(EntityConstants::GUNNER_TYPE, spawnArea, 0.7f * enemyRadius,
                    groupSpeed, Colors::lilla, -Directions::xDir, spikeLength * sphereUnit));
                bossGroup.push_back((Hostiles::Enemy*) new Hostiles::Gunner(EntityConstants::GUNNER_TYPE, spawnArea, 0.7f * enemyRadius,
                    groupSpeed, Colors::lilla, Directions::zDir, spikeLength * sphereUnit));
                bossGroup.push_back((Hostiles::Enemy*) new Hostiles::Gunner(EntityConstants::GUNNER_TYPE, spawnArea, 0.7f * enemyRadius,
                    groupSpeed, Colors::lilla, -Directions::zDir, spikeLength * sphereUnit));
                bossGroup.push_back(new Hostiles::Enemy(EntityConstants::BOSS_TYPE, spawnArea, 1.5f * enemyRadius,
                    0.5f, Colors::darkGreen, glm::vec3(0), spikeLength * sphereUnit, 60, 100));
            }
        }
    }
}

void Tema2::UpdateMinimapCamPos() {
    minimapCamPos = glm::vec3(Maze::getMinimapPos(scaleFactor2D / moveFactor, player->position), 0);
    glm::vec2 minimapPlayerPos = Maze::getMinimapPos(scaleFactor2D / moveFactor, player->position);
    float x = minimapPlayerPos.x - (numberOfSeenSpaces.x / 2) * scaleFactor2D;
    float y = minimapPlayerPos.y - (numberOfSeenSpaces.y / 2) * scaleFactor2D;
    minimapLogicSpace.x = x;
    minimapLogicSpace.y = y;
    //cout << minimapPlayerPos << " minimap is " << minimapLogicSpace.x << " " << minimapLogicSpace.y << " " << 
      //  minimapLogicSpace.width << " " << minimapLogicSpace.height << endl;
}

void Tema2::rechargeStamina(float deltaTime) {
    float baseRechargeFactor = 25;

    if (!sprinting) {
        if (moving) {
            baseRechargeFactor /= 2;
        }
        player->stamina.recharge(deltaTime, baseRechargeFactor);
    }
}

// return true if player is currently jumping
bool Tema2::jump(float deltaTime, bool noObstacle) {
    if (player->jumpCoolDown.isFull()) {

        if (player->jumpingTime.currentTime >= player->jumpingTime.timeLimit / 2) {
            mainCamera->TranslateUpward(deltaTime * fallSpeed * 2);
        }

        if (noObstacle) {
            mainCamera->position += jumpVelocity * deltaTime * jumpingDir;
        }

        player->jumpingTime.update(deltaTime);
        if (player->jumpingTime.isTimedOut()) {
            player->jumpingTime.reset();
            player->jumpCoolDown.empty();
        }
    }

    return !player->jumpingTime.isFull();
}

void Tema2::Update(float deltaTimeSeconds)
{
    static bool printed = false;
    if (bossGroup.empty()) {
        defeatedBoss = true;
    }

    // update Timers
    timeToLeaveMaze.update(deltaTimeSeconds);
    fireTime.update(deltaTimeSeconds);
    player->jumpCoolDown.recharge(deltaTimeSeconds);
    rechargeStamina(deltaTimeSeconds);

    if (timeToLeaveMaze.isTimedOut() || player->health <= 0 || (defeatedBoss && threeDEntity::checkCollision(exitCube, player->collisionBox))) {
        if (!printed) {
            cout << "====================" << endl << "Game Over" << endl << "====================";
            printed = true;
        }
        glm::ivec2 resolution = window->GetResolution();
        viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
        SetViewportArea(viewSpace, Colors::lightBlue, true);
        return;
    }

    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
    if (perspective) {
        projectionMatrix = glm::perspective(fov, window->props.aspectRatio, zNear, zFar);
    }
    else {
        projectionMatrix = glm::ortho(-width / 2, width / 2, -height / 2, height / 2, zNear, zFar);
    }

    lightPosition = player->position + glm::vec3(0, player->head.dimensions.y + player->torso.dimensions.y / 2 + 0.5f, 0);

    // Rendering the floor
    {
        float worldX = (maze.M / 2) * moveFactor;
        float worldY = (maze.N / 2) * moveFactor;
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(worldX, 0, worldY));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(planeUnit * 1.2f * moveFactor * (float)max(maze.M, maze.N)));
        MyRenderMesh(meshes["plane"], shaders["MyShader"], modelMatrix, glm::vec3(0, 1, 0));
    }

    // Rendering light source
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, lightPosition);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
    }
    drawMaze();
    drawBullets(deltaTimeSeconds);
    drawEnemies(deltaTimeSeconds, player, &enemies);
    drawEnemies(deltaTimeSeconds, player, &bossGroup);

    

    if (renderCameraTarget)
    {
        player->UpdatePosition(mainCamera->GetTargetPosition() - 0.5f * glm::vec3(0, player->head.dimensions.y + player->torso.dimensions.y, 0));
        float animationTime = !sprinting ? 1 / cameraSpeed : 1 / (cameraSpeed * speedAmplifier);
        player->Animate(moving, deltaTimeSeconds, animationTime);
        drawPlayer();
    }

    // draw HUD
    {
        glm::ivec2 resolution = window->GetResolution();
        viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
        SetViewportArea(viewSpace, glm::vec3(0, 0, 0), false);
        glm::mat3 visMatrix = glm::mat3(1);
        visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);
        drawHUD(visMatrix, logicSpace, *player);
    }

    // draw minimap
    {
        glm::ivec2 resolution = window->GetResolution();
        float aspectRatio = minimapLogicSpace.width / minimapLogicSpace.height;
        float XDim = resolution.x / 6;
        viewSpace = ViewportSpace(0, 0, XDim, (XDim / aspectRatio));
        SetViewportArea(viewSpace, glm::vec3(0, 0, 0), true);
        glm::mat3 visMatrix = glm::mat3(1);
        visMatrix *= VisualizationTransf2DUnif(minimapLogicSpace, viewSpace);
        drawMinimap(visMatrix);
    }
    glm::ivec2 resolution = window->GetResolution();
    viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
    SetViewportArea(viewSpace, glm::vec3(0, 0, 0), false);
}



void Tema2::FrameEnd()
{
    //DrawCoordinateSystem(mainCamera->GetViewMatrix(), projectionMatrix);
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    moving = false;
    sprinting = false;
    bool ableToMoveFWD = false;

    static glm::vec3 correctionDirection = glm::vec3(0);
    std::vector<threeDEntity::Cube*> neighbours = getNeighbours(Maze::getMazePos(maze, moveFactor, player->position));

    if (!godMode) {
        if (player->position.y - player->leg.dimensions.y - player->torso.dimensions.y / 2 - fallSpeed * deltaTime > 0) {
            mainCamera->position.y -= fallSpeed * deltaTime;
        }
    }


    if (window->KeyHold(GLFW_KEY_W) && !jumping) {
        if (threeDEntity::canMove(player->collisionBox, neighbours, cameraSpeed * deltaTime * mainCamera->forward) == nullptr) {
            ableToMoveFWD = true;
            moving = true;
        }
    }

    if (window->KeyHold(GLFW_KEY_LEFT_SHIFT)) {
        float drainFactor = 40;

        // speed up player
        if (!player->stamina.isTimedOut() && moving) {
            player->stamina.update(deltaTime, drainFactor);
            if (player->stamina.currentTime > 1.2f * deltaTime) {
                deltaTime *= speedAmplifier;
                sprinting = true;
            }
        }
    }

    if (jumping) {
        bool noObstacle = (threeDEntity::canMove(player->collisionBox, neighbours, jumpVelocity * deltaTime * jumpingDir) == nullptr);

        jumping = jump(deltaTime, noObstacle);
        moving = true;
    }

    if (ableToMoveFWD) {
        mainCamera->MoveForward(cameraSpeed * deltaTime);
    }



    if (window->KeyHold(GLFW_KEY_A) && !jumping) {
        if (threeDEntity::canMove(player->collisionBox, neighbours, -cameraSpeed * deltaTime * mainCamera->right) == nullptr) {
            mainCamera->TranslateRight(-cameraSpeed * deltaTime);
            moving = true;
        }
        
    }

    if (window->KeyHold(GLFW_KEY_S) && !jumping) {
        if (threeDEntity::canMove(player->collisionBox, neighbours, -cameraSpeed * deltaTime * mainCamera->forward) == nullptr) {
            mainCamera->MoveForward(-cameraSpeed * deltaTime);
            moving = true;
        }
    }

    if (window->KeyHold(GLFW_KEY_D) && !jumping) {
        if (threeDEntity::canMove(player->collisionBox, neighbours, cameraSpeed * deltaTime * mainCamera->right) == nullptr) {
            mainCamera->TranslateRight(cameraSpeed * deltaTime);
            moving = true;
        }
    }

    if (deltaTime != INFINITY) {
        threeDEntity::Cube* collisionCube;
        if ((collisionCube = threeDEntity::canMove(player->collisionBox, neighbours)) != nullptr) {
            glm::vec3 dir = glm::normalize(player->position - collisionCube->center);

            mainCamera->position += 0.01f * glm::vec3(dir.x, 0, dir.z);
        }
    }

    if (window->KeyHold(GLFW_KEY_Q) && godMode) {
        // TODO(student): Translate the camera downward
        mainCamera->TranslateUpward(-2 * cameraSpeed * deltaTime);
    }

    if (window->KeyHold(GLFW_KEY_E) && godMode) {
        // TODO(student): Translate the camera upward
        mainCamera->TranslateUpward(2 * cameraSpeed * deltaTime);
    }

}


void Tema2::OnKeyPress(int key, int mods)
{
    static float currentHealth, currentStamina;
    // Add key press event

    // toggle cursor
    if (key == GLFW_KEY_T)
    {
        toggleCursor = !toggleCursor;
        if (toggleCursor) {
            window->ShowPointer();
        }
        else {
            window->DisablePointer();
        }
    }

    if (key == GLFW_KEY_SPACE && !jumping) {
        jumping = true;
        jumpingDir = mainCamera->forward;
        jumpingDir.y = 0;
        jumpVelocity = !sprinting ? cameraSpeed : cameraSpeed * speedAmplifier;
        jumpVelocity *= 1.2f;
    }

    if (key == GLFW_KEY_TAB && godMode)
    {
        //switch (polygonMode) {
        //case GL_FILL:
        //    polygonMode = GL_LINE;
        //    break;
        //case GL_LINE:
        //    polygonMode = GL_FILL;
        //    break;
        //}
        toggleWireFrame = !toggleWireFrame;
    }

    if (key == GLFW_KEY_C) {
        cout << Maze::getMazePos(maze, moveFactor, player->position) << endl;
        cout << player->position << endl;
        std::vector<threeDEntity::Cube*> neighbours = getNeighbours(Maze::getMazePos(maze, moveFactor, player->position));
        cout << neighbours.size() << endl;
    }


    // fullscreen
    if (key == GLFW_KEY_ENTER && mods == GLFW_MOD_ALT) {
        toggleFullscreen = !toggleFullscreen;
        if (toggleFullscreen) {
            window->SetSize(fullScreenResolution.x, fullScreenResolution.y);
            window->CenterWindow();
        }
        else {
            window->SetSize(implicitResolution.x, implicitResolution.y);
            window->CenterWindow();
        }
    }

    // toggle god mode
    if (key == GLFW_KEY_G && mods == GLFW_MOD_ALT) {
        godMode = !godMode;
        if (godMode) {
            cout << "GOD MODE ENABLED" << endl;
            currentHealth = player->health;
            currentStamina = player->stamina.currentTime;
            player->health = INFINITY;
            player->stamina.timeLimit = INFINITY;
            player->stamina.currentTime = INFINITY;
            toggleWireFrame = true;
        }
        else {
            player->health = currentHealth;
            player->stamina.timeLimit = fullStaminaPlayer;
            player->stamina.currentTime = currentStamina;
            cout << "GOD MODE DISABLED" << endl;
            toggleWireFrame = false;
        }
    }


    // BONUS
    if (window->KeyHold(GLFW_KEY_Z)) {
        order = (int)((double)order + 1) % cameras.size();
        mainCamera = cameras[order];
    }

}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        renderCameraTarget = true;
        float OXinclination = -deltaY * sensivityOX;
        glm::vec3 planeDir = glm::normalize(glm::vec3(mainCamera->forward.x, 0, mainCamera->forward.z));
        float cameraInclination = glm::acos(glm::dot(mainCamera->forward, planeDir));
        cameraInclination *= mainCamera->forward.y > 0 ? 1 : -1;

        float OXMax = firstPerson ? OXMaxFirstPerson : OXMaxThirdPerson;
        float OXMin = firstPerson ? OXMinFirstPerson : OXMinThirdPerson;

        if (DEGREES(OXinclination + cameraInclination) >= OXMax) {
            OXinclination = -cameraInclination + RADIANS(OXMax);
        }
        else if (DEGREES(OXinclination + cameraInclination) <= OXMin) {
            OXinclination = -cameraInclination + RADIANS(OXMin);
        }
        mainCamera->RotateThirdPerson_OX(OXinclination);
        mainCamera ->RotateThirdPerson_OY(-deltaX * sensivityOY);
        player->UpdateOYRotation(DEGREES(-deltaX * sensivityOY));
      
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_RIGHT))
    {
        mainCamera->distanceToTarget = shootingDistance;
        mainCamera->position += (implicitDistance - shootingDistance) * mainCamera->forward;
        firstPerson = true;

        glm::vec3 planeDir = glm::normalize(glm::vec3(mainCamera->forward.x, 0, mainCamera->forward.z));
        float cameraInclination = glm::acos(glm::dot(mainCamera->forward, planeDir));
        cameraInclination *= mainCamera->forward.y > 0 ? 1 : -1;

        if (DEGREES(cameraInclination) >= OXMaxFirstPerson) {
            mainCamera->RotateThirdPerson_OX(-cameraInclination + RADIANS(OXMaxFirstPerson));
        }
        else if (DEGREES(cameraInclination) <= OXMinFirstPerson) {
            mainCamera->RotateThirdPerson_OX(-cameraInclination + RADIANS(OXMinFirstPerson));
        }
    }

    if (firstPerson && IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT) && fireTime.isTimedOut()) {
        bullets.push_back(threeDEntity::Bullet(mainCamera->position, mainCamera->forward, bulletSize, maxDistance));
        fireTime.reset();
    }
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_RIGHT))
    {
        mainCamera->distanceToTarget = implicitDistance;
        mainCamera->position -= (implicitDistance - shootingDistance) * mainCamera->forward;
        firstPerson = false;

        glm::vec3 planeDir = glm::normalize(glm::vec3(mainCamera->forward.x, 0, mainCamera->forward.z));
        float cameraInclination = glm::acos(glm::dot(mainCamera->forward, planeDir));
        cameraInclination *= mainCamera->forward.y > 0 ? 1 : -1;

        if (DEGREES(cameraInclination) >= OXMaxThirdPerson) {
            mainCamera->RotateThirdPerson_OX(-cameraInclination + RADIANS(OXMaxThirdPerson));
        }
        else if (DEGREES(cameraInclination) <= OXMinThirdPerson) {
            mainCamera->RotateThirdPerson_OX(-cameraInclination + RADIANS(OXMinThirdPerson));
        }
    }
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
    if (!toggleFullscreen) {
        implicitResolution.x = width;
        implicitResolution.y = height;
    }
    UpdateDiff();
}

// render functions
void Tema2::MyRenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, const float hitTimeValue, 
    const float health, const float fullHealth)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();

    // Set shader uniforms for light & material properties
    GLint location_light_position = glGetUniformLocation(shader->program, "light_position");
    glUniform3fv(location_light_position, 1, glm::value_ptr(lightPosition));

    glm::vec3 eyePosition = mainCamera->GetPosition();
    GLint location_eye_position = glGetUniformLocation(shader->program, "eye_position");
    glUniform3fv(location_eye_position, 1, glm::value_ptr(eyePosition));

    //// TODO(student): Set material property uniforms (shininess, kd, ks, object color)
    glUniform1i(glGetUniformLocation(shader->program, "material_shininess"), materialShininess);
    glUniform1f(glGetUniformLocation(shader->program, "material_kd"), materialKd);
    glUniform1f(glGetUniformLocation(shader->program, "material_ks"), materialKs);
    glUniform1f(glGetUniformLocation(shader->program, "lightIntensity"), lightIntensity);
    glUniform1f(glGetUniformLocation(shader->program, "spikeLength"), spikeLength);
    glUniform1f(glGetUniformLocation(shader->program, "hitTimeValue"), hitTimeValue);
    glUniform1f(glGetUniformLocation(shader->program, "health"), health);
    glUniform1f(glGetUniformLocation(shader->program, "fullHealth"), fullHealth);
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(mainCamera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniform3fv(glGetUniformLocation(shader->program, "object_color"), 1, glm::value_ptr(color));

    glUniform1f(glGetUniformLocation(shader->program, "timeValue"), Engine::GetElapsedTime());
    mesh->Render();
}

void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(mainCamera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

void Tema2::MyRenderMesh2D(Mesh* mesh, Shader* shader, const glm::mat3& modelMatrix, glm::mat3& visMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(visMatrix));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projMatrix2D));

    glm::mat3 mm = modelMatrix;
    glm::mat4 model = glm::mat4(
        mm[0][0], mm[0][1], mm[0][2], 0.f,
        mm[1][0], mm[1][1], mm[1][2], 0.f,
        0.f, 0.f, mm[2][2], 0.f,
        mm[2][0], mm[2][1], 0.f, 1.f);

    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(model));
    mesh->Render();
}

void Tema2::SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear)
{
    glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    glEnable(GL_SCISSOR_TEST);
    glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
    if (clear) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    glDisable(GL_SCISSOR_TEST);

    GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
    GetSceneCamera()->Update();
}

glm::mat3 Tema2::VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
    float sx, sy, tx, ty, smin;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    if (sx < sy)
        smin = sx;
    else
        smin = sy;
    tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
    ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

    return glm::transpose(glm::mat3(
        smin, 0.0f, tx,
        0.0f, smin, ty,
        0.0f, 0.0f, 1.0f));
}

void Tema2::UpdateDiff() {
    difference.x = 0; // width
    difference.y = 0; // height
    glm::ivec2 resolution = window->GetResolution();
    float visResolution = (float)resolution.x / resolution.y;
    float logicResolution = logicSpace.width / logicSpace.height;

    if (visResolution > logicResolution) {
        difference.x = visResolution * logicSpace.height - logicSpace.width;
    }
    else if ((visResolution < logicResolution)) {
        difference.y = 1 / visResolution * logicSpace.width - logicSpace.height;
    }
}

// draw functions
glm::mat4 Tema2::drawCube(threeDEntity::Cube cube, glm::mat4 startModelMatrix, glm::vec3 rotation) {

    glm::mat4 modelmatrix = glm::mat4(startModelMatrix);
    modelmatrix = glm::translate(modelmatrix, cube.center);
    modelmatrix = glm::rotate(modelmatrix, RADIANS(rotation.x), glm::vec3(1, 0, 0));
    modelmatrix = glm::rotate(modelmatrix, RADIANS(rotation.y), glm::vec3(0, 1, 0));
    modelmatrix = glm::rotate(modelmatrix, RADIANS(rotation.z), glm::vec3(0, 0, 1));
    glm::mat4 noScaleMatrix = modelmatrix;
    modelmatrix = glm::scale(modelmatrix, cube.dimensions);
    MyRenderMesh(meshes["box"], shaders["MyShader"], modelmatrix, cube.color);
    return noScaleMatrix;

}


void Tema2::drawMaze() {
    static auto flag = false;
    for (unordered_map<glm::ivec2, threeDEntity::Cube*>::iterator it = walls.begin(); it != walls.end(); it++) {
        glm::mat4 modelmatrix = glm::mat4(1);
        modelmatrix = glm::translate(modelmatrix, it->second->center);
        modelmatrix = glm::scale(modelmatrix, it->second->dimensions);
        MyRenderMesh(meshes["box"], shaders["MyShader"], modelmatrix, it->second->color);
    }
    if (!defeatedBoss) {
        glm::mat4 modelmatrix = glm::mat4(1);
        modelmatrix = glm::translate(modelmatrix, exitCube.center);
        modelmatrix = glm::scale(modelmatrix, exitCube.dimensions);
        MyRenderMesh(meshes["box"], shaders["MyShader"], modelmatrix, exitCube.color);
    }
}

void Tema2::drawEnemies(float deltaTime, Player* player, std::list<Hostiles::Enemy*>* listOfEnemies) {

    std::list<Hostiles::Enemy*>::iterator it = listOfEnemies->begin();
    while (it != listOfEnemies->end()) {
        Hostiles::Enemy* enemy = *it;
        {
            glm::mat4 modelmatrix = glm::translate(glm::mat4(1), enemy->position);
            modelmatrix = glm::scale(modelmatrix, glm::vec3(enemy->torso.radius * 2));
            MyRenderMesh(meshes["sphere"], shaders["EnemyShader"], modelmatrix, enemy->getDamageColor(),
                enemy->gotHitTimer.currentTime, enemy->health, enemy->fullHealth);
            if (toggleWireFrame) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glm::mat4 modelmatrix = glm::translate(glm::mat4(1), enemy->collisionBox.center);
                modelmatrix = glm::scale(modelmatrix, glm::vec3(enemy->collisionBox.radius * 2));
                MyRenderMesh(meshes["sphere"], shaders["MyShader"], modelmatrix);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
        enemy->gotHitTimer.update(deltaTime);

        if (enemy->health <= 0) {
            if (enemy->gotHitTimer.isTimedOut()) {
                if (enemy->type == EntityConstants::BOSS_TYPE) {
                    defeatedBoss = true;
                    walls.erase(Maze::getMazePos(maze, moveFactor, exitCube.center));
                }

                listOfEnemies->erase(it++);
                continue;
            }
            it++;
            continue;
        }
        enemy->move(player, deltaTime);

        if (enemy->type == EntityConstants::GUNNER_TYPE) {
            Hostiles::Gunner* gunner = (Hostiles::Gunner*)enemy;
            glm::vec3 dirToPlayer = glm::normalize(player->position - enemy->torso.center);
            gunner->shoot(deltaTime, &bullets, dirToPlayer, bulletSize * 1.4f, maxDistance);

            if (enemy->playerDetected) {
                std::list<threeDEntity::Bullet>::iterator itBullets = bullets.begin();
                threeDEntity::Bullet closestBullet;
                bool found = false;
                float minDistance = INFINITY;
                while (itBullets != bullets.end()) {
                    if (itBullets->type == EntityConstants::FRIENDLY) {
                        float dist = glm::distance(itBullets->body.center, enemy->collisionBox.center);
                        if (dist < minDistance) {
                            minDistance = dist;
                            closestBullet = *itBullets;
                            found = true;
                        }
                    }
                    itBullets++;
                }

                if (found) {
                    float dist = glm::distance(closestBullet.body.center, enemy->collisionBox.center);
                    glm::vec3 dirToDodge = closestBullet.direction;
                    dirToDodge = glm::normalize(glm::vec3(dirToDodge.z, 0, -dirToDodge.x));
                    if (dist > glm::distance(enemy->collisionBox.center + 0.1f * dirToDodge, closestBullet.body.center)) {
                        dirToDodge *= -1;
                    }
                    enemy->move(player, deltaTime, dirToDodge);
                }
            }
        }

        if (threeDEntity::checkCollision(enemy->collisionBox, player->collisionBox)) {
            player->health -= enemy->damage;
            if (enemy->type == EntityConstants::BOSS_TYPE) {
                defeatedBoss = true;
                walls.erase(Maze::getMazePos(maze, moveFactor, exitCube.center));
            }

            listOfEnemies->erase(it++);
        }
        else {
            it++;
        }
    }
}

void Tema2::drawBullets(float deltaTime) {
    static auto firstProvoked = false;
    std::list<threeDEntity::Bullet>::iterator itBullet = bullets.begin();
    while (itBullet != bullets.end()) {
        bool flagBullet = false;
        std::list<Hostiles::Enemy*>::iterator itEnemy = enemies.begin();
        std::list<Hostiles::Enemy*>::iterator itBossGroupEnemy = bossGroup.begin();

        if (itBullet->type == EntityConstants::FRIENDLY) {
            while (itEnemy != enemies.end()) {
                if ((*itEnemy)->health > 0 && threeDEntity::checkCollision(itBullet->body, (*itEnemy)->torso)) {
                    // alert enemy
                    (*itEnemy)->playerDetected = true;
                    (*itEnemy)->health -= itBullet->damage;
                    (*itEnemy)->gotHitTimer.reset();
                    bullets.erase(itBullet++);
                    flagBullet = true;
                    break;
                }
                itEnemy++;
            }

            while (itBossGroupEnemy != bossGroup.end() && !flagBullet) {
                if ((*itBossGroupEnemy)->health > 0 && threeDEntity::checkCollision(itBullet->body, (*itBossGroupEnemy)->torso)) {

                    // alert all group
                    for (Hostiles::Enemy* bossGroupEnemy : bossGroup) {
                        if (!firstProvoked) {
                            float randomSpeed = randomFloatValue(1, 1.4f);
                            if (bossGroupEnemy->type == EntityConstants::GUNNER_TYPE) {
                                bossGroupEnemy->speed = randomSpeed;
                            }
                        }
                        bossGroupEnemy->playerDetected = true;
                    }
                    firstProvoked = true;

                    (*itBossGroupEnemy)->health -= itBullet->damage;
                    (*itBossGroupEnemy)->gotHitTimer.reset();
                    bullets.erase(itBullet++);
                    flagBullet = true;
                    break;
                }
                itBossGroupEnemy++;
            }
        }
        else {
            if (threeDEntity::checkCollision(itBullet->body, player->collisionBox)) {
                player->health -= itBullet->damage;
                bullets.erase(itBullet++);
                flagBullet = true;
            }
        }

        if (!flagBullet) {
            threeDEntity::Bullet bullet = *itBullet;
            glm::mat4 modelmatrix = glm::translate(glm::mat4(1), bullet.position);
            modelmatrix = glm::scale(modelmatrix, glm::vec3(bullet.body.radius * 2));
            MyRenderMesh(meshes["sphere"], shaders["MyShader"], modelmatrix, bullet.body.color);

            itBullet->move(deltaTime);
        }

        if (!flagBullet) {
            if (itBullet->distanceTravelled >= itBullet->maxDistance) {
                bullets.erase(itBullet++);
                flagBullet = true;
            }
        }

        if (!flagBullet) {
            UMapIvec2::iterator itWalls = walls.begin();
            while (itWalls != walls.end()) {
                if (threeDEntity::checkCollision(itBullet->body, *(itWalls->second))) {
                    flagBullet = true;
                    bullets.erase(itBullet++);
                    break;
                }
                itWalls++;
            }
        }

        if (!flagBullet) {
            if (itBullet->position.y <= 0) {
                bullets.erase(itBullet++);
                flagBullet = true;
            }
        }

        if (!flagBullet) {
            itBullet++;
        }
    }
}

void Tema2::drawPlayer() {
    glm::mat4 torsoMatrix, leftJointLegMatrix, rightJointLegMatrix, leftJointArmMatrix, rightJointArmMatrix;
    glm::mat4 armMatrix;
    torsoMatrix = drawCube(player->torso, glm::mat4(1), glm::vec3(0, player->OYRotation, 0));
    leftJointLegMatrix = glm::mat4(1);
    rightJointLegMatrix = glm::mat4(1);
    leftJointArmMatrix = glm::mat4(1);
    rightJointArmMatrix = glm::mat4(1);
    armMatrix = glm::mat4(1);
    drawCube(player->head, torsoMatrix);

    // arms
    leftJointArmMatrix = glm::translate(torsoMatrix, player->leftArmJoint.center);
    leftJointArmMatrix = glm::rotate(leftJointArmMatrix, RADIANS(player->legArmOXRotation), glm::vec3(1, 0, 0));
    leftJointArmMatrix = glm::rotate(leftJointArmMatrix, RADIANS(-player->armOZRotation), glm::vec3(0, 0, 1));
    armMatrix = drawCube(player->arm, leftJointArmMatrix);
    drawCube(player->hand, armMatrix);

    rightJointArmMatrix = glm::translate(torsoMatrix, player->rightArmJoint.center);
    rightJointArmMatrix = glm::rotate(rightJointArmMatrix, RADIANS(-player->legArmOXRotation), glm::vec3(1, 0, 0));
    rightJointArmMatrix = glm::rotate(rightJointArmMatrix, RADIANS(player->armOZRotation), glm::vec3(0, 0, 1));
    armMatrix = drawCube(player->arm, rightJointArmMatrix);
    drawCube(player->hand, armMatrix);

    // legs
    leftJointLegMatrix = glm::translate(torsoMatrix, player->leftLegJoint.center);
    leftJointLegMatrix = glm::rotate(leftJointLegMatrix, RADIANS(player->legArmOXRotation), glm::vec3(1, 0, 0));
    drawCube(player->leg, leftJointLegMatrix);


    rightJointLegMatrix = glm::translate(torsoMatrix, player->rightLegJoint.center);
    rightJointLegMatrix = glm::rotate(rightJointLegMatrix, RADIANS(-player->legArmOXRotation), glm::vec3(1, 0, 0));
    drawCube(player->leg, rightJointLegMatrix);

    if (toggleWireFrame) {
        // collision box debug
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        drawCube(player->collisionBox, glm::mat4(1));
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

glm::vec2 Tema2::getBarCenter(Entity::Rectangle bar, LogicSpace logicSpace) {

    return glm::vec2(0);
}

void Tema2::writeInBarCenter(string text, Entity::Rectangle bar, LogicSpace logicSpace, glm::vec3 color) {
    float noChar = (float)text.length();
    glm::vec2 position;
    position.x = textScaleFactor.x * (difference.x / 2 + bar.corner.x + bar.width / 2 - (((float)noChar - 1) / 2 * 1 / 2 * textFontSize));
    position.y = textScaleFactor.y * (difference.y / 2 + (logicSpace.height - bar.corner.y - bar.height / 2 - textFontSize / 2));
    textRenderer->RenderText(text, position.x, position.y, 1, color);
}

void Tema2::drawHUD(glm::mat3 vm, LogicSpace logicSpace, Player player) {
    GetSceneCamera()->SetPosition(Positions::HUDPos);

    // health
    {

        // border
        glm::mat3 modelMatrix = vm;
        modelMatrix *= transform2D::Translate(logicSpace.x + maxHealthBar.corner.x + difference.x / 2, logicSpace.y + maxHealthBar.corner.y + difference.y / 2);
        modelMatrix *= transform2D::Scale(maxHealthBar.width, maxHealthBar.height);
        RenderMesh2D(meshes["healthBorder"], shaders["VertexColor"], modelMatrix);
        modelMatrix = vm;
        modelMatrix *= transform2D::Translate(logicSpace.x + maxHealthBar.corner.x + difference.x / 2, logicSpace.y + maxHealthBar.corner.y + difference.y / 2);
        modelMatrix *= transform2D::Scale(0.1f, 0.1f);
        RenderMesh2D(meshes["healthPack"], shaders["VertexColor"], modelMatrix);

        // fill
        modelMatrix = vm;
        modelMatrix *= transform2D::Translate(logicSpace.x + maxHealthBar.corner.x + difference.x / 2, logicSpace.y + maxHealthBar.corner.y + difference.y / 2);
        modelMatrix *= transform2D::Scale((player.health / fullHealthPlayer) * maxHealthBar.width, maxHealthBar.height);
        RenderMesh2D(meshes["healthFull"], shaders["VertexColor"], modelMatrix);
    }

    // timer
    {
        // border
        glm::mat3 modelMatrix = vm;
        modelMatrix *= transform2D::Translate(logicSpace.x + maxTimeBar.corner.x - difference.x / 2, logicSpace.y + maxTimeBar.corner.y + difference.y / 2);
        modelMatrix *= transform2D::Scale(maxTimeBar.width, maxTimeBar.height);
        RenderMesh2D(meshes["timeBorder"], shaders["VertexColor"], modelMatrix);

        // fill
        modelMatrix = vm;
        modelMatrix *= transform2D::Translate(logicSpace.x + maxTimeBar.corner.x - difference.x / 2, logicSpace.y + maxTimeBar.corner.y + difference.y / 2);
        modelMatrix *= transform2D::Scale((timeToLeaveMaze.currentTime / timeToLeaveMaze.timeLimit) * maxTimeBar.width, maxTimeBar.height);
        RenderMesh2D(meshes["timeFull"], shaders["VertexColor"], modelMatrix);
    }

    // sprint 
    {
        // border
        glm::mat3 modelMatrix = vm;
        modelMatrix *= transform2D::Translate(logicSpace.x + maxTimeBar.corner.x - difference.x / 2,
            logicSpace.y + maxTimeBar.corner.y + difference.y / 2 - maxTimeBar.height - 0.5f);
        modelMatrix *= transform2D::Scale(maxTimeBar.width, maxTimeBar.height);
        if (!player.stamina.isFull()) {
            RenderMesh2D(meshes["sprintBorder"], shaders["VertexColor"], modelMatrix);
        }

        // fill
        modelMatrix = vm;
        modelMatrix *= transform2D::Translate(logicSpace.x + maxTimeBar.corner.x - difference.x / 2,
            logicSpace.y + maxTimeBar.corner.y + difference.y / 2 - maxTimeBar.height - 0.5f);
        modelMatrix *= transform2D::Scale((player.stamina.currentTime / player.stamina.timeLimit) * maxTimeBar.width, maxTimeBar.height);
        if (!player.stamina.isFull()) {
            RenderMesh2D(meshes["sprintFull"], shaders["VertexColor"], modelMatrix);
        }
    }

    // jump boost
    {

        // border
        glm::mat3 modelMatrix = vm;
        modelMatrix *= transform2D::Translate(logicSpace.x + maxHealthBar.corner.x + difference.x / 2,
            logicSpace.y + maxHealthBar.corner.y + difference.y / 2 - maxHealthBar.height - 0.5f);
        modelMatrix *= transform2D::Scale(maxHealthBar.width, maxHealthBar.height);
        if (!player.jumpCoolDown.isFull()) {
            RenderMesh2D(meshes["jumpBorder"], shaders["VertexColor"], modelMatrix);
        }
        // fill
        modelMatrix = vm;
        modelMatrix *= transform2D::Translate(logicSpace.x + maxHealthBar.corner.x + difference.x / 2,
            logicSpace.y + maxHealthBar.corner.y + difference.y / 2 - maxHealthBar.height - 0.5f);
        modelMatrix *= transform2D::Scale((player.jumpCoolDown.currentTime / player.jumpCoolDown.timeLimit) * maxHealthBar.width, maxHealthBar.height);
        if (!player.jumpCoolDown.isFull()) {
            RenderMesh2D(meshes["jumpFull"], shaders["VertexColor"], modelMatrix);
        }
    }


}

void Tema2::drawMinimap(glm::mat3 vm) {
    UpdateMinimapCamPos();
    float conversionFactor = scaleFactor2D / moveFactor;

    // draw player
    {
        glm::mat3 modelmatrix = vm;
        glm::vec2 player2DPos = Maze::getMinimapPos(conversionFactor, player->position);
        glm::vec2 player2DDims = conversionFactor * glm::vec2(player->torso.dimensions.x, player->torso.dimensions.x);
        modelmatrix *= transform2D::Translate(player2DPos.x, player2DPos.y);
        modelmatrix *= transform2D::Rotate(RADIANS(player->OYRotation));
        modelmatrix *= transform2D::Scale(player2DDims.x * 1.5f, player2DDims.y);
        RenderMesh2D(meshes["playerMesh2D"], shaders["VertexColor"], modelmatrix);
    }

    // draw enemies
    {
        auto it = enemies.begin();
        while (it != enemies.end()) {
            Hostiles::Enemy* enemy = *it;
            {
                glm::mat3 modelmatrix = vm;
                glm::vec2 enemy2DPos = Maze::getMinimapPos(conversionFactor, enemy->position);
                glm::vec2 enemy2DDims = conversionFactor * glm::vec2(enemy->torso.radius, enemy->torso.radius);
                modelmatrix *= transform2D::Translate(enemy2DPos.x, enemy2DPos.y);
                modelmatrix *= transform2D::Scale(enemy2DDims.x, enemy2DDims.y);
                RenderMesh2D(meshes["enemyMesh2D"], shaders["VertexColor"], modelmatrix);
            }
            it++;
        }
    }

    {
        auto it = bossGroup.begin();
        while (it != bossGroup.end()) {
            Hostiles::Enemy* enemy = *it;
            {
                glm::mat3 modelmatrix = vm;
                glm::vec2 enemy2DPos = Maze::getMinimapPos(conversionFactor, enemy->position);
                glm::vec2 enemy2DDims = conversionFactor * glm::vec2(enemy->torso.radius, enemy->torso.radius);
                modelmatrix *= transform2D::Translate(enemy2DPos.x, enemy2DPos.y);
                modelmatrix *= transform2D::Scale(enemy2DDims.x, enemy2DDims.y);
                RenderMesh2D(meshes["enemyMesh2D"], shaders["VertexColor"], modelmatrix);
            }
            it++;
        }
    }

    // draw walls
    for (auto it = walls.begin(); it != walls.end(); it++) {
        threeDEntity::Cube wall = *it->second;
        glm::vec2 wall2DPos = Maze::getMinimapPos(conversionFactor, wall.center);
        glm::vec2 wall2DDims = conversionFactor * glm::vec2(wall.dimensions.z, wall.dimensions.x);
        glm::mat3 modelmatrix = vm;
        modelmatrix *= transform2D::Translate(wall2DPos.x - wall2DDims.x / 2,
            wall2DPos.y - wall2DDims.y / 2);
        modelmatrix *= transform2D::Scale(wall2DDims.x, wall2DDims.y);
        RenderMesh2D(meshes["wall2D"], shaders["VertexColor"], modelmatrix);
    }

    {
        glm::vec2 wall2DPos = Maze::getMinimapPos(conversionFactor, exitCube.center);
        glm::vec2 wall2DDims = conversionFactor * glm::vec2(exitCube.dimensions.z, exitCube.dimensions.x);
        glm::mat3 modelmatrix = vm;
        modelmatrix *= transform2D::Translate(wall2DPos.x - wall2DDims.x / 2,
            wall2DPos.y - wall2DDims.y / 2);
        modelmatrix *= transform2D::Scale(wall2DDims.x, wall2DDims.y);
        RenderMesh2D(meshes["exitCube2D"], shaders["VertexColor"], modelmatrix);
    }
}
