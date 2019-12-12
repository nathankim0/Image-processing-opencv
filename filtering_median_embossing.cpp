#include "opencv2/imgproc/imgproc.hpp"

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
Mat M(Mat image, int size) {
	Mat result = Mat(image.size(), CV_8U, Scalar(0));
	Size mSize(size, size);
	Point mSizeHalf = mSize / 2;

	Point minImageRange(mSizeHalf);
	Point maxImageRange(image.cols - mSizeHalf.x, image.rows - mSizeHalf.y);

	for (int y = minImageRange.y; y < maxImageRange.y; y++) {
		for (int x = minImageRange.x; x < maxImageRange.y; x++) {
			Point start = Point(x, y) - mSizeHalf;
			Rect roi(start, mSize);
			Mat mask;
			image(roi).copyTo(mask);
			Mat one_row = mask.reshape(1, 1);
			Mat sortedMask;
			cv::sort(one_row, sortedMask, SORT_EVERY_ROW);
			int medianIndex = (int)(one_row.total() / 2);
			result.at<uchar>(Point(x, y)) = sortedMask.at<uchar>(medianIndex);
		}
	}
	return result;
}
Mat AddImpulseNoise(Mat image) {
	Mat noiseImage = Mat(image.size(), CV_8U, Scalar(0));

	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			float rnd = rand() / 32767.0;
			if (rnd < 0.01) {
				noiseImage.at<uchar>(Point ( x, y)) = 255;
			}
			else {
				noiseImage.at<uchar>(Point(x, y)) = image.at<uchar>(Point(x, y));

			}
		}
	}
	return noiseImage;
}
Mat ConvolutionMask(Mat image, Mat mask) {
	Mat result = Mat(image.size(), CV_8U, Scalar(0));
	Size mSize(mask.size());
	Point mSizeHalf = mSize / 2;

	Point minImageRange(mSizeHalf);
	Point maxImageRange(image.cols - mSizeHalf.x, image.rows - mSizeHalf.y);

	for (int y = minImageRange.y; y < maxImageRange.y; y++) {
		for (int x = minImageRange.x; x < maxImageRange.y; x++) {
			Point start = Point(x, y) - mSizeHalf;
			float sum = 0;
			for (int my = 0; my < mask.rows; my++) {
				for (int mx = 0; mx < mask.cols; mx++) {
					float maskValue = mask.at<float>(Point(mx, my));
					float imagePixel = image.at<uchar>(Point(start.x + mx, start.y + my));
					sum += imagePixel * maskValue;
				}
			}
			sum += 128;
			if (sum > 255)
				sum = 255;
			else if (sum < 0)
				sum = 0;
			result.at<uchar>(Point(x, y)) = (uchar)sum;
		}
	}
	return result;
}
Mat embossing(Mat image) {
	float maskData[9] = {
		-1,0,0,
		0,0,0,
		0,0,1
	};
	Mat mask(3, 3, CV_32FC1, (void*)maskData);
	return ConvolutionMask(image, mask);
}
int main(int argc, char** argv)

{

	auto fileName = OpenFileDialog();
	Mat image = imread(fileName, IMREAD_GRAYSCALE);
	if (image.empty()) {
		cout << "파일 읽기 실패" << endl;
		return 0;
	}
	Mat nImage = AddImpulseNoise(image);
	Mat medianImage = M(nImage, 3);
	imshow("노이즈", nImage);
	imshow("미디안", medianImage);
	waitKey();
	
	Mat embImage = embossing(image);
	imshow("원본이미지", image);
	imshow("엠보싱", embImage);
	waitKey();
}