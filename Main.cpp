#include "MultipleViews.h"
#include "Renderpoints.h"
//#include <opencv2/core/opengl_interop.hpp>
//#include <windows.h>
//#include <GL\GLU.h>
//#include <GL\gl.h>
//#include </Program/GLFW/include/GLFW/glfw3.h>

//#define GL_BGR 0x80E0
//
//double fx;
//double fy;
//double px;
//double py;
//
//double fovy;
//double aspect;
//double nearCL;
//double farCL;

void drawColorCube(float r);
void glCallback(void* param);
void BundleAdjuster(Storage s);
MultipleViews *object;
int main(){
	//captureFrame();
	//cout << getBuildInformation()<< endl;
	Mat image1 = imread("images/features/image0.jpg", IMREAD_GRAYSCALE);
	Mat image2 = imread("images/features/image1.jpg", IMREAD_GRAYSCALE);
	Mat image3 = imread("images/features/image2.jpg", IMREAD_GRAYSCALE);
	Mat image4 = imread("images/features/image3.jpg", IMREAD_GRAYSCALE);
	Mat image5 = imread("images/features/image4.jpg", IMREAD_GRAYSCALE);

	if (!image1.data || !image2.data || !image3.data || !image4.data || !image5.data)
		return 0;
	Mat cameraMatrix, distCoeffs;

	readCalibration(cameraMatrix, distCoeffs);
	Mat undistort1 = undistortImage(image1, cameraMatrix, distCoeffs);
	Mat undistort2 = undistortImage(image2, cameraMatrix, distCoeffs);
	Mat undistort3 = undistortImage(image3, cameraMatrix, distCoeffs);
	Mat undistort4 = undistortImage(image4, cameraMatrix, distCoeffs);
	Mat undistort5 = undistortImage(image5, cameraMatrix, distCoeffs);

	object = new MultipleViews;
	object->setIntrinsicParameters(cameraMatrix, distCoeffs);
	object->initBaseLine(undistort1, undistort2);
	object->newView(undistort3);
	object->newView(undistort4);
	object->newView(undistort5);
	//cout << "object->storage.allProjectionMatrices[1]" << object->storage.allProjectionMatrices[1].at<double>(0, 3) << ", " << object->storage.allProjectionMatrices[1].at<double>(1, 3) << ", " << object->storage.allProjectionMatrices[1].at<double>(2, 3) << endl;
	cout <<"object->storage.pointCloud.size(): "<< object->storage.pointCloud.size() << endl;
	//BundleAdjuster(object->storage);
	//cout << "worldPoints: " << worldPoints3D.size() << endl;
	init(object->storage.pointCloud, object->storage.allProjectionMatrices);
	glfwMainLoop();
	shutDown(0);

	//fx = cameraMatrix.at<double>(0, 0);
	//fy = cameraMatrix.at<double>(1, 1);

	//px = cameraMatrix.at<double>(0, 2);
	//py = cameraMatrix.at<double>(1, 2);

	//fovy = 2 * atan(0.5*image1.size().height / fy) * 180 / 3.14;
	//aspect = (image1.size().width *fy) / (image1.size().height *fx);
	//nearCL = 0.1;
	//farCL = 100;

	//namedWindow("MyVideo", CV_WINDOW_OPENGL | CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"

	//Mat image = imread("images/features/image3.jpg", CV_LOAD_IMAGE_COLOR);
	//image = undistortImage(image, cameraMatrix, distCoeffs);

	//setOpenGlDrawCallback("MyVideo", glCallback, &image);
	//while (waitKey(30) < 0)
	//{
	//	updateWindow("MyVideo");
	//}

	//cv::Mat img(720,1280, CV_8UC3);
	//glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);
	//glPixelStorei(GL_PACK_ROW_LENGTH, img.step / img.elemSize());
	//glReadPixels(0, 0, img.cols, img.rows, GL_BGR_EXT, GL_UNSIGNED_BYTE, img.data);
	//cv::Mat flipped(img);
	//cv::flip(img, flipped, 0);
	//cv::imwrite("snapshot5.png", img);

	//setOpenGlDrawCallback("MyVideo", 0, 0);
	//destroyAllWindows();
	waitKey(0);
	return 0;
}

