//--------------------------------------������˵����-------------------------------------------
//		����˵������OpenCV3������š�OpenCV2���鱾����ʾ������68
//		����������ֱ��ͼ���⻯
//		�����������ò���ϵͳ�� Windows 7 64bit
//		������������IDE�汾��Visual Studio 2010
//		������������OpenCV�汾��	2.4.9
//		2014��06�� Created by @ǳī_ë����
//		2014��11�� Revised by @ǳī_ë����
//------------------------------------------------------------------------------------------------


//---------------------------------��ͷ�ļ��������ռ�������֡�---------------------------
//          ����������������ʹ�õ�ͷ�ļ��������ռ�
//-----------------------------------------------------------------------------------------------
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


using namespace cv;

//--------------------------------------��main( )������-----------------------------------------
//          ����������̨Ӧ�ó������ں��������ǵĳ�������￪ʼִ��
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

	// �ȴ��û������˳�����
	waitKey(0);
	return 0;

}