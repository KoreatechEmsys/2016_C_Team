#include "stdafx.h"

void getRatio(int radius, int* min, int* max);

HandGesture::HandGesture()
{
	cog.x = 0;
	cog.y = 0;

	iArmAngle = 0;
	iAramLastAngle = 0;
	iCircleRadius = 0;
	iFingerCount = 0;
	initFingerCnt = 0;
	calibration = false;

	skinMin = Scalar(0, 133, 77);
	skinMax = Scalar(255, 173, 127);

	morphKernel = getStructuringElement(MORPH_RECT, Size(5, 5));
	morphKernel.at<uchar>(0, 0) = 0;
	morphKernel.at<uchar>(0, 1) = 0;
	morphKernel.at<uchar>(1, 0) = 0;
	morphKernel.at<uchar>(0, 3) = 0;
	morphKernel.at<uchar>(0, 4) = 0;
	morphKernel.at<uchar>(1, 4) = 0;
	morphKernel.at<uchar>(3, 0) = 0;
	morphKernel.at<uchar>(4, 0) = 0;
	morphKernel.at<uchar>(4, 1) = 0;
	morphKernel.at<uchar>(3, 4) = 0;
	morphKernel.at<uchar>(4, 4) = 0;
	morphKernel.at<uchar>(4, 3) = 0;
	calibrationCount = 0;
}

bool HandGesture::openDevice(int devNum)
{
	vcDevice = VideoCapture(0);
	
	if (vcDevice.isOpened() == false)
	{
		vcDevice.release();
		return false;
	}

	return true;
}

void HandGesture::openVideo(const char * fileName)
{
	vcDevice.open(fileName);
}

bool HandGesture::readImage(void)
{
	if (vcDevice.read(imgOrigin) == false)
		return false;

	return true;
}

void HandGesture::gaussianBlur(void)
{
	GaussianBlur(imgOrigin, imgOrigin, Size(7, 7), 1);
}

void HandGesture::skinColorExtraction(void)
{
	cvtColor(imgOrigin, imgYCrCb, CV_BGR2YCrCb);
	inRange(imgYCrCb, skinMin, skinMax, imgSkin);
}

