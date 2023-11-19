#include "GameDemo.h"
#include <stdexcept>
#include <fstream>
// need to include this for reseting and querying mouse position
#include <wx/window.h>
#include <cstdlib>
#include <sstream>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/ext.hpp>

#include "../Terrain/TerrainGenerator.h"
#include "../Terrain/PerlinNoise.h"
#include "../Terrain/Normalization.h"
#include "../Terrain/InverseThermalErosion.h"
#include "../Terrain/ThermalErosion.h"

using namespace std;

static const int terrainSize = 512;
static const int waterGridSize = 50;
static const float waterGridScale = 12.0f;
static const float cloudPlaneSize = 300.0f;

GameDemo::GameDemo()
    : heightmap(nullptr)
    , levelGenerated(false)
    , waterLevel(50.0f)
    , time(0.0f)
    , resetX(50)
    , resetY(50)
    , rotationX(90.0f)
    , rotationY(0.0f)
    , mousePressing(false)
    , forward(false)
    , backward(false)
    , left(false)
    , right(false)
    , flyMode(true)
    , waterGridVertexBuffer(-1)
    , waterGridIndexBuffer(-1)
    , waterBoundsVertexBuffer(-1)
    , waterBoundsIndexBuffer(-1)
    , cloudsVertexBuffer(-1)
    , cloudsIndexBuffer(-1)
{
}

GameDemo::~GameDemo()
{
    delete skybox;
    delete dungeonEntranceMesh;

    glDeleteBuffers(1, &waterGridVertexBuffer);
    glDeleteBuffers(1, &waterGridIndexBuffer);
    glDeleteBuffers(1, &waterBoundsVertexBuffer);
    glDeleteBuffers(1, &waterBoundsIndexBuffer);
    glDeleteBuffers(1, &cloudsVertexBuffer);
    glDeleteBuffers(1, &cloudsIndexBuffer);
}


