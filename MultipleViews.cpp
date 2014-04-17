#include "MultipleViews.h"

vector<KeyPoint> MultipleViews::detectFeatures(Mat &image){

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
	Mat descriptors;
	extractor->compute(image, keyPoints, descriptors);
	cout << "descriptor1 matrix size: " << descriptors.rows << " by " << descriptors.cols << endl;
	return descriptors;
}

int MultipleViews::ratioTest(vector<vector<DMatch>>& matches) {
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
	return removed;
}
// Insert symmetrical matches in symMatches vector
void MultipleViews::symmetryTest(const std::vector<vector<DMatch>>& matches1,
	const vector<vector<DMatch>>& matches2,
	std::vector<cv::DMatch>& symMatches) {

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
}

void MultipleViews::ransacTest(const vector<DMatch>& matches,
	const vector<KeyPoint>& keypoints1,
	const vector<KeyPoint>& keypoints2,
	std::vector<DMatch>& outMatches) {

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
		inliers,      // match status (inlier ou outlier)  
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
}
vector<DMatch> MultipleViews::matchAndClear(vector<KeyPoint> &keyPoints1, vector<KeyPoint> &keyPoints2, Mat &descriptors1, Mat &descriptors2){
	BFMatcher matcher(NORM_L2);
	vector<vector<DMatch> > matches12, matches21;
	matcher.clear();

	//Matches from image1 -> image2 2NN
	matcher.knnMatch(descriptors1, descriptors2, matches12, 2);
	//Matches from image2 -> image1 2NN
	matcher.knnMatch(descriptors2, descriptors1, matches21, 2);
	cout << "Number of knnMatch (1): " << matches12.size() << endl;
	cout << "Number of knnMatch (2): " << matches21.size() << endl;
	/*Mat matchImg1, matchImg2;
	drawMatches(image1, keyPoints1, image2, keyPoints2, matches12, matchImg1, Scalar::all(-1));
	drawMatches(image2, keyPoints2, image1, keyPoints1, matches21, matchImg2, Scalar::all(-1));
	*/
	//Remove with ratio test. If match to similar disgard. 
	//int removed = ratioTest(matches12);
	//cout << "Number of matched points 1->2 (ratio test) : " << matches12.size() - removed << endl;
	int removed = ratioTest(matches21);
	cout << "Number of matched points 2->1 (ratio test) : " << matches21.size() - removed << endl;
	//Mat matchImg1, matchImg2;
	//drawMatches(image1, keyPoints1, image2, keyPoints2, matches12, matchImg1, Scalar::all(-1));
	//drawMatches(image2, keyPoints2, image1, keyPoints1, matches21, matchImg2, Scalar::all(-1));
	//imshow("Matches 1->2", matchImg1);
	//imshow("Matches 2->1", matchImg2);
	//imwrite("postratioImg1.jpg", matchImg1);
	//imwrite("postratioImg2.jpg", matchImg2);

	//Symmetry test
	vector<DMatch> symMatches;
	symmetryTest(matches12, matches21, symMatches);

	std::cout << "Number of matched points (symmetry test): " << symMatches.size() << std::endl;

	//Use ransac to remove outliers, check with epipolar line
	//Show features
	/*Mat matchImg3;
	drawMatches(image1, keyPoints1, image2, keyPoints2, symMatches, matchImg3, Scalar::all(-1));
	imshow("symMatches", matchImg3);*/
	vector<DMatch> ransacMatch;
	ransacTest(symMatches, keyPoints1, keyPoints2, ransacMatch);

	return ransacMatch;

}