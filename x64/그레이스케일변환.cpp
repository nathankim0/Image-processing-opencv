
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>

using namespace std;
using namespace cv;

/*
* @brief - grayscale 영상으로 변경하는 메소드
*/

void showGrayScaleImage(Mat img) {

	Mat gray;
	cvtColor(img, gray, COLOR_BGR2GRAY);
	imshow("Gray", gray);
}

int main() {

	Mat img = imread("image.png", IMREAD_COLOR);

	if (img.empty()) {
		cout << "이미지가 없거나, 유효한 파일 형식이 아닙니다." << endl;
		return -1;
	}

	imshow("원본 이미지", img);

	showGrayScaleImage(img);

	waitKey(0);

	return 0;
}