void GameDemo::initGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_MULTISAMPLE);

    // create an GL buffers etc. here
    loadShadersMap("shaders_map.txt");

    std::string skyboxDir = "skybox";
    std::string textures[6] = {
        skyboxDir + "/posx.jpg",
        skyboxDir + "/negx.jpg",
        skyboxDir + "/posy.jpg",
        skyboxDir + "/negy.jpg",
        skyboxDir + "/posz.jpg",
        skyboxDir + "/negz.jpg"
    };

    skybox = new Skybox(textures);

    // load the dungeon entrance mesh
    dungeonEntranceMesh = new Mesh("meshes/dungeon_entrance.txt");

    // load the mapping between shape symbols and mesh objects
    shapes.loadSymbolMeshMap("grammars/dungeon/mapping.txt");

    // Create the water grid vertex + index buffers
    {
        glGenBuffers(1, &waterGridVertexBuffer);
        waterGridVertices.resize(waterGridSize * waterGridSize * 3);

        // Set the x/z coordinates of the vertices (without height)
        const float startX = -(waterGridSize / 2.0f);
        const float startZ = -(waterGridSize / 2.0f);
        int index = 0;
        for (int z = 0; z < waterGridSize; ++z)
        {
            for (int x = 0; x < waterGridSize; ++x)
            {
                waterGridVertices[index++] = startX + x;
                index++; // ignore y
                waterGridVertices[index++] = startZ + z;
            }
        }

        // pre-calculate the water grid index buffer
        auto GetIndex = [](int x, int y) { return (y * waterGridSize) + x; };

        waterGridIndices.resize((waterGridSize - 1) * (waterGridSize - 1) * 6);
        index = 0;
        for (int x = 0; x < waterGridSize - 1; ++x)
        {
            for (int y = 0; y < waterGridSize - 1; ++y)
            {
                waterGridIndices[index++] = GetIndex(x, y);
                waterGridIndices[index++] = GetIndex(x, y + 1);
                waterGridIndices[index++] = GetIndex(x + 1, y + 1);
                waterGridIndices[index++] = GetIndex(x, y);
                waterGridIndices[index++] = GetIndex(x + 1, y + 1);
                waterGridIndices[index++] = GetIndex(x + 1, y);
            }
        }

        glGenBuffers(1, &waterGridIndexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterGridIndexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, waterGridIndices.size() * sizeof(int), &waterGridIndices[0], GL_STATIC_DRAW);
    }


    // Create the water bounds vertex + index buffers
    {
        waterBoundsVertices.resize(4 * 4 * 3); // grid of 4x4 vertices

        // construct water bounds vertices
        int index = 0;
        for (int j = 0; j < 4; ++j)
        {
            for (int i = 0; i < 4; ++i)
            {
                float x = (-1.5 + i) * waterGridSize;
                float z = (-1.5 + j) * waterGridSize;

                if (x < -waterGridSize) x = -10000.0f;
                if (x >  waterGridSize) x =  10000.0f;
                if (z < -waterGridSize) z = -10000.0f;
                if (z >  waterGridSize) z =  10000.0f;

                waterBoundsVertices[index++] = x;
                waterBoundsVertices[index++] = -4;
                waterBoundsVertices[index++] = z;
            }
        }

        glGenBuffers(1, &waterBoundsVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, waterBoundsVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, waterBoundsVertices.size() * sizeof(float), &waterBoundsVertices[0], GL_STATIC_DRAW);

        // index buffer...
        auto GetIndex = [](int x, int y) { return (y * 4) + x; };

        waterBoundsIndices.resize(3 * 3 * 6);
        index = 0;
        for (int y = 0; y < 3; ++y)
        {
            for (int x = 0; x < 3; ++x)
            {
                waterBoundsIndices[index++] = GetIndex(x, y);
                waterBoundsIndices[index++] = GetIndex(x, y + 1);
                waterBoundsIndices[index++] = GetIndex(x + 1, y + 1);
                waterBoundsIndices[index++] = GetIndex(x, y);
                waterBoundsIndices[index++] = GetIndex(x + 1, y + 1);
                waterBoundsIndices[index++] = GetIndex(x + 1, y);
            }
        }

        glGenBuffers(1, &waterBoundsIndexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterBoundsIndexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, waterBoundsIndices.size() * sizeof(int), &waterBoundsIndices[0], GL_STATIC_DRAW);
    }

    // Create the clouds vertex + index buffers
    {
        cloudsVertices.resize(2 * 2 * 3); // grid of 2x2 vertices

        // construct water bounds vertices
        int index = 0;
        for (int j = 0; j < 2; ++j)
        {
            for (int i = 0; i < 2; ++i)
            {
                const float x = ((i - 0.5f) * 2.0f) * cloudPlaneSize;
                const float z = ((j - 0.5f) * 2.0f) * cloudPlaneSize;

                cloudsVertices[index++] = x;
                cloudsVertices[index++] = 0.0;
                cloudsVertices[index++] = z;
            }
        }

        glGenBuffers(1, &cloudsVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, cloudsVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, cloudsVertices.size() * sizeof(float), &cloudsVertices[0], GL_STATIC_DRAW);

        // index buffer...
        cloudsIndices.resize(6);
        cloudsIndices[0] = 0;
        cloudsIndices[1] = 1;
        cloudsIndices[2] = 2;
        cloudsIndices[3] = 1;
        cloudsIndices[4] = 3;
        cloudsIndices[5] = 2;

        glGenBuffers(1, &cloudsIndexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cloudsIndexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, cloudsIndices.size() * sizeof(int), &cloudsIndices[0], GL_STATIC_DRAW);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void GameDemo::generateLevel(int seed)
{
    levelSeed = seed;
    delete heightmap;

    // call upon the terrain generator
    TerrainGenerator tergen(terrainSize, terrainSize);
    terrain_ops::PerlinNoise noise;
    noise.setSeed(seed);
    noise.setStartOctave(2);
    noise.setEndOctave(8);
    noise.setPersistance(0.4);
    tergen.addOperation(&noise);
    terrain_ops::Normalization norm;
    tergen.addOperation(&norm);

    terrain_ops::InverseThermalErosion ite;
    tergen.addOperation(&ite);

    terrain_ops::ThermalErosion te;
    te.setTalusAngle(0.0f);
    te.setNumIterations(2);
    tergen.addOperation(&te);

    Heightmap* h = tergen.generateTerrain(NULL);

    heightmap = new RenderableHeightmap(h);

    srand(seed);
    waterLevel = randomf() * 40 + 20;

    delete h;

    levelGenerated = false;
    srand(seed);
    // random generate a number of dungeon entrances
    dungeonEntrances.clear();
    for (int numAttempts = 0; numAttempts < 5; ++numAttempts)
    {
        // randomly select a point on the terrain
        vec3 pos;
        float w = heightmap->GetWidth() * heightmap->GetScaleX();
        float h = heightmap->GetHeight() * heightmap->GetScaleZ();

        pos.x = (randomf() * w) - (w / 2);
        pos.z = (randomf() * h) - (h / 2);

        pos.y = heightmap->GetWorldHeight(pos.x, pos.z);
        // if the placement point is above sea level then add the entrance
        if (pos.y > waterLevel)
        {
            DungeonEntrance e;
            e.position = pos;
            e.seed = rand();
            e.waitTime = 10.0f;
            dungeonEntrances.push_back(e);
            playerPos = pos;
        }
    }
    isInDungeon = false;
}

void GameDemo::updateMatrices()
{
    glGetFloatv(GL_PROJECTION_MATRIX, glm::value_ptr(projectionMatrix));
    glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(modelViewMatrix));

    // save model view projection matrix
    modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

    // calculate the normal matrix
    normalMatrix = glm::transpose(glm::inverse(modelViewMatrix));
}

