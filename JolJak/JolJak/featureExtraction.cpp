#include "stdafx.h"

bool HandGesture::featureExtraction()
{
	int resolution = iResolution;

	int rows = imgOrigin.rows;
	int cols = imgOrigin.cols;
	int lastLabelCount = 0;
	int labelCount = 0;
	int maxRes = sqrt((cols / 2 * cols / 2) + (rows / 2 * rows / 2)) - 5;
	Mat imgMasking(rows, cols, CV_8UC1, Scalar(0));

	vector<LabelInfo> labelInfo;

	while (iCircleRadius < maxRes)
	{
		labelInfo.clear();
		concentricCircleMasking(imgSkin, imgMasking, cog, iCircleRadius, rows, cols);
		if ((labelCount = getConnectedComponentInfo(labelInfo, cog, &imgMasking)) < 1)
		{
			if (lastLabelCount > 0)
			{
				if (labelCount < lastLabelCount)
					labelProcess(labelInfo, DEC);
				else if (labelCount == lastLabelCount)
					labelProcess(labelInfo, SAME);
				else
					errorException(__FUNCTION__, __LINE__, "labelCount unexpected case in lastLoop");

				break;
			}
			else
				break;
		}

		if (labelCount > MAX) //개복치 방지용 테스트 코드1
			return false;
#ifdef __DBGVIS__
		for (int i = 0; i < labelInfo.size(); i++)
		{
			circle(imgOrigin, labelInfo[i].centroid, 2, Scalar(255, 0, 0), -1);
		}
#endif // __DBGVIS__

		if (labelCount > lastLabelCount)
		{
			if (iCircleRadius < iCriterionDist * 1.15)
				labelProcess(labelInfo, INC);
			else
				labelProcess(labelInfo, INC2);

			resolution = getResolution(resolution);
		}
		else if (labelCount < lastLabelCount)
		{
			labelProcess(labelInfo, DEC);
			resolution = getResolution(resolution);
		}
		else if (labelCount == lastLabelCount)
		{
			labelProcess(labelInfo, SAME);
			resolution = getResolution(resolution);
		}
		else
			errorException(__FUNCTION__, __LINE__, "labelCount unexpected case");

		lastLabelCount = labelInfo.size();


		iCircleRadius += resolution;
	}
	return true;
}

int HandGesture::getConnectedComponentInfo(vector<LabelInfo>& lInfo, Point cog, Mat* pCImg)
{
	Mat label;
	Mat stats;
	Mat centroids;
	int numOfLabels = 0;
	int angle = 0;
	int dx = 0, dy = 0, x = 0, y = 0;


	numOfLabels = connectedComponentsWithStats(*pCImg, label, stats, centroids);

	for (int i = 1; i < numOfLabels; i++)
	{
		x = (int)centroids.at<double>(i, 0);
		y = (int)centroids.at<double>(i, 1);
		dx = x - cog.x;
		dy = cog.y - y;
		angle = fastAtan2(dy, dx);
		
		float theta = 0.5 * atan((2 * imgMoment.mu11) / (imgMoment.mu20 - imgMoment.mu02));
		theta = (theta / CV_PI) * 180;

		if (angle < 180 - theta)
			lInfo.push_back(LabelInfo(x, y, stats.at<int>(i, 4), angle));
	}
	return lInfo.size();
}

