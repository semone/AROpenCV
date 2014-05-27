#include "MultipleViews.h"
#include <windows.h>


double get_time()
{
	LARGE_INTEGER t, f;
	QueryPerformanceCounter(&t);
	QueryPerformanceFrequency(&f);
	return (double)t.QuadPart / (double)f.QuadPart;
}

vector<KeyPoint> MultipleViews::detectFeatures(Mat &image){
	cout << "Detect features.." << endl;
	Mat mask = Mat::zeros(image.size(), CV_8U);  // type of mask is CV_8U
	Mat roi(mask, Rect(20, 20, image.size().width - 40, image.size().height - 40));
	roi = Scalar(255, 255, 255);

	vector<KeyPoint> keyPoints;

	detector->hessianThreshold = 300;
	detector->detect(image, keyPoints, mask);
	cout << "Number of SURF points (1): " << keyPoints.size() << endl;
	return keyPoints;
}

Mat MultipleViews::describeFeatures(Mat &image, vector<KeyPoint> &keyPoints){
	cout << "Describe features..." << endl;
	//double d1 = get_time();

	Mat descriptors;
	extractor->compute(image, keyPoints, descriptors);
	//cout << "descriptor1 matrix size: " << descriptors.rows << " by " << descriptors.cols << endl;
	//double d2 = get_time();
	//cout << "describeFeatures time: " << d2 - d1 << endl;
	return descriptors;
}

int MultipleViews::ratioTest(vector<vector<DMatch>>& matches) {
	//double d1 = get_time();
	int removed = 0;
	// for all matches
	for (vector<vector<DMatch>>::iterator matchIterator = matches.begin();
		matchIterator != matches.end(); ++matchIterator) {
		// if 2 NN has been identified
		if (matchIterator->size() > 1) {
			// check distance ratio
			if ((*matchIterator)[0].distance / (*matchIterator)[1].distance > ratio) {
				matchIterator->clear(); // remove match
				removed++;
			}
		}
		else { // does not have 2 neighbours
			matchIterator->clear(); // remove match
			removed++;
		}
	}
	//double d2 = get_time();
	//cout << "ratioTest time: " << d2 - d1 << endl;

	return removed;
}
// Insert symmetrical matches in symMatches vector
void MultipleViews::symmetryTest(const std::vector<vector<DMatch>>& matches1,
	const vector<vector<DMatch>>& matches2,
	std::vector<cv::DMatch>& symMatches) {
	//double d1 = get_time();
	// for all matches image 1 -> image 2
	for (vector<vector<DMatch>>::const_iterator matchIterator1 = matches1.begin();
		matchIterator1 != matches1.end(); ++matchIterator1) {

		if (matchIterator1->size() < 2) // ignore deleted matches 
			continue;

		// for all matches image 2 -> image 1
		for (vector<vector<DMatch>>::const_iterator matchIterator2 = matches2.begin();
			matchIterator2 != matches2.end(); ++matchIterator2) {

			if (matchIterator2->size() < 2) // ignore deleted matches 
				continue;

			// Match symmetry test
			if ((*matchIterator1)[0].queryIdx == (*matchIterator2)[0].trainIdx &&
				(*matchIterator2)[0].queryIdx == (*matchIterator1)[0].trainIdx) {

				// add symmetrical match
				symMatches.push_back(cv::DMatch((*matchIterator1)[0].queryIdx,
					(*matchIterator1)[0].trainIdx,
					(*matchIterator1)[0].distance));
				break; // next match in image 1 -> image 2
			}
		}
	}
	//double d2 = get_time();
	//cout << "symmetryTest time: " << d2 - d1 << endl;
}