void GameDemo::setShader(Shader* shader)
{
    shader->Set();
    glUniform1f(glGetUniformLocation(shader->GetProgram(), "time"), time);
    // set the view matrix unform
    glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "viewMat"), 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelViewProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
}

void GameDemo::unsetShader(Shader*)
{
    Shader::Unset();
}

vec3 calculateViewDir(float xRot, float yRot)
{
    vec3 target;
    // x and z can be calculated from the y axis
    // rotation using sin and cos
    target.x = sin(yRot * M_PI / 180.0f);
    target.z = cos(yRot * M_PI / 180.0f);

    // y can be calculated from the
    // x rotation using cos
    target.y = cos(xRot * M_PI / 180.0f);

    if (target.y > 0.0f)
    {
        target.x *= (1.0f - target.y);
        target.z *= (1.0f - target.y);
    }
    else
    {
        target.x *= (1.0f + target.y);
        target.z *= (1.0f + target.y);
    }

    return target;
}

void GameDemo::mousePressed()
{
    wxPoint p = wxGetMousePosition();
    resetX = p.x;
    resetY = p.y;
    mousePressing = true;
}

void GameDemo::mouseReleased()
{
    mousePressing = false;
}

void GameDemo::mouseEnter()
{
}

void GameDemo::mouseExit()
{
    mousePressing = false;
}

