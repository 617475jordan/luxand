#pragma once
#include <opencv_all.h>
#include<LuxandFaceSDK.h>

struct ImgData
{
	vector<string>            m_strImgPath;
	vector<FSDK_FaceTemplate> m_vecFaceTemplate;
	vector<float>             m_fAge;
	vector<int>               m_Gender;
	void clear()
	{
		m_strImgPath.clear();
		m_vecFaceTemplate.clear();
		m_fAge.clear();
		m_Gender.clear();
	}
};
class luxandFace
{
public:
	luxandFace();
	~luxandFace();

	void initalImage(vector<string>  m_vecStrFilePath);
	ImgData imgData();
public:
	ImgData m_ImgData;
#define  m_strModelName "..//data//%d%d%d%d%d%d.index"
#define  m_featureNum 13324
};

