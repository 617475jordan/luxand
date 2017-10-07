#include "getFiles.h"


getFiles::getFiles()
{
}


getFiles::~getFiles()
{
}

void getFiles::getAllFiles(string path, vector<string>& files)
{
	// 文件句柄
	long hFile = 0;
	// 文件信息
	struct _finddata_t fileinfo;

	string p;

	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do {
			// 保存文件的全路径
			files.push_back(p.assign(path).append("\\").append(fileinfo.name));

		} while (_findnext(hFile, &fileinfo) == 0); //寻找下一个，成功返回0，否则-1

		_findclose(hFile);
	}
}

void getFiles::getAllFilesAndOthers(string path, vector<string>& files)
{
	//文件句柄
	long hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
		do {
			if ((fileinfo.attrib & _A_SUBDIR)) { //比较文件类型是否是文件夹
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					//递归搜索
					getAllFiles(p.assign(path).append("\\").append(fileinfo.name), files);
				}
			}
			else {
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0); //寻找下一个，成功返回0，否则-1
		_findclose(hFile);
	}
}



void getFiles::addFile(vector<string> m_vecStrFile)
{
	for (int i = 0; i < m_vecStrFile.size(); i++)
	{
		if (_mkdir(m_vecStrFile[i].c_str()) == -1)
		{
			_mkdir(m_vecStrFile[i].c_str());
		}
	}
}

void getFiles::deleteFile(vector<string> m_vecStrFile)
{
	for (int i = 0; i < m_vecStrFile.size(); i++)
	{
		if (_mkdir(m_vecStrFile[i].c_str()) == 0)
		{
			_rmdir(m_vecStrFile[i].c_str());
		}
	}
}

std::vector<std::string> getFiles::currentModel()
{
	return m_vecStrCurrentModel;
}

void getFiles::getSpecialType(string file_path, std::vector<string>& files, vector<string> m_vecStrFileType)

{
	intptr_t hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(file_path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getSpecialType(p.assign(file_path).append("\\").append(fileinfo.name), files, m_vecStrFileType);
			}
			else
			{
				char *ext = strrchr(fileinfo.name, '.');
				if (ext){
					ext++;
					for (int i = 0; i < m_vecStrFileType.size(); i++)
					{
						const char *m_charTmp = m_vecStrFileType[i].c_str();
						if (_stricmp(ext, m_charTmp) == 0)
						{
							files.push_back(p.assign(file_path).append("\\").append(fileinfo.name));
							break;
						}

					}

				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}