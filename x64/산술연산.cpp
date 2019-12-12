#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <string.h>
#include <iostream>
using namespace std;
using namespace cv;

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

typedef struct {
	string title;
	Mat image;

}TrackBar;

int Warping(int input) {
	return input %= 256;
}
int Clamping(int input) {
	if (input < 0)
		input = 0;
	else if (input > 255)
		input = 255;
	return input;
}

void BrightnessControl(string file, bool isWarpping) {
	auto image = imread(file, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "실패" << endl;
		return;
	}
	string title;
	if (!isWarpping) {
		title = "클램핑";
	}
	else {
		title = "워핑";
	}
	namedWindow(title, WINDOW_AUTOSIZE);
	int value = 128;
	TrackBar args{ title = title, image = image };
	createTrackbar(title + "밝기", title, &value, 255, [](int value, void* data) {
		auto args = (TrackBar*)data;
		string title = args->title;
		Mat image = args->image;
		auto tmp = image.clone();

		int change = value - 128;
		if (title.compare("클램핑")) {
			for (int i = 0; i < image.rows; i++) {
				for (int j = 0; j < image.cols; j++) {
					tmp.at<uchar>(i, j) = Clamping(image.at<uchar>(i, j) + change);
				}

			}
		}
		else {
			for (int i = 0; i < image.rows; i++) {
				for (int j = 0; j < image.cols; j++) {
					tmp.at<uchar>(i, j) = Warping(image.at<uchar>(i, j) + change);
				}

			}
		}
		imshow(title, tmp);
		}, &args);
	imshow(title, image);
	waitKey();
}
void DifferentialComputation(std::string file, std::string file2) {
	auto image1 = imread(file, IMREAD_GRAYSCALE); auto image2 = imread(file2, IMREAD_GRAYSCALE);
	if (image1.empty() || image2.empty()) {
		cout << "Different ia ComputationO:파일읽기실패" << endl;
		return;
	}
	if (image1.size() != image2.size()) {
		cout << "Different ia Computation():두 영상의 크기가 다름" << endl;
		return;
	}

	auto image = Mat(image1.size(), image1.type());
	for (int i = 0; i < image1.rows; i++) {
		for (int j = 0; j < image1.cols; j++) {
			image.at<uchar>(i, j) = Clamping(image1.at<uchar>(i, j) - image2.at<uchar>(i, j));
		}
	}
	imshow("차연산", image);
	waitKey();
}
void MultiplicationComputation(std::string file, double mul) {
	auto image = imread(file, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "MultiplicationComput 실패" << endl;
			return;
	}

	auto tmp = Mat(image.size(), image.type());
		for (int i = 0; i < image.rows; i++) {
			for (int j = 0; j < image.cols; j++) {
				tmp.at<uchar>(i, j) = Clamping(image.at<uchar>(i, j) * mul);
			}
		}
	imshow("곱나눗셈연산", tmp);
	waitKey();
}
void AndComputation(std::string file1, std::string file2) {
	auto image1 = imread(file1, IMREAD_GRAYSCALE);
	auto image2 = imread(file2, IMREAD_GRAYSCALE);
	if (image1.empty() || image2.empty()) {
		cout << "AndComputation() : 실패" << endl;
		return;
	}
	if (image1.size() != image2.size()) {
		cout << "AndComputation(): 두개 다름" << endl;
		return;
	}
	auto image = Mat(image1.size(), image1.type());
	for (int i = 0; i < image1.rows; i++) {
		for (int j = 0; j < image1.cols; j++) {
			image.at<uchar>(i, j) = image1.at<uchar>(i, j) & image2.at <uchar>(i, j);
		}
	}
	imshow("AND 연산", image);
	waitKey();

}
void OrComputation(std::string file1, std::string file2) {
	auto image1 = imread(file1, IMREAD_GRAYSCALE);
	auto image2 = imread(file2, IMREAD_GRAYSCALE);
	if (image1.empty() || image2.empty()) {
		cout << "OrComputation() : 실패" << endl;
		return;
	}
	if (image1.size() != image2.size()) {
		cout << "OrComputation(): 두개 다름" << endl;
		return;
	}
	auto image = Mat(image1.size(), image1.type());
	for (int i = 0; i < image1.rows; i++) {
		for (int j = 0; j < image1.cols; j++) {
			image.at<uchar>(i, j) = image1.at<uchar>(i, j) | image2.at<uchar>(i, j);
		}
	}
	imshow("OR 연산", image);
	waitKey();
}
void XorComputation(std::string file, int con) {
	auto image = imread(file, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "XorComputation() : 실패" << endl;
		return;
	}

	auto tmp = Mat(image.size(), image.type());
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			tmp.at<uchar>(i, j) = image.at<uchar>(i, j) ^ (uchar)con;
		}
	}
	imshow("XOR 연산", tmp);
	waitKey();
}
void NotComputation(std::string file) {
	auto image = imread(file, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "NotComputation() : 실패" << endl;
		return;
	}

	auto tmp = Mat(image.size(), image.type());
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			tmp.at<uchar>(i, j) = ~image.at<uchar>(i, j);
		}
	}
	imshow("NOT 연산", tmp);
	waitKey();
}
void InvertedConversion(std::string file) {
	auto image = imread(file, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "InvertedComputation() : 실패" << endl;
		return;
	}

	auto tmp = Mat(image.size(), image.type());
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			tmp.at<uchar>(i, j) = 255-image.at<uchar>(i, j);
		}
	}
	imshow("반전변환", tmp);
	waitKey();
}
void CalibrateGamma(std::string file,double gamma) {
	auto image = imread(file, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "CalibrateGamma() : 실패" << endl;
		return;
	}

	auto tmp = Mat(image.size(), image.type());
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			tmp.at<uchar>(i, j) = Clamping(pow(image.at<uchar>(i, j), 1.0 / gamma));

		}
	}
	imshow("감마보정", tmp);
	waitKey();
}
void Posterizing(std::string file,  int n) {
	auto image = imread(file, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "Posterizing() : 실패" << endl;
		return;
	}
	if (n<2) {
		cout << "Posterizing() : 경계값 최소(2)조건 실패" << endl;
		return;
	}
	double step = 256.0 / n;
	auto tmp = Mat(image.size(), image.type());
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			tmp.at<uchar>(i, j) = ((uchar)(image.at<uchar> (i, j)/ step)) * step;
		}
	}
	imshow("감마보정", tmp);
	waitKey();
}

