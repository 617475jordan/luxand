#pragma once
#include<LuxandFaceSDK.h>
#include <opencv_all.h>
class hbitmapAndMat
{
public:
	hbitmapAndMat();
	~hbitmapAndMat();
	Mat HbitMapToMat(HBITMAP& _hBmp);
	HBITMAP MatToHBitmap(Mat m_matImg);
};

