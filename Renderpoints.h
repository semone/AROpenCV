#include <windows.h>
#include <GL\GLU.h>
#include <GL\gl.h>
#include <iostream>
#include <opencv2\core\core.hpp>
#include </Program/GLFW/include/GLFW/glfw3.h>

using namespace std;
using namespace cv;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void error_callback(int error, const char* description);
void init(Mat worldPoints3D);
void shutDown(int return_code);
void glfwMainLoop(void);
void setUpCamera(void);
vector<Point3f> modifyPoints(Mat worldPoints3D);
