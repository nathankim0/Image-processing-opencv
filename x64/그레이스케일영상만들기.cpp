#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <string.h>
#include <iostream>
using namespace std;
using namespace cv;

void CreateGrayScaleImage();

int main() {
	CreateGrayScaleImage();
}
void CreateGrayScaleImage() {
	Mat im1(32, 256, CV_8UC1, Scalar(0));
	for (int i = 0; i < im1.rows; i++) {
		for (int j = 0; j < im1.cols; j++) {
			im1.at<uchar>(i, j) = j;
		}
	}
	imshow("im1", im1);
	waitKey();
}