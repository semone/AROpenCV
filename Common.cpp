#include "Common.h"
#include "FeatureMatcher.h"


Mat undistortImage(Mat image, Mat cameraMatrix, Mat distCoeffs){
	Mat undistorted, map1, map2;
	Mat r = Mat::eye(3, 3, CV_64F);
	Mat newCameraMatrix = getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, image.size(), 1, image.size(), 0);
	initUndistortRectifyMap(cameraMatrix, distCoeffs, r, newCameraMatrix, image.size(), CV_16SC2, map1, map2);
	remap(image, undistorted, map1, map2, INTER_LINEAR);
	return undistorted;
}

void readCalibration(Mat &cameraMatrix, Mat &distCoeffs){
	FileStorage xmlread("out_camera_data.xml", FileStorage::READ);
	xmlread["Camera_Matrix"] >> cameraMatrix;
	xmlread["Distortion_Coefficients"] >> distCoeffs;
	xmlread.release();
}

//bool captureFrame(){
//	
//	VideoCapture *capture = new VideoCapture(0);
//	capture->set(CV_CAP_PROP_FRAME_WIDTH, 1280);
//	capture->set(CV_CAP_PROP_FRAME_HEIGHT, 720);
//
//	//return false if camera can't be opened
//	if (!capture->isOpened())
//	{
//		std::cout << "cannot open the video cam" << std::endl;
//		return false;
//	}
//
//	double dwidth = capture->get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
//	double dheight = capture->get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
//	std::cout << "success! frame size : " << dwidth << " x " << dheight << std::endl;
//	Mat frame, undistort1, undistort2, keyImage;
//	int successes = 0; 
//	String imagePath;
//
//	Mat cameraMatrix, distortCoeffs;
//	readCalibration(cameraMatrix, distortCoeffs);
//	FeatureMatcher f;
//
//	while (1){
//		if (!capture->read(frame))
//		{
//			std::cout << "cannot read a frame from video stream" << std::endl;
//			break;
//		}
//		undistort1 = undistortImage(frame, cameraMatrix, distortCoeffs);
//		keyImage = f.detectKeyPoints(undistort1);
//		imshow("window1", keyImage);
//
//		int key = waitKey(30);
//		
//		if (key == 27) //wait for 'esc' key press for 30ms. if 'esc' key is pressed, break loop
//		{
//			std::cout << "esc key pressed, exit!" << std::endl;
//			break;
//		}
//
//		if (key == 13 && successes < 5) {
//			imagePath = "images/reconstruction/image" + std::to_string(successes) + ".jpg ";
//			imwrite(imagePath, frame);
//		
//			std::cout << "Save image" << std::endl;
//			successes++;
//		}
//		
//	}
//	capture->release();
//	return false;
//}