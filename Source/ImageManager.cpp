
#include "ImageManager.h"
#include <wx/image.h>

void ImageManager::SaveAsJpeg(const Array2D<float>& array, const char* filename, const ColorSpline& spline)
{
    wxImage img(array.GetWidth(), array.GetHeight());
    int j = 0;
    for (int x = 0; x < img.GetWidth(); ++x)
    {
        for (int y = 0; y < img.GetHeight(); ++y)
        {
            Color c = spline.GetColor(array.GetValue(j++));

            img.SetRGB(x, y, c.red * 255, c.green * 255, c.blue * 255);
        }
    }
    img.SaveFile(filename);
}


void ImageManager::SaveAsJpeg(const Array2D<float>& array, const char* filename)
{
    wxImage img(array.GetWidth(), array.GetHeight());

    int j = 0;
    for (int x = 0; x < array.GetWidth(); ++x)
    {
        for (int y = 0; y < array.GetHeight(); ++y)
        {

            unsigned char r = array.GetValue(j++) * 255;
            unsigned char g = array.GetValue(j++) * 255;
            unsigned char b = array.GetValue(j++) * 255;

            img.SetRGB(x, y, r, g, b);
        }
    }
    img.SaveFile(filename);
}


// converts to a interlaced RGB image
unsigned char* ConvertImage(const wxImage& img)
{
    unsigned char* pixels = new unsigned char[img.GetWidth() * img.GetHeight() * 3];
    int i = 0;
    for (int x = 0; x < img.GetWidth(); ++x)
    {
        for (int y = 0; y < img.GetHeight(); ++y)
        {
            pixels[i++] = img.GetRed(x, y);
            pixels[i++] = img.GetGreen(x, y);
            pixels[i++] = img.GetBlue(x, y);
        }
    }
    return pixels;
}

GLuint ImageManager::LoadTexture(const char* filename)
{
    wxImage img;
    if (!img.LoadFile(filename))
        return -1;

    glEnable(GL_TEXTURE_2D);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    unsigned char* pixels = ConvertImage(img);

    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, img.GetWidth(), img.GetHeight(), GL_RGB, GL_UNSIGNED_BYTE, pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    delete[] pixels;

    return texture;
}

GLuint ImageManager::LoadAlphaTexture(const char* filename)
{
    wxImage img;
    if (!img.LoadFile(filename))
        return -1;

    glEnable(GL_TEXTURE_2D);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    unsigned char* pixels = new unsigned char[img.GetWidth() * img.GetHeight() * 4];
    int i = 0;
    for (int x = 0; x < img.GetWidth(); ++x)
    {
        for (int y = 0; y < img.GetHeight(); ++y)
        {
            pixels[i++] = img.GetRed(x, y);
            pixels[i++] = img.GetGreen(x, y);
            pixels[i++] = img.GetBlue(x, y);
            pixels[i++] = img.GetAlpha(x, y);
        }
    }

    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.GetWidth(), img.GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    delete[] pixels;

    return texture;
}

GLuint ImageManager::LoadSkyboxTexture(const char* filename)
{
    wxImage img;
    if (!img.LoadFile(filename))
        return -1;

    img = img.Rotate90();

    glEnable(GL_TEXTURE_2D);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    unsigned char* pixels = ConvertImage(img);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.GetWidth(), img.GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    // set the texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    delete[] pixels;

    glDisable(GL_TEXTURE_2D);
    return texture;
}