//void glCallback(void* param){
//	//cout << "glcallback" << endl;
//
//	Mat *inputImage = static_cast<Mat*>(param);
//	Mat tempImage;
//
//	flip((*inputImage), tempImage, 0);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	glDisable(GL_DEPTH_TEST);
//	glDrawPixels(tempImage.size().width, tempImage.size().height, GL_BGR, GL_UNSIGNED_BYTE, tempImage.ptr());
//	glEnable(GL_DEPTH_TEST);
//
//	//set projection matrix using intrinsic camera params
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	//values based on the intrinsic camera parameters
//	gluPerspective(fovy, aspect, nearCL, farCL);
//	glViewport(0, 0, (GLsizei)tempImage.size().width, (GLsizei)tempImage.size().height);
//
//	//set modelview matrix using extrinsic camera params
//	double*_r = object->storage.allProjectionMatrices[0].ptr<double>();
//	double rMat[16] = { _r[0], -_r[4], -_r[8], 0,
//			_r[1], -_r[5], -_r[9], 0,
//			_r[2], -_r[6], -_r[10], 0,
//			_r[3], -_r[7], -_r[11], 1 };
//	glMatrixMode(GL_MODELVIEW);
//	glLoadMatrixd(rMat);
//
//	glPushMatrix();
//	glTranslatef(4.f, -1.0f, 12.f);
//	glRotatef(15, 1, 0, 0);
//	glRotatef(10, 0, 1, 0);
//
//	drawColorCube(0.3f);
//	glPopMatrix();
//}
//
//void drawColorCube(float r) {
//	glBegin(GL_QUADS);
//	// +X face
//	glColor3f(1, 1, 0);
//	glNormal3f(1, 0, 0);
//	glVertex3f(r, r, -r);
//	glVertex3f(r, r, r);
//	glVertex3f(r, -r, r);
//	glVertex3f(r, -r, -r);
//	// -X face
//	glColor3f(0, 0, 1);
//	glNormal3f(-1, 0, 0);
//	glVertex3f(-r, r, r);
//	glVertex3f(-r, r, -r);
//	glVertex3f(-r, -r, -r);
//	glVertex3f(-r, -r, r);
//	// +Y face
//	glColor3f(1, 0, 1);
//	glNormal3f(0, 1, 0);
//	glVertex3f(-r, r, r);
//	glVertex3f(r, r, r);
//	glVertex3f(r, r, -r);
//	glVertex3f(-r, r, -r);
//	// -Y face
//	glColor3f(0, 1, 0);
//	glNormal3f(0, -1, 0);
//	glVertex3f(-r, -r, r);
//	glVertex3f(-r, -r, -r);
//	glVertex3f(r, -r, -r);
//	glVertex3f(r, -r, r);
//	// +Z face
//	glColor3f(0, 1, 1);
//	glNormal3f(0, 0, 1);
//	glVertex3f(-r, r, r);
//	glVertex3f(-r, -r, r);
//	glVertex3f(r, -r, r);
//	glVertex3f(r, r, r);
//	// -Z face
//	glColor3f(1, 0, 0);
//	glNormal3f(0, 0, -1);
//	glVertex3f(-r, r, -r);
//	glVertex3f(r, r, -r);
//	glVertex3f(r, -r, -r);
//	glVertex3f(-r, -r, -r);
//	glEnd();
//}

//void BundleAdjuster(Storage s){
//	//1. Get size of point cloud and how many cameras used.
//	int N = s.pointCloud.size();
//	int M = s.allProjectionMatrices.size();
//	cout << "N: " << N << endl;
//	cout << "M: " << M << endl;
//	
//	//2. Initialize variables which should be sent to bundleadjuster
//	vector<Point3d> points(N);		// positions of points in global coordinate system (input and output)
//	vector<vector<Point2d> > imagePoints(M, vector<Point2d>(N)); // projections of 3d points for every camera
//	vector<vector<int> > visibility(M, vector<int>(N)); // visibility of 3d points for every camera
//	vector<Mat> cameraMatrix(M);	// intrinsic matrices of all cameras (input and output)
//	vector<Mat> R(M);				// rotation matrices of all cameras (input and output)
//	vector<Mat> T(M);				// translation vector of all cameras (input and output)
//	vector<Mat> distCoeffs(M);
//
//	for (int pt3d = 0; pt3d < N; pt3d++){
//		//3. Get vector of point cloud points from storage and put in points.
//		points[pt3d] = s.pointCloud[pt3d].pt;
//
//		for (int cameras = 0; cameras < M; cameras++){
//			//4. Check if point is visible in each camera frame.
//			std::map<int, int>::const_iterator search = s.pointCloud[pt3d].indexOf2DOrigin.find(cameras);
//			if (search != s.pointCloud[pt3d].indexOf2DOrigin.end()) {
//				cout << "cameras: " << cameras;
//				std::cout << "Found " << search->first << " " << search->second << "\n";
//			}
//
//		}
//	}
//
//
//
//}