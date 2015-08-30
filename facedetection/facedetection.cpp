// facedetection.cpp : �������̨Ӧ�ó������ڵ㡣
#include<opencv/cv.h>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/opencv.hpp>
#include <opencv2/opencv.hpp> 
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#include<vector>
#include<fstream>
#include<sstream>
#include<iostream>
#include "timer.h"

using namespace std;
using namespace cv;
vector<string> testSamples;
cv::CascadeClassifier cascade;
const int detectSizeX = 320;//���������ǽ�ͼ���ŵ���320*240
const int detectSizeY = 240;
const int detectRecXStart = 80;//��ⴰ�ڵ���ʼ��С
const int detectRecYStart = 80;
const int detectRecYEnd = 220;//��ⴰ�ڵ���ֹ��С
//const int detectRecYEnd = 220;

//***************************************************************
// ����:    find_overlap
// ����:    ������ο��overlap
// Ȩ��:    public 
// ����ֵ:  float
// ����:    Rect x
// ����:    Rect y
//***************************************************************
float find_overlap(Rect x,Rect y)//�жϿ��ͼ����ص����
{
	float endx=max(x.x+x.width,y.x+y.width);
	float startx=min(x.x,y.x);
	float endy=max(x.y+x.height,y.y+y.height);
	float starty=min(x.y,y.y);
	float w=x.width+y.width-(endx-startx);
	float h=x.height+y.height-(endy-starty);
	if (w<=0||h<=0)
		return 0;
	else
	{
		float area=w*h;
		return area/(x.width*x.height);
	}
}

//***************************************************************
// ����:    readFileList
// ����:    ������ͼ���б�
// Ȩ��:    public 
// ����ֵ:  void
// ����:    string testImgFile txt�ļ����������ͼ��
// ����:    string basePath ����ַ���������ӵ�txt�е����λ�ù��ɾ��Ե�ַ
//***************************************************************
//void readFileList(string testImgFile = "D:\\DataSet\\CarFaceTestDataSet\\CarFace_ImageList.txt", string basePath="")
void readFileList(string testImgFile, string basePath)
{
		string buffer;
	std::ifstream fIn(testImgFile.c_str());
	while (fIn)
	{
		if (getline(fIn,buffer))
		{
			testSamples.push_back(basePath + buffer);
		}
	}
	cout<<"Load FileList Successfully"<<endl;
}

//***************************************************************
// ����:    loadCascadeModel
// ����:    ����cascadeģ���ļ�
// Ȩ��:    public 
// ����ֵ:  void
// ����:    string xmlPath
//***************************************************************
void loadCascadeModel(string xmlPath)
{
	cascade.load(xmlPath.c_str());
	if (!cascade.empty())
	{
		cout<<"Load Cascade Model Successfully"<<endl;
	}
}

