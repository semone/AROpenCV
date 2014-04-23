#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <iostream>
#include <math.h>
#include <algorithm>
#include "five-point-nister\five-point.hpp"
#include "Storage.h"

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
	MultipleViews(/**/):confidenceLevel(0.98f), minDistance(1.0f), ratio(0.4f){
		detector = new SurfFeatureDetector;
		extractor = new SurfDescriptorExtractor;
	};
	//functions
	//Should have a storage var from a created storage class
	Storage storage;
	vector<KeyPoint> MultipleViews::detectFeatures(Mat &image);
	Mat MultipleViews::describeFeatures(Mat &image, vector<KeyPoint> &keypoints);
	vector<DMatch> MultipleViews::matchAndClear(vector<KeyPoint> &keyPoints1, vector<KeyPoint> &keyPoints2, Mat &descriptors1, Mat &descriptors2);
	void newView(Mat &image);
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
	void initBaseLine(Mat &image1, Mat &image2, Mat &worldPoints3D);
	void recoverProjectionMatrix(Mat cameraMatrix, vector<DMatch> &matches, vector<Point2f> &points1, vector<Point2f> &points2, Mat &projMatrix1, Mat &projMatrix2);
	void triangulateAndAdd(Mat &projMatrix1, Mat &projMatrix2, vector<Point2f> &points1, vector<Point2f> &points2, Mat &worldPoints3D, vector<int> p1, vector<int>p2);
	void createCloudPoints(Mat worldPoints3D, vector<int> p1, vector<int>p2);

};
#endif