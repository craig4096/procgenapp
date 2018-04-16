#ifndef GAMEDEMO_H
#define GAMEDEMO_H

#include "../RenderableHeightmap.h"
#include "ShadersMap.h"
#include "Skybox.h"
#include "DungeonEntrance.h"
#include "Mesh.h"
#include "../Structural/ShapeDatabase.h"
#include "../Structural/BasicShapeGrammar.h"
#include "math3d.h"

class GameDemo {
    // the current level seed
    int levelSeed;
    // current water level
    float waterLevel;
    // current time
    float time;
    // background skybox
    Skybox* skybox;

    // determines if the player is in a dungeon or not
    bool isInDungeon;
    // dungeon data
    ShapeDatabase shapes;
    //BasicShapeGrammar grammar;
    float dungeonExitTimer;
    // when a player enters a dungeon this
    // will represent the index of the dungeon entrances from which they eneterd
    int dungeonIndex;

    RenderableHeightmap* heightmap;

    bool levelGenerated;

    ShadersMap shaders;
    void loadShadersMap(const std::string& filename);

    // sets the shaders and bind certain uniforms
    // such as 'time' variable
    void setShader(Shader*);
    void unsetShader(Shader*);

    // player & camera variables
    vec3 playerPos;
    vec3 viewDirection;
    vec3 rightVec;
    float rotationX;
    float rotationY;
    int resetX, resetY;
    bool mousePressing;
    Mesh* dungeonEntranceMesh;

    bool forward, backward, left, right;
    bool rforward, rbackward, rleft, rright;

    // when a player enteres a dungeon their position
    // will be saved for them coming out again
    vec3 playerExitPos;

    // draws a mesh using the shaders map to assign textures
    void drawMesh(Mesh*);

    // dungeon entrances
    std::vector<DungeonEntrance> dungeonEntrances;

    void drawDungeonEntrances();

    void generateDungeon(int seed);

    void drawOutside();
    void drawDungeon();

    // saved view matrix for each frame (used for setting lightDir in shaders)
    matrix4 viewMat;

public:
    GameDemo();
    ~GameDemo();

    void generateLevel(int seed);
    void update(float timeStep);
    void draw();
    void initGL();

    void mousePressed();
    void mouseReleased();
    // direction
    // 0 - forward
    // 1 - backward
    // 2 - left
    // 3 - right
    // release = move or not to move
    void move(int direction, bool moving);

    // same as above except with camera rotation
    void rotate(int direction, bool rotating);
};

#endif // GAMEDEMO_H
