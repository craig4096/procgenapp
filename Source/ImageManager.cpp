
#include "ImageManager.h"
#include "CImg.h"
using namespace cimg_library;

void ImageManager::SaveAsJpeg(const Array2D<float>& array, const char* filename, const ColorSpline& spline) {
	CImg<unsigned char> img(array.GetWidth(), array.GetHeight(), 1, 3);
	int j = 0;
	for(int x = 0; x < img.width(); ++x) {
		for(int y = 0; y < img.height(); ++y) {
			Color c = spline.GetColor(array.GetValue(j++));
			img(x, y, 0, 0) = c.red * 255;
			img(x, y, 0, 1) = c.green * 255;
			img(x, y, 0, 2) = c.blue * 255;
		}
	}
	img.save(filename);
}


void ImageManager::SaveAsJpeg(const Array2D<float>& array, const char* filename) {
	CImg<unsigned char> img(array.GetWidth(), array.GetHeight(), 1, 1);
	int j = 0;
	for(CImg<unsigned char>::iterator it = img.begin(); it != img.end(); ++it) {
		(*it) = array.GetValue(j++)* 255;
	}
	img.save(filename);
}


// converts to a interlaced RGB image
unsigned char* ConvertImage(const CImg<unsigned char>& img) {
	unsigned char* pixels = new unsigned char[img.width() * img.height() * 3];
	int i = 0;
	for(int x = 0; x < img.width(); ++x) {
		for(int y = 0; y < img.height(); ++y) {
			pixels[i++] = img(x, y, 0, 0);
			pixels[i++] = img(x, y, 0, 1);
			pixels[i++] = img(x, y, 0, 2);
		}
	}
	return pixels;
}

GLuint ImageManager::LoadTexture(const char* filename) {
	CImg<unsigned char> img(filename);

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
    CImg<unsigned char> img(filename);
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
            pixels[i++] = img(x, y, 0, 0);
            pixels[i++] = img(x, y, 0, 1);
            pixels[i++] = img(x, y, 0, 2);
            pixels[i++] = img(x, y, 0, 3);
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
    glEnable(GL_TEXTURE_2D);
	CImg<unsigned char> img(filename);
	img.transpose();
	img.mirror('x');
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

