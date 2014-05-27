#include "Renderpoints.h"
#define GL_BGR 0x80E0

GLFWwindow* window;
double width = 1280, height = 720;
float rotate_y = 0.f, rotate_x = 0.f, rotate_r = 0.f, delta_rotate;
double angleY, angleX;
const float rotations_per_tick = .02f;
vector<Point3f> v;
vector<Mat> projMatrices;
void drawColorCube(float r);
void yawPitchRoll(double* _r, float &yaw, float &pitch, float &roll);
void moveMouse();
double fx;
double fy;
double px;
double py;

double fovy;
double aspect;
double nearCL;
double farCL;

bool mouseDown = false;
double initx, inity;
double releaseAngleX, releaseAngleY;

static void onMouseButtonPress(GLFWwindow * window, int button, int action, int mods);
vector<Point3f> modifyPoints(vector<CloudPoint> worldPoints3D){
	int i = 0;
	vector<Point3f> points;
	Point3f p;

	for (int r = 0; r < worldPoints3D.size(); r++){
		p.x = worldPoints3D[r].pt.x;
		p.y = worldPoints3D[r].pt.y;
		p.z = worldPoints3D[r].pt.z;
		points.push_back(p);
	}
	return points;
}


void init(vector<CloudPoint> worldPoints3D, vector<Mat> projMats)
{
	Mat cameraMatrix, distCoeffs;
	readCalibration(cameraMatrix, distCoeffs);
	fx = cameraMatrix.at<double>(0, 0);
	fy = cameraMatrix.at<double>(1, 1);

	px = cameraMatrix.at<double>(0, 2);
	py = cameraMatrix.at<double>(1, 2);

	fovy = 2 * atan(0.5*height / fy) * 180 / 3.14;
	aspect = (width*fy) / (height*fx);
	nearCL = 0.1;
	farCL = 100;
	v = modifyPoints(worldPoints3D);
	projMatrices = projMats;
	double*_r = projMatrices[0].ptr<double>();


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
	glfwSetMouseButtonCallback(window, onMouseButtonPress);

	glfwSetKeyCallback(window, key_callback);
	glEnable(GL_CULL_FACE); // Cull away all back facing polygons
	glEnable(GL_DEPTH_TEST); // Use the Z buffer
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void glfwMainLoop(void){
	double old_time = glfwGetTime();
	VideoWriter writer("file.avi", CV_FOURCC('D', 'I', 'V', 'X'), 24, Size(1280, 720), true);
	while (!glfwWindowShouldClose(window))
	{
		Mat img(720,1280, CV_8UC3);
		glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);
		glPixelStorei(GL_PACK_ROW_LENGTH, img.step / img.elemSize());
		glReadPixels(0, 0, img.cols, img.rows, GL_BGR_EXT, GL_UNSIGNED_BYTE, img.data);
		Mat flipped(img);
		flip(img, flipped, 0);
		//Mat tempImage;
		
		double current_time = glfwGetTime();
		Mat image = imread("images/features/image1.jpg", CV_LOAD_IMAGE_COLOR);

		delta_rotate = (float)(current_time - old_time) * rotations_per_tick * 360;
		old_time = current_time;
		float ratio;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		Mat tempImage;
		//flip(image, tempImage, 0);

		////glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glDisable(GL_DEPTH_TEST);
		//glDrawPixels(tempImage.size().width, tempImage.size().height, GL_BGR, GL_UNSIGNED_BYTE, tempImage.ptr());
		//glEnable(GL_DEPTH_TEST);
	

		setUpCamera();
		if (mouseDown){
			moveMouse();
		}

		glRotatef((GLfloat)angleY, 0, 1, 0);
		glRotatef((GLfloat)angleX, 1, 0, 0);

		//glRotatef((float)glfwGetTime() * 70.f, 0.f, 1.f, 0.f);
		//glPushMatrix();
		//glTranslatef(-1.6f, -1.0f, 12.f);
		//glRotatef(10, 1, 0, 0);
		//drawColorCube(0.3);
		//glPopMatrix();
		glPointSize(3);
		glBegin(GL_POINTS);
		for (int i = 0; i < v.size(); i++){
			glPushMatrix();
			glColor3ub(i * 20, 100 + i * 10, i * 42);
			glVertex3f(v[i].x, v[i].y, v[i].z);
			glPopMatrix();
		}
		glEnd();
		//glPointSize(10);
		//glBegin(GL_POINTS);
		//glBegin(GL_LINES);

		for (int i = 0; i < projMatrices.size(); i++){
			//cout << "proj: " << i << endl;
			double*_r = projMatrices[i].ptr<double>();
			float y, p, ro;
			yawPitchRoll(_r, y, p, ro);
			/*cout << "yaw" << y << endl; 
			cout << "pich" << p << endl;
			cout << "roll" << ro << endl;*/

			glPushMatrix();
			glRotatef(y,0,0,1);
			glRotatef(p,0, 1, 0);
			glRotatef(ro,1, 0, 0);

			//glColor3ub(i * 20, 100 + i * 10, i * 42);
			//glVertex3f(-(GLfloat)projMatrices[i].at<double>(0, 3), (GLfloat)projMatrices[i].at<double>(1, 3), (GLfloat)projMatrices[i].at<double>(2, 3));
			/*glColor3f(1, 0, 0); glVertex3f(0,0,0); glVertex3f(1, 0, 0);
			glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 1, 0);
			glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 1);*/
			//cout <<"1: "<< _r[3] << "2: " <<-_r[7] << "3: "<< -_r[11] << endl;
			glBegin(GL_LINES);
			glColor3f(1, 0, 0); glVertex3f(-_r[3], -_r[7], -_r[11]); glVertex3f(-_r[3] + 1, -_r[7], -_r[11]);
			glColor3f(0, 1, 0); glVertex3f(-_r[3], -_r[7], -_r[11]); glVertex3f(-_r[3], -_r[7] + 1, -_r[11]);
			glColor3f(0, 0, 1); glVertex3f(-_r[3], -_r[7], -_r[11]); glVertex3f(-_r[3], -_r[7], -_r[11] + 1);
			glEnd();
			glPopMatrix();
		}
		//glEnd();
	
		//draw grid floor
		/*glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);
		for (GLfloat i = -5.5; i <= 5.5; i += 0.25) {
		glVertex3f(i, 0, 5.5); glVertex3f(i, 0, -5.5);
		glVertex3f(5.5, 0, i); glVertex3f(-5.5, 0, i);
		}
		glEnd();*/

		//draw coordinate system
		double*_r = projMatrices[1].ptr<double>();
	/*	cout << _r[0] << endl;
		cout << _r[1] << endl;
		cout << _r[2] << endl;
		cout << _r[3] << endl;
		cout << _r[4] << endl;
		cout << _r[5] << endl;
		cout << _r[6] << endl;
		cout << _r[7] << endl;
		cout << _r[8] << endl;
		cout << _r[9] << endl;
		cout << _r[10] << endl;
		cout << _r[11] << endl;*/
		//glBegin(GL_LINES);
		///*glColor3f(1, 0, 0); glVertex3f(0,0,0); glVertex3f(1, 0, 0);
		//glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 1, 0);
		//glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 1);*/
		////cout <<"1: "<< _r[3] << "2: " <<-_r[7] << "3: "<< -_r[11] << endl;
		//glColor3f(1, 0, 0); glVertex3f(-_r[3], _r[7], _r[11]); glVertex3f(-_r[3]+1, _r[7], _r[11]);
		//glColor3f(0, 1, 0); glVertex3f(-_r[3], _r[7], _r[11]); glVertex3f(-_r[3], _r[7]+1, _r[11]);
		//glColor3f(0, 0, 1); glVertex3f(-_r[3], _r[7], _r[11]); glVertex3f(-_r[3], _r[7], _r[11]+1);
		//glEnd();
		writer.write(img);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	//glfwTerminate();
	//exit(EXIT_SUCCESS);
}

void yawPitchRoll(double*_r, float &yaw, float &pitch, float &roll){
	
	//yaw = atan2(_r[9], _r[10]);
	//pitch = atan2(-_r[8], sqrt(_r[9] * _r[9] + _r[10] * _r[10]));
	//roll = atan2(_r[4], _r[0]);
	yaw = (float)atan2(_r[6], _r[10]);
	pitch = (float)atan2(-_r[2], sqrt(_r[6] * _r[6] + _r[10] * _r[10]));
	roll = (float)atan2(_r[1], _r[0]);

	//Convert to degrees
	yaw = yaw*(180.f / 3.1415f);
	pitch = pitch*(180.f / 3.1415f);
	roll = roll*(180.f / 3.1415f);
}

void drawColorCube(float r) {
	glBegin(GL_QUADS);
	// +X face
	glColor3f(1, 1, 0);
	glNormal3f(1, 0, 0);
	glVertex3f(r, r, -r);
	glVertex3f(r, r, r);
	glVertex3f(r, -r, r);
	glVertex3f(r, -r, -r);
	// -X face
	glColor3f(0, 0, 1);
	glNormal3f(-1, 0, 0);
	glVertex3f(-r, r, r);
	glVertex3f(-r, r, -r);
	glVertex3f(-r, -r, -r);
	glVertex3f(-r, -r, r);
	// +Y face
	glColor3f(1, 0, 1);
	glNormal3f(0, 1, 0);
	glVertex3f(-r, r, r);
	glVertex3f(r, r, r);
	glVertex3f(r, r, -r);
	glVertex3f(-r, r, -r);
	// -Y face
	glColor3f(0, 1, 0);
	glNormal3f(0, -1, 0);
	glVertex3f(-r, -r, r);
	glVertex3f(-r, -r, -r);
	glVertex3f(r, -r, -r);
	glVertex3f(r, -r, r);
	// +Z face
	glColor3f(0, 1, 1);
	glNormal3f(0, 0, 1);
	glVertex3f(-r, r, r);
	glVertex3f(-r, -r, r);
	glVertex3f(r, -r, r);
	glVertex3f(r, r, r);
	// -Z face
	glColor3f(1, 0, 0);
	glNormal3f(0, 0, -1);
	glVertex3f(-r, r, -r);
	glVertex3f(r, r, -r);
	glVertex3f(r, -r, -r);
	glVertex3f(-r, -r, -r);
	glEnd();
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
	//gluPerspective(90.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f);
	gluPerspective(fovy, aspect, nearCL, farCL);
	double*_r = projMatrices[1].ptr<double>();
	double rMat[16] = { _r[0], -_r[4], -_r[8], 0,
		_r[1], -_r[5], -_r[9], 0,
		_r[2], -_r[6], -_r[10], 0,
		_r[3], -_r[7], -_r[11], 1 };
	// Select and setup the modelview matrix.
	glMatrixMode(GL_MODELVIEW);
	//cout <<"projMatrices[0]" << projMatrices[0] << endl;
	//set projection matrix using intrinsic camera params
	glLoadMatrixd(rMat);
	//glLoadIdentity();
	//glScalef(1.0f, -1.0f, -1.0f);

	//move view back a bit
	//glTranslatef(0, 0, -30);

	//gluLookAt(0.0f, 0.0f, 0.0f, //eye
	//	1.f, 0.f, -10.f,	  //center
	//	0.0f, 1.0f, 0.0f); //up
	//Rotate + translate


	glTranslatef(0, rotate_y, 0);
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
		rotate_x -= delta_rotate;

	}
	if (key == GLFW_KEY_RIGHT && GLFW_PRESS){
		rotate_x += delta_rotate;
		//cout << "rotate_y: " << rotate_y << endl;

	}

	if (key == GLFW_KEY_UP && GLFW_PRESS){
		rotate_y += delta_rotate;


	}
	if (key == GLFW_KEY_DOWN && GLFW_PRESS){
		rotate_y -= delta_rotate;
	}

	//if (key == GLFW_KEY_PAGE_UP && GLFW_PRESS){
	//	rotate_r += delta_rotate;
	//	//cout << "rotate_r: " << rotate_r << endl;

	//}

	//if (key == GLFW_KEY_PAGE_DOWN && GLFW_PRESS){
	//	rotate_r -= delta_rotate;
	//	//cout << "rotate_r: " << rotate_r << endl;
	//}
}


static void onMouseButtonPress(GLFWwindow * window, int button, int action, int mods){
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		cv::Mat img(720,1280, CV_8UC3);
		glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);
		glPixelStorei(GL_PACK_ROW_LENGTH, img.step / img.elemSize());
		glReadPixels(0, 0, img.cols, img.rows, GL_BGR_EXT, GL_UNSIGNED_BYTE, img.data);
		cv::Mat flipped(img);
		cv::flip(img, flipped, 0);
		cv::imwrite("pc2.png", img);
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
			//cout << "lefty press" << endl;
			glfwGetCursorPos(window, &initx, &inity);
			mouseDown = true;
	}

	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		//cout << "lefty release" << endl;
		mouseDown = false;
		releaseAngleX = angleX;
		releaseAngleY = angleY;

		//cout << "anglex after: " << angleX << endl;
	}
}

void moveMouse(){

	double xpos, ypos, xprev = initx, yprev = inity;
	glfwGetCursorPos(window, &xpos, &ypos);

	angleY = releaseAngleY + (xpos - xprev) * rotations_per_tick;
	angleX = releaseAngleX + (ypos - yprev) * rotations_per_tick;
	xprev = xpos;
	yprev = ypos;

}