void Run() {
	auto fileName = OpenFileDialog();
	//클램핑 
	BrightnessControl(fileName, 0);
	BrightnessControl(fileName, 1);
	auto differentialComputationFileName1 = OpenFileDialog();
	auto differentialComputationFileName2 = OpenFileDialog();
	//차 연산 
	DifferentialComputation(differentialComputationFileName1, differentialComputationFileName2);
	//곱 연산
	MultiplicationComputation(fileName, 1.5);
	//나눗셈 연산 
	MultiplicationComputation(fileName, 0.66);
	auto andComputationImageFileName = OpenFileDialog();
	auto andComputationMaskFileName = OpenFileDialog();
	// AND
	AndComputation(andComputationImageFileName, andComputationMaskFileName);
	// OR
	OrComputation(andComputationImageFileName, andComputationMaskFileName);
	// XOR
	XorComputation(andComputationImageFileName, 128);
	//NOT 연산 
	NotComputation(andComputationImageFileName);
	//반전 변환 
	InvertedConversion(andComputationImageFileName);
	//감마 보정
	CalibrateGamma(andComputationImageFileName, 0.8);
	CalibrateGamma(andComputationImageFileName, 1.2);
	//포스터 라이징 
	Posterizing(andComputationImageFileName, 2);
	Posterizing(andComputationImageFileName, 8);
}

int main() {
	Run();
}