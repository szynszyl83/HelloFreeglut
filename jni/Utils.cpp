/*
 * Utils.cpp
 *
 *  Created on: Mar 3, 2014
 *      Author: michal
 */

#include "Utils.h"

#include <GL/freeglut.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <assert.h>
#include <android/log.h>

namespace gles {

typedef void (PFNGLGETSHADERIVPROC) (GLuint shader, GLenum pname, GLint *params);
typedef void (PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);

// Load the shader from the source text
static void loadShaderSrc(const char *szShaderSrc, GLuint shader) {
	GLchar *fsStringPtr[1];

	fsStringPtr[0] = (GLchar *) szShaderSrc;
	glShaderSource(shader, 1, (const GLchar **) fsStringPtr, NULL);
}

static bool checkStatus(
		GLuint objectID,
		PFNGLGETSHADERIVPROC objectPropertyGetter,
		PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
		GLenum statusType) {
	GLint status;
	objectPropertyGetter(objectID, statusType, &status);
	if (status != GL_TRUE) {
		GLint infoLength;
		objectPropertyGetter(objectID, GL_INFO_LOG_LENGTH, &infoLength);
		GLchar* buffer = new GLchar[infoLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLength, &bufferSize, buffer);
		__android_log_print(ANDROID_LOG_ERROR, "nazgee", "error status: %s", buffer);
		return false;
	}

	return true;
}

GLuint loadShaderPair(const char *szVertexProg, const char *szFragmentProg) {
	// Temporary Shader objects
	GLuint hVertexShader;
	GLuint hFragmentShader;
	GLuint programID = 0;
	GLint testVal;

	// Create shader objects
	hVertexShader = glCreateShader(GL_VERTEX_SHADER);
	hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	loadShaderSrc(szVertexProg, hVertexShader);
	loadShaderSrc(szFragmentProg, hFragmentShader);

	// Compile them
	glCompileShader(hVertexShader);
	glCompileShader(hFragmentShader);

	// Check for errors
	if (!checkStatus(hVertexShader, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS)) {
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		return (GLuint) NULL;
	}

	if (!checkStatus(hFragmentShader, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS)) {
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		return (GLuint) NULL;
	}

	// Link them - assuming it works...
	programID = glCreateProgram();
	glAttachShader(programID, hVertexShader);
	glAttachShader(programID, hFragmentShader);

	glLinkProgram(programID);

	// These are no longer needed
	glDeleteShader(hVertexShader);
	glDeleteShader(hFragmentShader);

	// Make sure link worked too
	if (!checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS)) {
		glDeleteProgram(programID);
		return (GLuint) NULL;
	}

	return programID;
}

void assertNoError() {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		__android_log_print(ANDROID_LOG_ERROR, "gles", "glGetError=%d", error);
	}

	assert(error == GL_NO_ERROR);
}

} /* namespace gles */
