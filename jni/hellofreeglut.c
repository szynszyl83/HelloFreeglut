#include <string.h>
#include <jni.h>

#include <GL/freeglut.h>
#include <GLES2/gl2.h>

static float gAspectRatio;

static void callbackInitContext() {
	float r = (float)rand() / (float)RAND_MAX;
	float g = (float)rand() / (float)RAND_MAX;
	float b = (float)rand() / (float)RAND_MAX;
	glClearColor(r, g, b, 1.0f);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glutInitContextFlags(GLUT_CORE_PROFILE);
}

static void callbackDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);

	/* XXX: Add your GLES2 rendering here */

	glutSwapBuffers();
}

static void callbackResize(int width, int height) {
	gAspectRatio = (float) width / (float) height;
	glViewport(0, 0, width, height);
}

static void callbackIdle(void)
{
	glutPostRedisplay();
}


int main(int argc, char *argv[])
{
	glutInitWindowSize(640,480);
	glutInitWindowPosition(40,40);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA );

	glutCreateWindow("my window title");

	glutSetOption ( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	glutIdleFunc( callbackIdle );
	glutDisplayFunc( callbackDisplay );
	glutReshapeFunc( callbackResize );
	glutInitContextFunc( callbackInitContext );

	glutMainLoop();

	return EXIT_SUCCESS;
}

/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/hellojni/HelloJni.java
 */
jstring
Java_com_example_hellojni_HelloJni_stringFromJNI( JNIEnv* env,
                                                  jobject thiz )
{
#if defined(__arm__)
  #if defined(__ARM_ARCH_7A__)
    #if defined(__ARM_NEON__)
      #define ABI "armeabi-v7a/NEON"
    #else
      #define ABI "armeabi-v7a"
    #endif
  #else
   #define ABI "armeabi"
  #endif
#elif defined(__i386__)
   #define ABI "x86"
#elif defined(__mips__)
   #define ABI "mips"
#else
   #define ABI "unknown"
#endif

    return (*env)->NewStringUTF(env, "Hello from JNI !  Compiled with ABI " ABI ".");
}
