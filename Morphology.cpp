/*
영상처리 과제 그레이 레벨 영상에 대한 형태학 처리
2019.11.19.목요일
한국산업기술대학교 컴공 2015154007 김진엽
*/
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include <Windows.h>
#include <string.h>
#include <queue>
#include <vector>
#include <algorithm>
using namespace cv;
using namespace std;

Mat OpenImageDialog()
{
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

	Mat image = Mat();
	if (GetOpenFileNameA(&ofn)) {
		image = imread(name, IMREAD_GRAYSCALE);
		if (image.empty()) {
			cout << "파일 읽기 실패" << endl;
			exit(1);
		}
	}
	else {
		cout << "파일 지정 오류" << endl;
		exit(1);
	}
	return image;
}

//이진 침/팽용
bool CheckMatch(Mat img, Point start, Mat mask, int mode) {
	for (int u = 0; u < mask.rows; u++) {
		for (int v = 0; v < mask.cols; v++) {
			//순회할 좌표
			Point pt(v, u);
			
			//마스크 계수
			int m = mask.at<uchar>(pt);

			//계산에 사용될 화소
			int p = img.at<uchar>(start+pt);

			//계수와 화소가 일치하는지 비교
			bool ch = (p == 255);

			//mode=0:침식, mode=1: 팽창
			if (m == 1 && ch == mode) {
				return false;
			}
		}
		return true;
	}
}

//이진 침식
void Erosion(Mat img, Mat& dst, Mat mask) {
	dst = Mat(img.size(), CV_8U, Scalar(0));
	if (mask.empty()) {
		//기본 마스크
		mask = Mat(3, 3, CV_8UC1, Scalar(1));
	}
	Point maskHalf = mask.size() / 2;
	for (int i = maskHalf.y; i < img.rows - maskHalf.y; i++) {
		for (int j = maskHalf.x; j < img.cols - maskHalf.x; j++) {
			Point start = Point(j, i) - maskHalf;

				//형태소 일치 여부 비교
				bool check = CheckMatch(img, start, mask, 0);
				dst.at <uchar>(i, j) = (check) ? 255 : 0;
		}
	}
}

//이진 팽창
void Dilation(Mat img, Mat& dst, Mat mask)
{
	dst = Mat(img.size(), CV_8U, Scalar(0));

	if (mask.empty())
		mask = Mat(3, 3, CV_8UC1, Scalar(0));

	Point maskHalf = mask.size() / 2;
	for (int i = maskHalf.y; i < img.rows - maskHalf.y; i++) {
		for (int j = maskHalf.x; j < img.cols - maskHalf.x; j++) {
			Point start = Point(j, i) - maskHalf;

			//형태소 일치 여부 비교
			bool check = CheckMatch(img, start, mask, 1);
			dst.at<uchar>(i, j) = (check) ? 0:255;

		}
	}
}

//이진 열림
void Opening(Mat img, Mat& dst, Mat mask)
{
	Mat dst1;
	Erosion(img, dst1, mask);
	Dilation(dst1, dst, mask);
}

//이진 닫힘
void Closing(Mat img, Mat& dst, Mat mask)
{
	Mat dst1;
	Dilation(img, dst1, mask);
	Erosion(dst1, dst, mask);
}

//그레이 침식용
int mycheck(Mat img, Point start, Mat mask) {
	priority_queue<int, vector<int>, greater<int>> q; //오름차순, top에는 제일 작은값
	for (int u = 0; u < mask.rows; u++) {
		for (int v = 0; v < mask.cols; v++) {
			//순회할 좌표
			Point pt(v, u);

			//마스크 계수
			int m = mask.at<uchar>(pt);

			//계산에 사용될 화소
			int p = img.at<uchar>(start + pt);
			q.push(p - m);
		}
	}
	return q.top();
}

//그레이 팽창용
int mycheck2(Mat img, Point start, Mat mask) {
	priority_queue<int> q; //내림차순, top에는 제일 큰값
	for (int u = 0; u < mask.rows; u++) {
		for (int v = 0; v < mask.cols; v++) {
			//순회할 좌표
			Point pt(v, u);

			//마스크 계수
			int m = mask.at<uchar>(pt);

			//계산에 사용될 화소
			int p = img.at<uchar>(start + pt);
			q.push(m + p);
		}
	}
	return q.top();
}

