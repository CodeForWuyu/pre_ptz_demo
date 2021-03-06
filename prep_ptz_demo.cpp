// prep_ptz_demo.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

int detectMoveObject(String video_path);
void Draw_and_Calc(Mat frame, Mat detect_img);
Point getCoordinate(int img_center_x, int img_center_y, Rect rect);

int main()
{
	String VideoPath = "H://Project//prep_ptz_demo//videos//black.mp4";
	
	int i = detectMoveObject(VideoPath);
	cout << i << endl;
	getchar();
    return i;
}

/*
输入：视频路径
功能：利用三帧差法检测运动目标
*/
int detectMoveObject(String video_path) {
	VideoCapture capture(video_path);
	if (!capture.isOpened()) {
		cout << "打开视频失败！" << endl;
		return 0;
	}
	cout << "is it working?" << endl;
	Mat frame, preframe;
	Mat gray, pregray;
	Mat prediff,currentdiff;
	Mat bi_img, prebi_img;
	Mat and_img;
	int frame_count = 0;
	while (capture.read(frame)) {
		//imshow("ori", frame);
		cout << "processing............." << endl;
		cvtColor(frame, gray, COLOR_BGR2GRAY);
		
		if (frame_count == 0) {
			gray.copyTo(pregray);
		}
		if (frame_count == 1) {
			absdiff(gray, pregray,prediff);
			gray.copyTo(pregray);
		}
		if (frame_count > 1) {
			cout << "detect.............." << endl;
			absdiff(gray, pregray, currentdiff);
			gray.copyTo(pregray);
			//对两个帧差做阈值处理
			
			threshold(currentdiff, bi_img, 0, 255, THRESH_OTSU);
			threshold(prediff, prebi_img, 0, 255, THRESH_OTSU);
			
			bitwise_and(bi_img, prebi_img, and_img);
			//形态学处理
			Mat element = getStructuringElement(0, Size(3, 3));
			erode(and_img, and_img, element);
			dilate(and_img, and_img, element);

			gray.copyTo(pregray);
			currentdiff.copyTo(prediff);
			
			//画框
			Draw_and_Calc(frame, and_img);
		}
		frame_count++;
		
	}
	getchar();
	return 1;
}


/*
输入：一个三通道图像，一个对应的二值图像
功能：进行画框，以及绘制到图像中心的路径
*/
void Draw_and_Calc(Mat frame, Mat detect_img) {
	cout << "draw image!" << endl;
	if (!(frame.channels() == 3) || !(detect_img.channels() == 1)) {
		return;
	}
	Rect rect = boundingRect(detect_img);
	rectangle(frame, rect, Scalar(0, 0, 255), 2);
	int height = frame.rows;
	int width = frame.cols;

	//转换坐标
	//计算原点
	int img_center_x = width / 2;
	int img_center_y = height / 2;
	Point center = getCoordinate(img_center_x, img_center_y, rect);

	//绘制路径
	line(frame, Point(img_center_x, img_center_y), Point(img_center_x + center.x, img_center_y),
		Scalar(0,255,0),2);
	line(frame, Point(img_center_x + center.x, img_center_y),Point(img_center_x + center.x, img_center_y+center.y),
		Scalar(0, 255, 0), 2);

	imshow("result", frame);
	waitKey(20);
}


/*
输入：图像中心点坐标，一个rect
输出：rect中心点以图像中心为原点的坐标
*/
Point getCoordinate(int img_center_x, int img_center_y, Rect rect) {
	
	//计算rect中心点
	int rect_center_x = rect.x + rect.width / 2;
	int rect_center_y = rect.y + rect.height / 2;

	//计算新坐标系坐标
	int new_x = rect_center_x - img_center_x;
	int new_y = rect_center_y - img_center_y;
	return Point(new_x, new_y);

}
