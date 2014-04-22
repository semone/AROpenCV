#include "Storage.h"

void Storage::addKeyPoints(vector<KeyPoint> keyPoints){
	allKeypoints.push_back(keyPoints);
}

void Storage::setLastDescriptor(Mat descriptor){
	lastDescriptor = descriptor;

}
void Storage::addMatches(int view1, int view2, vector<DMatch> matches){

	map<pair<int, int>, vector<DMatch> > thisMatch;
	thisMatch[make_pair(view1, view2)] = matches;
	matchMatrix.push_back(thisMatch);
}

void Storage::addCloudPoints(CloudPoint cloudPoint){
	pointCloud.push_back(cloudPoint);
}

void Storage::addProjectionMatrix(Mat projectionMatrix){
	allProjectionMatrices.push_back(projectionMatrix);

}

