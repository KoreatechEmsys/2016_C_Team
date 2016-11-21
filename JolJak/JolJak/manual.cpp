#include "stdafx.h"

#define VIDEO_0 "manual\\0.wmv"
#define VIDEO_1 "manual\\1.mp4"
#define VIDEO_2 "manual\\2.mp4"
#define VIDEO_3 "manual\\3.mp4"
#define VIDEO_4 "manual\\4.mp4"
#define VIDEO_5 "manual\\5.mp4"
#define IMAGE_0 "manual\\list.png"

void playManual()
{
	VideoCapture mVC;
	namedWindow("manual");
	Mat videoImage;
	int videoSeq = 0;
	bool breakSignal = false;
	while (true)
	{
		switch (videoSeq)
		{
		case 0:
			mVC = VideoCapture(VIDEO_0);
			break;
		case 1:
			mVC = VideoCapture(VIDEO_1);
			break;
		case 2:
			mVC = VideoCapture(VIDEO_2);
			break;
		case 3:
			mVC = VideoCapture(VIDEO_3);
			break;
		case 4:
			mVC = VideoCapture(VIDEO_4);
			break;
		case 5:
			mVC = VideoCapture(VIDEO_5);
		case 6:
			videoImage = imread(IMAGE_0);
			break;
		}

		if (mVC.isOpened() == false && videoSeq < 6)
		{
			MessageBox(NULL, _T("video open error"), NULL, NULL);
			return;
		}

		if (videoSeq < 6)
		{
			while (mVC.read(videoImage) == true)
			{
				imshow("manual", videoImage);
				if (cvWaitKey(25) > 0)
					breakSignal = true;

				if (breakSignal)
					break;
			}

			if (breakSignal)
				break;
			videoSeq++;
		}
		else
		{
			imshow("manual", videoImage);
			
			if (cvWaitKey() == 'R' || cvWaitKey() == 'r')
			{
				videoSeq = 0;
				continue;
			}
			else
				break;
		}
	}
	
	destroyWindow("manual");
}