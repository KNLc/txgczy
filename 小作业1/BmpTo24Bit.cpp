#include "opencv2/opencv.hpp"
#include <iostream>
#include<windows.h>
#include<stdio.h>

using namespace std;
using namespace cv;

//#define _CRT_SECURE_NO_WARNINGS //VS下需要在预处理器定义该宏才能编译通过



/** @brief Convert 8/24-bit bmp file to 24-bit bmp file.

The function convert 8/24-bit bmp file to 24-bit bmp file.
@param path_in The path/name of input image
@param path_out The path/name of output image
*/
void BmpTo24Bit(const char *path_in, const char *path_out )
{	
	BYTE *p_temp = NULL, *pImg = NULL, *pCur = NULL;
	FILE *binFile;//文件
	FILE *File_out;//文件
	BITMAPFILEHEADER fileHeader;//文件头
	BITMAPINFOHEADER infoHeader;//信息头
	BOOL isRead = TRUE;
	int linenum, extend; //linenum:一行像素的字节总数，包括填充字节 
	BYTE(*Palette)[4];//调色板，注意，调色板一个颜色是32bit
	Palette = NULL;
	//open file
	if ((binFile = fopen(path_in, "rb")) == NULL)
	{
		printf("open error! path_in:%s\n", path_in);
		return ;
	}
	//读文件头和信息头
	//为什么用fread读到结构体可行？因为bmp数据存储方式跟windows正好都是小端模式
	//从binFile以1byte为单位读取sizeof(fileHeader)个数据，保存到fileHeader首地址的地方，由于windows是小端存储，正好跟bmp数据格式对应
	//fread 如果读取成功，则返回实际读到的单元个数
	if (fread((void *)&fileHeader, 1, sizeof(fileHeader), binFile) != sizeof(fileHeader)) isRead = FALSE;
	if (fread((void *)&infoHeader, 1, sizeof(infoHeader), binFile) != sizeof(infoHeader)) isRead = FALSE;
	//问，这里的判断是为了避免什么样问题
	if (isRead == FALSE || fileHeader.bfOffBits < sizeof(fileHeader) + sizeof(infoHeader)) {

		fclose(binFile);
		return ;
	}

	//read image info
	uint8_t biBitCount = infoHeader.biBitCount;//像素位数8，16，24，32
	uint8_t ch_nums = infoHeader.biBitCount / 8;//通道数
	linenum = (infoHeader.biWidth * ch_nums + 3) / 4 * 4;//行数，每行像素需要四字节对齐向上取整
	extend = linenum - infoHeader.biWidth * ch_nums;         //每一行的填充字节数
	
	//read Palette
	fseek(binFile, sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), SEEK_SET);//设置文件指针指向调色板
	if (8 == infoHeader.biBitCount)
	{
		Palette = new BYTE[256][4]; //(*Palette)[4]
		int r = fread(Palette, sizeof(BYTE), fileHeader.bfOffBits-sizeof(BITMAPFILEHEADER)-sizeof(BITMAPINFOHEADER), binFile);
		if (r != fileHeader.bfOffBits - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER))
		{
			
			fclose(binFile);
			return;
		}
	}
	else if (4 == infoHeader.biBitCount)
	{
		Palette = new BYTE[16][4]; //(*Palette)[4]
		int r = fread(Palette, sizeof(BYTE), fileHeader.bfOffBits - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER), binFile);
		if (r != fileHeader.bfOffBits - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER))
		{
			
			fclose(binFile);
			return;
		}
	}


	fseek(binFile, fileHeader.bfOffBits, SEEK_SET);//设置文件指针指向第一个数据
	
	p_temp = new BYTE[ infoHeader.biWidth * infoHeader.biHeight ]; // 申请空间，多通道还需要x通道数
	pImg = new BYTE[ infoHeader.biWidth * infoHeader.biHeight * 3 ]; // 申请空间，多通道还需要x通道数
	if (NULL == pImg || NULL == pImg) {
		printf("memory allocation failed!\n");
		fclose(binFile);
		return;
	}


	
	switch (biBitCount)
	{
	default:
		fclose(binFile);
		return;
		break;
	case 4://TODO: maybe need bit field or union

		break;
	case 8:
		for (int i = 0; i < infoHeader.biHeight; i++)
		{
			int r = fread(p_temp + i*(infoHeader.biWidth), sizeof(BYTE), infoHeader.biWidth, binFile);
			if (r != infoHeader.biWidth)
			{
				delete pImg;
				fclose(binFile);
				return;
			}
			for (int j = 0; j < infoHeader.biWidth; j++)
			{
				pImg[i*infoHeader.biWidth * 3 + j * 3 + 0] = Palette[p_temp[i* infoHeader.biWidth + j]][0];
				pImg[i*infoHeader.biWidth * 3 + j * 3 + 1] = Palette[p_temp[i* infoHeader.biWidth + j]][1];
				pImg[i*infoHeader.biWidth * 3 + j * 3 + 2] = Palette[p_temp[i* infoHeader.biWidth + j]][2];
			}
			fseek(binFile, extend, SEEK_CUR);
		}

		break;
	case 24:
		for (int i = 0; i < infoHeader.biHeight; i++)
		{
			int r = fread(pImg + i*(infoHeader.biWidth) * 3, sizeof(BYTE), (infoHeader.biWidth) * 3, binFile);
			if (r != (infoHeader.biWidth)*3)
			{
				delete pImg;
				fclose(binFile);
				return;
			}
			fseek(binFile, extend, SEEK_CUR);
		}
		break;
	case 32://TODO: like case_24
		break;
	}
	fclose(binFile);

	


	//write a new 24bit bmp file
	linenum = (infoHeader.biWidth * 3 + 3) / 4 * 4;//行数，每行像素需要四字节对齐向上取整
	extend = linenum - infoHeader.biWidth * 3;         //每一行的填充字节数
	infoHeader.biBitCount = 24;//颜色位数为24bit
	infoHeader.biClrUsed = 0;//位图使用的颜色数，如果为0，则颜色数为2的biBitCount次方
	infoHeader.biClrImportant = 0;//重要的颜色数，0代表所有颜色都重要
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);// +256 * 4L，偏移数，即位图文件头+位图信息头+调色板 的大小，这里调色板为0
	fileHeader.bfSize = fileHeader.bfOffBits + (infoHeader.biWidth*3 + extend)*infoHeader.biHeight;//整个bmp文件大小
	//write the fileHeader and infoHeader 
	bool Suc = true;
	// Open File
	if ((File_out = fopen(path_out, "w+b")) == NULL)  return;
	if (fwrite((void *)&fileHeader, 1, sizeof(fileHeader), File_out) != sizeof(fileHeader)) Suc = false;
	if (fwrite((void *)&infoHeader, 1, sizeof(infoHeader), File_out) != sizeof(infoHeader)) Suc = false;
	//write data
	BYTE* ex = nullptr;
	if (extend)
	{
		ex = new BYTE[extend]; //填充数组大小为 0~3
		memset(ex, 0, extend);
	}
	// 灰度值x0.5
	for (pCur = pImg; pCur < pImg + infoHeader.biHeight * (infoHeader.biWidth*3 + extend) - 1; pCur++)
	{
		*pCur = *pCur *0.5;
	}
	//write data
	for (pCur = pImg; pCur < pImg + infoHeader.biHeight*(infoHeader.biWidth * 3 + extend); pCur += (infoHeader.biWidth*3 + extend))
	{
		if (fwrite((void *)pCur, 1, infoHeader.biWidth*3, File_out) != (unsigned int)infoHeader.biWidth) Suc = false; // 真实的数据
		if (extend) // 扩充的数据 这里填充0
			if (fwrite((void *)ex, 1, extend, File_out) != 1) Suc = false;
	}

	fclose(File_out);
	if (extend)
		delete[] ex;
	
	return ;
}

int main()
{
	int width, height, rwidth;

	//判断多维数组存储顺序
	uint8_t aa[3][2];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 2; j++)
			printf(" &(aa[%d][%d])=%d\r\n",i,j, &(aa[i][j]) );
	//判断数据大端小端存储
	uint32_t bb = 0x11223344;
	printf("判断是大端还是小端\r\n");
	printf("bb=%x \r\n", bb);
	for (int i = 0; i < sizeof(bb); i++)
		printf("((uint8_t*)&bb)[%d]=%x\r\n", i, ((uint8_t*)&bb)[i]);
	printf("是小端模式\r\n");
	
	BmpTo24Bit("8bit.bmp", "o.bmp");

	Mat bmp_image1 = imread("8bit.bmp");
	imshow("1", bmp_image1);
	Mat bmp_image = imread("o.bmp");
	imshow("o", bmp_image);
	waitKey(0);
}
