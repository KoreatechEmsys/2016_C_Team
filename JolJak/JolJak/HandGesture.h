#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <opencv2\opencv.hpp>


enum OPER {INC = 0x01,DEC,SAME,INC2};
enum FINGERPOS {THUMB = 0x10, INDEX, MIDDLE, RING, BABY, ERR};
enum GESTURE {G1 = 0x10, G2, G3, G4, UNKNOWN, NEEDCALIB, CALIBCOMPLETE};
#define MAX 10
#define __DBGVIS__

#define THUMBMAX 210
#define THUMBMIN 150
#define INDEXMAX 150
#define INDEXMIN 104
#define MIDDLEMAX 104
#define MIDDLEMIN 80
#define RINGMAX 80
#define RINGMIN 50
#define BABYMAX 50
#define BABYMIN 0

using namespace std;
using namespace cv;

class GestureInfo
{
public:
	vector<Point> vecFingerPts;
	Point cog;
	uchar gesture;

	GestureInfo();
};


class LabelInfo 
{
public:
	cv::Point centroid;
	int area;
	int angle;
	LabelInfo(int x, int y, int area, int angle);
};
class WidthInfo
{
public:
	int area;
	int count;
	WidthInfo(int area);
};

class RefInfo
{
public:
	vector<int> idx;
	int refCount;
	
	RefInfo(void);
	void addIdx(int idx);
};

class FingerPosInfo
{
public:
	int fingerAngle[5];
	int fingerWidth[5];
	int arrKvalues[5];

	FingerPosInfo();
};

class FingerInfo
{
public:
	int fingerAngle;
	int FingerWidth;
	FingerInfo(int angle, int width);
};


class HandGesture
{
public:
	VideoCapture vcDevice;

	Mat imgOrigin;
	Mat imgYCrCb;
	Mat imgSkin;
	Mat morphKernel;

	Moments imgMoment;
	Point cog;

	int iArmAngle;
	int iAramLastAngle;
	int iCircleRadius;
	int iDTRadius;
	int iFingerCount;
	int iCriterionDist;
	int iResolution; 
	unsigned int calibrationCount;

	int initFingerCnt;

	int iMin;
	int iMax;

	bool calibration;
	FingerPosInfo fPosInfo;


	vector<Point> vecFingerPts;
	vector<Point> vecFirstFingerPts;
	vector<int> vecFingerAngle;
	vector<WidthInfo> vecFingerWidth;

	vector<uchar> vecFingerPos;
	vector<Point> vecFingerPtsT;
	vector<Point> vecFirstFingerPtsT;
	vector<int> vecFingerAngleT;
	vector<WidthInfo> vecFingerWidthT;

	Scalar skinMin;
	Scalar skinMax;

	vector<Point> vecHandContour;


	HandGesture();

	bool openDevice(int devNum);
	void openVideo(const char* fileName);
	bool readImage(void);

	void gaussianBlur(void);
	void skinColorExtraction(void);
	bool drawHandRegion(void);
	void getMoment(void);
	void morphologyOperation(int iterCnt);
	bool handValidation(void);
	void getDistanceTransform(void);
	bool featureExtraction();
	int getConnectedComponentInfo(vector<LabelInfo>& lInfo, Point cog, Mat * pCImg);
	void labelProcess(vector<LabelInfo>& lInfo, uchar operation);
	bool gestureClassification(void);

	void initFingerAngle(void);
	void updateFingerAnlge(void);

	void fingerPositionEstimation(void);

	void displayImage(void);
	void destroyObject(void);
	void WarningMSG(string str);
	void dbgVisualization(void);
	void testFunc();

	int getResolution(int resolution);
	
	bool main_loop(void);

	void makeGestureInform(GestureInfo& gInfo);
};


void errorException(const char* str, int line);
void errorException(const char* str, int line, const char* msg);
void concentricCircleMasking(Mat& src, Mat& dst, Point cog, int radius, int rows, int cols);
