

#include "Skybox.h"
#include "ImageManager.h"
#include <iostream>
using namespace std;


Skybox::Skybox(const std::string textures[6])
{
    // create the vertex buffer
    float verts[] = {
        1.0, -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, -1.0,		// posx
        -1.0, -1.0, 1.0, -1.0, -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0,	// negx
        1.0, 1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0,		// posy
        1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0,	// negy
        1.0, -1.0, 1.0, -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, 1.0,		// posz
        -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, -1.0	// negz
    };

    glGenBuffers(1, &vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);


    float texcoords[] = {
        1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0,
        1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0,
        1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0,
        1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0,
        1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0,
        1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0
    };

    glGenBuffers(1, &this->texcoords);
    glBindBuffer(GL_ARRAY_BUFFER, this->texcoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

    for(int i = 0; i < 6; ++i) {
        cout << "LOADING TEXTURE: " << textures[i] << endl;
        this->textures[i] = ImageManager::LoadSkyboxTexture(textures[i].c_str());
    }
}

Skybox::~Skybox()
{
    glDeleteBuffers(1, &vertices);
    glDeleteBuffers(1, &texcoords);
    for(int i = 0; i < 6; ++i) {
        glDeleteTextures(1, &textures[i]);
    }
}


void Skybox::Draw(const vec3& campos, float scale)
{
    glPushMatrix();
    glTranslatef(campos.x, campos.y, campos.z);
    glScalef(scale,scale,scale);
    glEnable(GL_TEXTURE_2D);

    glEnableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, vertices);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, texcoords);
    glTexCoordPointer(2, GL_FLOAT, 0, 0);

    for(int i = 0; i < 6; ++i) {
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glDrawArrays(GL_QUADS, i*4, 4);
    }

    //glDisableClientState(GL_VERTEX_ARRAY);
    //glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

