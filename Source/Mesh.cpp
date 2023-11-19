
#include "Mesh.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include "ImageManager.h"
using namespace std;

void SubMesh::Load(std::ifstream& ifs)
{
    ifs >> materialName;
    cout << "mesh material name: " << materialName << endl;
    //this->texture = 0;//ImageManager::LoadTexture(texFile.c_str());

    int nVerts;
    ifs >> nVerts;

    std::vector<float> verts(nVerts * 3);
    std::vector<float> norms(nVerts * 3);
    std::vector<float> uvs(nVerts * 2);

    for (int i = 0; i < verts.size(); ++i)
    {
        ifs >> verts[i];
    }
    for (int i = 0; i < norms.size(); ++i)
    {
        ifs >> norms[i];
    }
    for (int i = 0; i < uvs.size(); ++i)
    {
        ifs >> uvs[i];
        if ((i % 2) == 0)
        {
            uvs[i] = 1.0f - uvs[i];
        }
    }

    // calculate object bounding box
    // assert(verts.size()>2)
    bounds.min = bounds.max = vec3(verts[0], verts[1], verts[2]);
    for (int i = 0; i < verts.size(); i += 3)
    {
        bounds.addPoint(vec3(verts[i + 0], verts[i + 1], verts[i + 2]));
    }

    int numTris;
    ifs >> numTris;
    indexCount = numTris * 3;
    std::vector<unsigned int> inds(indexCount);

    for (int i = 0; i < indexCount; ++i)
    {
        ifs >> inds[i];
    }
    cout << "creating buffers" << endl;
    // create the buffer objects
    glGenBuffers(1, &this->vertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vertices);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_STATIC_DRAW);

    glGenBuffers(1, &this->normals);
    glBindBuffer(GL_ARRAY_BUFFER, this->normals);
    glBufferData(GL_ARRAY_BUFFER, norms.size() * sizeof(float), &norms[0], GL_STATIC_DRAW);

    glGenBuffers(1, &this->texcoords);
    glBindBuffer(GL_ARRAY_BUFFER, this->texcoords);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), &uvs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &this->indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(int), &inds[0], GL_STATIC_DRAW);
    cout << "buffers created" << endl;
}

void SubMesh::Unload()
{
    glDeleteBuffers(1, &vertices);
    glDeleteBuffers(1, &indices);
    glDeleteBuffers(1, &normals);
    glDeleteBuffers(1, &texcoords);
}

void SubMesh::Draw(Shader& shader)
{
    glBindBuffer(GL_ARRAY_BUFFER, vertices);
    GLint vertexLoc = glGetAttribLocation(shader.GetProgram(), "vertexPosition");
    glEnableVertexAttribArray(vertexLoc);
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, normals);
    const GLint normalLoc = glGetAttribLocation(shader.GetProgram(), "vertexNormal");
    glEnableVertexAttribArray(normalLoc);
    glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, texcoords);
    const GLint texLoc = glGetAttribLocation(shader.GetProgram(), "vertexTexCoord");
    glEnableVertexAttribArray(texLoc);
    glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(vertexLoc);
    glDisableVertexAttribArray(normalLoc);
    glDisableVertexAttribArray(texLoc);
}

Mesh::Mesh(const string& filename)
{
    ifstream ifs(filename.c_str());
    if (!ifs.good())
    {
        std::stringstream ss;
        ss << "could not open mesh file: " << filename << endl;
        throw std::logic_error(ss.str());
    }
    int numSubMeshes;
    ifs >> numSubMeshes;
    meshes.resize(numSubMeshes);
    // load all submeshes
    for (int i = 0; i < numSubMeshes; ++i)
    {
        meshes[i].Load(ifs);
        if (i == 0)
        {
            bounds = meshes[0].getBounds();
        }
        else
        {
            bounds.addBounds(meshes[i].getBounds());
        }
    }
}

Mesh::~Mesh()
{
    for (int i = 0; i < meshes.size(); ++i)
    {
        meshes[i].Unload();
    }
}

void Mesh::Draw(Shader& shader)
{
    for (int i = 0; i < meshes.size(); ++i)
    {
        meshes[i].Draw(shader);
    }
}

