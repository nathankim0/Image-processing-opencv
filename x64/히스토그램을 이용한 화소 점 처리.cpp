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

// params: image 
// 히스토그램을 확인할 영상 
// params: histogram 
// 계산되어 반환될 히스토그램 
// params : bins
//히스토그램의 x축 요소 수 
// params : range_max
//히스토그램의 축 최대 요소 수 
void CalcHistogram(Mat image, Mat& histogram, int bins, int range_max = 256) {
	//히스토그램 값을 저장할 Mat 인스턴스 생성, 0으로 초기화 
	histogram = Mat(bins, 1, CV_32F, Scalar(0));
	//x축 값 간격 
	float gap = (float)range_max / bins;
	//영상을 순회하면서 화소를 처리 
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			// var : workPixel 
			//처리할 화소 
			//auto : 컴파일 시점에서 컴파일러가 문맥을 보고 자동으로 변수 타입을 지정, 템플릿 같은 곳에서 사용 시 편리 
			auto workPixel = image.at<uchar>(i, j);
			//해당 화소가 들어갈 영역을 계산 
			int idx = (int)(workPixel / gap);
			//히스토그램에 영역에 값을 누적 
			histogram.at<float>(idx)++;
		}
	}
}

// params : histogram
//처리할 히스토그램 
// params : histogram Image
//반환될 히스토그램 영상 
// params : size 
// 히스토그램 영상의 크기 
void GetHistogramImage(Mat histogram, Mat& histogramImage, Size size = Size(256, 200)) {
	//255로 빈 히스토그램 영상을 생성 
	histogramImage = Mat(size, CV_8U, Scalar(255));
	//히스토그램 한 영역을 표현할 너비 
	float gap = (float)(histogramImage.cols / histogram.rows);
	// 히스토그램의 빈도를 최소가 0, 최대가 출력 영상의 높이값을 갖도록 조정 
	normalize(histogram, histogram, 0, histogramImage.rows, NORM_MINMAX);
	for (int i = 0; i < histogram.rows; i++) {
		//히스토그램 막대 사각형의 시작과 끝의 좌표 
		float sx = i * gap;
		float ex = (i + 1) * gap;
		//사각형의 좌측 하단좌표와 우측 상단 좌표 
		Point2f pt_Ib(sx, 0), pt_rt(ex, histogram.at<float>(i));
		//사각형이 높이 값을 가지면 사각형을 그림 
		if (pt_rt.y > 0)
			rectangle(histogramImage, pt_Ib, pt_rt, Scalar(0), -1);
	}
		// X축 기준으로 영상을 뒤집음 (영상에서 좌표계는 아래가 양의 값을 가지므로, 위가 양의 값을 갖는 좌표계로 변환) 
		flip(histogramImage, histogramImage, 0);
}

//실습 1 : 히스토그램 보기
void ShowHistogram() {
#pragma region 중복코드
	auto fileName = OpenFileDialog();
	//영상을 그레이스케일(8비트 = 256 흑백영상)로 읽어옵니다.
	Mat image = imread(fileName, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "파일 읽기 실패" << endl;
		return;
	}
#pragma endregion
	Mat histogram, histogramImage;
	//히스토그램 계산 
	CalcHistogram(image, histogram, 256);
	//내용 출력 
	cout << histogram.t() << endl;
	GetHistogramImage(histogram, histogramImage);
	imshow("실습 1 : 히스토그램 보기 - 원본 영상", image);
	imshow("실습 1 : 히스토그램 보기 - 히스토그램", histogramImage);
	waitKey();
}

int Clamping(int input) {
	if (input < 0)
		input = 0; 
	else if (input > 255)
		input = 255; 
	return input;
}

void HistogramTransform() {
#pragma region 중복코드
	auto fileName = OpenFileDialog();
	//영상을 그레이 스케일(8비트 = 256 흑백 영상)로 읽어 옵니다.
	Mat image = imread(fileName, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "파일 읽기 실패" << endl;
		return;
	}
#pragma endregion
	//화소값 제어 
	auto tmp = Mat(image.size(), image.type());
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			tmp.at<uchar>(i, j) = Clamping(image.at<uchar>(i, j) * 1.2);
		}
	}
	Mat histogram, histogramImage;
	//원본의 히스토그램 계산 
	CalcHistogram(image, histogram, 256);
	GetHistogramImage(histogram, histogramImage);
	imshow("실습 2 : 곱나눗셈 연산한 영상의 히스토그램 보기 - 원본", histogramImage);
	//처리된 영상의 히스토그램 계산 
	CalcHistogram(tmp, histogram, 256);
	GetHistogramImage(histogram, histogramImage);
	imshow("실습 2 : 곱 나눗셈 연산한 영상의 히스토그램 보기 - 1.2곱연산", histogramImage);
	waitKey();
}

