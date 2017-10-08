#include "luxandFace.h"


luxandFace::luxandFace()
{
}


luxandFace::~luxandFace()
{
}

void luxandFace::initalImage(vector<string> m_vecStrFilePath)
{
	cout << "-----------------Initialize...-----------------" << endl;
	int  m_tTime = clock();
	for (unsigned int i = 0; i < m_vecStrFilePath.size(); i++)
	{
		HImage  m_Image;
		const char *m_charImgPath = m_vecStrFilePath[i].c_str();
		if (FSDK_LoadImageFromFile(&m_Image, m_charImgPath) != FSDKE_OK)
		{
			cout << "No Image" << endl;
			cout << i << endl;
			m_charImgPath = NULL;
			delete[] m_charImgPath;
			continue;
		}
		m_charImgPath = NULL;
		delete[] m_charImgPath;
		FSDK_FaceTemplate m_originalFace;
		FSDK_GetFaceTemplate(m_Image, &m_originalFace);
		m_ImgData.m_vecFaceTemplate.push_back(m_originalFace);
		m_ImgData.m_strImgPath.push_back(m_vecStrFilePath[i]);
		FSDK_FreeImage(m_Image);	
	}

	time_t m_currentTime = time(0);
	struct tm * timeinfo;
	timeinfo = localtime(&m_currentTime);
	char m_charDstModelName[100];
	sprintf(m_charDstModelName, m_strModelName, timeinfo->tm_year, timeinfo->tm_mon + 1,
		timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);


	std::ofstream file;
	file.open(m_charDstModelName);
	file << m_ImgData.m_strImgPath.size() << endl;
	for (unsigned int i = 0; i < m_ImgData.m_strImgPath.size(); i++)
	{
		file << m_ImgData.m_strImgPath[i] << endl;
		for (int j = 0; j < m_featureNum; j++)
		{
			float m_fFaceTmp;
			m_fFaceTmp=m_ImgData.m_vecFaceTemplate[i].ftemplate[j];
			file << m_fFaceTmp << " ";
		}
		file << std::endl;
		if ((i + 1) % 5 == 0)
		{
			cout << "The Processing:" << i*1.0 / m_ImgData.m_strImgPath.size()*100 <<"%"<< endl;
			//cout << m_ImgData.m_strImgPath[i] << endl;
		}
		const char *m_charImgPath = m_vecStrFilePath[i].c_str();
		HImage  m_Image;
		if (FSDK_LoadImageFromFile(&m_Image, m_charImgPath) != FSDKE_OK)
		{
			continue;
		}
		FSDK_Features m_features;
		char AttributeValues[1024];

		float AgeValue = 0.0f;
		FSDK_DetectFacialFeatures(m_Image, &m_features);
		FSDK_DetectFacialAttributeUsingFeatures(m_Image, &m_features, "Age", AttributeValues, sizeof(AttributeValues));
		FSDK_GetValueConfidence(AttributeValues, "Age", &AgeValue);
		file << AgeValue << endl;

		float ConfidenceMale = 0.0f;
		float ConfidenceFemale = 0.0f;
		FSDK_DetectFacialAttributeUsingFeatures(m_Image, &m_features, "Gender", AttributeValues, sizeof(AttributeValues));
		FSDK_GetValueConfidence(AttributeValues, "Male", &ConfidenceMale);
		FSDK_GetValueConfidence(AttributeValues, "Female", &ConfidenceFemale);
		if (ConfidenceMale > ConfidenceFemale)
		{
			file << 0 << endl;
		}
		else if(ConfidenceMale < ConfidenceFemale)
		{
			file << 1 << endl;
		}
		else
		{
			file << -1 << endl;
		}
		FSDK_FreeImage(m_Image);
	}
	file.close();
	cout << "The Processing:" <<  100 << "%" << endl;
	cout << "-----------------cost time:" << clock() - m_tTime <<"ms"<<"----------------" << endl;
	cout << "-----------------All are over-----------------" << endl;
}

ImgData luxandFace::imgData()
{
	return m_ImgData;
}
