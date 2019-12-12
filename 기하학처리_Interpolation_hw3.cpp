/*
2015154007_컴공_김진엽
영상처리 과제
19.11.07
*/

#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include <Windows.h>
#include <string.h>

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

//영상 이동
void Translation(Mat img, Mat& dst, Point pt) {
	Rect rect(Point(0, 0), img.size());
	dst = Mat(img.size(), img.type(), Scalar(0));

	//목적 영상 순회 - 역방향 사상
	for (int i = 0; i < dst.rows; i++) {
		for (int j = 0; j < dst.cols; j++) {
			Point dstPt(j, i);
			Point imgPt = dstPt - pt;

			//영역 내에 매칭되는 화소 복사
			if (rect.contains(imgPt))
				dst.at<uchar>(dstPt) = img.at<uchar>(imgPt);
		}
	}
}
//스케일링
void Scaling(Mat img, Mat& dst, Size size) {
	dst = Mat(size, CV_8U, Scalar(0));
	double ratioY = (double)size.height / img.rows;
	double ratioX = (double)size.width / img.cols;

	//목적 영상 순회 - 순방향 사상
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			int x = (int)(j * ratioX);
			int y = (int)(i * ratioY);
			dst.at<uchar>(y, x) = img.at<uchar>(i, j);
		}
	}
}

//최근접 이웃 보간법
void ScalingNearest(Mat img, Mat& dst, Size size) {
	dst = Mat(size, CV_8U, Scalar(0));
	double ratioY = (double)size.height / (img.rows - 1);
	double ratioX = (double)size.width / (img.cols - 1);

	//목적 영상 순회 - 역방향 사상
	for (int i = 0; i < dst.rows; i++) {
		for (int j = 0; j < dst.cols; j++) {
			int x = (int)cvRound(j / ratioX);
			int y = (int)cvRound(i / ratioY);
			dst.at<uchar>(i, j) = img.at<uchar>(y, x);
		}
	}
}

uchar BilinearValue(Mat img, double x, double y) {
	if (x >= img.cols - 1)x--;
	if (y >= img.rows - 1)y--;

	//4개 화소
	Point pt((int)x, (int)y);
	int lt = img.at<uchar>(pt),
		lb = img.at<uchar>(pt + Point(0, 1)),
		rt = img.at<uchar>(pt + Point(1, 0)),
		rb = img.at<uchar>(pt + Point(1, 1));

	//거리 비율
	double alpha = y - pt.y, //y축
		beta = x - pt.x;//x축
	
	int M1 = lt + (int)cvRound(alpha * (lb - lt));
	int M2 = rt + (int)cvRound(alpha * (rb - rt));
	int P = M1 + (int)cvRound(beta * (M2 - M1));

	//실수에서 uchar 형으로 변환
	return saturate_cast<uchar>(P);
}

void ScalingBilinear(Mat img, Mat& dst, Size size) {
	dst = Mat(size, img.type(), Scalar(0));
	double ratioY = (double)size.height / img.rows;
	double ratioX = (double)size.width / img.cols;

	//목적 영상 순회 - 역방향 사상
	for (int i = 0; i < dst.rows; i++) {
		for (int j = 0; j < dst.cols; j++) {
			double x = j / ratioX;
			double y = i / ratioY;

			dst.at<uchar>(i, j) = BilinearValue(img, x, y);
		}
	}
}

void Rotation(Mat img, Mat& dst, double dgree) {
	double radian = dgree / 180 * CV_PI;
	double sinVal = sin(radian);
	double cosVal = cos(radian);

	Rect rect(Point(0, 0), img.size());
	dst = Mat(img.size(), img.type(), Scalar(0));

	//목적 영상 순회 - 역방향 사상
	for (int i = 0; i < dst.rows; i++) {
		for (int j = 0; j < dst.cols; j++) {
			//회전 변환
			double x = j * cosVal + i * sinVal;
			double y = -j * sinVal + i * cosVal;

			if(rect.contains(Point2d(x,y)))
			dst.at<uchar>(i, j) = BilinearValue(img, x, y);
		}
	}
}
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
void run() {
	auto img = OpenImageDialog();

	Mat rotImg, transImg, scalingImg1, scalingImg2, scalingImg3, scalingImg4, scalingImg5;

//	Translation(img, transImg, Point(20, 20));
//	Scaling(img, scalingImg1, Size(1024, 1024));
//	Rotation(img, rotImg, 25);

	//스케일링, 기존영상 -> 4096x4096
	ScalingNearest(img, scalingImg2, Size(4096, 4096)); //최근접 
	ScalingBilinear(img, scalingImg3, Size(4096, 4096)); //양선형
	resize(img, scalingImg4, Size(4096, 4096), INTER_CUBIC); //바이큐빅
	resize(img, scalingImg5, Size(4096, 4096), INTER_LANCZOS4); //LANCZOS

	//일부분 자르기
	Rect rect(1024, 1024, 512, 512);
	Mat subImage2 = scalingImg2(rect);
	Mat subImage3 = scalingImg3(rect);
	Mat subImage4 = scalingImg4(rect);
	Mat subImage5 = scalingImg5(rect);

	//히스토그램 계산
	Mat histogram2, histogram3, histogram4, histogram5,
		histogramImage2, histogramImage3, histogramImage4, histogramImage5;
	CalcHistogram(subImage2, histogram2, 256);
	CalcHistogram(subImage3, histogram3, 256);
	CalcHistogram(subImage4, histogram4, 256);
	CalcHistogram(subImage5, histogram5, 256);

	GetHistogramImage(histogram2, histogramImage2);
	GetHistogramImage(histogram3, histogramImage3);
	GetHistogramImage(histogram4, histogramImage4);
	GetHistogramImage(histogram5, histogramImage5);

	//출력
//	imshow("이동", transImg);
//	imshow("회전", rotImg);
	imshow("최근접", subImage2);
	imshow("양선형", subImage3);
	imshow("바이큐빅", subImage4);
	imshow("LANCZOS", subImage5);

	imshow("최근접 히스토그램", histogramImage2);
	imshow("양선형 히스토그램", histogramImage3);
	imshow("바이큐빅 히스토그램", histogramImage4);
	imshow("LANCZOS 히스토그램", histogramImage5);

	waitKey();

}

int main() {
	run();
}