void MultipleViews::ransacTest(const vector<DMatch>& matches,
	const vector<KeyPoint>& keypoints1,
	const vector<KeyPoint>& keypoints2,
	vector<DMatch>& outMatches) {
	//double d1 = get_time();
	// Convert keypoints into Point2f       
	vector<Point2f> points1, points2;
	for (vector<DMatch>::const_iterator it = matches.begin();
		it != matches.end(); ++it) {
		// Get the position of left keypoints
		points1.push_back(keypoints1[it->queryIdx].pt);
		// Get the position of right keypoints
		points2.push_back(keypoints2[it->trainIdx].pt);
	}

	// Compute F matrix using RANSAC
	vector<uchar> inliers(points1.size(), 0);
	Mat fundemental = findFundamentalMat(
		Mat(points1), Mat(points2), // matching points
		inliers,      // match status (inlier or outlier)  
		FM_RANSAC, // RANSAC method
		minDistance,     // distance to epipolar line
		confidenceLevel);  // confidence probability

	// extract the surviving (inliers) matches
	vector<uchar>::const_iterator itIn = inliers.begin();
	vector<DMatch>::const_iterator itM = matches.begin();
	// for all matches
	for (; itIn != inliers.end(); ++itIn, ++itM) {

		if (*itIn) { // it is a valid match

			outMatches.push_back(*itM);
		}
	}
	//double d2 = get_time();
	//cout << "ransacTest time: " << d2 - d1 << endl;
}
vector<DMatch> MultipleViews::matchAndClear(vector<KeyPoint> &keyPoints1, vector<KeyPoint> &keyPoints2, Mat &descriptors1, Mat &descriptors2){
	BFMatcher matcher(NORM_L2);
	vector<vector<DMatch> > matches12, matches21;
	matcher.clear();
	//double d1 = get_time();
	cout << "Recover knnmatches..." << endl;
	//Matches from image1 -> image2 2NN
	matcher.knnMatch(descriptors1, descriptors2, matches12, 2);
	//Matches from image2 -> image1 2NN
	matcher.knnMatch(descriptors2, descriptors1, matches21, 2);
	/*cout << "Number of knnMatch (1): " << matches12.size() << endl;
	cout << "Number of knnMatch (2): " << matches21.size() << endl;*/
/*	double d2 = get_time();
	cout << "knnMatch: " << d2 - d1 << endl*/;
	//Remove with ratio test. If match to similar disgard. 
	cout << "Ratiotest..." << endl;
	int removed = ratioTest(matches12);
	//cout << "Number of matched points 1->2 (ratio test) : " << matches12.size() - removed << endl;
	removed = ratioTest(matches21);
	//cout << "Number of matched points 2->1 (ratio test) : " << matches21.size() - removed << endl;

	//Symmetry test
	cout << "Find symmetric matches" << endl;
	vector<DMatch> symMatches;
	symmetryTest(matches12, matches21, symMatches);

	//cout << "Number of matched points (symmetry test): " << symMatches.size() << endl;

	//Use ransac to remove outliers, check with epipolar line
	cout << "RANSAC test" << endl;
	vector<DMatch> ransacMatch;
	ransacTest(symMatches, keyPoints1, keyPoints2, ransacMatch);
	//cout << "RANSAC matches.size(): " << ransacMatch.size() << endl;

	return ransacMatch;
}

void MultipleViews::initBaseLine(Mat &image1, Mat &image2){
	//cout << "Current View (1): " << currentView << endl;
	vector<KeyPoint> keyPoints1 = detectFeatures(image1);
	vector<KeyPoint> keyPoints2 = detectFeatures(image2);
	storage.addKeyPoints(keyPoints1);
	storage.addKeyPoints(keyPoints2);

	Mat descriptors1 = describeFeatures(image1, keyPoints1);
	Mat descriptors2 = describeFeatures(image2, keyPoints2);

	storage.setLastDescriptor(descriptors2);
	vector<DMatch> matches = matchAndClear(keyPoints1, keyPoints2, descriptors1, descriptors2);
	storage.addMatches(0,1, matches);
	//Mat matchImg3;
	//drawMatches(image1, keyPoints1, image2, keyPoints2, matches, matchImg3, Scalar::all(-1));
	//imshow("Initial matches", matchImg3);
	//imwrite("outsideCleanMatches2.png", matchImg3);

	vector<Point2f> points1, points2;
	vector<int> pointIdx1, pointIdx2;
	
	//Get the matching key points and put them in vectors. 
	for (vector<DMatch>::const_iterator it = matches.begin();
		it != matches.end(); ++it) {
		points1.push_back(keyPoints1[it->queryIdx].pt);
		points2.push_back(keyPoints2[it->trainIdx].pt);
		pointIdx1.push_back(it->queryIdx);
		pointIdx2.push_back(it->trainIdx);
	}
	
	Mat projMatrix1(3, 4, CV_64F), projMatrix2(3, 4, CV_64F);

	recoverProjectionMatrix(cameraMatrix, matches, points1, points2, projMatrix1, projMatrix2);
	triangulateAndAdd(projMatrix1, projMatrix2, points1, points2, pointIdx1, pointIdx2);
	currentView++;
	//cout << "Current View (2): " << currentView << endl;
	//LevMarqSparse::bundleAdjust()
}

