#include "resource.h"
#include <time.h>
#include "hbitmapAndMat.h"
#include "getFiles.h"
#include "luxandFace.h"


bool  initialize(string m_strModelPath);
void  loadImgPath(string m_strImgPath);
void  run();
vector<string>      m_vecStrImgPath;
vector<string>      m_vecStrDatalFilePath;
ImgData m_imgData;
luxandFace *m_luxandFace = new luxandFace();
#define  m_featureNum 13324
#define  activateLibrary "hbdCpD+tb3dFNk0I9gQ4DrBocUmlpkGbVmg1ausq6mfqb0qF167rpoxxxjTpJkznbDP4WNwgnUJ1DT8oz7QvNAA/0CxFRID26IRrGIbiT8tFpbYGQge/2rvwAbvun6gEdkASuQgpgsZOjUTgr+V1IHlNDSUUbO953CCEzUhzDuw="
