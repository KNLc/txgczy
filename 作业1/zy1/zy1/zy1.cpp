
#include <opencv2\opencv.hpp>  
using namespace cv;  

void DrawPolygon(Mat img);
int main( )  
{  

	
	
	Mat frame_ref;//上一帧
	Mat frame_curr;//当前帧
	Mat raw_sub;//原始帧与相减帧对比
	Mat Polygon;//一个填充多边形
	Mat curr_and_Polygon;//与操作

	namedWindow("raw_sub", WINDOW_AUTOSIZE);
	namedWindow("curr_and_Polygon", WINDOW_AUTOSIZE);
	startWindowThread();
	//读入视频
	VideoCapture capture("video.avi");
	capture >> frame_curr;//读取第一帧
	raw_sub = Mat( frame_curr.rows * 2, frame_curr.cols, CV_8UC3);
	Polygon = Mat(frame_curr.rows, frame_curr.cols, CV_8UC3);
	DrawPolygon(Polygon);

	curr_and_Polygon = Mat(frame_curr.rows, frame_curr.cols, CV_8UC3);
	VideoWriter writer_raw_sub("out_raw_sub.avi", CV_FOURCC('M', 'J', 'P', 'G'), 25.0, Size(raw_sub.cols, raw_sub.rows));
	VideoWriter writer_curr_and_Polygon("out_curr_and_Polygon.avi", CV_FOURCC('M', 'J', 'P', 'G'), 25.0, Size(curr_and_Polygon.cols, curr_and_Polygon.rows));

	while(1)  
	{  
		frame_ref = frame_curr.clone();
		capture >> frame_curr;  //读取当前帧

		//若视频播放完成，退出循环
		if(frame_curr.empty())
			break;

		Mat frame_sub = frame_curr - frame_ref;
		frame_curr.copyTo(raw_sub(Rect(0,0, frame_curr.cols, frame_curr.rows)) );
		frame_sub.copyTo(raw_sub(Rect(0, frame_curr.rows, frame_curr.cols, frame_curr.rows)));
		imshow("raw_sub", raw_sub);  //显示减法帧
		bitwise_and(frame_curr, Polygon, curr_and_Polygon);
		imshow("curr_and_Polygon", curr_and_Polygon);  //显示减法帧

		writer_raw_sub << raw_sub;
		writer_curr_and_Polygon << curr_and_Polygon;

		waitKey(1);  //延时30ms
	}  
	return 0;     
}  


// 描述：绘制一个填充的凹多边形
void DrawPolygon(Mat img)
{
	int lineType = 8;
	int  WINDOW_WIDTH = img.cols;//定义窗口大小

	//创建一些点
	Point rookPoints[1][20];
	rookPoints[0][0] = Point(WINDOW_WIDTH / 4, 7 * WINDOW_WIDTH / 8);
	rookPoints[0][1] = Point(3 * WINDOW_WIDTH / 4, 7 * WINDOW_WIDTH / 8);
	rookPoints[0][2] = Point(3 * WINDOW_WIDTH / 4, 13 * WINDOW_WIDTH / 16);
	rookPoints[0][3] = Point(11 * WINDOW_WIDTH / 16, 13 * WINDOW_WIDTH / 16);
	rookPoints[0][4] = Point(19 * WINDOW_WIDTH / 32, 3 * WINDOW_WIDTH / 8);
	rookPoints[0][5] = Point(3 * WINDOW_WIDTH / 4, 3 * WINDOW_WIDTH / 8);
	rookPoints[0][6] = Point(3 * WINDOW_WIDTH / 4, WINDOW_WIDTH / 8);
	rookPoints[0][7] = Point(26 * WINDOW_WIDTH / 40, WINDOW_WIDTH / 8);
	rookPoints[0][8] = Point(26 * WINDOW_WIDTH / 40, WINDOW_WIDTH / 4);
	rookPoints[0][9] = Point(22 * WINDOW_WIDTH / 40, WINDOW_WIDTH / 4);
	rookPoints[0][10] = Point(22 * WINDOW_WIDTH / 40, WINDOW_WIDTH / 8);
	rookPoints[0][11] = Point(18 * WINDOW_WIDTH / 40, WINDOW_WIDTH / 8);
	rookPoints[0][12] = Point(18 * WINDOW_WIDTH / 40, WINDOW_WIDTH / 4);
	rookPoints[0][13] = Point(14 * WINDOW_WIDTH / 40, WINDOW_WIDTH / 4);
	rookPoints[0][14] = Point(14 * WINDOW_WIDTH / 40, WINDOW_WIDTH / 8);
	rookPoints[0][15] = Point(WINDOW_WIDTH / 4, WINDOW_WIDTH / 8);
	rookPoints[0][16] = Point(WINDOW_WIDTH / 4, 3 * WINDOW_WIDTH / 8);
	rookPoints[0][17] = Point(13 * WINDOW_WIDTH / 32, 3 * WINDOW_WIDTH / 8);
	rookPoints[0][18] = Point(5 * WINDOW_WIDTH / 16, 13 * WINDOW_WIDTH / 16);
	rookPoints[0][19] = Point(WINDOW_WIDTH / 4, 13 * WINDOW_WIDTH / 16);

	const Point* ppt[1] = { rookPoints[0] };
	int npt[] = { 20 };

	fillPoly(img,
		ppt,
		npt,
		1,
		Scalar(255, 255, 255),
		lineType);
}