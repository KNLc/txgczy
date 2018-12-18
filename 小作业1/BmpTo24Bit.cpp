#include "opencv2/opencv.hpp"
#include <iostream>
#include<windows.h>
#include<stdio.h>

using namespace std;
using namespace cv;

//#define _CRT_SECURE_NO_WARNINGS //VS����Ҫ��Ԥ����������ú���ܱ���ͨ��



/** @brief Convert 8/24-bit bmp file to 24-bit bmp file.

The function convert 8/24-bit bmp file to 24-bit bmp file.
@param path_in The path/name of input image
@param path_out The path/name of output image
*/
void BmpTo24Bit(const char *path_in, const char *path_out )
{	
	BYTE *p_temp = NULL, *pImg = NULL, *pCur = NULL;
	FILE *binFile;//�ļ�
	FILE *File_out;//�ļ�
	BITMAPFILEHEADER fileHeader;//�ļ�ͷ
	BITMAPINFOHEADER infoHeader;//��Ϣͷ
	BOOL isRead = TRUE;
	int linenum, extend; //linenum:һ�����ص��ֽ���������������ֽ� 
	BYTE(*Palette)[4];//��ɫ�壬ע�⣬��ɫ��һ����ɫ��32bit
	Palette = NULL;
	//open file
	if ((binFile = fopen(path_in, "rb")) == NULL)
	{
		printf("open error! path_in:%s\n", path_in);
		return ;
	}
	//���ļ�ͷ����Ϣͷ
	//Ϊʲô��fread�����ṹ����У���Ϊbmp���ݴ洢��ʽ��windows���ö���С��ģʽ
	//��binFile��1byteΪ��λ��ȡsizeof(fileHeader)�����ݣ����浽fileHeader�׵�ַ�ĵط�������windows��С�˴洢�����ø�bmp���ݸ�ʽ��Ӧ
	//fread �����ȡ�ɹ����򷵻�ʵ�ʶ����ĵ�Ԫ����
	if (fread((void *)&fileHeader, 1, sizeof(fileHeader), binFile) != sizeof(fileHeader)) isRead = FALSE;
	if (fread((void *)&infoHeader, 1, sizeof(infoHeader), binFile) != sizeof(infoHeader)) isRead = FALSE;
	//�ʣ�������ж���Ϊ�˱���ʲô������
	if (isRead == FALSE || fileHeader.bfOffBits < sizeof(fileHeader) + sizeof(infoHeader)) {

		fclose(binFile);
		return ;
	}

	//read image info
	uint8_t biBitCount = infoHeader.biBitCount;//����λ��8��16��24��32
	uint8_t ch_nums = infoHeader.biBitCount / 8;//ͨ����
	linenum = (infoHeader.biWidth * ch_nums + 3) / 4 * 4;//������ÿ��������Ҫ���ֽڶ�������ȡ��
	extend = linenum - infoHeader.biWidth * ch_nums;         //ÿһ�е�����ֽ���
	
	//read Palette
	fseek(binFile, sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), SEEK_SET);//�����ļ�ָ��ָ���ɫ��
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


	fseek(binFile, fileHeader.bfOffBits, SEEK_SET);//�����ļ�ָ��ָ���һ������
	
	p_temp = new BYTE[ infoHeader.biWidth * infoHeader.biHeight ]; // ����ռ䣬��ͨ������Ҫxͨ����
	pImg = new BYTE[ infoHeader.biWidth * infoHeader.biHeight * 3 ]; // ����ռ䣬��ͨ������Ҫxͨ����
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
	linenum = (infoHeader.biWidth * 3 + 3) / 4 * 4;//������ÿ��������Ҫ���ֽڶ�������ȡ��
	extend = linenum - infoHeader.biWidth * 3;         //ÿһ�е�����ֽ���
	infoHeader.biBitCount = 24;//��ɫλ��Ϊ24bit
	infoHeader.biClrUsed = 0;//λͼʹ�õ���ɫ�������Ϊ0������ɫ��Ϊ2��biBitCount�η�
	infoHeader.biClrImportant = 0;//��Ҫ����ɫ����0����������ɫ����Ҫ
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);// +256 * 4L��ƫ��������λͼ�ļ�ͷ+λͼ��Ϣͷ+��ɫ�� �Ĵ�С�������ɫ��Ϊ0
	fileHeader.bfSize = fileHeader.bfOffBits + (infoHeader.biWidth*3 + extend)*infoHeader.biHeight;//����bmp�ļ���С
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
		ex = new BYTE[extend]; //��������СΪ 0~3
		memset(ex, 0, extend);
	}
	// �Ҷ�ֵx0.5
	for (pCur = pImg; pCur < pImg + infoHeader.biHeight * (infoHeader.biWidth*3 + extend) - 1; pCur++)
	{
		*pCur = *pCur *0.5;
	}
	//write data
	for (pCur = pImg; pCur < pImg + infoHeader.biHeight*(infoHeader.biWidth * 3 + extend); pCur += (infoHeader.biWidth*3 + extend))
	{
		if (fwrite((void *)pCur, 1, infoHeader.biWidth*3, File_out) != (unsigned int)infoHeader.biWidth) Suc = false; // ��ʵ������
		if (extend) // ��������� �������0
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

	//�ж϶�ά����洢˳��
	uint8_t aa[3][2];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 2; j++)
			printf(" &(aa[%d][%d])=%d\r\n",i,j, &(aa[i][j]) );
	//�ж����ݴ��С�˴洢
	uint32_t bb = 0x11223344;
	printf("�ж��Ǵ�˻���С��\r\n");
	printf("bb=%x \r\n", bb);
	for (int i = 0; i < sizeof(bb); i++)
		printf("((uint8_t*)&bb)[%d]=%x\r\n", i, ((uint8_t*)&bb)[i]);
	printf("��С��ģʽ\r\n");
	
	BmpTo24Bit("8bit.bmp", "o.bmp");

	Mat bmp_image1 = imread("8bit.bmp");
	imshow("1", bmp_image1);
	Mat bmp_image = imread("o.bmp");
	imshow("o", bmp_image);
	waitKey(0);
}
