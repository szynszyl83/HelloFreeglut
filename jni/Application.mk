# we have only ARM binary of freeglut, hence arm only
APP_ABI := armeabi
APP_MODULES := freeglut-gles2 hello-freeglut
APP_PLATFORM := android-9

# needed to use e.g. iostream
APP_STL := gnustl_static