void GameDemo::update(float timeStep)
{
    time += timeStep;

    float speed = 16.0f;

    vec3 newPos = playerPos;
    float ytrans = (flyMode) ? viewDirection.y : 0.0f;

    if (forward)
    {
        newPos += vec3(viewDirection.x, ytrans, viewDirection.z) * speed * timeStep;
    }
    if (backward)
    {
        newPos -= vec3(viewDirection.x, ytrans, viewDirection.z) * speed * timeStep;
    }
    if (left)
    {
        newPos -= vec3(rightVec.x, 0.0, rightVec.z) * speed * timeStep;
    }
    if (right)
    {
        newPos += vec3(rightVec.x, 0.0, rightVec.z) * speed * timeStep;
    }

    static const float rspeed = 45.0f;
    if (rforward)
    {
        rotationX -= rspeed * timeStep;
    }
    if (rbackward)
    {
        rotationX += rspeed * timeStep;
    }
    if (rleft)
    {
        rotationY += rspeed * timeStep;
    }
    if (rright)
    {
        rotationY -= rspeed * timeStep;
    }
    // clamp x rotation somewhere
    // between 0 and 180
    if (rotationX < 1.0f)
    {
        rotationX = 1.0f;
    }
    if (rotationX > 179.0f)
    {
        rotationX = 179.0f;
    }

    if (rotationY >= 360.0f)
    {
        rotationY -= 360.0f;
    }
    if (rotationY < 0.0f)
    {
        rotationY += 360.0f;
    }

    // only update the camera if the user is clicking the mouse
    if (mousePressing)
    {
        // get the cursor position
        wxPoint p = wxGetMousePosition();
        // calculate the delta between the point and the screen
        // center...
        int dx = resetX - p.x;
        int dy = p.y - resetY;

        // rotate the camera around the y axis based
        // on how much the player moved the mouse left
        // or right
        rotationY += ((float)dx * 0.1);
        if (rotationY >= 360.0f)
            rotationY -= 360.0f;
        if (rotationY < 0.0f)
            rotationY += 360.0f;

        // rotate the camera around the x axis
        // based on how much the player moved the
        // mouse up or down
        rotationX += ((float)dy * 0.1);

        // clamp x rotation somewhere
        // between 0 and 180
        if (rotationX < 1.0f)
        {
            rotationX = 1.0f;
        }
        if (rotationX > 179.0f)
        {
            rotationX = 179.0f;
        }

        resetX = p.x;
        resetY = p.y;
    }

    // calculate the new view direction
    viewDirection = calculateViewDir(rotationX, rotationY);
    rightVec = viewDirection.crossProduct(vec3(0, 1, 0));
    rightVec.normalize();

    if (isInDungeon)
    {
        // dungeon collision etc.  here
        playerPos = newPos;
        //playerPos.y = 30.0f;
        dungeonExitTimer -= timeStep;
        if (dungeonExitTimer < 0.0f)
        {
            dungeonExitTimer = 0.0f;
        }

        // check if the player is within range of the dungeon starting point(0,0,0)
        if (dungeonExitTimer == 0.0f && playerPos.length() < 5.0f)
        {
            // exit the dungeon
            isInDungeon = false;
            // set player position back to previous pos
            playerPos = playerExitPos;

            // set the dungeon entrance timer to 10.0 so player doesnt immediately
            // go back into dungeon
            dungeonEntrances[dungeonIndex].waitTime = 10.0f;
        }
    }
    else
    {
        // player is outside
        if (flyMode)
        {
            playerPos = newPos;
        }
        else
        {
            // process collision with terrain
            if (heightmap != nullptr)
            {
                float height = heightmap->GetWorldHeight(newPos.x, newPos.z) + 1;
                newPos.y = height;
                // don't let players travel under water level
                if (newPos.y > waterLevel + 0.05f || true) {
                    playerPos = newPos;
                }
            }
        }

        playerPos = newPos;

        for (int i = 0; i < dungeonEntrances.size(); ++i)
        {
            DungeonEntrance& de = dungeonEntrances[i];

            // update the dungeon wait time
            de.waitTime -= timeStep;
            if (de.waitTime < 0.0f)
            {
                de.waitTime = 0.0f;
            }

            if ((playerPos - de.position).length() < 5.0f && de.waitTime <= 0.0f)
            {
                // load this dungeon
                generateDungeon(de.seed);
                // player is now in a dungeon
                isInDungeon = true;
                // save the players position for when they exit the dungeon
                playerExitPos = playerPos;

                // set the players new position to the start position (i.e. zero)
                playerPos = vec3(0, 2, 0);

                dungeonIndex = i;
                // set dungeon exit timer so player does immediately spawn outside
                dungeonExitTimer = 30.0f;
            }
        }
    }
}

