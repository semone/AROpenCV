#include "PoseEstimation.h"


void recoverProjectionMatrix(Mat cameraMatrix, vector<DMatch> &matches, vector<Point2f> &points1, vector<Point2f> &points2, Mat &projMatrix1, Mat &projMatrix2){
	//vector<Point2f> points1, points2; 

	//for (vector<DMatch>::const_iterator it = matches.begin();
	//	it != matches.end(); ++it) {
	//	points1.push_back(keyPoints1[it->queryIdx].pt);
	//	points2.push_back(keyPoints2[it->trainIdx].pt);
	//}

	cout << "pp.x: " << cameraMatrix.at<double>(0, 2) << endl;
	cout << "pp.y: " << cameraMatrix.at<double>(1, 2) << endl;

	cout << "focalx: " << cameraMatrix.at<double>(0, 0) << endl;
	cout << "focaly: " << cameraMatrix.at<double>(1, 1) << endl;

	Point2d pp = Point2d(cameraMatrix.at<double>(0, 2), cameraMatrix.at<double>(1, 2));
	double focal = cameraMatrix.at<double>(0, 0);

	Mat E = findEssentialMat(points1, points2, focal, pp, CV_RANSAC, 0.99, 1.0, noArray());

	std::cout << "=====================================================" << std::endl;
	//Mat R1_5pt, R2_5pt, tvec_5pt, rvec1_5pt, rvec2_5pt;
	//decomposeEssentialMat(E, R1_5pt, R2_5pt, tvec_5pt);
	//Rodrigues(R1_5pt, rvec1_5pt);
	//Rodrigues(R2_5pt, rvec2_5pt);
	//std::cout << "5-pt-nister rvec: " << std::endl;
	//std::cout << rvec1_5pt << std::endl;
	//std::cout << rvec2_5pt << std::endl;
	//std::cout << "5-pt-nister tvec: " << std::endl;
	//std::cout << tvec_5pt << std::endl;
	//std::cout << -tvec_5pt << std::endl;

	Mat R, t;
	recoverPose(E, points1, points2,R, t, focal, pp, noArray() );

	//Mat Rpose;
	//Rodrigues(R, Rpose);
	/*std::cout << "recoverPose: " << std::endl;
	std::cout << "R: " << R << std::endl;
	std::cout << "t: "<< t << std::endl;*/
	Mat P2 = (Mat_<double>(3, 4) << R.at<double>(0, 0), R.at<double>(0, 1), R.at<double>(0, 2), t.at<double>(0, 0), R.at<double>(1, 0), R.at<double>(1, 1), R.at<double>(1, 2), t.at<double>(1, 0), R.at<double>(2, 0), R.at<double>(2, 1), R.at<double>(2, 2), t.at<double>(2, 0));
	cout << "P2: "<< P2 << endl;
	Mat P1 = (Mat_<double>(3, 4) << 1, 0, 0 ,0, 0, 1, 0, 0,0,0, 1, 0);
	cout << "P1: " << P1 << endl;

	projMatrix1 = cameraMatrix*P1;
	projMatrix2 = cameraMatrix*P2;
}


//int main()
//{
//	double N = 50;
//	double bound_2d = 5;
//
//	double focal = 300;
//	Point2d pp(0, 0);
//
//	Mat rvec = (cv::Mat_<double>(3, 1) << 0.1, 0.2, 0.3);
//	Mat tvec = (cv::Mat_<double>(3, 1) << 0.4, 0.5, 0.6);
//	normalize(tvec, tvec);
//	std::cout << "Expected rvec: " << rvec << std::endl;
//	std::cout << "Expected tvec: " << tvec << std::endl;
//
//
//	Mat rmat;
//	Rodrigues(rvec, rmat);
//
//	Mat K = (Mat_<double>(3, 3) << focal, 0, pp.x, 0, focal, pp.y, 0, 0, 1);
//
//	RNG rng;
//	Mat Xs(N, 3, CV_64F);
//	rng.fill(Xs, RNG::UNIFORM, -bound_2d, bound_2d);
//
//	Mat x1s = K * Xs.t();
//	Mat x2s = rmat * Xs.t();
//	for (int j = 0; j < x2s.cols; j++) x2s.col(j) += tvec;
//	x2s = K * x2s;
//
//	x1s.row(0) /= x1s.row(2);
//	x1s.row(1) /= x1s.row(2);
//	x1s.row(2) /= x1s.row(2);
//
//	x2s.row(0) /= x2s.row(2);
//	x2s.row(1) /= x2s.row(2);
//	x2s.row(2) /= x2s.row(2);
//
//	x1s = x1s.t();
//	x2s = x2s.t();
//
//	x1s = x1s.colRange(0, 2) * 1.0;
//	x2s = x2s.colRange(0, 2) * 1.0;
//	Mat E = findEssentialMat(x1s, x2s, focal, pp, CV_RANSAC, 0.99, 1, noArray());
//	std::cout << "=====================================================" << std::endl;
//	Mat R1_5pt, R2_5pt, tvec_5pt, rvec1_5pt, rvec2_5pt;
//	decomposeEssentialMat(E, R1_5pt, R2_5pt, tvec_5pt);
//	Rodrigues(R1_5pt, rvec1_5pt);
//	Rodrigues(R2_5pt, rvec2_5pt);
//	std::cout << "5-pt-nister rvec: " << std::endl;
//	std::cout << rvec1_5pt << std::endl;
//	std::cout << rvec2_5pt << std::endl;
//	std::cout << "5-pt-nister tvec: " << std::endl;
//	std::cout << tvec_5pt << std::endl;
//	std::cout << -tvec_5pt << std::endl;
//	Mat R, t;
//	recoverPose(E, x1s, x2s,R, t, focal, pp, noArray() );
//
//	Mat Rpose;
//	Rodrigues(R, Rpose);
//	std::cout << "recoverPose: " << std::endl;
//	std::cout << Rpose << std::endl;
//	std::cout << t << std::endl;
//}
