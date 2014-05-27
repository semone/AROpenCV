#include "MultipleViews.h"
#include "Renderpoints.h"
#define GL_BGR 0x80E0

double fxx;
double fyy;
double pxx;
double pyy;

double fovyy;
double aspect1;
double nearCL1;
double farCL1;
void glCallback(void* param);


MultipleViews *object;
int main(){

	int choice;
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

	cout << "Enter 1 for point cloud or 2 for image: ";
	cin >> choice;

	if (choice == 1){
		cout << "object->storage.allProjectionMatrices[1]" << object->storage.allProjectionMatrices[1].at<double>(0, 3) << ", " << object->storage.allProjectionMatrices[1].at<double>(1, 3) << ", " << object->storage.allProjectionMatrices[1].at<double>(2, 3) << endl;
		cout <<"object->storage.pointCloud.size(): "<< object->storage.pointCloud.size() << endl;
		init(object->storage.pointCloud, object->storage.allProjectionMatrices);
		glfwMainLoop();
		shutDown(0);
	}
	else if (choice == 2){
		fxx = cameraMatrix.at<double>(0, 0);
		fyy = cameraMatrix.at<double>(1, 1);

		pxx = cameraMatrix.at<double>(0, 2);
		pyy = cameraMatrix.at<double>(1, 2);

		fovyy = 2 * atan(0.5*image1.size().height / fyy) * 180 / 3.14;
		aspect1 = (image1.size().width *fyy) / (image1.size().height *fxx);
		nearCL1 = 0.1;
		farCL1 = 100;

		namedWindow("MyVideo", CV_WINDOW_OPENGL | CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"

		Mat image = imread("images/features/image1.jpg", CV_LOAD_IMAGE_COLOR);
		image = undistortImage(image, cameraMatrix, distCoeffs);

		setOpenGlDrawCallback("MyVideo", glCallback, &image);
		while (waitKey(30) < 0)
		{
			updateWindow("MyVideo");
		}

		Mat img(720, 1280, CV_8UC3);
		glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);
		glPixelStorei(GL_PACK_ROW_LENGTH, img.step / img.elemSize());
		glReadPixels(0, 0, img.cols, img.rows, GL_BGR_EXT, GL_UNSIGNED_BYTE, img.data);
		Mat flipped(img);
		flip(img, flipped, 0);
		imwrite("snapshotout9.png", img);
		setOpenGlDrawCallback("MyVideo", 0, 0);
		destroyAllWindows();
	}
	else{
		cout << "Not a valid choice..";
	}
	
	waitKey(0);
	return 0;
}

void glCallback(void* param){
	//cout << "glcallback" << endl;

	Mat *inputImage = static_cast<Mat*>(param);
	Mat tempImage;

	flip((*inputImage), tempImage, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	glDisable(GL_DEPTH_TEST);
	glDrawPixels(tempImage.size().width, tempImage.size().height, GL_BGR, GL_UNSIGNED_BYTE, tempImage.ptr());
	glEnable(GL_DEPTH_TEST);

	//set projection matrix using intrinsic camera params
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//values based on the intrinsic camera parameters
	gluPerspective(fovyy, aspect1, nearCL1, farCL1);
	glViewport(0, 0, (GLsizei)tempImage.size().width, (GLsizei)tempImage.size().height);

	//set modelview matrix using extrinsic camera params
	double*_r = object->storage.allProjectionMatrices[1].ptr<double>();
	double rMat[16] = { _r[0], -_r[4], -_r[8], 0,
			_r[1], -_r[5], -_r[9], 0,
			_r[2], -_r[6], -_r[10], 0,
			_r[3], -_r[7], -_r[11], 1 };
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(rMat);

	glPushMatrix();
	glTranslatef(0.f, 2.0f, 12.f);
	glRotatef(15, 1, 0, 0);
	glRotatef(10, 0, 1, 0);

	drawColorCube(0.5f);
	glPopMatrix();
}
