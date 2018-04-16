#include "RenderableHeightmap.h"
#include <iostream>
using namespace std;

RenderableHeightmap::RenderableHeightmap(Heightmap* h)
    : Heightmap(*h)
    , renderDataCreated(false)
{
    this->vertexCount = GetWidth()*GetHeight();
    //this->texcoords = new vec2[vertexCount];
    this->vertices = new vec3[vertexCount];
    this->normals = new vec3[vertexCount];
    this->indexCount = (GetWidth()-1)*(GetHeight()-1)*6;
    this->indices = new int[indexCount];

    CalculateVertices();
    CalculateNormals();
    CalculateIndices();
}

RenderableHeightmap::~RenderableHeightmap() {
    delete[] vertices;
    //delete[] texcoords;
    delete[] normals;
    delete[] indices;

    glDeleteBuffers(1, &vertexBuffer);
    //glDeleteBuffers(1, &texcoordsBuffer);
    glDeleteBuffers(1, &normalBuffer);
    glDeleteBuffers(1, &indexBuffer);
}

void RenderableHeightmap::CalculateVertices() {
    float startX = (GetWidth()/2.0f) * -scaleX;
    float startZ = (GetHeight()/2.0f) * -scaleZ;
    int index = 0;

    // generate an array of vertices
    float zr = startZ;
    for(int z = 0; z < GetHeight(); ++z) {
        float xr = startX;
        for(int x = 0; x < GetWidth(); ++x) {
            vertices[index].x = xr;
            vertices[index].y = GetValue(x, z) * scaleY;
            vertices[index].z = zr;
            index++;
            xr += scaleX;
        }
        zr += scaleZ;
    }
}

void RenderableHeightmap::CalculateNormals() {
    // calculate normals...
    // loop all triangles and add tri normals to normals array
    for(int z = 0; z < GetHeight(); ++z) {
        for(int x = 0; x < GetWidth(); ++x) {
            /*
            int a = GetIndex(x, z);
            int b = GetIndex(x, z+1);
            int c = GetIndex(x+1, z+1);
            int d = GetIndex(x+1, z);

            vec3 triNorm1 = CalculateNormal(vertices[a], vertices[b], vertices[c]);
            vec3 triNorm2 = CalculateNormal(vertices[a], vertices[c], vertices[d]);

            normals[a] += triNorm1;
            normals[b] += triNorm1;
            normals[c] += triNorm1;

            normals[a] += triNorm2;
            normals[c] += triNorm2;
            normals[d] += triNorm2;
            */
            normals[GetIndex(x, z)] = GetNormal(x, z);
        }
    }
    // normalize normals and upload to gpu
    for(int i = 0; i < GetSize(); ++i) {
        //normals[i].normalize();
    }
}

void RenderableHeightmap::CalculateIndices()
{
    // now generate indices
    int index = 0;
    for(int z = 0; z < GetHeight()-1; ++z) {
        for(int x = 0; x < GetWidth()-1; ++x) {
            indices[index++] = GetIndex(x, z);
            indices[index++] = GetIndex(x, z+1);
            indices[index++] = GetIndex(x+1, z+1);

            indices[index++] = GetIndex(x, z);
            indices[index++] = GetIndex(x+1, z+1);
            indices[index++] = GetIndex(x+1, z);
        }
    }
}

/*
void RenderableHeightmap::CalculateTextureCoords() {
    const float uinc = (float)texscale / (float)(GetWidth()-1);
    const float vinc = (float)texscale / (float)(GetHeight()-1);

    float v = 0.0f;
    for(int y = 0; y < GetHeight(); ++y) {
        float u = 0.0f;
        for(int x = 0; x < GetWidth(); ++x) {
            texcoords[y * GetWidth() + x] = vec2(u, v);
            u += uinc;
        }
        v += vinc;
    };
}
*/

void RenderableHeightmap::CreateRenderData() {
    if(renderDataCreated) {
        return;
    }

    glGenBuffers(1, &vertexBuffer);
    //cout << "Vertex Buffer: " << vertexBuffer << endl;
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertexCount*sizeof(vec3), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &normalBuffer);
    //cout << "Normal Buffer: " << normalBuffer << endl;
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertexCount*sizeof(vec3), normals, GL_STATIC_DRAW);

    //glGenBuffersARB(1, &texcoordsBuffer);
   // glBindBufferARB(GL_ARRAY_BUFFER, texcoordsBuffer);
   // glBufferDataARB(GL_ARRAY_BUFFER, vertexCount*sizeof(vec2), texcoords, GL_STATIC_DRAW);

    glGenBuffers(1, &indexBuffer);
    //cout << "Index Buffer: " << indexBuffer << endl;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount*sizeof(int), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    renderDataCreated = true;
}

void RenderableHeightmap::Draw() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);

    GLfloat ambient[4] = { 0,0,0,0 };
    GLfloat diffuse[4] = { 1,1,1,1 };
    GLfloat specular[4] = { 0,0,0,0 };
    GLfloat emission[4] = { 0,0,0,0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_EMISSION, emission);

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);

    GLfloat pos[4] = { 0,1,1,0 };
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    //cout << "enabling client states" << endl;
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    //glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    //cout << "Binding vertex buffer: " << vertexBuffer << endl;
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    //cout << "Binding normalsBuffer: " << normalBuffer << endl;
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glNormalPointer(GL_FLOAT, 0, 0);

   // glBindBufferARB(GL_ARRAY_BUFFER, texcoordsBuffer);
   // glTexCoordPointer(2, GL_FLOAT, 0, 0);

   // cout << "Binding indices buffer" << endl;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    //cout << "drawing elements: indexBuffer: " << indexBuffer << " indexCount: " << indexCount << endl;
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

    //cout << "disabling client state" << endl;
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    //glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glColor3f(1,1,1);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
}