void HandGesture::labelProcess(vector<LabelInfo>& lInfo, uchar operation)
{
	int iterCnt = lInfo.size();
	int iterCnt2 = vecFingerPtsT.size();

	int dx = 0;
	int dy = 0;
	int offset = 0;

	RefInfo rInfo[MAX];

	switch (operation)
	{
	case INC:
		vecFingerAngleT.clear();
		vecFingerWidthT.clear();
		vecFingerPtsT.clear();
		vecFirstFingerPtsT.clear();
		for (int i = 0; i < iterCnt; i++)
		{
			vecFingerWidthT.push_back(WidthInfo(lInfo[i].area));
			vecFingerPtsT.push_back(lInfo[i].centroid);
			vecFingerAngleT.push_back(lInfo[i].angle);
			vecFirstFingerPtsT.push_back(lInfo[i].centroid);
		}
		break;
	case INC2:
	case DEC:
	case SAME:
		for (int i = 0; i < iterCnt; i++)
		{
			int dist = 0, maxDist = 100000;
			int idx = -1;
			for (int j = 0; j < iterCnt2; j++)
			{
				dx = abs(vecFingerPtsT[j].x - lInfo[i].centroid.x);
				dy = abs(vecFingerPtsT[j].y - lInfo[i].centroid.y);
				dist = dx + dy;

				if (dist < maxDist)
				{
					maxDist = dist;
					idx = j;
				}
			}

			if (idx == -1)
				errorException(__FUNCTION__, __LINE__, "negative idx value");

			rInfo[idx].addIdx(i);
		}


		for (int i = 0; i < iterCnt2; i++)
		{
			if (rInfo[i].refCount == 1) //refCount가 1일때
			{
				int idx = rInfo[i].idx[0];
				vecFingerPtsT[(i + offset)] = lInfo[idx].centroid;
				vecFingerWidthT[(i + offset)].area = (lInfo[idx].area + vecFingerWidthT[(i + offset)].area);
				vecFingerWidthT[(i + offset)].count++;
			}
			else if (rInfo[i].refCount > 1) //refCount가 1보다 클때.
			{
				int idx = 0;
				int iterT = rInfo[i].idx.size();
				int sumX = 0, sumY = 0;
				int sumWidth = 0;

				for (int j = 0; j < iterT; j++)
				{
					idx = rInfo[i].idx[j];
					sumX += lInfo[idx].centroid.x;
					sumY += lInfo[idx].centroid.y;
					sumWidth += lInfo[idx].area;
				}
				vecFingerPtsT[(i + offset)] = Point((sumX / iterT), (sumY / iterT));
				vecFingerWidthT[(i + offset)].area += sumWidth; //이거 고민좀 하자!!!!!!!
				vecFingerWidthT[(i + offset)].count++;
			}
			else if (rInfo[i].refCount == 0) //refCount가 0일때
			{
				int temp = i + offset;

				vecFingerPts.push_back(vecFingerPtsT[(temp)]);
				vecFingerAngle.push_back(vecFingerAngleT[(temp)]);
				vecFingerWidth.push_back(WidthInfo(vecFingerWidthT[(temp)].area / vecFingerWidthT[(temp)].count)); //이부분이 약간 야시꾸리하다.....
				vecFirstFingerPts.push_back(vecFirstFingerPtsT[temp]);

				vecFingerAngleT.erase((vecFingerAngleT.begin() + temp));
				vecFingerWidthT.erase((vecFingerWidthT.begin() + temp));
				vecFingerPtsT.erase((vecFingerPtsT.begin() + temp));
				vecFirstFingerPtsT.erase((vecFirstFingerPtsT.begin() + temp));

				offset--;
			}
		}
		break;
	default:
		errorException(__FUNCTION__, __LINE__, "default");
	}

#ifdef __DBGVIS__
	for (int i = 0; i < vecFingerPtsT.size(); i++)
	{
		circle(imgOrigin, vecFingerPtsT[i], 2, Scalar(128, 0, 0), -1);
	}
#endif // __DBGVIS__
}

bool HandGesture::gestureClassification(void)
{
	int iterCnt = vecFingerPts.size();
	int idx = 0;
	int offset = 0;
	int dx = 0, dy = 0;
	int dist = 0;
	int fingerDist = 0;
	int angle = 0;
	float fAngle = 0.0;
	string str;

	Point ptT;

	float theta = 0.5 * atan((2 * imgMoment.mu11) / (imgMoment.mu20 - imgMoment.mu02));
	theta = (theta / CV_PI) * 180;
	iArmAngle = (int)theta;

	if (iterCnt > 5)
		return false;

	for (int i = 0; i < iterCnt; i++)
	{
		idx = i + offset;
		dx = vecFingerPts[idx].x - cog.x;
		dy = vecFingerPts[idx].y - cog.y;
		dist = sqrt(dx*dx + dy*dy);

		if (dist < iCriterionDist)
		{

			vecFingerAngle.erase((vecFingerAngle.begin() + idx));
			vecFingerWidth.erase((vecFingerWidth.begin() + idx));
			vecFingerPts.erase((vecFingerPts.begin() + idx));
			vecFirstFingerPts.erase((vecFirstFingerPts.begin() + idx));

			offset--;
		}
		else
		{
			dx = vecFingerPts[idx].x - vecFirstFingerPts[idx].x;
			dy = vecFirstFingerPts[idx].y - vecFingerPts[idx].y;
			fingerDist = sqrt(dx*dx + dy*dy);

			dx = vecFingerPts[idx].x - cog.x;
			dy = vecFingerPts[idx].y - cog.y;
			dist = sqrt(dx*dx + dy*dy);
			dist = dist - fingerDist - iDTRadius;

			dx = vecFingerPts[idx].x - vecFirstFingerPts[idx].x;
			dy = vecFirstFingerPts[idx].y - vecFingerPts[idx].y;
			fAngle = fastAtan2(dy, dx);
			angle = fAngle;

			fAngle = fAngle * (CV_PI / 180.0);
			ptT.x = vecFirstFingerPts[idx].x - dist * cos(fAngle);
			ptT.y = vecFirstFingerPts[idx].y + dist * sin(fAngle);

			dx = ptT.x - cog.x;
			dy = cog.y - ptT.y;

			vecFingerAngle[idx] = fastAtan2(dy, dx);

			vecFingerAngle[idx] = (vecFingerAngle[idx]);
			iFingerCount++;

#ifdef __DBGVIS__
			//line(imgOrigin, vecFingerPts[idx], vecFirstFingerPts[idx], Scalar(255, 0, 0), 1);
			//line(imgOrigin, vecFingerPts[idx], ptT, Scalar(255, 0, 0), 1);
			circle(imgOrigin, ptT, 2, Scalar(0, 255, 255), -1);
			//str = cv::format("%d", angle);
			//putText(imgOrigin, str, vecFirstFingerPts[idx], CV_FONT_HERSHEY_COMPLEX, 0.4, Scalar(192, 192, 192));
#endif // __DBGVIS__	
		}
	}

	iterCnt = vecFingerPts.size();

	if (iterCnt == 5)
	{
		initFingerAngle();
		
		if (!calibration)
		{
			if (calibrationCount == 30)
			{
				calibration = true;
				calibrationCount++;
			}
			else if (calibrationCount < 30)
				calibrationCount++;
		}
	}
	else
	{
		if (calibration)
			updateFingerAnlge();
		else
			return false;
	}

	fingerPositionEstimation();


#ifdef __DBGVIS__
	circle(imgOrigin, cog, iCriterionDist, Scalar(0, 30, 255));
#endif // __DBGVIS__

	return true;
}


