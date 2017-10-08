#include "Header.h"

int main()
{
	int choice;
	if (!initialize("..\\data"))
	{
		loadImgPath("..\\Img");
		m_luxandFace->initalImage(m_vecStrImgPath);
		m_imgData = m_luxandFace->imgData();
	}
	while (1)
	{
		std::cout << "Input 1 to build database index, 2 to test image,3 to exit: ";
		std::cin >> choice;

		switch (choice)
		{
		case 1:
			loadImgPath("..\\Img");
			m_luxandFace->initalImage(m_vecStrImgPath);
			m_imgData = m_luxandFace->imgData();
			break;
		case 2:
			run();
			break;
		case 3:
			cout << "exit" << endl;
			return 0;
		default:
			cout << "Input Num is error!" << endl;
		}
	}
	
	return 0;
}

void  run()
{

	int CameraCount;
	wchar_t ** CameraList;
	if (0 == FSDK_GetCameraList(&CameraList, &CameraCount))
	for (int i = 0; i < CameraCount; i++)
		wprintf(L"camera: %s\n", CameraList[i]);

	if (0 == CameraCount)
	{
		MessageBox(0, L"Please attach a camera", L"Error", MB_ICONERROR | MB_OK);
		return ;
	}

	int CameraIdx = 0; // choose the first camera
	int VideoFormatCount = 0;
	FSDK_VideoFormatInfo * VideoFormatList = 0;
	FSDK_GetVideoFormatList(CameraList[CameraIdx], &VideoFormatList, &VideoFormatCount);
	for (int i = 0; i < VideoFormatCount; i++)
		printf("format %d: %dx%d, %d bpp\n", i, VideoFormatList[i].Width, VideoFormatList[i].Height, VideoFormatList[i].BPP);

	int VideoFormat = 0; // choose a video format
	int width = VideoFormatList[VideoFormat].Width;
	int height = VideoFormatList[VideoFormat].Height;
	int bitsPerPixel = VideoFormatList[VideoFormat].BPP;
	FSDK_SetVideoFormat(CameraList[CameraIdx], VideoFormatList[VideoFormat]);

	printf("Trying to open the first camera...\n");
	int cameraHandle = 0;
	if (FSDKE_OK != FSDK_OpenVideoCamera(CameraList[CameraIdx], &cameraHandle))
	{
		MessageBox(0, L"Error opening the first camera", L"Error", MB_ICONERROR | MB_OK);
		return ;
	}

	// creating a Tracker
	HTracker tracker = 0;
	FSDK_CreateTracker(&tracker);

	int err = 0; // set realtime face detection parameters
	FSDK_SetTrackerMultipleParameters(tracker, "RecognizeFaces=true; DetectAge=true; DetectGender=true; HandleArbitraryRotations=false; DetermineFaceRotationAngle=false; InternalResizeWidth=200; FaceDetectionThreshold=5", &err);
	FSDK_SetFaceDetectionParameters(false, false, 200);
	FSDK_SetFaceDetectionThreshold(5);

	FSDK_FaceTemplate  m_currentface;
	float m_fSimilarity = 0;
	long long  *reassignedID = NULL;
	//long long  *count = NULL;
	Mat m_matImg;
	while (1)
	{
		int m_iCurrent = clock();
		HImage imageHandle;
		if (FSDK_GrabFrame(cameraHandle, &imageHandle) == FSDKE_OK)
		{ // grab the current frame from the camera

			long long faceCount = 0;
			long long IDs[256];
			FSDK_FeedFrame(tracker, 0, imageHandle, &faceCount, IDs, sizeof(IDs));

			HBITMAP hbitmapHandle; // to store the HBITMAP handle
			FSDK_SaveImageToHBitmap(imageHandle, &hbitmapHandle);
			hbitmapAndMat *m_hbitmapAndMat = new hbitmapAndMat();
			m_matImg = m_hbitmapAndMat->HbitMapToMat(hbitmapHandle);
			m_hbitmapAndMat = NULL;
			delete[] m_hbitmapAndMat;

			for (int i = 0; i < faceCount; i++)
			{
				TFacePosition facePosition;
				FSDK_GetTrackerFacePosition(tracker, 0, IDs[i], &facePosition);

				int x1 = facePosition.xc - (int)(facePosition.w*0.6);
				int y1 = facePosition.yc - (int)(facePosition.w*0.5);
				int x2 = facePosition.xc + (int)(facePosition.w*0.6);
				int y2 = facePosition.yc + (int)(facePosition.w*0.7);
				cv::Rect m_rect(x1, y1, x2 - x1, y2 - y1);
				cv::rectangle(m_matImg, m_rect, Scalar(255, 255, 0));
				char AttributeValues[1024];

				float AgeValue = 0.0f;
				float ConfidenceMale = 0.0f;
				float ConfidenceFemale = 0.0f;
				int res = 0;

				res = FSDK_GetTrackerFacialAttribute(tracker, 0, IDs[i], "Age", AttributeValues, sizeof(AttributeValues));
				FSDK_GetValueConfidence(AttributeValues, "Age", &AgeValue);

				res = FSDK_GetTrackerFacialAttribute(tracker, 0, IDs[i], "Gender", AttributeValues, sizeof(AttributeValues));
				FSDK_GetValueConfidence(AttributeValues, "Male", &ConfidenceMale);
				FSDK_GetValueConfidence(AttributeValues, "Female", &ConfidenceFemale);

				FSDK_GetFaceTemplate(tracker, &m_currentface);

				int m_igender=-1;
				if (ConfidenceMale > ConfidenceFemale)
				{
					m_igender = 0;
				}
				else if (ConfidenceMale < ConfidenceFemale)
				{
					m_igender = 1;
				}
				else
				{
					m_igender = -1;
				}
				float m_fMaxSimilarity = 0;
				int   m_iId = 0;
				for (unsigned int j = 0; j<m_imgData.m_vecFaceTemplate.size(); j++)
				{
					if (abs(AgeValue - m_imgData.m_fAge[i])>8||m_igender!=m_imgData.m_Gender[i])
					{
						continue;
					}
					FSDK_MatchFaces(&m_imgData.m_vecFaceTemplate[j], &m_currentface, &m_fSimilarity);
					if (m_fSimilarity>m_fMaxSimilarity)
					{
						m_fMaxSimilarity = m_fSimilarity;
						m_iId = j;
					}
				}

				cout << "m_fSimilarity:" << m_fMaxSimilarity << endl;
				char str[1024];
				sprintf_s(str, sizeof(str)-1, "Age: %d; %s, %d%%,simlarity:%.2f;ID:%d", (int)AgeValue, (ConfidenceMale > ConfidenceFemale ? "Male" : "Female"),
					ConfidenceMale > ConfidenceFemale ? (int)(ConfidenceMale * 100) : (int)(ConfidenceFemale * 100), m_fMaxSimilarity, m_iId);
				putText(m_matImg, str, Point(x1, y2 + 10), 1, 1, Scalar(255, 0, 255));

			}
		
			DeleteObject(hbitmapHandle); // delete the HBITMAP object
			FSDK_FreeImage(imageHandle);// delete the FaceSDK image handle

		}
		int m_iCurrentTime = clock() - m_iCurrent;
		std::cout << m_iCurrentTime << "ms" << std::endl;
		double m_fFps = 1000.0 / m_iCurrentTime;
		char str[1024];
		sprintf_s(str, "fps:%.1f",m_fFps);
		putText(m_matImg, str, Point(10, 10), 1, 1, Scalar(255, 0, 255));
		if (!m_matImg.empty())
		{
			cv::imshow("demo", m_matImg);
			waitKey(1);
		}
	}

	//ReleaseDC(hwnd, dc);
	FSDK_FreeTracker(tracker);

	if (FSDKE_OK != FSDK_CloseVideoCamera(cameraHandle))
	{
		MessageBox(0, L"Error closing camera", L"Error", MB_ICONERROR | MB_OK);
		return ;
	}
	FSDK_FreeVideoFormatList(VideoFormatList);
	FSDK_FreeCameraList(CameraList, CameraCount);

	FSDK_FinalizeCapturing();
	FSDK_Finalize();
	reassignedID = NULL;
	delete[] reassignedID;
	m_luxandFace = NULL;
	delete[] m_luxandFace;
	m_imgData.clear();
	m_vecStrImgPath.clear();
}
bool  initialize(string m_strModelPath)
{
	if (FSDKE_OK != FSDK_ActivateLibrary(activateLibrary))
	{
		MessageBox(0, L"Please run the License Key Wizard (Start - Luxand - FaceSDK - License Key Wizard)\n",
			L"Error activating FaceSDK", MB_ICONERROR | MB_OK);
		return false;
	}
	FSDK_Initialize("");
	FSDK_InitializeCapturing();

	getFiles *m_getFiles = new getFiles();
	vector<string> m_vecStrFileType;
	m_vecStrFileType.push_back("index");
	m_getFiles->getSpecialType(m_strModelPath, m_vecStrDatalFilePath, m_vecStrFileType);
	m_getFiles = NULL;
	delete[] m_getFiles;
	if (!m_vecStrDatalFilePath.size())
	{
		return false;
	}
	else
	{
		cout << "-----------------Initialize...-----------------" << endl;
		int  m_tTime = clock();
		std::ifstream file;
		file.open(m_vecStrDatalFilePath[m_vecStrDatalFilePath.size() - 1]);
		int m_ImgNum;
		file >> m_ImgNum;
		char* buffer = new char[1000];

		for (int i = 0; i < m_ImgNum; i++)
		{
			file.getline(buffer, 1000);
			while (buffer[0] == '\0' || buffer[0] == ' ')
			{
				file.getline(buffer, 1000);
			}
			
			string m_str = buffer;
			m_imgData.m_strImgPath.push_back(m_str);
 
			FSDK_FaceTemplate m_faceTmp;
			for (int j = 0; j < m_featureNum; j++)
			{
				float m_fFaceTmp;
				file >> m_fFaceTmp;
				m_faceTmp.ftemplate[j] = m_fFaceTmp;
			}
			m_imgData.m_vecFaceTemplate.push_back(m_faceTmp);
			float m_fAge;
			file >> m_fAge;
			m_imgData.m_fAge.push_back(m_fAge);
			int m_fGender;
			file >> m_fGender;
			m_imgData.m_Gender.push_back(m_fGender);
			if ((i+1) % 5 == 0)
			{
				cout << "The Processing:" << 100 * i*1.0 / m_ImgNum << "%" << endl;
				cout << buffer << endl;
			}
		}
		cout << "The Processing:" << 100  << "%" << endl;
		cout << "-----------Loading is Successful! cost time:" << clock() - m_tTime << "ms" << "---------------" << endl;
		cout << "-----------------All are over-----------------" << endl;
		return true;
	}
}

void loadImgPath(string m_strImgPath)
{
	m_vecStrImgPath.clear();
	getFiles *m_getFiles = new getFiles();
	vector<string> m_vecStrFileType;
	m_vecStrFileType.push_back("png");
	m_vecStrFileType.push_back("jpg");
	m_vecStrFileType.push_back("bmp");
	m_getFiles->getSpecialType(m_strImgPath, m_vecStrImgPath, m_vecStrFileType);
	m_getFiles = NULL;
	delete[] m_getFiles;

}

