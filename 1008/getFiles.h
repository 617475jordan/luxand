#pragma once
#include <io.h>
#include <direct.h>
#include <string>
#include <vector>
#include <fstream>
using namespace std;
class getFiles
{
public:
	getFiles();
	~getFiles();
	//���ָ��Ŀ¼�µ������ļ������������ļ��У�
	void getAllFiles(string path, vector<string>& files);
	//��ȡָ��Ŀ¼�µ������ļ����������ļ��У�
	void getAllFilesAndOthers(string path, vector<string>& files);
	//��ȡָ����ʽ���ļ�(���������ļ��У�
	void addFile(vector<string> m_vecStrFile);	  //���ָ���ļ���
	void deleteFile(vector<string> m_vecStrFile); //ɾ��ָ���ļ���
	void getSpecialType(string file_path, std::vector<string>& files, vector<string> m_vecStrFileType);
	vector<string> currentModel();
public:
	vector<string>   m_vecStrCurrentModel;
};