bool operator <(const FingerInfo &a, const FingerInfo &b)
{
	return a.fingerAngle > b.fingerAngle;
}


void HandGesture::initFingerAngle(void)
{

	int iterCnt = vecFingerAngle.size();
	vector<FingerInfo> vecFingerInfo;

	
		for (int i = 0; i < iterCnt; i++)
		{
			vecFingerInfo.push_back(FingerInfo(vecFingerAngle[i], vecFingerWidth[i].area));
		}
		sort(vecFingerInfo.begin(), vecFingerInfo.end());

		float ratio1 = 0, ratio2 = 0;

		for (int i = 0; i < iterCnt; i++)
		{
			if (fPosInfo.arrKvalues[i] == 0) {
				fPosInfo.fingerAngle[i] = vecFingerInfo[i].fingerAngle;
				fPosInfo.fingerWidth[i] = vecFingerInfo[i].FingerWidth;
				fPosInfo.arrKvalues[i]++;
			}
			else
			{
				fPosInfo.arrKvalues[i]++;
				//ratio1 = ((float)(fPosInfo.arrKvalues[i] - 1) / fPosInfo.arrKvalues[i]) * fPosInfo.fingerAngle[i];
				//ratio2 = ((float)1.0 / fPosInfo.arrKvalues[i]) *  vecTempAngle[i];
				fPosInfo.fingerAngle[i] = 0.8*fPosInfo.fingerAngle[i] + 0.2*vecFingerInfo[i].fingerAngle;
				fPosInfo.fingerWidth[i] = (0.9*fPosInfo.fingerWidth[i] + 0.1* vecFingerInfo[i].FingerWidth);
			}
		}

		
}

void HandGesture::updateFingerAnlge(void)
{
	for (int i = 0; i < 5; i++)
	{
		fPosInfo.fingerAngle[i] += (iAramLastAngle - iArmAngle);
	}
}

void HandGesture::fingerPositionEstimation(void)
{
	int iterCnt = vecFingerPts.size();
	int arrMinDist[5] = { -1, };
	int arrMinIdx[5] = { -1, };
	memset(arrMinDist, -1, sizeof(int) * 5);
	memset(arrMinIdx, -1, sizeof(int) * 5);

	for (int i = 0; i < iterCnt; i++)
	{
		int idx = -1;
		int maxDist = 3000;
		int dist = 0;
		for (int j = 0; j < 5; j++)
		{
			dist = abs(vecFingerAngle[i] - fPosInfo.fingerAngle[j]);
			if (maxDist > dist)
			{
				idx = j;
				maxDist = dist;
			}
		}

		if (idx == -1)
			errorException(__FUNCTION__, __LINE__);

		if (arrMinIdx[idx] == -1)
		{
			arrMinIdx[idx] = i;
			arrMinDist[idx] = maxDist;
		}
		else
		{
			if (maxDist < arrMinDist[idx])
			{
				arrMinIdx[idx + 1] = arrMinIdx[idx];
				arrMinDist[idx + 1] = arrMinDist[idx];
				arrMinIdx[idx] = i;
				arrMinDist[idx] = maxDist;
			}
			else
			{
				arrMinIdx[idx + 1] = i;
				arrMinDist[idx + 1] = maxDist;
			}
		}

	}

	vecFingerPos.resize(5);

	for (int i = 0; i < 5; i++)
	{
		if (arrMinIdx[i] != -1) {
			uchar temp;
			switch (i)
			{
			case 0:
				temp = THUMB;
				break;
			case 1:
				temp = INDEX;
				break;
			case 2:
				temp = MIDDLE;
				break;
			case 3:
				temp = RING;
				break;
			case 4:
				temp = BABY;
				break;
			default:
				temp = ERR;
				break;
			}
			vecFingerPos[arrMinIdx[i]] = temp;
		}
	}
}