void detecctObject(string savePath)
{
	
	TM_STATE timer;//�����������ʱ��
	TM_COUNTER start, end;
	double duration, max_duration, total_duration, average_duration;
	max_duration = total_duration = 0.;
	ofstream fout;

	fout.open(savePath + "\\cascade_detect_result.txt");//�洢���ͼ��������ļ���	

	int num=0;

	for (int i = 0; i < testSamples.size(); i++)
	{
		start_timer (&timer, &start);//������ʱ��

		IplImage * img = cvLoadImage(testSamples[i].c_str());
		vector<cv::Rect> detectedRect;//�洢��⵽��ͼ��
		IplImage * copyImg = cvCreateImage(cvSize(detectSizeX,detectSizeY), 8, 3);//ͼ�����ŵ�detectSizeX*detectSizeY���Լ����ú��ʵ���ֵ��������ԭʼͼ�ϼ�⣬̫���������������
		cvResize(img, copyImg,1 );
		IplImage * grayImage = cvCreateImage(cvGetSize(copyImg), 8, 1);

		cvCvtColor(copyImg, grayImage, CV_BGR2GRAY);//ת�����Ҷ�ͼ���

		detectedRect.clear();
		cascade.detectMultiScale(grayImage, detectedRect, 1.1, 3, CV_HAAR_SCALE_IMAGE | CV_HAAR_DO_CANNY_PRUNING, cvSize(detectRecXStart, detectRecXStart), cvSize(detectRecYEnd, detectRecYEnd));//��⺯��

		fout<<testSamples[i].c_str()<<" "<<detectedRect.size()<<" ";
		cout<<testSamples[i].c_str()<<" "<<detectedRect.size()<<" ";
		for (vector<cv::Rect>::iterator k= detectedRect.begin(); k != detectedRect.end(); k++)
		{
			Rect r = *k;
			vector<cv::Rect>::iterator j=  detectedRect.begin();
			for (j=  detectedRect.begin(); j != detectedRect.end(); j++)
			{
				if ( k != j && (r & *j) == r)//��������Ƕ�׵ģ�һ����������һ�����ο��е����
					break;
				if(find_overlap(*k, *j)>0.6 && k->height*k->width < j->height*j->width)//��������������������ص�����0.6�Ŀ�һ����Ϊ����
					break;
			}
			if(j ==detectedRect.end())
			{
				//�˴��������ͼ��Ŵ���0.03�� ʹ�û����������Կ���
				//cvRectangle(img, cvPoint(k->x*img->width/detectSizeX-k->width*img->width/detectSizeX*0.03, k->y*img->width/detectSizeX-k->width*img->width/detectSizeX*0.03), cvPoint(k->x*img->width/detectSizeX + k->width*img->width/detectSizeX + k->width*img->width/detectSizeX*0.03, k->y*img->width/detectSizeX + k->height*img->width/detectSizeX +k->width*img->width/detectSizeX*0.03),Scalar(0, 255, 0));
				//cvRectangle(img, cvPoint(k->x*img->width/detectSizeX, k->y*img->width/detectSizeX), cvPoint(k->x*img->width/detectSizeX + k->width*img->width/detectSizeX, k->y*img->width/detectSizeX + k->height*img->width/detectSizeX),Scalar(0, 255, 0));

				fout << k->x*img->width/detectSizeX-k->width*img->width/detectSizeX*0.03 << " "<<k->y*img->height/detectSizeY-k->width*img->width/detectSizeX*0.03<<" "<<k->width*img->width/detectSizeX + k->width*img->width/detectSizeX*0.06<<" "<<k->height*img->height/detectSizeY + k->width*img->width/detectSizeX*0.06<<" ";
				cout << k->x*img->width/detectSizeX-k->width*img->width/detectSizeX*0.03 << " "<<k->y*img->height/detectSizeY-k->width*img->width/detectSizeX*0.03<<" "<<k->width*img->width/detectSizeX + k->width*img->width/detectSizeX*0.06<<" "<<k->height*img->height/detectSizeY + k->width*img->width/detectSizeX*0.06<<" ";

				//������ȡ�����趨ROI
				cvSetImageROI(img, cvRect(k->x*img->width/detectSizeX - k->width*img->width/detectSizeX*0.03,k->y*img->height/detectSizeY - k->width*img->width/detectSizeX*0.03,k->width*img->width/detectSizeX+ k->width*img->width/detectSizeX*0.06,k->width*img->width/detectSizeX+ k->width*img->width/detectSizeX*0.06));
				//cvSetImageROI(img, cvRect(k->x*img->width/detectSizeX,k->y*img->height/detectSizeY,k->width*img->width/detectSizeX,k->width*img->width/detectSizeX));
				string  filePartName=testSamples[i].substr(testSamples[i].find_first_of("/\\") + 1 ,testSamples[i].find(".jpg") -testSamples[i].find_last_of("/\\") - 1);

				std::stringstream ss;
				string numstr;
				ss<<num;
				ss>>numstr;
				num++;
				string name=savePath+filePartName+"_"+numstr+".jpg";//�洢ͼ������
				cout<<name<<endl;
				cvSaveImage(name.c_str(),img);
			}
		}

		fout <<endl;
		cout<<endl;
		stop_timer (&end);
		duration = elapsed_time (&timer, &start, &end);

		if (duration > max_duration)
			max_duration = duration;

		total_duration += duration;
		cout<<"duration"<<duration<<" max_duration"<<max_duration<<" total_duration"<<total_duration<<endl;
		////��ʾ����
		cvNamedWindow("output");
		cvShowImage("output",copyImg);
		cvShowImage("output",img);
		waitKey(0);

		cvReleaseImage(&img);
		cvReleaseImage(&grayImage);
		cvReleaseImage(&copyImg);
	}
	fout.close();
}
void main ()
{
	string testImgFile = "D:\\DataSet\\CarFaceTestDataSet\\CarFace_ImageList.txt";
	string basePath="";
	string xmlPath = "D:\\WorkSpace\\VS_Projects\\facedetection\\cascade.xml";
	string savePath = "D:\\DataSet\\CarFaceTestDataSet\\result";
	readFileList(testImgFile,basePath);
	loadCascadeModel(xmlPath);
	detecctObject(savePath);

}