bool HandGesture::drawHandRegion(void)
{
	vector<vector<Point>> contours;
	int area = 0;
	int maxArea = 0;
	int idx = -1;
	int iterCnt = 0;

	findContours(imgSkin, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	iterCnt = contours.size();

	if (iterCnt == 0)
		return false;

	for (int i = 0; i < iterCnt; i++)
	{
		area = abs(contourArea(contours[i]));
		if (area > maxArea)
		{
			maxArea = area;
			idx = i;
		}
	}


	if (idx != -1)
		vecHandContour = contours[idx];
	else
	{
		return false;
	}

	imgSkin = Scalar(0);
	drawContours(imgSkin, contours, idx, Scalar(255),-1);
	return true;
}


void HandGesture::getMoment(void)
{
	imgMoment = moments(vecHandContour);
}

void HandGesture::morphologyOperation(int iterCnt)
{
	for (int i = 0; i < iterCnt; i++)
	{
		morphologyEx(imgSkin, imgSkin, MORPH_CLOSE, morphKernel);
	}
}

bool HandGesture::handValidation(void)
{
	if (vecHandContour.size() < 1)
		errorException(__FUNCTION__, __LINE__);
	
	int area = abs(contourArea(vecHandContour));
	if (area < 20000)
		return false;
	
	return true;
}

void HandGesture::getDistanceTransform(void)
{
	int arrPt[2] = { 0, };
	double maxVal = 0;
	Mat imgDist;

	distanceTransform(imgSkin, imgDist, CV_DIST_L2, 3);
	minMaxIdx(imgDist, NULL, &maxVal, NULL, arrPt);

	cog.x = arrPt[1];
	cog.y = arrPt[0];

	iResolution = maxVal / 2;

	iDTRadius = maxVal; 
	
	iCircleRadius = maxVal + iResolution;
	
	getRatio((int)maxVal, &iMin, &iMax);
	
	iCriterionDist = maxVal * 1.75;

	
#ifdef __DBGVIS__
	//circle(imgOrigin, cog, iCircleRadius, Scalar(0, 100, 255));
	circle(imgOrigin, cog, maxVal, Scalar(0, 100, 255));
	//circle(imgOrigin, cog, iMin, Scalar(13, 255, 255));
	//circle(imgOrigin, cog, iMax, Scalar(13, 255, 255));
#endif // __DBGVIS__
}

void HandGesture::displayImage(void)
{
	imshow("origin", imgOrigin);
	imshow("binary", imgSkin);
}

void HandGesture::destroyObject(void)
{
	vecHandContour.clear();

	vecFingerAngle.clear();
	vecFingerPts.clear();
	vecFingerWidth.clear();

	vecFingerAngleT.clear();
	vecFingerPtsT.clear();
	vecFingerWidthT.clear();
	vecFingerPos.clear();


	vecFirstFingerPtsT.clear();
	vecFirstFingerPts.clear();

	iAramLastAngle = iArmAngle;

	iArmAngle = 0;
	iCircleRadius = 0;
	iFingerCount = 0;
	iResolution = 0;
	cog.x = 0;
	cog.y = 0;
}

void HandGesture::WarningMSG(string str)
{
	putText(imgSkin, str, Point(10, 60), CV_FONT_HERSHEY_TRIPLEX, 1.0, Scalar(255),2);
}

void HandGesture::dbgVisualization(void)
{
#define MARGIN 80
	int iterCnt = iFingerCount;
	int yMargin = 400;
	int xMargin = 20;
	//draw COG
	circle(imgOrigin, cog, 2, Scalar(0, 0, 255), -1);

	string strMsg;
	strMsg = format("Finger Count %d", iFingerCount);
	putText(imgOrigin, strMsg, Point(20, 20), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 0));

	putText(imgOrigin, "Position", Point(xMargin, yMargin), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0));
	putText(imgOrigin, "Angle", Point(xMargin, yMargin + 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0));
	putText(imgOrigin, "Width", Point(xMargin, yMargin + 40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0));

	for (int i = 0; i < iterCnt; i++)
	{
		switch (vecFingerPos[i])
		{
		case THUMB:
			strMsg = "THUMB";
			xMargin = 20 + MARGIN * 1;
			break;
		case INDEX:
			strMsg = "INDEX";
			xMargin = 20 + MARGIN * 2;
			break;
		case MIDDLE:
			strMsg = "MIDDLE";
			xMargin = 20 + MARGIN * 3;
			break;
		case RING:
			strMsg = "RING";
			xMargin = 20 + MARGIN * 4;
			break;
		case BABY:
			strMsg = "BABY"; 
			xMargin = 20 + MARGIN * 5;
			break;
			
		case ERR:
			strMsg = "ERROR";
			xMargin = 20 + MARGIN * 6;
			break;
		case 0:
			break;
		}
		putText(imgOrigin, strMsg, Point(xMargin, yMargin), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(30, 70, 255));
		
		strMsg = cv::format("%d", vecFingerAngle[i]);
		putText(imgOrigin, strMsg, Point(xMargin, yMargin + 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(100, 0, 0));
		
		strMsg = cv::format("%d", vecFingerWidth[i]);
		putText(imgOrigin, strMsg, Point(xMargin, yMargin + 40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(100, 0, 0));
	}

	for (int i = 0; i < vecFingerPts.size(); i++)
	{
		circle(imgOrigin, vecFingerPts[i], 2, Scalar(30,30,255), -1);
	}

	circle(imgSkin, cog, iCriterionDist, Scalar(128));
}

void HandGesture::testFunc()
{
	Point pt;
	for (int i = 0; i < 5; i++)
	{
		float fAngle = (float)(fPosInfo.fingerAngle[i]) * (CV_PI / 180.0);
		pt.x = cog.x + iDTRadius * cos(fAngle);
		pt.y = cog.y - iDTRadius * sin(fAngle);
#ifdef __DEBUGVISUAL__
		circle(imgOrigin, pt, 3, Scalar(123, 255, 35), -1);
#endif
	}
}

int HandGesture::getResolution(int resolution)
{
	float ratio = 0.25;
	int newResolution = (float)iDTRadius * ratio;
	
	return newResolution;
}

void HandGesture::makeGestureInform(GestureInfo & gInfo)
{
	gInfo.cog = cog;
	gInfo.vecFingerPts = vecFingerPts;
	int gData[5] = { 0, };
	int sum = 0;

	for (int i = 0; i < 5; i++)
	{
		switch (vecFingerPos[i])
		{
		case THUMB:
			gData[0] = 1;
			break;
		case INDEX:
			gData[1] = 1;
			break;
		case MIDDLE:
			gData[2] = 1;
			break;
		case RING:
			gData[3] = 1;
			break;
		case BABY:
			gData[4] = 1;
			break;
		case ERR:
			break;
		case 0:
			break;
		}
	}

	for (int i = 0; i < 5; i++)
	{
		sum |= (gData[i] << i);
	}

	if (sum == 2)
		gInfo.gesture = G1;
	else if (sum == 3 || sum == 5 || sum == 6)
		gInfo.gesture = G2;
	else if (sum == 19 || sum == 11)
		gInfo.gesture = G3;
	else if (sum == 16 || sum == 8)
		gInfo.gesture = G4;
	else
		gInfo.gesture = UNKNOWN;
}

void getRatio(int radius, int* min, int* max)
{
	float ratioT = 9.0 / 4.0 * (float)radius;
	*min = (int)ratioT;
	ratioT = 12.0 / 4.0* (float)radius;
	*max = (int)ratioT;
}


 

void errorException(const char * str, int line)
{
	printf("[ERROR] FUNCTION: [%s] | LINE: %d\n", str, line);
	exit(1);
}

void errorException(const char * str, int line, const char * msg)
{
	printf("[ERROR] FUNCTION: [%s] | LINE: %d\nMESSAGE : %s\n", str, line, msg);
	exit(1);
}

RefInfo::RefInfo(void)
{
	refCount = 0;
}

void RefInfo::addIdx(int idx)
{
	this->idx.push_back(idx);
	refCount++;
}

WidthInfo::WidthInfo(int area)
{
	this->area = area;
	count = 1;
}

GestureInfo::GestureInfo()
{
	cog.x = 0;
	cog.y = 0;
	vecFingerPts.clear();
	gesture = ERR;
}