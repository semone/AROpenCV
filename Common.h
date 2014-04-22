#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\calib3d\calib3d.hpp>
#include "opencv2/opencv.hpp"


#include <iostream>

using namespace cv;

struct CloudPoint{
	Point3f pt;
	vector<int> indexOf2DOrigin;
};

Mat undistortImage(Mat image, Mat cameraMatrix, Mat distCoeffs);
void readCalibration(Mat &cameraMatrix, Mat &distCoeffs);
//vector<Point3f> modifyPoints(Mat worldPoints3D);
bool captureFrame();