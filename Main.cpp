#include "MultipleViews.h"
#include "Renderpoints.h"

int main(){
	//captureFrame();
	//cout << getBuildInformation()<< endl;
	Mat image1 = imread("images/features/image0.jpg", IMREAD_GRAYSCALE);
	Mat image2 = imread("images/features/image1.jpg", IMREAD_GRAYSCALE);
	Mat image3 = imread("images/features/image2.jpg", IMREAD_GRAYSCALE);
	Mat image4 = imread("images/features/image3.jpg", IMREAD_GRAYSCALE);

	if (!image1.data || !image2.data || !image3.data)
		return 0;
	Mat cameraMatrix, distCoeffs;

	readCalibration(cameraMatrix, distCoeffs);
	Mat undistort1 = undistortImage(image1, cameraMatrix, distCoeffs);
	Mat undistort2 = undistortImage(image2, cameraMatrix, distCoeffs);
	Mat undistort3 = undistortImage(image3, cameraMatrix, distCoeffs);
	Mat undistort4 = undistortImage(image4, cameraMatrix, distCoeffs);

	MultipleViews *object = new MultipleViews;
	object->setIntrinsicParameters(cameraMatrix, distCoeffs);
	object->initBaseLine(undistort1, undistort2);
	object->newView(undistort3);
	//object->newView(undistort4);

	cout <<"object->storage.pointCloud.size(): "<< object->storage.pointCloud.size() << endl;
	//cout << "worldPoints: " << worldPoints3D.size() << endl;
	init(object->storage.pointCloud);
	glfwMainLoop();
	shutDown(0);

	waitKey(0);
	return 0;
}