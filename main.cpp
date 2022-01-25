//20201514 HyunSoo Lee
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <vector>

using namespace cv;
using namespace std;

int main() {

	// 1. Load image, Gray Scaling, Canny edge detection, Gaussian Blur
	Mat img = imread("hello.jpg");
	Mat gray;
	resize(img, img, Size((img.size().width) / 1.5, (img.size().height) / 1.5));
	cvtColor(img, gray, COLOR_BGR2GRAY);
	GaussianBlur(gray, gray, Size(5, 5), 0);
	Mat canny;
	Canny(gray, canny, 150, 10);
	imshow("Canny", canny);

	// 2. Find Contours
	vector<vector<Point>> contours;
	//2 dimension array, number of Contours - location of The Contour
	vector<Vec4i> hierarchy;
	findContours(canny, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_TC89_L1);

	// 3. Draw Contour
	vector<vector<Point>> conpoly(contours.size());
	vector<Point2f> square;
	for (int i = 0; i < contours.size(); i++) {
		approxPolyDP(contours[i], conpoly[i], 1.95, true);
		if (conpoly[i].size() == 4) {
			drawContours(img, conpoly, i, Scalar(255 , 0 , 255), 2);
			//cout << "Contour #" << i << ": r.vertices=" << contours[i].size() << ", n.vertices=" << conpoly[i].size() << endl;
			//cout << "start point of Contour" << conpoly[i] << endl;
			copy(conpoly[i].begin(), conpoly[i].end(), back_inserter(square));
		}
		imshow("Img", img);
	}

	// 4. Warping - Size Fixed
	Size warp_size(400, 400);
	Mat warp_img(warp_size, img.type());
	vector<Point2f> warp_square(4);

	warp_square[0] = Point(warp_img.cols, 0);
	warp_square[1] = Point(warp_img.cols, warp_img.rows);
	warp_square[2] = Point(0, warp_img.rows);
	warp_square[3] = Point(0, 0);	

	Mat trans = getPerspectiveTransform(square, warp_square);
	warpPerspective(img, warp_img, trans, warp_size);
	imshow("warping", warp_img);

	waitKey();
	return 0;
}