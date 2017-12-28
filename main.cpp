#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "sgfplib.h"

using namespace std;

LPSGFPM sgfplib = NULL;
DWORD err;

int fail(DWORD, const char *);
void blinkLed(LPSGFPM, int);
int capture(int);
int validate();
int foo();

int main()
{   
    err = CreateSGFPMObject(&sgfplib);
    if (err != SGFDX_ERROR_NONE) {
        return fail(err, "Can't create SGFPM object");
    }
    
    err = validate();
    if (err != SGFDX_ERROR_NONE) {
        return fail(err, "Failed capture finger");
    }

    return 0;
}

int foo()
{
    FILE * pFile;
    long lSize;
    BYTE * buffer;
    BYTE * ansiMinutiaeBuffer1;
    size_t result;
    SGFingerInfo fingerInfo;

    if((pFile = fopen("./out/captured_1.raw", "rb")) == (FILE *)NULL) {
       fprintf(stderr, "ERROR: read_raw \n");
       return(-5);
    }

    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    rewind (pFile);

    buffer = (BYTE*) malloc (sizeof(BYTE)*lSize);
    if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

    result = fread (buffer,1,lSize,pFile);
    if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

    fclose(pFile);

    ansiMinutiaeBuffer1 = (BYTE *)malloc(400);

    fingerInfo.FingerNumber = SG_FINGPOS_UK;
    fingerInfo.ViewNumber = 1;
    fingerInfo.ImpressionType = SG_IMPTYPE_LP;
    fingerInfo.ImageQuality = 10;

    err = sgfplib->CreateTemplate(&fingerInfo, buffer, ansiMinutiaeBuffer1);
    if (err != SGFDX_ERROR_NONE) {
        return fail(err, "Can't create template (2)");
    }
}

/*******************************************************/
int capture(int _cap = 1)
{
    SGDeviceInfoParam deviceInfo;
    SGFingerInfo fingerInfo;
    BYTE *captureBuffer1;
    BYTE *ansiMinutiaeBuffer1;
    FILE *fp = NULL;
    char fileName[20];
    DWORD templateSize, templateSizeMax, quality;

    err = sgfplib->Init(SG_DEV_AUTO);
    if (err != SGFDX_ERROR_NONE) {
        return fail(err, "Can't initialize SGFPM object");
    }

    err = sgfplib->OpenDevice(0);
    if (err != SGFDX_ERROR_NONE) {
        return fail(err, "Can't open device");
    }

    deviceInfo.DeviceID = 0;
    err = sgfplib->GetDeviceInfo(&deviceInfo);
    if (err != SGFDX_ERROR_NONE) {
        return fail(err, "Can't get device info");
    }

    blinkLed(sgfplib, 5);
    captureBuffer1 = (BYTE *)malloc(deviceInfo.ImageWidth * deviceInfo.ImageHeight);

    err = sgfplib->GetImage(captureBuffer1);
	if (err != SGFDX_ERROR_NONE) {
		return fail(err, "GetImage Failed");
    }

    err = sgfplib->GetMaxTemplateSize(&templateSizeMax);

    printf("\tGetImage %u: Success\n", _cap);

    sprintf(fileName, "./out/captured_%u.raw",_cap);
    fp = fopen(fileName,"wb");
    fwrite (captureBuffer1 , sizeof (BYTE) , deviceInfo.ImageWidth*deviceInfo.ImageHeight , fp);
    fclose(fp);

    sgfplib->CloseDevice();

    // err = sgfplib->GetMaxTemplateSize(&templateSizeMax);
    // ansiMinutiaeBuffer1 = (BYTE *)malloc(templateSizeMax);

    // fingerInfo.FingerNumber = SG_FINGPOS_UK;
    // fingerInfo.ViewNumber = 1;
    // fingerInfo.ImpressionType = SG_IMPTYPE_LP;
    // fingerInfo.ImageQuality = quality;

    // err = sgfplib->CreateTemplate(&fingerInfo, captureBuffer1, ansiMinutiaeBuffer1);

    if (_cap == 1) {
        err = capture(2);
    }
    usleep(100 * 3000);
    return 0;
}

