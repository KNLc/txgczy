//--------------------------------------【程序说明】-------------------------------------------
//		程序说明：《OpenCV3编程入门》OpenCV2版书本配套示例程序68
//		程序描述：直方图均衡化
//		开发测试所用操作系统： Windows 7 64bit
//		开发测试所用IDE版本：Visual Studio 2010
//		开发测试所用OpenCV版本：	2.4.9
//		2014年06月 Created by @浅墨_毛星云
//		2014年11月 Revised by @浅墨_毛星云
//------------------------------------------------------------------------------------------------


//---------------------------------【头文件、命名空间包含部分】---------------------------
//          描述：包含程序所使用的头文件和命名空间
//-----------------------------------------------------------------------------------------------
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


using namespace cv;

//--------------------------------------【main( )函数】-----------------------------------------
//          描述：控制台应用程序的入口函数，我们的程序从这里开始执行
//-----------------------------------------------------------------------------------------------
int main( )
{
	int step = 10;
	int step_height = 50;
	//Mat image_grey = Mat::zeros(100,500,CV_8UC1);
	Mat image_color = Mat::zeros(600, 1000, CV_8UC3);
	image_color = Scalar(255, 255, 255) - image_color;
	/*for (int i = 0; i < image_color.cols; i++)
	{
		image_color.at<Vec3b>(y, x)[c];
	}*/
	for (int i = 0; i < image_color.cols/ step; i++)
	{
		uchar value = 255 - i * 1;
		//uchar value = i * 1;
		rectangle(image_color, Rect(step*i, 0, step, step_height), Scalar(value, value, value), -1);
		rectangle(image_color, Rect(step * i, step_height * 2, step, step_height), Scalar(value, value, value), -1);
		line(image_color, Point(step * i, step_height * 2), Point(step * i, step_height * 2 + step_height), Scalar(0, 0, 255));

	}
	//Y = ((R*299)+(G*587)+(B*114))/1000
	for (int i = 0; i < image_color.cols / step; i++)
	{
		uchar value = 255 - i * 1;
		uchar B = 255;
		uchar G = i;
		uchar R = 255;

		//uchar value = i * 1;
		rectangle(image_color, Rect(step*i, step_height * 3, step, step_height), Scalar(B, G, R), -1);
		rectangle(image_color, Rect(step * i, step_height * 5, step, step_height), Scalar(B, G, R), -1);
		line(image_color, Point(step * i, step_height * 5), Point(step * i, step_height * 5 + step_height), Scalar(0, 0, 255));

	}
	imshow("image_color", image_color);
	imwrite("image_color.png", image_color);

	// 等待用户按键退出程序
	waitKey(0);
	return 0;

}