void BasicContrastStretching(Mat image, Mat& stretchedImage, int low, int high) {
	//영상을 복사할 공간을 생성 
	stretchedImage = Mat(image.size(), CV_8U, Scalar(0));


	// 영상을 순회하면서 화소를 처리
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			uchar oldPixel = image.at<uchar>(i, j);
			uchar result;

			// 앤드 인 탐색 
			if (oldPixel <= low) 
				result = 0;
			
			else if (oldPixel < high) {
				// 기본 명암 대비 스트레칭 공색 수행
				result = (uchar)((double)((int)oldPixel - low) / (high - low) * 255);
			}
			else
				result = 255;
			//처리할 화소 
			stretchedImage.at<uchar>(i, j) = result;
		}
	}
}

void HistogramStretching() {
#pragma region 중복코드
	auto fileName = OpenFileDialog();
	//영상을 그레이스케일로 읽어옵니다.
	Mat image = imread(fileName, IMREAD_GRAYSCALE);

	if (image.empty()) {
		cout << "파일 읽기 실패" << endl;
		return;
	}
#pragma endregion

	Mat histogram, histogramImage;
	//원본의 히스토그램 계산 
	CalcHistogram(image, histogram, 256);
	GetHistogramImage(histogram, histogramImage);
	imshow("실습3: 히스토그램 스트레칭 - 원본 ", histogramImage);

	int low, high;
	cout << "최저값 : ";
	cin >> low;
	cout << endl << "최대 값 : ";
	cin >> high;
	cout << endl;

	Mat stretchedlmage;
	BasicContrastStretching(image, stretchedlmage, low, high);
	// 처리된 영상의 히스토그램 계산
	CalcHistogram(stretchedlmage, histogram, 256);
	GetHistogramImage(histogram, histogramImage);
	imshow("실습 3: 히스토그램 스트레칭- 기본명암대비 및 앤드인 탐색", histogramImage);
	imshow("실습 3: 히스토그램 스트레칭- 영상 ", stretchedlmage);
	waitKey();

}


void CalcNormalizedCumulativeFrequency(Mat histogram, Mat& histogramSum) {
	//히스토그램값을 저장할 Mat 인스턴스 생성, 이으로 초기화
	histogramSum = Mat(histogram.rows, 1, CV_32F, Scalar(0));

	// 히스토그램의 누적 합를 계산 
	float sum = 0;
	for (int i = 0; i < histogram.rows; i++) {
		sum += histogram.at<float>(i);
		histogramSum.at<float>(i) = sum;
	}

	// 정규화 
	for (int i = 0; i < histogramSum.rows; i++) {
		histogramSum.at<float>(i) = histogramSum.at<float>(i) / sum * 255.0;
	}
}
	


void HistogramEqualizing() {
#pragma region 중복코드
	auto fileName = OpenFileDialog();
	//영상을 그레이스케일(8비트 = 256 흑백영상)로 읽어 옵니다.
	Mat image = imread(fileName, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "파일 읽기 실패" << endl;
		return;
	}
#pragma endregion
	// 1단계 : 히스토그램 생성 
	Mat histogram, histogramSum;

	//2단계 : 누적 빈도 수 계산 
	CalcHistogram(image, histogram, 256);

	// 3단계 : 누적 빈도 수 정규화 
	CalcNormalizedCumulativeFrequency(histogram, histogramSum);

	//정규화된 누적 빈도 수를 기반으로 기존의 화소를 변환
	   // 화소값 제어
	auto tmp = Mat(image.size(), image.type());
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			tmp.at <uchar>(i, j) = (uchar)histogramSum.at<float>(image.at<uchar>(i, j));
		}
	}
	// 처리된 영상의 히스토그램 계산 
	Mat histogramImage;
	CalcHistogram(tmp, histogram, 256);
	GetHistogramImage(histogram, histogramImage);
	imshow(" 실습 4 : 히스토그램 평활화 - 히스토그램", histogramImage);
	imshow(" 실습 4 : 히스토그램 평활화 - 영상", tmp);
	waitKey();
}

void Run() {
	//ShowHistogram(); //히스토그램으로
	//HistogramTransform(); //변형
//	HistogramStretching(); //스트레칭
	HistogramEqualizing(); //평활화
}

int main() 
{
	Run();
}