#include "MultipleViews.h"
//#include "PoseEstimation.h"
#include "Renderpoints.h"
//#include "Common.h"

int main(){
	//captureFrame();
	//cout << getBuildInformation()<< endl;
	Mat image1 = imread("images/reconstruction/image1.jpg", IMREAD_GRAYSCALE);
	Mat image2 = imread("images/reconstruction/image2.jpg", IMREAD_GRAYSCALE);
	if (!image1.data || !image2.data)
		return 0;
	Mat cameraMatrix, distCoeffs;

	readCalibration(cameraMatrix, distCoeffs);
	Mat undistort1 = undistortImage(image1, cameraMatrix, distCoeffs);
	Mat undistort2 = undistortImage(image2, cameraMatrix, distCoeffs);

	MultipleViews *object = new MultipleViews;
	Mat worldPoints3D;
	object->setIntrinsicParameters(cameraMatrix, distCoeffs);
	object->initBaseLine(undistort1, undistort2, worldPoints3D);
	cout << "worldPoints3D.at<float>(0, 0)" << worldPoints3D.at<float>(0, 0) << endl;
	
	cout <<"last: "<< object->storage.pointCloud[0].pt << endl;
	init(worldPoints3D);
	glfwMainLoop();
	shutDown(0);

	waitKey(0);
	return 0;
}