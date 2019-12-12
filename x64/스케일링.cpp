#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <string.h>
#include <iostream>
using namespace std;
using namespace cv;

//file open
string OpenFileDialog() {
	char name[MAX_PATH] = { 0, };
	OPENFILENAMEA ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "모든파일(*.*)\0*.*\0";
	ofn.lpstrFile = name;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "";

	string strName;
	if (GetOpenFileNameA(&ofn)) {
		strName = name;
	}
	return strName;
}

//영상 화소 샘플링
Mat myScaling1(Mat image, Size size) {
	Mat ret(size, CV_8UC1, Scalar(0));
	double scaleX = (double)size.width / image.cols;
	double scaleY = (double)size.height / image.rows;

	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			int x = (int)(j * scaleX);
			int y = (int)(i * scaleY);
			ret.at<uchar>(y, x) = image.at<uchar>(i, j);
		}
	}
	return ret;
}

//변경될사이즈에 원래 영상으로부터 화소를 추출함
Mat myScaling2(Mat image, Size size) {
	Mat ret(size, CV_8UC1, Scalar(0));
	double scaleX = (double)size.width / image.cols;
	double scaleY = (double)size.height / image.rows;

	for (int i = 0; i < ret.rows; i++) {
		for (int j = 0; j < ret.cols; j++) {
			int x = (int)(j / scaleX);
			int y = (int)(i / scaleY);
			ret.at<uchar>(i, j) = image.at<uchar>(y, x);
		}
	}
	return ret;
}

//스케일링
void Scaling() {
	auto name = OpenFileDialog();
	if (name != "") {
		auto image = imread(name, IMREAD_GRAYSCALE);

		Mat m1, m2, m3, m4, m5;

		resize(image, m1, Size(256, 256), 0, 0);
		imshow("Down", m1);
		waitKey();

		resize(image, m2, Size(1024, 1024), 0, 0);
		imshow("Up", m2);
		waitKey();

		m3 = myScaling1(image, Size(256, 256));
		imshow("MyDownSampling", m3);
		waitKey();

		m4 = myScaling1(image, Size(1024, 1024));
		imshow("MyUpSampling1", m4);
		waitKey();

		m5 = myScaling2(image, Size(256, 256));
		imshow("MyUpSampling2", m5);
		waitKey();

	}
}

int main() {
	Scaling();
}