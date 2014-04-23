#include "Renderpoints.h"

GLFWwindow* window;
double width = 1280, height = 720;
float rotate_y = 0.f, rotate_x = 0.f, delta_rotate;
const float rotations_per_tick = .2f;
vector<Point3f> v;

vector<Point3f> modifyPoints(vector<CloudPoint> worldPoints3D){
	int i = 0;
	vector<Point3f> points;
	Point3f p;

	for (int r = 0; r < worldPoints3D.size(); r++)
	{
		p.x = worldPoints3D[r].pt.x;
		p.y = -worldPoints3D[r].pt.y;
		p.z = -worldPoints3D[r].pt.z;
		points.push_back(p);
	}
	//cout << "points2" << points[0] << endl;
	return points;
}

void init(vector<CloudPoint> worldPoints3D)
{
	v = modifyPoints(worldPoints3D);
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	window = glfwCreateWindow((int)width, (int)height, "OpenGL / OpenCV / GLFW", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glEnable(GL_CULL_FACE); // Cull away all back facing polygons
	glEnable(GL_DEPTH_TEST); // Use the Z buffer
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void glfwMainLoop(void){
	double old_time = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		//Mat tempImage;
		double current_time = glfwGetTime();

		delta_rotate = (float)(current_time - old_time) * rotations_per_tick * 360;
		old_time = current_time;
		float ratio;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		//glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glDisable(GL_DEPTH_TEST);
		//glDrawPixels(tempImage.size().width, tempImage.size().height, GL_BGR, GL_UNSIGNED_BYTE, tempImage.ptr());
		//glEnable(GL_DEPTH_TEST);

		setUpCamera();


		//glRotatef((float)glfwGetTime() * 70.f, 0.f, 1.f, 0.f);
		glTranslatef(0.f, 0.0f, 0.f);
		//drawColorCube(1.0);
		glPointSize(2);
		glBegin(GL_POINTS);
		for (int i = 0; i < v.size(); i++)
		{
			glPushMatrix();
			glColor3ub(i * 20, 100 + i * 10, i * 42);
			glVertex3f(v[i].x, v[i].y, v[i].z);
			glPopMatrix();
		}
		glEnd();

		//draw grid floor
		/*glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);
		for (GLfloat i = -2.5; i <= 2.5; i += 0.25) {
		glVertex3f(i, 0, 2.5); glVertex3f(i, 0, -2.5);
		glVertex3f(2.5, 0, i); glVertex3f(-2.5, 0, i);
		}
		glEnd();*/

		//draw coordinate system
		glBegin(GL_LINES);
		glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(1, 0, 0);
		glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 1, 0);
		glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 1);
		glEnd();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	//glfwTerminate();
	//exit(EXIT_SUCCESS);
}

void shutDown(int return_code)
{

	glfwTerminate();
	exit(return_code);
}

/*Camera should be set up according to the real camera pose
Using the intrinsic/extrinsic parameters from the camera*/
void setUpCamera(void)
{
	// reset view matrix
	glLoadIdentity();
	glViewport(0, 0, (GLsizei)width, (GLsizei)height); // The entire window

	// Select and setup the projection matrix.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); // Reset the matrix to identity
	// 90 degrees FOV, same aspect ratio as window, depth range 1 to 100
	gluPerspective(90.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f);

	// Select and setup the modelview matrix.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glScalef(1.0f, -1.0f, -1.0f);

	//move view back a bit
	//glTranslatef(0, 0, -30);

	gluLookAt(0.0f, 0.0f, 0.0f, //eye
		0.f, 0.f, -1.f,	  //center
		0.0f, 1.0f, 0.0f); //up
	//Rotate		
	glTranslatef(rotate_y, 0, 0);
	glTranslatef(0,0,rotate_x);

}


static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_LEFT && GLFW_PRESS){
		rotate_y += delta_rotate;

	}
	if (key == GLFW_KEY_RIGHT && GLFW_PRESS){
		rotate_y -= delta_rotate;
		//cout << "rotate_y: " << rotate_y << endl;

	}

	if (key == GLFW_KEY_UP && GLFW_PRESS){
		rotate_x += delta_rotate;


	}
	if (key == GLFW_KEY_DOWN && GLFW_PRESS){
		rotate_x -= delta_rotate;
	}
}