void concentricCircleMasking(Mat& src, Mat& dst, Point cog, int radius, int rows, int cols)
{
	dst = Scalar(0);

	uchar* pts = src.data;
	uchar* pts2 = dst.data;
	int p;
	int x, y;
	p = 1 - radius;
	x = 0;
	y = radius;

	if (cog.y + y < rows  && cog.x + x < cols)
	{
		if (pts[(cog.y + y)*cols + cog.x + x] == 255)
			pts2[(cog.y + y)*cols + cog.x + x] = 255;
	}

	if (cog.y - y > 0 && cog.x + x < cols)
	{
		if (pts[(cog.y - y)*cols + cog.x + x] == 255)
			pts2[(cog.y - y)*cols + cog.x + x] = 255;
	}

	if (cog.y - x > 0 && cog.x - y >0)
	{
		if (pts[(cog.y - x)*cols + cog.x - y] == 255)
			pts2[(cog.y - x)*cols + cog.x - y] = 255;
	}

	if (cog.y - x > 0 && cog.x + y < cols)
	{
		if (pts[(cog.y - x)*cols + cog.x + y] == 255)
			pts2[(cog.y - x)*cols + cog.x + y] = 255;
	}

	while (x < y - 1)
	{
		x++;

		if (p < 0) {
			p += 2 * x + 1;
		}
		else {
			p += 2 * (x - y) + 1;
			y--;
		}

		if (cog.y + y < rows  && cog.x + x < cols)
		{
			if (pts[(cog.y + y)*cols + cog.x + x] == 255)
				pts2[(cog.y + y)*cols + cog.x + x] = 255;
		}

		if (cog.y - y > 0 && cog.x + x < cols)
		{
			if (pts[(cog.y - y)*cols + cog.x + x] == 255)
				pts2[(cog.y - y)*cols + cog.x + x] = 255;
		}

		if (cog.y - x > 0 && cog.x - y >0)
		{
			if (pts[(cog.y - x)*cols + cog.x - y] == 255)
				pts2[(cog.y - x)*cols + cog.x - y] = 255;
		}

		if (cog.y - x > 0 && cog.x + y < cols)
		{
			if (pts[(cog.y - x)*cols + cog.x + y] == 255)
				pts2[(cog.y - x)*cols + cog.x + y] = 255;
		}

		if (cog.y + y < rows && cog.x - x >0)
		{

			if (pts[(cog.y + y)*cols + cog.x - x] == 255)
				pts2[(cog.y + y)*cols + cog.x - x] = 255;
		}

		if (cog.y - y > 0 && cog.x - x > 0)
		{
			if (pts[(cog.y - y)*cols + cog.x - x] == 255)
				pts2[(cog.y - y)*cols + cog.x - x] = 255;
		}

		if (cog.y + x < rows && cog.x + y < cols)
		{
			if (pts[(cog.y + x)*cols + cog.x + y] == 255)
				pts2[(cog.y + x)*cols + cog.x + y] = 255;
		}

		if (cog.y + x < rows && cog.x - y > 0)
		{
			if (pts[(cog.y + x)*cols + cog.x - y] == 255)
				pts2[(cog.y + x)*cols + cog.x - y] = 255;
		}
	}
}

FingerPosInfo::FingerPosInfo(void)
{
	memset(this->fingerAngle, 0, sizeof(this->fingerAngle));
	memset(this->arrKvalues, 0, sizeof(this->arrKvalues));
	memset(this->fingerWidth, 0, sizeof(this->fingerWidth));
}

LabelInfo::LabelInfo(int x, int y, int area, int angle)
{
	centroid = cv::Point(x, y);
	this->area = area;
	this->angle = angle;
}

FingerInfo::FingerInfo(int angle, int width)
{
	this->fingerAngle = angle;
	this->FingerWidth = width;
}

