#include "stdafx.h"
/*bool HandGesture::main_loop(void)
{
	HandGesture hg;
	//hg.openVideo("test3.mp4");
	hg.openDevice(0);
	cv::namedWindow("origin");
	cv::namedWindow("binary");
	int cnt = 0;
	while (true)
	{
		cnt++;
		if (!hg.readImage())
			break;

		hg.gaussianBlur();
		hg.skinColorExtraction();
		hg.morphologyOperation(2);
		
		if (!hg.drawHandRegion())
		{
			hg.displayImage();
			if (cvWaitKey(3) > 0)
				break;
			continue;
		}


		if (!hg.handValidation()) 
		{
			hg.displayImage();
			hg.destroyObject();
			hg.iAramLastAngle = 0;
			hg.initFingerCnt = 0;

			if (cvWaitKey(3) > 0)
				break;
			continue;
		}
			
		hg.getMoment();
		hg.getDistanceTransform();
		if (!hg.featureExtraction())
		{
			hg.displayImage();
			if (cvWaitKey(3) > 0)
				break;
			continue;
		}

		if (!hg.gestureClassification())
		{
			hg.displayImage();
			hg.destroyObject();
			continue;
		}

		hg.dbgVisualization();
		hg.testFunc();
		hg.displayImage();
		hg.destroyObject();
		
		if (cvWaitKey(3) > 0)
			break;
	}
}*/
