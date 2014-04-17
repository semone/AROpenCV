#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <math.h>


//Namespaces
using namespace cv;
using namespace std;

#ifndef FEATUREMATCHER_H
#define FEATUREMATCHER_H
class FeatureMatcher{

private:
	SurfFeatureDetector *detector;
	SurfDescriptorExtractor *extractor;
	float confidenceLevel;
	float ratio;
	float minDistance;
public:
	//constructor
	FeatureMatcher() : confidenceLevel(0.98f), minDistance(1.0f), ratio(0.4f){
		detector = new SurfFeatureDetector;
		extractor = new SurfDescriptorExtractor;
	}
	//member functions
	Mat FeatureMatcher::detectKeyPoints(Mat &image);
	vector<DMatch> FeatureMatcher::detectAndMatch(Mat &image1, Mat &image2, vector<KeyPoint> &keyPoints1, vector<KeyPoint> &keyPoints2);
	int ratioTest(vector<vector<DMatch>>& matches);
	void symmetryTest(const vector<vector<DMatch>>& matches1, const vector<vector<DMatch>>& matches2,
		vector<DMatch>& symMatches);
	void ransacTest(const vector<DMatch>& matches,
		const vector<KeyPoint>& keypoints1,
		const vector<KeyPoint>& keypoints2,
		vector<DMatch>& outMatches);

};

#endif // !FEATUREMATCHER_H


