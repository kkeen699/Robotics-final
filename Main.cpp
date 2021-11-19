include "stdafx.h"
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int _tmain(int argc, _TCHAR* argv[])
{	
	// Load Image
	// Mat src = imread("tower.png", CV_LOAD_IMAGE_COLOR);
	Mat src_gray = imread("tower.png", CV_LOAD_IMAGE_GRAYSCALE);
	imshow("Raw image", src_gray);

	// Pre-processing and Find Edges
	Mat edge;
	medianBlur(src_gray, src_gray, 7);
	Laplacian(src_gray, edge, CV_8U, 5);
	threshold(edge, edge, 100, 255, THRESH_BINARY);
	imshow("Laplacian", edge);

	// Find Contours
	Mat contoursImg(src_gray.rows, src_gray.cols, CV_8U, Scalar(255));
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(edge, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
	for(int i = 0; i < contours.size(); i++){
		drawContours(contoursImg, contours, i, Scalar(0), 1, 4, hierarchy, 0);
	}
	imshow("result", contoursImg);
	vector<int> quality;
	quality.push_back(CV_IMWRITE_JPEG_QUALITY);
	quality.push_back(95);
	imwrite("./picture/tower_contours.jpg", contoursImg, quality);

	// Generate G Code for SCARA
	ofstream f1("x.txt");
	ofstream f2("y.txt");
	ofstream G("PP_420");
	*G << "G01 Z180 F25000" <<endl;
	*for(int i = 0; i < contours.size(); i++){
		cout<<i<<":"<<endl;
		for(int j = 0; j < contours[i].size(); j++){
			cout<<contours[i][j];
			f1 << contours[i][j].x/2 -180 << " ";
			f2 << contours[i][j].y/2*(-1) +570 << " ";
			if(j == 0){
				G << "G01 L1 M0 X" << contours[i][j].x/2-180 << " Y" <<contours[i][j].y*(-1)/2+570 << endl;
				G << "G01 Z170 F25000" <<endl;
			}
			else
				G << "G01 L1 M0 X" << contours[i][j].x/2-180 << " Y" <<contours[i][j].y*(-1)/2+570 << endl;
		}
		G << "G01 Z180 F25000" <<endl;
		drawContours(contoursImg, contours, i, Scalar(0), 1, 4, hierarchy, 0);
		cout<<endl;
	}
	G << "M99"<<endl;
	
	waitKey(0);
	return 0;
}