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

#include "../Terrain/TerrainGenerator.h"
#include "../Terrain/PerlinNoise.h"
#include "../Terrain/Normalization.h"
#include "../Terrain/InverseThermalErosion.h"
#include "../Terrain/ThermalErosion.h"

using namespace std;

GameDemo::GameDemo()
    : heightmap(nullptr)
    , levelGenerated(false)
    , waterLevel(50.0f)
    , time(0.0f)
    , resetX(50)
    , resetY(50)
    , rotationX(0.0f)
    , rotationY(0.0f)
    , mousePressing(false)
    , forward(false)
    , backward(false)
    , left(false)
    , right(false)
    , flyMode(true)
{
}

GameDemo::~GameDemo()
{
    delete skybox;
    delete dungeonEntranceMesh;
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
}


void GameDemo::generateLevel(int seed)
{
    levelSeed = seed;
    delete heightmap;

    // call upon the terrain generator
    TerrainGenerator tergen(512, 512);
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


void GameDemo::setShader(Shader* shader)
{
    shader->Set();
    glUniform1f(glGetUniformLocation(shader->GetProgram(), "time"), time);
    // set the view matrix unform
    glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "viewMat"), 1, GL_FALSE, (GLfloat*)&viewMat);
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
    //rleft=rright=rforward=rbackward=false;
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
        newPos -= vec3(rightVec.x, ytrans, rightVec.z) * speed * timeStep;
    }
    if (right)
    {
        newPos += vec3(rightVec.x, ytrans, rightVec.z) * speed * timeStep;
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
        cout << ">>>>>>>>>>>>>>loading rule file: " << ss.str() << endl;
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
            setShader(it->second);
            m->Draw();
            unsetShader(nullptr);
        }
        else
        {
            m->Draw();
        }
    }
}

void GameDemo::drawDungeonEntrances()
{
    for (int i = 0; i < dungeonEntrances.size(); ++i)
    {
        const DungeonEntrance& e = dungeonEntrances[i];
        glPushMatrix();
        glTranslatef(e.position.x, e.position.y, e.position.z);
        glScalef(1, 3, 1);
        drawMesh(dungeonEntranceMesh);
        glPopMatrix();
    }
}

void drawWaterGrid(int x, int y, float time)
{
    float startX = -(x / 2.0f);
    float startY = -(y / 2.0f);

    glBegin(GL_QUADS);
    for (int i = 0; i < x; ++i)
    {
        for (int j = 0; j < y; ++j)
        {
            float xx = startX + i;
            float yy = startY + j;

            float h1 = sin((i / (float)x) * 2 * M_PI * 10 + (time * 0.5));
            float h2 = sin(((i + 1) / (float)x) * 2 * M_PI * 10 + (time * 0.5));

            float hh1 = sin((j / (float)y) * 2 * M_PI * 5 + (time * 1.0));
            float hh2 = sin(((j + 1) / (float)y) * 2 * M_PI * 5 + (time * 1.0));


            glVertex3f(xx, h1 * hh2 * 4, yy + 1);
            glVertex3f(xx + 1, h2 * hh2 * 4, yy + 1);
            glVertex3f(xx + 1, h2 * hh1 * 4, yy);
            glVertex3f(xx, h1 * hh1 * 4, yy);
        }
    }
    glEnd();
}


void drawWaterBounds(float waterGridWorldSize)
{
    // draw 8 quads surrounding the water grid
    glBegin(GL_QUADS);
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (i == 1 && j == 1) continue;
            float x1 = (-1.5 + i) * waterGridWorldSize;
            float y1 = (-1.5 + j) * waterGridWorldSize;
            float x2 = x1 + waterGridWorldSize;
            float y2 = y1 + waterGridWorldSize;

            if (x1 < -waterGridWorldSize)
            {
                x1 = -10000.0f;
            }
            if (x2 > waterGridWorldSize)
            {
                x2 = 10000.0f;
            }
            if (y1 < -waterGridWorldSize)
            {
                y1 = -10000.0f;
            }
            if (y2 > waterGridWorldSize)
            {
                y2 = 10000.0f;
            }

            glVertex3f(x1, -4, y2);
            glVertex3f(x2, -4, y2);
            glVertex3f(x2, -4, y1);
            glVertex3f(x1, -4, y1);
        }
    }
    glEnd();
}

void GameDemo::drawOutside()
{
    if (!levelGenerated)
    {
        heightmap->CreateRenderData();
        levelGenerated = true;
    }

    skybox->Draw(playerPos);

    //static const float waterSize = 1000.0f;

    // first draw the scene (not inlcuding alpha-blended stuff (clouds)) to the depth buffer
    glColorMask(0, 0, 0, 0);
    if (heightmap)
    {
        heightmap->Draw();
    }

    static const int gridSize = 50;
    static const float gridScale = 12.0f;

    //static const float waterGridWorldSize = gridSize * gridScale;

    glPushMatrix();
    glTranslatef(0, waterLevel, 0);
    glScalef(gridScale, 1.0f, gridScale);
    drawWaterBounds(gridSize);
    drawWaterGrid(gridSize, gridSize, time);
    glPopMatrix();
    drawDungeonEntrances();

    glColorMask(1, 1, 1, 1);


    // now draw scene without depth test
    glDepthMask(GL_FALSE);
    if (heightmap != nullptr)
    {
        setShader(shaders["terrain"]);
        heightmap->Draw();
        unsetShader(nullptr);
    }

    // get the water shader
    Shader* s = shaders["water"];
    // bind shader
    setShader(s);

    // draw the water quad
    glPushMatrix();
    glTranslatef(0, waterLevel, 0);
    glScalef(gridScale, 1.0f, gridScale);
    drawWaterBounds(gridSize);
    drawWaterGrid(gridSize, gridSize, time);
    glPopMatrix();

    unsetShader(s);

    drawDungeonEntrances();

    glDepthMask(GL_TRUE);

    static const int cloudLayers = 2;
    static float cloudLayerHeights[cloudLayers] = {
        140.0f, 120.0f
    };
    // get the cloud shader
    s = shaders["clouds"];
    // bind shader
    setShader(s);
    // enable alpha blending
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
    glTranslatef(playerPos.x, 0.0f, playerPos.z);
    // draw the clounds texture scolling over the top
    for (int i = 0; i < cloudLayers; ++i)
    {
        // draw the water quad
        glBegin(GL_QUADS);
        static const float size = 300.0f;
        glVertex3f(-size, cloudLayerHeights[i], -size);
        glVertex3f(size, cloudLayerHeights[i], -size);
        glVertex3f(size, cloudLayerHeights[i], size);
        glVertex3f(-size, cloudLayerHeights[i], size);
        glEnd();
    }
    glPopMatrix();
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    unsetShader(s);
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
        drawMesh(shapes.getSymbolMeshMap().find(shape.symbol)->second);
        glPopMatrix();
    }
}

void GameDemo::draw()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    vec3 dst = playerPos + viewDirection;
    gluLookAt(playerPos.x, playerPos.y, playerPos.z, dst.x, dst.y, dst.z, 0, 1, 0);

    // save the view matrix
    glGetFloatv(GL_MODELVIEW_MATRIX, (float*)&viewMat);

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
