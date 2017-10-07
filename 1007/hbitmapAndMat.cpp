#include "hbitmapAndMat.h"


hbitmapAndMat::hbitmapAndMat()
{
}


hbitmapAndMat::~hbitmapAndMat()
{
}


Mat hbitmapAndMat::HbitMapToMat(HBITMAP& _hBmp)
{
	BITMAP bmp;
	GetObject(_hBmp, sizeof(BITMAP), &bmp);
	int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
	int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;
	Mat v_mat;
	v_mat.create(cvSize(bmp.bmWidth, bmp.bmHeight), CV_MAKETYPE(CV_8U, nChannels));
	GetBitmapBits(_hBmp, bmp.bmHeight*bmp.bmWidth*nChannels, v_mat.data);
	return v_mat;
}

HBITMAP hbitmapAndMat::MatToHBitmap(Mat m_matImg)
{
	HBITMAP m_HBITMAP;
	//MATÀàµÄTYPE=£¨nChannels-1+ CV_8U£©<<3
	int nChannels = (m_matImg.type() >> 3) - CV_8U + 1;
	int iSize = m_matImg.cols*m_matImg.rows*nChannels;
	m_HBITMAP = CreateBitmap(m_matImg.cols, m_matImg.rows,
		1, nChannels * 8, m_matImg.data);
	return m_HBITMAP;
}
