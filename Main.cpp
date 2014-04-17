#include "MultipleViews.h"
#include "PoseEstimation.h"
#include "Renderpoints.h"
#include "Common.h"

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
	//Mat undistort3 = undistortImage(image3, cameraMatrix, distCoeffs);

	MultipleViews *object = new MultipleViews;

	object->setIntrinsicParameters(cameraMatrix, distCoeffs);
	vector<KeyPoint> keyPoints1 = object->detectFeatures(undistort1);
	vector<KeyPoint> keyPoints2 = object->detectFeatures(undistort2);

	Mat descriptors1 = object->describeFeatures(undistort1, keyPoints1);
	Mat descriptors2 = object->describeFeatures(undistort2, keyPoints2);

	object->matchAndClear(keyPoints1, keyPoints2, descriptors1, descriptors2);
	//FeatureMatcher featurMathcer;
	//vector<DMatch> matches;
	//vector<KeyPoint> keyPoints1, keyPoints2;

	//matches = featurMathcer.detectAndMatch(undistort1, undistort2, keyPoints1, keyPoints2);

	//cout << "Number of matched points (after cleaning): " << matches.size() << endl;
	//cout << "KeyPoints1: " << keyPoints1.size() << endl;
	//cout << "KeyPoints2: " << keyPoints2.size() << endl;

	//vector<Point2f> points1, points2;
	//
	////Get the matching key points and put them in vectors. 
	//for (vector<DMatch>::const_iterator it = matches.begin();
	//	it != matches.end(); ++it) {
	//	points1.push_back(keyPoints1[it->queryIdx].pt);
	//	points2.push_back(keyPoints2[it->trainIdx].pt);
	//}

	//Mat projMatrix1(3, 4, CV_64F), projMatrix2(3, 4, CV_64F);
	//recoverProjectionMatrix(cameraMatrix, matches, points1, points2, projMatrix1, projMatrix2);
	////cout << "P1: " << projMatrix1 << endl;
	//cout << "matches size: " << matches.size()<< endl;
	////cout << "P2: " << projMatrix2 << endl;
	//Mat worldPoints4D, worldPoints3D;
	//triangulatePoints(projMatrix1, projMatrix2,points1,points2,worldPoints4D);
	////cout << "Triangulated points (homogeneous): " << worldPoints.t() << endl;
	//convertPointsFromHomogeneous(worldPoints4D.t(), worldPoints3D);
	//cout<< "worldPoints3D.at<float>(0, 0)" << worldPoints3D.at<float>(0, 0) << endl;

	//map<pair<int, int>, vector<DMatch> > thisMatch;
	//thisMatch[make_pair(0, 1)] = matches;
	//
	//matchMatrix.push_back(thisMatch);
	//projMatrices.push_back(projMatrix1);
	//projMatrices.push_back(projMatrix2);

	////keyPointMatrix, lastDescriptor, pointCloud



	//init(worldPoints3D);
	//glfwMainLoop();
	//shutDown(0);

	waitKey(0);
	return 0;
}