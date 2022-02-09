//20201514 HyunSoo Lee
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <vector>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

using namespace cv;
using namespace std;
using namespace tesseract;

double  pythagoras(int  x1, int y1, int  x2, int y2);

int main() {

	// 1. Load image, Gray Scaling, Canny edge detection, Gaussian Blur
	Mat img = imread("hello.jpg");
	Mat gray;
	resize(img, img, Size((img.size().width) / 1.5, (img.size().height) / 1.5));
	cvtColor(img, gray, COLOR_BGR2GRAY);
	GaussianBlur(gray, gray, Size(5, 5), 0);
	Mat canny;
	Canny(gray, canny, 150, 10);
	//imshow("Canny", canny);

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
			copy(conpoly[i].begin(), conpoly[i].end(), back_inserter(square));
		}
		imshow("Img", img);
	}

	// 4. Warping (Size : Using pythagoras)
	double width = pythagoras((int)square[0].x, (int)square[0].y, (int)square[3].x, (int)square[3].y);
	double height = pythagoras((int)square[0].x, (int)square[0].y, (int)square[1].x, (int)square[1].y);

	Size warp_size((int) width, (int) height);
	Mat warp_img(warp_size, img.type());
	vector<Point2f> warp_square(4);

	warp_square[0] = Point(warp_img.cols, 0);
	warp_square[1] = Point(warp_img.cols, warp_img.rows);
	warp_square[2] = Point(0, warp_img.rows);
	warp_square[3] = Point(0, 0);	

	Mat trans = getPerspectiveTransform(square, warp_square);
	warpPerspective(img, warp_img, trans, warp_size);
	imshow("warping", warp_img);
	imwrite("warp_img.jpg", warp_img);
	waitKey();
	return 0;

	char* outText;
	TessBaseAPI* api = new TessBaseAPI();
	if (api->Init(NULL, "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}

	// 5. Open input image with leptonica library
	Pix* image = pixRead("/usr/src/tesseract/testing/phototest.tif");
	api->SetImage(image);

	// 6. Get OCR result
	outText = api->GetUTF8Text();
	printf("OCR output:\n%s", outText);

	// 7. Destroy used object and release memory
	api->End();
	delete api;
	delete[] outText;
	pixDestroy(&image);

	return 0;

}

double  pythagoras(int  x1, int y1, int  x2, int y2) {
	int a, b = 0;
	if (x1 < x2)
		a = (x2 - x1) ^ 2;
	else
		a = (x1 - x2) ^ 2;
	if (y1 < y2)
		b = (y2 - y1) ^ 2;
	else
		b = (y1 - y2) ^ 2;
	return (a + b) ^ 1/2;
}