//그레이 침식
void ErosionGray(Mat img, Mat& dst, Mat mask) {
	dst = Mat(img.size(), CV_8U, Scalar(0));
	if (mask.empty()) {
		//기본 마스크
		mask = Mat(3, 3, CV_8UC1, Scalar(1));
	}
	Point maskHalf = mask.size() / 2;
	for (int i = maskHalf.y; i < img.rows - maskHalf.y; i++) {
		for (int j = maskHalf.x; j < img.cols - maskHalf.x; j++) {
			Point start = Point(j, i) - maskHalf;

			int a = mycheck(img, start, mask);
			dst.at <uchar>(i, j) = a;
		}
	}
}

//그레이 팽창
void DilationGray(Mat img, Mat& dst, Mat mask)
{
	dst = Mat(img.size(), CV_8U, Scalar(0));

	if (mask.empty())
		mask = Mat(3, 3, CV_8UC1, Scalar(0));

	Point maskHalf = mask.size() / 2;
	for (int i = maskHalf.y; i < img.rows - maskHalf.y; i++) {
		for (int j = maskHalf.x; j < img.cols - maskHalf.x; j++) {
			Point start = Point(j, i) - maskHalf;

			//형태소 일치 여부 비교
			int a = mycheck2(img, start, mask);
			dst.at <uchar>(i, j) = a;
		}
	}
}

//그레이 열림
void GrayOpening(Mat img, Mat& dst, Mat mask)
{
	Mat dst1;
	ErosionGray(img, dst1, mask);
	DilationGray(dst1, dst, mask);
}

//그레이 닫힘
void GrayClosing(Mat img, Mat& dst, Mat mask)
{
	Mat dst1;
	DilationGray(img, dst1, mask);
	ErosionGray(dst1, dst, mask);
}

void Run()
{
	Mat img = OpenImageDialog();
	Mat bImg, dst1, dst2, dst3, dst4, dst5, dst6, dst7;

	//// 영상 이진화
	//threshold(img, bImg, 128, 255, THRESH_BINARY);

	//uchar data1[] = {
	//	0, 1, 0,
	//	1, 1, 1,
	//	0, 1, 0
	//};
	//uchar data2[] = {
	//	1, 0, 1,
	//	0, 1, 0,
	//	1, 0, 1
	//};

	//Mat mask1(3, 3, CV_8UC1, data1);
	//Mat mask2(3, 3, CV_8UC1, data2);


	//// 사용자정의 침식
	//Erosion(bImg, dst1, (Mat)mask1);
	//// OPEN_CV 침식
	//morphologyEx(bImg, dst2, MORPH_ERODE, mask1);

	//// 사용자정의 팽창
	//Dilation(bImg, dst3, (Mat)mask2);
	//// OPEN_CV 팽창
	//morphologyEx(bImg, dst4, MORPH_DILATE, mask2);

	//// 열림
	//Opening(bImg, dst5, (Mat)mask1);

	//// 닫힘
	//Closing(bImg, dst6, (Mat)mask1);

	//imshow("원본", img);
	//imshow("이진 영상", bImg);
	//imshow("침식 (사용자)", dst1);
	//imshow("침식 (라이브러리)", dst2);
	//imshow("팽창 (사용자)", dst3);
	//imshow("팽창 (라이브러리)", dst4);
	//imshow("열림", dst5);
	//imshow("닫힘", dst6);

	//waitKey();

	///////////////////////////////////////////
	// 여기서부터는 과제용 테스트 코드입니다.//
	///////////////////////////////////////////

	// 과제용 마스크
	uchar data3[] = {
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	};
	Mat mask3(3, 3, CV_8UC1, data3);


	ErosionGray(img, dst1, (Mat)mask3);
	DilationGray(img, dst2, (Mat)mask3);

	GrayOpening(img, dst5, (Mat)mask3);
	GrayClosing(img, dst6, (Mat)mask3);

	imshow("원본 (김진엽)", img);
	imshow("침식 (김진엽)", dst1);
	imshow("팽창 (김진엽)", dst2);
	imshow("열림 (김진엽)", dst5);
	imshow("닫힘 (김진엽)", dst6);
	waitKey();
}
int main() {
	Run();
}