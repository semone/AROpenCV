#include "Common.h"
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <iostream>
#include <math.h>

//---------------------------------------------------
//Namespaces
//---------------------------------------------------
using namespace std;
using namespace cv;

//----------------------------------------------------
// MultipleView class, this is where the magic happens 
// Get the poses and the point cloud for several views
//----------------------------------------------------
#ifndef MULTIPLEVIEWS_H
#define MULTIPLEVIEWS_H
class MultipleViews{

private:
	SurfFeatureDetector *detector;
	SurfDescriptorExtractor *extractor;
	float confidenceLevel;
	float ratio;
	float minDistance;
	int currentView;
	Mat cameraMatrix;
	Mat distCoeffs;

public:
	//constructor
	MultipleViews(/**/){
		detector = new SurfFeatureDetector;
		extractor = new SurfDescriptorExtractor;
	};
	//functions
	//Should have a storage var from a created storage class
	void initSystem(/**/);
	vector<KeyPoint> MultipleViews::detectFeatures(Mat &image);
	Mat MultipleViews::describeFeatures(Mat &image, vector<KeyPoint> &keypoints);
	vector<DMatch> MultipleViews::matchAndClear(vector<KeyPoint> &keyPoints1, vector<KeyPoint> &keyPoints2, Mat &descriptors1, Mat &descriptors2);
	void recoverProjectionMatrix(/**/);
	void triangulateAndAdd(/**/);
	void get3D2DCorrespondances(/**/);
	void recoverNewPose(/**/);

	int ratioTest(vector<vector<DMatch>>& matches);
	void symmetryTest(const vector<vector<cv::DMatch>>& matches1, const vector<vector<cv::DMatch>>& matches2,
		vector<DMatch>& symMatches);
	void ransacTest(const vector<DMatch>& matches,
		const vector<KeyPoint>& keypoints1,
		const vector<KeyPoint>& keypoints2,
		vector<DMatch>& outMatches);
	void MultipleViews::setIntrinsicParameters(Mat cameraMatrix, Mat distCoeffs){
		this->cameraMatrix = cameraMatrix;
		this->distCoeffs = distCoeffs;
	}
};
#endif