#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <string.h>
#include <iostream>
using namespace std;
using namespace cv;
void ReadImageToString();

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

int main() {
	ReadImageToString();
	/*auto name = OpenFileDialog();
	if (name != "") {
		ImreadModes type[] = { IMREAD_GRAYSCALE,IMREAD_COLOR };
		for (int i = 0; i < 2; i++) {
			auto image = imread(name, type[i]);
			cout << "Depth = " << image.depth() << "\n";
			cout << "Channels Num = " << image.channels() << "\n";
			cout << "Width = " << image.cols << "\n";
			cout << "Height = " << image.rows << "\n";
			imshow("영상보기", image);
			waitKey(0);
		}
	}*/
}

void ReadImageToString() {
	auto name = OpenFileDialog();
	if (name != "") {
		auto image = imread(name, IMREAD_GRAYSCALE);

		Rect roi(0, 0, 32, 32);

		Mat roi_img = image(roi);

		cout << "[roi_img] = " << endl;

		for (int i = 0; i < roi_img.rows; i++) {
			for (int j = 0; j < roi_img.cols; j++) {
				cout.width(4);
				cout << (int)roi_img.at<uchar>(i, j);
			}
			cout << endl;
		}
		rectangle(image, roi, Scalar(255), 1);
		imshow("image", image);
		waitKey();
	}

}