/*******************************************************/
int validate()
{
    SGDeviceInfoParam deviceInfo;
    SGFingerInfo fingerInfo;
    BYTE *captureBuffer1, *ansiMinutiaeBuffer1, *ansiMinutiaeBuffer2, *bufferToCompare;
    FILE *fp = NULL;
    char fileName[20];
    size_t result;
    DWORD templateSize, templateSizeMax, quality;
    FILE * pFile;
    long lSize;

    err = sgfplib->Init(SG_DEV_AUTO);
    if (err != SGFDX_ERROR_NONE) {
        return fail(err, "Can't initialize SGFPM object");
    }

    err = sgfplib->OpenDevice(0);
    if (err != SGFDX_ERROR_NONE) {
        return fail(err, "Can't open device");
    }

    deviceInfo.DeviceID = 0;
    err = sgfplib->GetDeviceInfo(&deviceInfo);
    if (err != SGFDX_ERROR_NONE) {
        return fail(err, "Can't get device info");
    }

    blinkLed(sgfplib, 5);
    captureBuffer1 = (BYTE *)malloc(deviceInfo.ImageWidth * deviceInfo.ImageHeight);

    err = sgfplib->GetImage(captureBuffer1);
	if (err != SGFDX_ERROR_NONE) {
		return fail(err, "GetImage Failed");
    }

    err = sgfplib->GetMaxTemplateSize(&templateSizeMax);

    printf("\tGetImage Success\n");

    err = sgfplib->GetMaxTemplateSize(&templateSizeMax);
    ansiMinutiaeBuffer1 = (BYTE *)malloc(templateSizeMax);

    fingerInfo.FingerNumber = SG_FINGPOS_UK;
    fingerInfo.ViewNumber = 1;
    fingerInfo.ImpressionType = SG_IMPTYPE_LP;
    fingerInfo.ImageQuality = quality;

    err = sgfplib->CreateTemplate(&fingerInfo, captureBuffer1, ansiMinutiaeBuffer1);
    if (err != SGFDX_ERROR_NONE) {
        return fail(err, "Can't create template (1)");
    }

    printf("\tCreate template 1 success\n");
    
    /***/
    if((pFile = fopen("./out/detran.raw", "rb")) == (FILE *)NULL) {
       fprintf(stderr, "ERROR: read_raw \n");
       return(-5);
    }

    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    rewind (pFile);

    bufferToCompare = (BYTE*) malloc (sizeof(BYTE)*lSize);
    if (bufferToCompare == NULL) {fputs ("Memory error",stderr); exit (2);}

    result = fread (bufferToCompare,1,lSize,pFile);
    if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

    fclose(pFile);

    ansiMinutiaeBuffer2 = (BYTE *)malloc(400);

    fingerInfo.FingerNumber = SG_FINGPOS_UK;
    fingerInfo.ViewNumber = 1;
    fingerInfo.ImpressionType = SG_IMPTYPE_LP;
    fingerInfo.ImageQuality = 10;

    err = sgfplib->CreateTemplate(&fingerInfo, bufferToCompare, ansiMinutiaeBuffer2);
    if (err != SGFDX_ERROR_NONE) {
        return fail(err, "Can't create template (2)");
    }

    printf("\tCreate template 2 success\n");
    
    BOOL finger_matched = false;
    err = sgfplib->MatchTemplate(ansiMinutiaeBuffer1, ansiMinutiaeBuffer2, SL_NORMAL, &finger_matched);
    if (err != SGFDX_ERROR_NONE) {
        return fail(err, "Can't match template");
    }
}

/*******************************************************/
void blinkLed(LPSGFPM m_sgfplib, int times) 
{
    for (int i = 0; i < times; i++) {
        m_sgfplib->SetLedOn(true);
        usleep(100 * 1000);
        m_sgfplib->SetLedOn(false);
        usleep(100 * 1000);
    }
}

/*******************************************************/
int fail(DWORD err, const char* message)
{
    cerr << "ERROR" << " (" << err << "): " << message << endl;
    return err;
}