void GameDemo::generateDungeon(int seed)
{
    shapes.clear();
    // add axiom shape
    Shape shape;
    shape.position.y = 1.0f;
    shape.yRotation = 0.0f;
    shape.symbol = "start_point";
    shape.terminal = false;
    shape.calculateBoundingBox(shapes.getSymbolMeshMap());
    shapes.insert(shapes.begin(), shape);
    srand(seed);
    int numIterations = (rand() % 100) + 10;

    static const int NUM_STAGES = 6;
    int num_iter[6] = {
        numIterations / 4,
        1,
        numIterations,
        1,
        8,
        1
    };

    for (int i = 1; i <= NUM_STAGES; ++i)
    {
        std::stringstream ss;
        ss << "grammars/dungeon/stage" << i << ".txt";
        BasicShapeGrammar grammar(ss.str());
        grammar.generate(shapes, num_iter[i - 1]);
    }
}

void GameDemo::drawMesh(Mesh* mesh)
{
    // for all submeshes
    for (int i = 0; i < mesh->getSubMeshCount(); ++i)
    {
        SubMesh* m = mesh->getSubMesh(i);
        // find the associated shader
        ShadersMap::iterator it = shaders.find(m->getMaterialName());
        if (it != shaders.end())
        {
            Shader* shader = it->second;
            if (shader)
            {
                setShader(shader);
                m->Draw(*shader);
                unsetShader(nullptr);
            }
        }
    }
}

void GameDemo::drawOutside()
{
    if (!levelGenerated)
    {
        heightmap->CreateRenderData();
        levelGenerated = true;
    }

    skybox->Draw(playerPos);

    drawTerrain();
    drawWater();
    drawDungeonEntrances();
    drawClouds();
}

void GameDemo::drawTerrain()
{
    if (heightmap != nullptr)
    {
        Shader* shader = shaders["terrain"];

        setShader(shader);
        heightmap->Draw(*shader);
        unsetShader(nullptr);
    }
}

void GameDemo::drawWater()
{
    glPushMatrix();
    glTranslatef(0, waterLevel, 0);
    glScalef(waterGridScale, 1.0f, waterGridScale);
    updateMatrices();

    // get the water shader
    Shader* s = shaders["water"];

    // bind shader
    setShader(s);

    drawWaterBounds(*s);
    drawWaterGrid(*s, time);

    unsetShader(s);

    glPopMatrix();
    updateMatrices();
}

void GameDemo::drawWaterBounds(Shader& shader)
{
    glBindBuffer(GL_ARRAY_BUFFER, waterBoundsVertexBuffer);
    GLint vertexLoc = glGetAttribLocation(shader.GetProgram(), "vertexPosition");
    glEnableVertexAttribArray(vertexLoc);
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterBoundsIndexBuffer);
    glDrawElements(GL_TRIANGLES, waterBoundsIndices.size(), GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(vertexLoc);
}

