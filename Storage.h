#include "Common.h"
#include <opencv2\opencv.hpp>
#include <iostream>

//---------------------------------------------------
//Namespaces
//---------------------------------------------------
using namespace std;
using namespace cv;

//--------------------------------------------------------
//Storage class, this object contains all the things
//that needs to be stored and used in the successive frames
//--------------------------------------------------------

#ifndef STORAGE_H
#define STORAGE_H

class Storage {
public:
	//constructor 
	Storage(){
	}; 
	vector<vector<KeyPoint> > allKeypoints;
	Mat lastDescriptor;
	vector<map<pair<int, int>, vector<DMatch> > >matchMatrix;
	vector<CloudPoint> pointCloud;
	vector<Mat> allProjectionMatrices;

	void addKeyPoints(vector<KeyPoint> keyPoints);
	void setLastDescriptor(Mat descriptor);
	void addMatches(int view1, int view2, vector<DMatch> matches);
	void addCloudPoints(CloudPoint cloudPoint);
	void addProjectionMatrix(Mat projectionMatrix);

};
#endif