#include <evart/EVaRT2.h>
#include <stdlib.h>

extern "C"{
int EVaRT2_GetSdkVersion(unsigned char Version[4]){
  return RC_Okay;
}


int EVaRT2_SetVerbosityLevel(int iLevel){
  return RC_Okay;
}

int EVaRT2_SetErrorMsgHandlerFunc(void (*MyFunction)(int iLogLevel, char* szLogMessage)){
  return RC_Okay;
}

int EVaRT2_SetDataHandlerFunc(void (*MyFunction)(sFrameOfData* pFrameOfData)){
    return RC_Okay;
}

int EVaRT2_Initialize(char* szMyNicCardAddress, char* szEVaNicCardAddress){
    return RC_Okay;
}

int EVaRT2_Request(char* szCommand, void** ppResponse, int *pnBytes){
  return RC_Okay;
}

int EVaRT2_FreeBodyDefs(sBodyDefs* pBodyDefs){
  return RC_Okay;
}

int EVaRT2_PushBasePosition(sHierarchy* pHierarchy, tSegmentData* pPosition){
  return RC_Okay;
}

int EVaRT2_Exit(){
  return RC_Okay;
}

int EVaRT2_GetHostInfo(sHostInfo *pHostInfo){
    return RC_Okay;
}

sBodyDefs*     EVaRT2_GetBodyDefs(){return NULL;}
sFrameOfData*  EVaRT2_GetCurrentFrame(){return NULL;}

int EVaRT2_CopyFrame(const sFrameOfData* pSrc, sFrameOfData* pDst){
  return RC_Okay;
}
int EVaRT2_FreeFrame(sFrameOfData* pFrame){
  return RC_Okay;
}

void  EVaRT2_ConstructRotationMatrix(
        double angles[3],
        int iRotationOrder,
        double matrix[3][3]){
}

 void  EVaRT2_ExtractEulerAngles(
        double matrix[3][3],
        int    iRotationOrder,
        double angles[3]){
 }
}
