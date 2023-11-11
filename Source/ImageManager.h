
#pragma once
#include "GL/glew.h"
#include "Array2D.h"
#include "ColorSpline.h"

class ImageManager
{
public:
    static void SaveAsJpeg(const Array2D<float>& array, const char* filename, const ColorSpline& spline);
    static void SaveAsJpeg(const Array2D<float>& array, const char* filename);

    static GLuint LoadAlphaTexture(const char* filename);
    static GLuint LoadTexture(const char* filename);
    static GLuint LoadSkyboxTexture(const char* filename);
};
