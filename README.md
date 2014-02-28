HelloFreeglut
=============
Minimal stub for freeglut-based android app (apk will work only on ARM devices, since I've not provided precomipled x86/MIPS binary of freeglut)



Building
=============

1. Create a new project in Eclipse from the existing sample source OR use the android tool to update the project so it generates a build.xml file that you can use to build the sample.
   * In Eclipse:
      * Click File > New Android Project...
      * Select the Create project from existing source radio button.
      * Select any API level above Android 1.5.
      * In the Location field, click Browse... and select the <project_root> directory.
      * Click Finish.
   * On the command line:

                cd <project_root>
                android update project -p . -s

2. Compile the native code using the ndk-build command.

                cd <project_root>
                ndk-build NDK_MODULE_PATH=./jni

3. Build and install the application as you would a normal Android application. 
   * If you are using Eclipse:
       * Run the application to build and install it on a device.
   * If you are using Ant:
       * Run the following commands from the project directory:

                ant debug
                adb install bin/HelloFreeglut-debug.apk
