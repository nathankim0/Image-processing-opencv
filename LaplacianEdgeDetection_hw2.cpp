/*
영상처리 과제
2019.10.29 화요일
한국산업기술대학교 컴공 2015154007 김진엽
*/

#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include <Windows.h>
#include <string.h>

using namespace cv;
using namespace std;

string OpenFileDialog() { //파일 열기 다이얼로그
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
Mat OpenImageDialog()
{
	auto fileName = OpenFileDialog();
	Mat image = imread(fileName, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "파일 읽기 실패" << endl;
		exit(1);
	}
	return image;
}

Mat ConvolutionMask(Mat image, Mat mask) { 
	Mat result = Mat(image.size(), CV_32F, Scalar(0));
	Size mSize(mask.size());
	Point mSizeHalf = mSize / 2;

	//회선의 경계부분에 대해서 마스크가 완전하게 중첩되는 부분만 연산
	Point minImageRange(mSizeHalf);
	Point maxImageRange(image.cols - mSizeHalf.x, image.rows - mSizeHalf.y);
	for (int y = minImageRange.y; y < maxImageRange.y; y++) {
		for (int x = minImageRange.x; x < maxImageRange.x; x++) {
			//연산의 시작 지점
			Point start = Point(x, y) - mSizeHalf;

			//마스크 연산
			float sum = 0;
			for (int my = 0; my < mask.rows; my++) {
				for (int mx = 0; mx < mask.cols; mx++) {
					float maskValue = mask.at<float>(Point(mx, my));
					if (maskValue == 0)
						continue;
					float imagePixel = image.at<uchar>(Point(start.x + mx, start.y + my));
					sum += imagePixel * maskValue;
				}
			}
			result.at<float>(Point(x, y)) = sum;
		}
	}
	return result;
}
void LaplacianEdgeDetection(Mat image) { //라플라시안 
	float mask1[9] = { //4방향
		0, -1, 0,
	   -1,  4,-1,
	    0, -1, 0
	};
	float mask2[9] = { //8방향
		1, 1, 1,
		1, -8, 1,
		1, 1, 1
	};
	Mat maskV(3, 3, CV_32FC1, (void*)mask1), 
		maskH(3, 3, CV_32FC1, (void*)mask2);
	auto four = ConvolutionMask(image, maskV);
	auto eight = ConvolutionMask(image, maskH);

	Mat totalConvImage;
//	magnitude(four, eight, totalConvImage);

	four = abs(four);
	eight = abs(eight);

//	totalConvImage.convertTo(totalConvImage, CV_8U);
	four.convertTo(four, CV_8U);
	eight.convertTo(eight, CV_8U);

	imshow("원본", image);
	imshow("4방향 필터링", four);
	imshow("8방향 필터링", eight);
	waitKey();
}

void Run()
{
	Mat image = OpenImageDialog();
	LaplacianEdgeDetection(image); //라플라시안
}

int main() {
	Run();
}