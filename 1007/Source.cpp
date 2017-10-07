#include "resource.h"
#include <time.h>
#include "hbitmapAndMat.h"
#include "getFiles.h"
#include "luxandFace.h"

vector<string>      m_vecStrImgPath;
vector<string>      m_vecStrDatalFilePath;
ImgData m_imgData;
luxandFace *m_luxandFace = new luxandFace();
string m_strModelPath = "..\\data";
void initialize();
void loadImgPath(string m_strImgPath);
int getlen(char *result);
int main()
{
	if (FSDKE_OK != FSDK_ActivateLibrary("hbdCpD+tb3dFNk0I9gQ4DrBocUmlpkGbVmg1ausq6mfqb0qF167rpoxxxjTpJkznbDP4WNwgnUJ1DT8oz7QvNAA/0CxFRID26IRrGIbiT8tFpbYGQge/2rvwAbvun6gEdkASuQgpgsZOjUTgr+V1IHlNDSUUbO953CCEzUhzDuw=")) {
		MessageBox(0, L"Please run the License Key Wizard (Start - Luxand - FaceSDK - License Key Wizard)\n", L"Error activating FaceSDK", MB_ICONERROR | MB_OK);
		exit(-1);
	}
	FSDK_Initialize("");
	FSDK_InitializeCapturing();

	int CameraCount;
	wchar_t ** CameraList;
	if (0 == FSDK_GetCameraList(&CameraList, &CameraCount))
	for (int i = 0; i < CameraCount; i++)
		wprintf(L"camera: %s\n", CameraList[i]);

	if (0 == CameraCount) {
		MessageBox(0, L"Please attach a camera", L"Error", MB_ICONERROR | MB_OK);
		return -1;
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
		return -2;
	}

	// creating a Tracker
	HTracker tracker = 0;
	FSDK_CreateTracker(&tracker);

	int err = 0; // set realtime face detection parameters
	FSDK_SetTrackerMultipleParameters(tracker, "RecognizeFaces=true; DetectAge=true; DetectGender=true; HandleArbitraryRotations=false; DetermineFaceRotationAngle=false; InternalResizeWidth=200; FaceDetectionThreshold=5", &err);
	FSDK_SetFaceDetectionParameters(false, false, 200);
	FSDK_SetFaceDetectionThreshold(5);

	FSDK_FaceTemplate  m_currentface;
	
	
	initialize();
	float m_fSimilarity = 0;
	
	long long  *reassignedID=NULL;
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
				//FSDK_GetIDReassignment(tracker, IDs[i], reassignedID);

				int x1 = facePosition.xc - (int)(facePosition.w*0.6);
				int y1 = facePosition.yc - (int)(facePosition.w*0.5);
				int x2 = facePosition.xc + (int)(facePosition.w*0.6);
				int y2 = facePosition.yc + (int)(facePosition.w*0.7);
				//Rectangle(dc, x1, 16 + y1, x2, 16 + y2);
				cv::Rect m_rect(x1, y1, x2 - x1, y2 - y1);
				cv::rectangle(m_matImg, m_rect, Scalar( 255, 255,0));
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

				//FSDK_GetFaceTemplateInRegion(tracker, &m_currentPosition, &m_currentface);
				FSDK_GetFaceTemplate(tracker, &m_currentface);
				//FSDK_DetectFacialFeatures(tracker, &m_currentFeatures);
				//FSDK_GetFaceTemplateUsingFeatures(tracker, &m_currentFeatures,&m_currentface);
				float m_fMaxSimilarity = 0;
				for (int j = 0; j<m_imgData.m_vecFaceTemplate.size(); j++)
				{
					FSDK_MatchFaces(&m_imgData.m_vecFaceTemplate[j], &m_currentface, &m_fSimilarity);
					if (m_fSimilarity>m_fMaxSimilarity)
					{
						m_fMaxSimilarity = m_fSimilarity;
					}
				}
				
				cout << "m_fSimilarity:" << m_fMaxSimilarity << endl;
				char str[1024];
				sprintf_s(str, sizeof(str)-1, "Age: %d; %s, %d%%,simlarity:%.2f", (int)AgeValue, (ConfidenceMale > ConfidenceFemale ? "Male" : "Female"),
					ConfidenceMale > ConfidenceFemale ? (int)(ConfidenceMale * 100) : (int)(ConfidenceFemale * 100), m_fMaxSimilarity);
				

				putText(m_matImg, str, Point(x1, y2 + 10),1,1,Scalar(255,0,255) );
				
				/*FSDK_GetSimilarIDCount(tracker, IDs[i], count);
			
				FSDK_GetIDReassignment(tracker, IDs[i], reassignedID);*/
				
			}
			//resize(m_matImg, m_matImg, Size(m_matImg.cols * 2, m_matImg.rows * 2));
			if (!m_matImg.empty())
			{
				cv::imshow("demo", m_matImg);
				waitKey(1);
			}
			DeleteObject(hbitmapHandle); // delete the HBITMAP object
			FSDK_FreeImage(imageHandle);// delete the FaceSDK image handle
			//FSDK_GetSimilarIDList(tracker,IDs,)
			
		  	
		};

		std::cout << clock() - m_iCurrent << "ms" << std::endl;
	}

	//ReleaseDC(hwnd, dc);
	FSDK_FreeTracker(tracker);

	if (FSDKE_OK != FSDK_CloseVideoCamera(cameraHandle)) 
	{
		MessageBox(0, L"Error closing camera", L"Error", MB_ICONERROR | MB_OK);
		return -5;
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
	return 0;
}


void initialize()
{

	getFiles *m_getFiles = new getFiles();
	vector<string> m_vecStrFileType;
	m_vecStrFileType.push_back("index");
	m_getFiles->getSpecialType(m_strModelPath, m_vecStrDatalFilePath, m_vecStrFileType);
	m_getFiles = NULL;
	delete[] m_getFiles;
	if (!m_vecStrDatalFilePath.size())
	{
		loadImgPath("..\\Img");
		m_luxandFace->initalImage(m_vecStrImgPath);
		m_imgData = m_luxandFace->imgData();
	}
	else
	{
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
			cout << buffer << endl;
			string m_str = buffer;
			m_imgData.m_strImgPath.push_back(m_str);
			//m_str.clear();

			 
			FSDK_FaceTemplate m_faceTmp;
			for (int j = 0; j < 13324; j++)
			{
				float m_fValue;
				file >> m_fValue;
				m_faceTmp.ftemplate[j] = m_fValue;
			}
			m_imgData.m_vecFaceTemplate.push_back(m_faceTmp);

		}
	}
	//cout << m_imgData.m_strImgPath.size() << endl;
}
int getlen(char *result)
{
	int i = 0;
	while (result[i] != '\0'){
		i++;
	}
	return i;
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

