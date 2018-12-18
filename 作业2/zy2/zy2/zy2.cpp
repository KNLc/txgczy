#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

int main( )
{
	Mat image_raw;
	Mat image_linear;
	Mat image_nonlinear;
	Mat image_Equalization;

	image_raw = imread( "1.jpg", IMREAD_GRAYSCALE);
	resize(image_raw, image_raw, Size(256, 256) );
	if(image_raw.empty() ) { printf("读取图片错误，请确定目录下是否有imread函数指定图片存在~！ \n"); return false; }
	
	
	Mat imag_compare = Mat::zeros(image_raw.rows * 3, image_raw.cols * 2, CV_8UC1);
	Mat imag_compare_Equa = Mat::zeros(image_raw.rows * 2, image_raw.cols * 2, CV_8UC1);

	
	// 线性操作 image_linear(i,j) = a*image_raw(i,j) + b
	double a = 3;
	double b = 30;

	image_linear = Mat::zeros( image_raw.rows, image_raw.cols, CV_8UC1);
	for (int y = 0; y < image_raw.rows; y++)
	{
		for (int x = 0; x < image_raw.cols; x++)
		{
			int temp = saturate_cast<uchar>( a*(image_raw.at<uchar>(y, x)) + b);
			if (temp > 255)
				image_linear.at<uchar>(y, x) = 255;
			else if( temp < 0 )
				image_linear.at<uchar>(y, x) = 0;
			else
				image_linear.at<uchar>(y, x) = temp;
		}
	}

	line(imag_compare(Rect(image_raw.cols, 0, image_raw.cols, image_raw.rows)), Point(0, image_raw.rows-1), Point(image_raw.cols-1, 0), Scalar(255, 0, 0));
	double x0 = 0;
	double y0 = a*x0 + b;
	y0 = image_raw.rows - 1 - y0;
	double y1 = image_raw.rows - 1;
	double x1 = (y1 - b) / a;
	y1 = image_raw.rows - 1 - y1;
	line(imag_compare(Rect(image_raw.cols, image_raw.rows, image_raw.cols, image_raw.rows)), Point(x0, y0), Point(x1, y1), Scalar(255, 0, 0));



	// 非线性操作 image_nonlinear(i,j) = aa*image_raw(i,j)*image_raw(i,j) + bb*image_raw(i,j) + cc
	double aa = 0.01;
	double bb = 0.2;
	double cc = 10;
	image_nonlinear = Mat::zeros(image_raw.rows, image_raw.cols, CV_8UC1);
	for (int y = 0; y < image_raw.rows; y++)
	{
		for (int x = 0; x < image_raw.cols; x++)
		{
			int temp = saturate_cast<uchar>(aa*(image_raw.at<uchar>(y, x))*(image_raw.at<uchar>(y, x)) 
				+ bb*(image_raw.at<uchar>(y, x)) 
				+ cc );
			if (temp > 255)
				image_nonlinear.at<uchar>(y, x) = 255;
			else if (temp < 0)
				image_nonlinear.at<uchar>(y, x) = 0;
			else
				image_nonlinear.at<uchar>(y, x) = temp;
		}
	}
	for (int i=0;i<image_raw.cols*10;i++)
	{
		double x = i / 10.0;
		double y = aa*x*x + bb*x + cc;
		y = image_raw.rows - 1 - y;
		circle(imag_compare(Rect(image_raw.cols, image_raw.rows*2, image_raw.cols, image_raw.rows)), Point(x, y), 0, cv::Scalar(255, 0, 0), 1, 8, 0);

	}

	// 直方图均衡化
	equalizeHist(image_raw, image_Equalization);

	int channels = 0;
	MatND dstHist;
	MatND dstHist2;
	int histSize[] = { 256 };       //如果写成int histSize = 256;调用计算直方图的函数的时，该变量需写成&histSize  
	float midRanges[] = { 0, 256 };
	const float *ranges[] = { midRanges };
	calcHist(&image_raw, 1, &channels, Mat(), dstHist, 1, histSize, ranges, true, false);
	calcHist(&image_Equalization, 1, &channels, Mat(), dstHist2, 1, histSize, ranges, true, false);
	
	//绘制直方图,首先先创建一个黑底的图像，为了可以显示彩色，所以该绘制图像是一个8位的3通道图像  
	Mat hist = Mat::zeros(Size(256, 256), CV_8UC1);
	Mat hist2 = Mat::zeros(Size(256, 256), CV_8UC1);
	//任何一个图像的某个像素的总个数有可能会很多，甚至超出所定义的图像的尺寸，
	//所以需要先对个数进行范围的限制，用minMaxLoc函数来得到计算直方图后的像素的最大个数  
	double g_dHistMaxValue, g_dHistMaxValue2;
	minMaxLoc(dstHist, 0, &g_dHistMaxValue, 0, 0);
	minMaxLoc(dstHist2, 0, &g_dHistMaxValue2, 0, 0);
	//将像素的个数整合到图像的最大范围内  
	for (int i = 0; i < 256; i++)
	{
		int value = cvRound(dstHist.at<float>(i) * 256 * 0.9 / g_dHistMaxValue);
		int value2 = cvRound(dstHist2.at<float>(i) * 256 * 0.9 / g_dHistMaxValue2);

		line(hist, Point(i, hist.rows - 1), Point(i, hist.rows - 1 - value), Scalar(255));
		line(hist2, Point(i, hist2.rows - 1), Point(i, hist2.rows - 1 - value2), Scalar(255));

	}
	/*imshow("hist", hist);
	imshow("hist2", hist2);*/

	
	// 显示结果
	image_raw.copyTo(imag_compare(Rect(0, 0, image_raw.cols, image_raw.rows)));
	image_linear.copyTo(imag_compare(Rect(0, image_linear.rows, image_linear.cols, image_linear.rows)));
	image_nonlinear.copyTo(imag_compare(Rect(0, image_nonlinear.rows*2, image_nonlinear.cols, image_nonlinear.rows)));
	

	image_raw.copyTo(imag_compare_Equa(Rect(0, 0, image_raw.cols, image_raw.rows)));
	image_Equalization.copyTo(imag_compare_Equa(Rect(0, image_raw.rows, image_raw.cols, image_raw.rows)));
	hist.copyTo(imag_compare_Equa(Rect(image_raw.cols, 0, image_raw.cols, image_raw.rows)));
	hist2.copyTo(imag_compare_Equa(Rect(image_raw.cols, image_raw.rows, image_raw.cols, image_raw.rows)));

	/*imshow("image_raw", image_raw);
	imshow("image_Equalization", image_Equalization);
	imshow("image_linear", image_linear);
	imshow("image_nonlinear", image_nonlinear);*/
	imshow("imag_compare", imag_compare);
	imshow("imag_compare_Equa", imag_compare_Equa);


	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);  //选择jpeg
	compression_params.push_back(100); //图片质量
	imwrite("imag_compare.jpg", imag_compare);
	imwrite("imag_compare_Equa.jpg", imag_compare_Equa, compression_params);
	waitKey(0);
	return 0;

}