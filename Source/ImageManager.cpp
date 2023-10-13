
#include "ImageManager.h"
#include <QImage>

void ImageManager::SaveAsJpeg(const Array2D<float>& array, const char* filename, const ColorSpline& spline) {
    QImage img(array.GetWidth(), array.GetHeight(), QImage::Format_RGB32);
    int j = 0;
    for(int x = 0; x < img.width(); ++x) {
        for(int y = 0; y < img.height(); ++y) {
            Color c = spline.GetColor(array.GetValue(j++));

            img.setPixelColor(x, y, QColor(c.red * 255, c.green * 255, c.blue * 255));
        }
    }
    img.save(filename);
}


void ImageManager::SaveAsJpeg(const Array2D<float>& array, const char* filename) {
    QImage img(array.GetWidth(), array.GetHeight(), QImage::Format_RGB32);

    int j = 0;
    for(int x = 0; x < array.GetWidth(); ++x) {
        for (int y = 0; y < array.GetHeight(); ++y) {

            float r = array.GetValue(j++) * 255;
            float g = array.GetValue(j++) * 255;
            float b = array.GetValue(j++) * 255;

            img.setPixelColor(x, y, QColor(r, g, b));
        }
    }
    img.save(filename);
}


// converts to a interlaced RGB image
unsigned char* ConvertImage(const QImage& img) {
    unsigned char* pixels = new unsigned char[img.width() * img.height() * 3];
    int i = 0;
    for(int x = 0; x < img.width(); ++x) {
        for(int y = 0; y < img.height(); ++y) {
            QColor color = img.pixelColor(x, y);
            pixels[i++] = color.red();
            pixels[i++] = color.green();
            pixels[i++] = color.blue();
        }
    }
    return pixels;
}

GLuint ImageManager::LoadTexture(const char* filename) {
    QImage img;
    if (!img.load(filename))
        return -1;

    glEnable(GL_TEXTURE_2D);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    unsigned char* pixels = ConvertImage(img);

    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, img.width(), img.height(), GL_RGB, GL_UNSIGNED_BYTE, pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    delete[] pixels;

    return texture;
}

GLuint ImageManager::LoadAlphaTexture(const char* filename) {
    QImage img;
    if (!img.load(filename))
        return -1;
    //if(img.spectrum() != 4) {
    //    exit(0);
   // }

    glEnable(GL_TEXTURE_2D);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    unsigned char* pixels = new unsigned char[img.width() * img.height() * 4];
    int i = 0;
    for(int x = 0; x < img.width(); ++x) {
        for(int y = 0; y < img.height(); ++y) {
            QColor color = img.pixelColor(x, y);
            pixels[i++] = color.red();
            pixels[i++] = color.green();
            pixels[i++] = color.blue();
            pixels[i++] = color.alpha();
        }
    }

    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.width(), img.height(), GL_RGBA, GL_UNSIGNED_BYTE, pixels);

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
    QImage img(filename);
    if (!img.load(filename))
        return -1;

    QTransform transform;
    transform.rotate(90);
    img = img.transformed(transform);

    glEnable(GL_TEXTURE_2D);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    unsigned char* pixels = ConvertImage(img);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.width(), img.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

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