void MultipleViews::recoverProjectionMatrix(Mat cameraMatrix, vector<DMatch> &matches, vector<Point2f> &points1, vector<Point2f> &points2, Mat &projMatrix1, Mat &projMatrix2){
	cout << "Recover pose..." << endl;
	//double d1 = get_time();

	Point2d pp = Point2d(cameraMatrix.at<double>(0, 2), cameraMatrix.at<double>(1, 2));
	double focal = cameraMatrix.at<double>(0, 0);

	Mat E = findEssentialMat(points1, points2, focal, pp, CV_RANSAC, 0.99, 1.0, noArray());
	Mat R, t;
	recoverPose(E, points1, points2, R, t, focal, pp, noArray());

	Mat P2 = (Mat_<double>(3, 4) << R.at<double>(0, 0), R.at<double>(0, 1), R.at<double>(0, 2), t.at<double>(0, 0), R.at<double>(1, 0), R.at<double>(1, 1), R.at<double>(1, 2), t.at<double>(1, 0), R.at<double>(2, 0), R.at<double>(2, 1), R.at<double>(2, 2), t.at<double>(2, 0));
	cout << "P1: " << P2 << endl;
	Mat P1 = (Mat_<double>(3, 4) << 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0);
	cout << "P0: " << P1 << endl;

	projMatrix1 = P1;
	projMatrix2 = P2;

	storage.addProjectionMatrix(P1);
	storage.addProjectionMatrix(P2);
	//double d2 = get_time();
	//cout << "recobverPPose time: " << d2 - d1 << endl;
}

void MultipleViews::triangulateAndAdd(Mat &projMatrix1, Mat &projMatrix2, vector<Point2f> &points1, vector<Point2f> &points2, vector<int> &p1, vector<int> &p2){
	Mat worldPoints3D, worldPoints4D;
	double d1 = get_time();
	triangulatePoints(cameraMatrix*projMatrix1, cameraMatrix*projMatrix2, points1, points2, worldPoints4D);
	convertPointsFromHomogeneous(worldPoints4D.t(), worldPoints3D);
	double d2 = get_time();

	//Add points to cloud, convert and store which frame 
	//cout << "matchmatrix size: " << storage.matchMatrix.size()<< endl;
	createCloudPoints(worldPoints3D, p1, p2);
	cout << "triangulate time: " << d2 - d1 << endl;
}

void MultipleViews::createCloudPoints(Mat worldPoints3D, vector<int> &p1, vector<int> &p2){
	for (unsigned int pointCloudPt = 0; pointCloudPt < storage.pointCloud.size(); pointCloudPt++){
		storage.pointCloud[pointCloudPt].indexOf2DOrigin.resize(currentView+2, -1);
	}
	CloudPoint p;
	for (int r = 0; r < worldPoints3D.rows; r++){
		p.indexOf2DOrigin.clear();
		p.pt = Point3f(worldPoints3D.at<float>(r, 0), worldPoints3D.at<float>(r, 1), worldPoints3D.at<float>(r, 2));
		if (currentView == 0){
			p.indexOf2DOrigin.push_back(p1[r]);
			p.indexOf2DOrigin.push_back(p2[r]);
			p.indexOf2DOrigin.push_back(-1);
			/*p.indexOf2DOrigin.insert(pair<int, int>(currentView, p1[r]));
			p.indexOf2DOrigin.insert(pair<int, int>(currentView + 1, p2[r]));*/
		}
		else{
			p.indexOf2DOrigin.resize(currentView + 2, -1);
			p.indexOf2DOrigin[currentView-1] = p1[r];
			p.indexOf2DOrigin[currentView] = p2[r];


			//p.indexOf2DOrigin.insert(pair<int, int>(currentView-1, p1[r]));
			//p.indexOf2DOrigin.insert(pair<int, int>(currentView, p2[r]));
		}
		storage.addCloudPoints(p);

	}
	cout << "storage.addCloudPoints(p): " << storage.pointCloud[0].indexOf2DOrigin.size() << endl;

}

