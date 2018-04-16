#include "Shader.h"
#include <stdexcept>
#include <stdio.h>
#include <iostream>
using namespace std;

Shader::Shader(const string& filename) {
    load(filename+".glslv", filename+".glslf");
}

Shader::Shader(const string& vertexfile, const string& fragfile) {
    load(vertexfile, fragfile);
}

void Shader::load(const string& vertexfile, const string& fragfile) {
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	FILE* file;
	int size;

	// load shader from file
    file = fopen(vertexfile.c_str(), "rb");
	if(!file) {
		throw logic_error("could not open vertex shader file");
	}

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* buffer = new char[size];

	fread(buffer, size, 1, file);
	glShaderSource(vertexShader, 1, (const char**)&buffer, &size);
	glCompileShader(vertexShader);
	delete[] buffer;
	fclose(file);

	// for catching comilation errors
	char errormsg[2048];
	GLsizei length;
	GLint compileError;

	// make sure it compiled
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileError);
	if(compileError == GL_FALSE) {
		// get the error
		glGetShaderInfoLog(vertexShader, 2048, &length, errormsg);

		throw logic_error(errormsg);
	}


	// create the fragment shader
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    file = fopen(fragfile.c_str(), "rb");
	if(!file) {
		throw logic_error("could not open fragment shader file");
	}

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	buffer = new char[size];
	fread(buffer, size, 1, file);
	glShaderSource(fragmentShader, 1, (const char**)&buffer, &size);
	glCompileShader(fragmentShader);
	delete[] buffer;
	fclose(file);


	// make sure it compiled
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compileError);
	if(compileError == GL_FALSE) {
		// get the error
		glGetShaderInfoLog(fragmentShader, 2048, &length, errormsg);

		throw logic_error(errormsg);
	}

	// create and link the program
	program = glCreateProgram();
	glAttachShader(program, fragmentShader);
	glAttachShader(program, vertexShader);
	glLinkProgram(program);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if(status == GL_FALSE) {
		throw logic_error("could not link shaders");
	}
}


Shader::~Shader() {
	// first detach the shaders
	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);

	// now delete the shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// finally delete the program
	glDeleteProgram(program);
}


void Shader::Set() {
	glUseProgram(this->program);
}

void Shader::Unset() {
	glUseProgram(0);
}
