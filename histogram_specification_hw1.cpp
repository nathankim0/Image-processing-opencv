/*
영상처리 과제 히스토그램 명세화
2019.10.03.목요일 
한국산업기술대학교 컴공 2015154007 김진엽
*/

#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include <Windows.h>
#include <string.h>

using namespace cv;
using namespace std;

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

void CalcHistogram(Mat image, Mat& histogram, int bins, int range_max = 256) {
	histogram = Mat(bins, 1, CV_32F, Scalar(0));
	float gap = (float)range_max / bins;
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			auto workPixel = image.at<uchar>(i, j);
			int idx = (int)(workPixel / gap);
			histogram.at<float>(idx)++;
		}
	}
}

void GetHistogramImage(Mat histogram, Mat& histogramImage, Size size = Size(256, 200)) {
	histogramImage = Mat(size, CV_8U, Scalar(255));
	float gap = (float)(histogramImage.cols / histogram.rows);
	normalize(histogram, histogram, 0, histogramImage.rows, NORM_MINMAX);
	for (int i = 0; i < histogram.rows; i++) {
		float sx = i * gap;
		float ex = (i + 1) * gap;
		Point2f pt_Ib(sx, 0), pt_rt(ex, histogram.at<float>(i));
		if (pt_rt.y > 0)
			rectangle(histogramImage, pt_Ib, pt_rt, Scalar(0), -1);
	}
	flip(histogramImage, histogramImage, 0);
}

void CalcNormalizedCumulativeFrequency(Mat histogram, Mat& histogramSum) {
	histogramSum = Mat(histogram.rows, 1, CV_32F, Scalar(0));
	//누적 합 
	float sum = 0;
	for (int i = 0; i < histogram.rows; i++) {
		sum += histogram.at<float>(i);
		histogramSum.at<float>(i) = sum;
	}
	//정규화 
	for (int i = 0; i < histogramSum.rows; i++) {
		histogramSum.at<float>(i) = histogramSum.at<float>(i) / sum * 255.0;
	}
}


void jinyeob_histogram_specificaion() {


	/////////////////////////////////////////////////////////////////
	//                                                             //
	//                                                             //
	// 명세화할 영상(입력영상) 불러오고 출력                       //
	//                                                             //
	//                                                             //
	/////////////////////////////////////////////////////////////////

	auto fileName = OpenFileDialog();
	Mat image = imread(fileName, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "파일 읽기 실패" << endl;
		return;
	}
	Mat histogram, histogramImage;
	CalcHistogram(image, histogram, 256);

	cout << histogram.t() << endl;
	GetHistogramImage(histogram, histogramImage);
	imshow("원본 영상", image);
	imshow("히스토그램", histogramImage);
	waitKey();

	/////////////////////////////////////////////////////////////////
	//                                                             //
	//                                                             //
	// 원하는 기준 영상 불러오고 출력                              //
	//                                                             //
	//                                                             //
	/////////////////////////////////////////////////////////////////

	auto fileName2 = OpenFileDialog();
	Mat image2 = imread(fileName2, IMREAD_GRAYSCALE);
	if (image2.empty()) {
		cout << "파일 읽기 실패" << endl;
		return;
	}
	Mat histogram2, histogramImage2;
	CalcHistogram(image2, histogram2, 256);

	cout << histogram2.t() << endl;
	GetHistogramImage(histogram2, histogramImage2);
	imshow("원본 영상", image2);
	imshow("히스토그램", histogramImage2);
	waitKey();

	/////////////////////////////////////////////////////////////////
	//                                                             //
	//                                                             //
	// 명세화할 영상(입력영상)의 평활화                            //
	//                                                             //
	//                                                             //
	/////////////////////////////////////////////////////////////////

	// 1단계 : 히스토그램 생성 
	Mat histogramSum;
	CalcHistogram(image, histogram, 256);

	// 2단계 : 누적 빈도 수 계산 
	// 3단계 : 누적 빈도 수 정규화 
	CalcNormalizedCumulativeFrequency(histogram, histogramSum);

	//정규화된 누적 빈도 수를 기반, 기존의 화소 변환
	auto tmp = Mat(image.size(), image.type());

	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			tmp.at <uchar>(i, j) = (uchar)histogramSum.at<float>(image.at<uchar>(i, j));
		}
	}

	/////////////////////////////////////////////////////////////////
	//                                                             //
	//                                                             //
	// 기준 영상 평활화                                            //
	//                                                             //
	//                                                             //
	/////////////////////////////////////////////////////////////////

	// 1단계 : 히스토그램 생성 
	Mat histogramSum2;
	CalcHistogram(image2, histogram2, 256);

	// 2단계 : 누적 빈도 수 계산 
	// 3단계 : 누적 빈도 수 정규화 
	CalcNormalizedCumulativeFrequency(histogram2, histogramSum2);

	//정규화된 누적 빈도 수를 기반, 기존의 화소 변환
	auto tmp2 = Mat(image2.size(), image2.type());
	for (int i = 0; i < image2.rows; i++) {
		for (int j = 0; j < image2.cols; j++) {
			tmp2.at <uchar>(i, j) = (uchar)histogramSum2.at<float>(image2.at<uchar>(i, j));
		}
	}

	Mat histogram_result;
	CalcHistogram(tmp2, histogram_result, 256);

	/////////////////////////////////////////////////////////////////
	//                                                             //
	//                                                             //
	// 	평활화된 기준영상 역평활화                                 //
	//                                                             //
	//                                                             //
	/////////////////////////////////////////////////////////////////
	
	/*
	정규화된 누적합 -> 명도
	명도 -> 역평활화값
	*/
	int two = 255;
	int one = 254;
	while (1) {
		for (int i = histogramSum2.at<float>(one); i <= histogramSum2.at<float>(two); i++) {
			histogram_result.at<float>(i) = two;
		}
		two = one;
		--one;
		if (one < 0)
			break;
	}

	//위에서 계산한 룩업테이블(역평활화값)을 통해, 평활화된 입력영상 화소 변환
	auto result = Mat(tmp.size(), tmp.type());

	for (int i = 0; i < tmp.rows; i++) {
		for (int j = 0; j < tmp.cols; j++) {
			result.at <uchar>(i, j) = (uchar)histogram_result.at <float>(tmp.at<uchar>(i, j));
		}
	}

	// 최종 히스토그램 계산 
	CalcHistogram(result, histogram_result, 256);
	GetHistogramImage(histogram_result, histogramImage);

	imshow("명세화 - 영상", result);
	imshow("명세화 - 히스토그램", histogramImage);
	waitKey();

}

int main()
{
	jinyeob_histogram_specificaion();
}