void MultipleViews::newView(Mat &image){
	currentView++;
	//cout << "current view (1): " << currentView << endl;
	//Check size of vector(s) first....
	vector<KeyPoint> prevKeyPoints = storage.allKeypoints.back();
	//Calc keypoints and add to storage
	vector<KeyPoint> keyPoints = detectFeatures(image);
	storage.addKeyPoints(keyPoints);
	//Calc descriptor
	Mat descriptors = describeFeatures(image, keyPoints);
	//Match with prev frame
	vector<DMatch> matches = matchAndClear(prevKeyPoints, keyPoints, storage.lastDescriptor, descriptors);
	//cout << "matches.size(): " << matches.size() << endl;
	vector<Point2f> points1, points2;
	vector<int> pointIdx1, pointIdx2;

	for (vector<DMatch>::const_iterator it = matches.begin();
		it != matches.end(); ++it) {
		points1.push_back(prevKeyPoints[it->queryIdx].pt);
		points2.push_back(keyPoints[it->trainIdx].pt);
		pointIdx1.push_back(it->queryIdx);
		pointIdx2.push_back(it->trainIdx);
	}

	//Add matches and new descriptor to storage. (Should not be hard coded) 
	storage.addMatches(currentView-1, currentView, matches);
	storage.setLastDescriptor(descriptors);
	vector<Point3f> localPC;
	vector<Point2f> localImagePts;
	get3D2DCorrespondances(localPC, localImagePts);
	if (localPC.size() > 0){
		recoverNewPose(localPC, localImagePts);
		triangulateAndAdd(storage.allProjectionMatrices[currentView-1], storage.allProjectionMatrices[currentView], points1, points2, pointIdx1, pointIdx2);
	}

	//Perform bundle adjustment. 
}
void MultipleViews::get3D2DCorrespondances(vector<Point3f> &localPC, vector<Point2f> &localImagePts){
	//double d1 = get_time();
	cout << "get3D2DCorrespondances" << endl;
	//cout << "matches.size: " << storage.matchMatrix.size() << endl;
	//cout << "keypoints.size: " << storage.allKeypoints.size() << endl;
	//cout << "cloudpoints.size: " << storage.pointCloud.size() << endl;
	cout << "Current View (3): " << currentView << endl;
	localPC.clear(); localImagePts.clear();
	cout << "get2d3d (before): " << storage.pointCloud[0].indexOf2DOrigin.size() << endl;
	int io = 0;
	vector<DMatch> currentMatches = storage.matchMatrix[make_pair(currentView-1,currentView)];
	for (unsigned int matchFromOld = 0; matchFromOld < currentMatches.size(); matchFromOld++){
		//get index in prev if contrib to pc
		int indexInPrevView = currentMatches[matchFromOld].queryIdx;

		for (unsigned int pointCloudPt = 0; pointCloudPt < storage.pointCloud.size(); pointCloudPt++){
			if (indexInPrevView == storage.pointCloud[pointCloudPt].indexOf2DOrigin[currentView - 1]){
				localPC.push_back(storage.pointCloud[pointCloudPt].pt);
				Point2f pt_ = storage.allKeypoints.back()[currentMatches[matchFromOld].trainIdx].pt;
				localImagePts.push_back(pt_);
				//storage.pointCloud[pointCloudPt].indexOf2DOrigin[currentView] = currentMatches[matchFromOld].queryIdx;
				io++;
				break;
			}			
		}
	}
	cout << "found " << localPC.size() << " 3d-2d point correspondences" << endl;
	cout << "get2d3d (after): " << storage.pointCloud[0].indexOf2DOrigin.size() << endl;
	cout << "io: " << io << endl;
	//double d2 = get_time();
	//cout << "2d3dcorr time: " << d2 - d1 << endl;

}

void MultipleViews::recoverNewPose(vector<Point3f> &localPC, vector<Point2f> &localImagePt){
	if (localPC.size() > 0){
		Mat t, rvec, R;
		double d1 = get_time();
		solvePnPRansac(localPC, localImagePt, cameraMatrix, distCoeffs, rvec, t, false);
		//get rotation in 3x3 matrix form
		Rodrigues(rvec, R);
		double d2 = get_time();
		Mat P = (Mat_<double>(3, 4) << R.at<double>(0, 0), R.at<double>(0, 1), R.at<double>(0, 2), t.at<double>(0, 0), R.at<double>(1, 0), R.at<double>(1, 1), R.at<double>(1, 2), t.at<double>(1, 0), R.at<double>(2, 0), R.at<double>(2, 1), R.at<double>(2, 2), t.at<double>(2, 0));
		cout << "P2: " << P << endl;
		//Store pose to storage vector
		Mat projMatrix1 = cameraMatrix*P;
		storage.addProjectionMatrix(P);
		cout << "recover new SolvePnp time: " << d2 - d1 << endl;

	}
	//double d2 = get_time();
}