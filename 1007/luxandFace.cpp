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
	for (int i = 0; i < m_vecStrFilePath.size(); i++)
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
	for (int i = 0; i < m_ImgData.m_strImgPath.size(); i++)
	{
		file << m_ImgData.m_strImgPath[i] << endl;
		for (int j = 0; j < 13324; j++)
		{
			float m_value;
			m_value=m_ImgData.m_vecFaceTemplate[i].ftemplate[j];
			file << m_value << " ";
		}
		file << std::endl;
	}
	file.close();
	cout << "-----------------cost time:" << clock() - m_tTime <<"ms"<<"----------------" << endl;
	cout << "-----------------All are over-----------------" << endl;
}

ImgData luxandFace::imgData()
{
	return m_ImgData;
}
