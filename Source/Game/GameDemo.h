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
#include <glm/glm.hpp>

class GameDemo
{
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

    void drawTerrain();
    void drawDungeonEntrances();
    void drawWater();
    void drawWaterGrid(Shader& shader, float time);
    void drawWaterBounds(Shader& shader);
    void drawOutside();
    void drawDungeon();
    void drawClouds();

    void generateDungeon(int seed);

    // saved view matrix for each frame (used for setting lightDir in shaders)
    // TODO: use Viewport3D
    glm::mat4 modelViewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 modelViewProjectionMatrix;
    glm::mat4 normalMatrix;
    void updateMatrices();

    bool flyMode;

    // Water grid (TODO: Use RenderableHeightmap)
    GLuint waterGridVertexBuffer;
    GLuint waterGridIndexBuffer;
    std::vector<float> waterGridVertices;
    std::vector<int> waterGridIndices;

    // Water bounds (TODO: Use Mesh)
    GLuint waterBoundsVertexBuffer;
    GLuint waterBoundsIndexBuffer;
    std::vector<float> waterBoundsVertices;
    std::vector<int> waterBoundsIndices;

    // Clouds (TODO: Use Mesh)
    GLuint cloudsVertexBuffer;
    GLuint cloudsIndexBuffer;
    std::vector<float> cloudsVertices;
    std::vector<int> cloudsIndices;

public:
    GameDemo();
    ~GameDemo();

    void generateLevel(int seed);
    void update(float timeStep);
    void draw();
    void initGL();
    void setProjectionMatrix(int w, int h);

    void mousePressed();
    void mouseReleased();
    void mouseEnter();
    void mouseExit();
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
