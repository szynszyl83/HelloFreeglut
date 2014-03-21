#include <string.h>
#include <jni.h>

#include <GL/freeglut.h>
#include <GLES2/gl2.h>
#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "hello-freeglut", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "hello-freeglut", __VA_ARGS__))
#include <android/native_activity.h>
#include <android/sensor.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "Utils.h"

static ANativeActivity* gActivity = 0;
static float gAspectRatio;
GLuint gProgramID = 0;
GLuint gBufferID = 0;
GLuint gModelMatrixUniform = 0;
GLuint gCameraViewMatrixUniform = 0;
glm::mat4 gModelMatrix;
glm::mat4 gCameraViewMatrix;

struct Vertex
{
	glm::vec3 pos;
	glm::vec4 col;
};

const char* vertShaderText =
			"#version 100\n"  // OpenGL ES 2.0
			"precision mediump float;\n"
			"attribute vec3 attr_pos;\n"
			"attribute vec4 attr_color;\n"
			"uniform mat4 u_mModel;\n"
			"uniform mat4 u_mCameraView;\n"
			"varying vec4 v_color;\n"
			"void main()"
			"{\n"
			"   gl_Position = u_mCameraView * u_mModel * vec4(attr_pos, 1);\n"
			"   v_color = attr_color;\n"
			"}\n";

const char* fragShaderText =
			"#version 100\n"  // OpenGL ES 2.0
			"precision mediump float;\n"
			"varying vec4 v_color;\n"
			"void main()"
			"{\n"
			"   gl_FragColor = v_color;\n"
			"}\n";

void initializeCamera() {
	gCameraViewMatrix = glm::ortho(-gAspectRatio, gAspectRatio, 1.0f, -1.0f);
}

void initializeModel() {
	gProgramID = gles::loadShaderPair(vertShaderText, fragShaderText);
	glUseProgram(gProgramID);

	gModelMatrixUniform = glGetUniformLocation(gProgramID, "u_mModel");
	gCameraViewMatrixUniform = glGetUniformLocation(gProgramID, "u_mCameraView");

	gles::assertNoError();
	glGenBuffers(1, &gBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, gBufferID);
	gles::assertNoError();

	Vertex data[3];
	data[0].col = glm::vec4(1, 0, 0, 1);
	data[0].pos = glm::vec3(-0.25f, -0.25f, 0);
	data[1].col = glm::vec4(0, 1, 0, 1);
	data[1].pos = glm::vec3(0, 0.25f, 0);
	data[2].col = glm::vec4(0, 0, 1, 1);
	data[2].pos = glm::vec3(0.25f, -0.25f, 0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), &data, GL_STATIC_DRAW);
	gles::assertNoError();

	GLuint attr_col = glGetAttribLocation(gProgramID, "attr_color");
	gles::assertNoError();

	GLuint attr_pos = glGetAttribLocation(gProgramID, "attr_pos");
	gles::assertNoError();

	glEnableVertexAttribArray(attr_pos);
	glVertexAttribPointer(attr_pos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET_OF_STRUCT(Vertex, pos));
	gles::assertNoError();

	glEnableVertexAttribArray(attr_col);
	glVertexAttribPointer(attr_col, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET_OF_STRUCT(Vertex, col));
	gles::assertNoError();
}

void drawModel() {
	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, &gModelMatrix[0][0]);
	glUniformMatrix4fv(gCameraViewMatrixUniform, 1, GL_FALSE, &gCameraViewMatrix[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	gles::assertNoError();
}

static void callbackInitContext() {
	glutInitContextFlags(GLUT_CORE_PROFILE);
	gles::assertNoError();

	float r = (float)rand() / (float)RAND_MAX;
	float g = (float)rand() / (float)RAND_MAX;
	float b = (float)rand() / (float)RAND_MAX;
	glClearColor(r, g, b, 1.0f);
	gles::assertNoError();

	initializeModel();
	initializeCamera();

	gles::assertNoError();
}

static void callbackDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);

	drawModel();
	glutSwapBuffers();
}

static void callbackResize(int width, int height) {
	gAspectRatio = (float) width / (float) height;
	glViewport(0, 0, width, height);

	initializeCamera();
}

static void callbackIdle(void) {
	gModelMatrix = glm::rotate(3.14f/60, glm::vec3(0,0,1)) * gModelMatrix;
	glutPostRedisplay();
}

static void callbackAppStatus(int status) {
	LOGI("callbackAppStatus; status=%d", status);
	if (status & GLUT_APPSTATUS_PAUSE) {

	} else if (status & GLUT_APPSTATUS_RESUME) {

	}
}

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Hook to mess around with freeglut's callbacks (if needed). By adding
 * android.app.func_name metadata entry to AndroidManifest we've forced
 * NativeActivity to use this function as callback setting entry-point, instead
 * of a default one (which is provided by freeglut)
 *
 * @param activity
 * @param savedState
 * @param savedStateSize
 */
void ANativeActivity_onCreate_Mine(ANativeActivity* activity, void* savedState,
		size_t savedStateSize) {
	/* Call freeglut's implementation of onCreate */
	::ANativeActivity_onCreate(activity, savedState, savedStateSize);

	/* Mess around with callbacks installed by freeglut HERE */

	/* Store ANativeActivity - to be used when needed */
	gActivity = activity;
}
#ifdef __cplusplus
};
#endif

/**
 * Forces screen to stay ON for the whole time we're visible
 *
 * @param activity
 */
static void forceScreenOn(ANativeActivity* activity) {
	JNIEnv* env = 0;
	JavaVM* jvm = activity->vm;

	jvm->AttachCurrentThread(&env, NULL);
	jclass clazz_activity = env->GetObjectClass(activity->clazz);
	jmethodID method_getWin = env->GetMethodID(clazz_activity, "getWindow", "()Landroid/view/Window;");
	jobject winObj = env->CallObjectMethod(activity->clazz, method_getWin);

	jclass clazz_win = env->GetObjectClass(winObj);
	jmethodID method_addFlags = env->GetMethodID(clazz_win, "addFlags", "(I)V");
	env->CallVoidMethod(winObj, method_addFlags, 0x80); // FLAG_KEEP_SCREEN_ON

	if (env->ExceptionOccurred()) {
		env->ExceptionDescribe();
		env->ExceptionClear();
	}

	jvm->DetachCurrentThread();
}

/**
 * This method get's called by freeglut from android_main()
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[]) {
	forceScreenOn(gActivity);

	glutInitWindowSize(640,480);
	gAspectRatio = 640.0f/480.0f;
	glutInitWindowPosition(40,40);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA );

	glutCreateWindow("my window title");

	glutSetOption ( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	glutIdleFunc( callbackIdle );
	glutDisplayFunc( callbackDisplay );
	glutReshapeFunc( callbackResize );
	glutInitContextFunc( callbackInitContext );
	glutAppStatusFunc( callbackAppStatus );

	glutMainLoop();

	return EXIT_SUCCESS;
}