void GameDemo::drawWaterGrid(Shader& shader, float time)
{
    // Update the heights of the water grid vertices
    int index = 0;
    for (int z = 0; z < waterGridSize; ++z)
    {
        for (int x = 0; x < waterGridSize; ++x)
        {
            const float h1 = sin((x / (float)waterGridSize) * 2 * M_PI * 10 + (time * 0.5));
            const float hh1 = sin((z / (float)waterGridSize) * 2 * M_PI * 5 + (time * 1.0));

            index++; // skip x
            waterGridVertices[index++] = h1 * hh1 * 4;
            index++; // skip z
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, waterGridVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, waterGridVertices.size() * sizeof(float), &waterGridVertices[0], GL_DYNAMIC_DRAW);

    GLint vertexLoc = glGetAttribLocation(shader.GetProgram(), "vertexPosition");
    glEnableVertexAttribArray(vertexLoc);
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterGridIndexBuffer);
    glDrawElements(GL_TRIANGLES, waterGridIndices.size(), GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(vertexLoc);
}

void GameDemo::drawDungeonEntrances()
{
    for (int i = 0; i < dungeonEntrances.size(); ++i)
    {
        const DungeonEntrance& e = dungeonEntrances[i];
        glPushMatrix();
        glTranslatef(e.position.x, e.position.y, e.position.z);
        glScalef(1, 3, 1);
        updateMatrices();
        drawMesh(dungeonEntranceMesh);
        glPopMatrix();
        updateMatrices();
    }
}

void GameDemo::drawClouds()
{
    static const int cloudLayers = 2;
    static float cloudLayerHeights[cloudLayers] = {
        140.0f, 120.0f
    };
    // get the cloud shader
    Shader* s = shaders["clouds"];

    // enable alpha blending
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // draw the clouds texture scolling over the top
    for (int i = 0; i < cloudLayers; ++i)
    {
        // draw the water quad
        glPushMatrix();
        glTranslatef(playerPos.x, cloudLayerHeights[i], playerPos.z);
        updateMatrices();
        // bind shader
        setShader(s);

        // Draw cloud plane
        glBindBuffer(GL_ARRAY_BUFFER, cloudsVertexBuffer);
        GLint vertexLoc = glGetAttribLocation(s->GetProgram(), "vertexPosition");
        glEnableVertexAttribArray(vertexLoc);
        glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cloudsIndexBuffer);
        glDrawElements(GL_TRIANGLES, cloudsIndices.size(), GL_UNSIGNED_INT, 0);

        glDisableVertexAttribArray(vertexLoc);

        unsetShader(s);

        glPopMatrix();
        updateMatrices();
    }
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
}

void GameDemo::drawDungeon()
{
    skybox->Draw(playerPos);
    // draw the shapes in the shape database
    for (ShapeDatabase::iterator i = shapes.begin(); i != shapes.end(); ++i)
    {
        const Shape& shape = (*i);
        glPushMatrix();
        glTranslatef(shape.position.x, shape.position.y, shape.position.z);
        glRotatef(shape.yRotation, 0, 1, 0);
        updateMatrices();
        drawMesh(shapes.getSymbolMeshMap().find(shape.symbol)->second);
        glPopMatrix();
        updateMatrices();
    }
}


void GameDemo::setProjectionMatrix(int w, int h)
{
    glViewport(0, 0, (GLint)w, (GLint)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (h != 0)
    {
        gluPerspective(60.0f, w / (float)h, 0.01f, 10000.0f);
    }
    else
    {
        gluPerspective(60.0f, 0.0f, 0.01f, 1000.0f);
    }

    glMatrixMode(GL_MODELVIEW);
}

void GameDemo::draw()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    vec3 dst = playerPos + viewDirection;
    gluLookAt(playerPos.x, playerPos.y, playerPos.z, dst.x, dst.y, dst.z, 0, 1, 0);
    updateMatrices();

    if (isInDungeon)
    {
        drawDungeon();
    }
    else
    {
        drawOutside();
    }
}

void GameDemo::move(int direction, bool moving)
{
    switch (direction)
    {
    case 0: forward = moving; break;
    case 1: backward = moving; break;
    case 2: left = moving; break;
    case 3: right = moving; break;
    }
}

void GameDemo::rotate(int direction, bool rotating)
{
    switch (direction)
    {
    case 0: rforward = rotating; break;
    case 1: rbackward = rotating; break;
    case 2: rleft = rotating; break;
    case 3: rright = rotating; break;
    }
}

void GameDemo::loadShadersMap(const std::string& filename)
{
    std::ifstream ifs(filename.c_str());
    if (!ifs.is_open())
    {
        throw std::logic_error("could not open shaders file");
    }

    while (!ifs.eof())
    {
        string shaderName, fileName;
        ifs >> shaderName >> fileName;
        if (!shaderName.empty())
        {
            shaders[shaderName] = new Shader("shaders/std_vertex.glslv", fileName);
        }
    }
}
