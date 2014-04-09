#include <algorithm>
#include <opencv2/opencv.hpp>
#include "five-point-nister\five-point.hpp"

using namespace std;
using namespace cv;
void recoverProjectionMatrix(Mat cameraMatrix, vector<DMatch> &matches, vector<Point2f> &points1, vector<Point2f> &points2, Mat &projMatrix1, Mat &projMatrix2);
