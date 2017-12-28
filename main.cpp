#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <string.h>
#include <fstream>
#include <vector>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <dlfcn.h>
#include "sgfplib.h"


using namespace std;

LPSGFPM  sgfplib = NULL;

void procargs(int, char **, float *, char **, char **, int *, int *, int *, int *, int *, char **);
void print_usage(char *);
int fail(DWORD err, const char *message);
int capture();
int foo(int argc, char **argv);
void cwsq();
uint8_t *stringToByteArray(char* arr);

int main(int argc, char **argv) {
    DWORD err;
    err = CreateSGFPMObject(&sgfplib);
    // vector<char*> arg_v;
    
    // execl("/usr/local/src/nbis/bin/dwsq", "raw", "./out/dig.wsq");
    
    return 0;
}

/*****************************************************************/
int fail(DWORD err, const char *message) {
  cerr << "ERROR"
       << " (" << err << "): " << message << endl;
  return err;
}

/*****************************************************************/
void cwsq()
{

}

/*****************************************************************/
void print_usage(char *arg0)
{
   fprintf(stderr, "Usage: %s ", arg0);
   fprintf(stderr, "<r bitrate> <outext> <image file>\n");
   fprintf(stderr,
           "                 [-raw_in w,h,d,[ppi]] [comment file]\n\n");
   fprintf(stderr,
           "   r bitrate = compression bit rate (2.25==>5:1, .75==>15:1)\n\n");
}

/*****************************************************************/
int foo(int argc, char **argv)
{
    DWORD err;
    SGDeviceInfoParam deviceInfo;
    BYTE *imageBuffer1;
    BYTE *imageBuffer2;
    BYTE *minutiaeBuffer2;
    DWORD nfiq;
    DWORD quality;
    char kbBuffer[100];
    FILE *fp = NULL;
    SGFingerInfo fingerInfo;
    DWORD templateSize, templateSizeMax;

    if (argc < 2) {
        return fail(1, "Inufficient paramenters");
    }

    ///////////////////////////////////////////////
    // Instantiate SGFPLib object

    err = CreateSGFPMObject(&sgfplib);
    if (!sgfplib)
    {
        printf("ERROR - Unable to instantiate FPM object.\n\n");
        return 0;
    }

    //err = capture();

    vector<string> fingers;
    char *fingers_filename = argv[1];
    ifstream fingers_file (argv[1]);
    if (fingers_file.is_open()) {
        string line;
        while(getline(fingers_file, line)) {
            fingers.push_back(line);
        }
        fingers_file.close();
    } else {
        return fail(1, "Invalid fingers file");
    }
    
    char* buffer;
    int sizeWSQ;

    //while (fingers.size() > 0) {
        buffer = (char*) fingers.back().c_str();
        
        imageBuffer2 = stringToByteArray(buffer);
        
        
        fp = fopen("./out/dig.wsq","wb");
        fwrite (imageBuffer2 , sizeof (BYTE) , 300*400 , fp);
        fclose(fp);

        // minutiaeBuffer2 = (BYTE*) malloc(300*400);
        // fingerInfo.FingerNumber = SG_FINGPOS_UK;
        // fingerInfo.ViewNumber = 1;
        // fingerInfo.ImpressionType = SG_IMPTYPE_LP;
        // fingerInfo.ImageQuality = quality; //0 to 100
        
        // //err = sgfplib->SetTemplateFormat(TEMPLATE_FORMAT_ANSI378);
        // err = sgfplib->CreateTemplate(&fingerInfo, imageBuffer2, minutiaeBuffer2);
        
   // }
}

/*****************************************************************/
int capture()
{
    DWORD err;
    SGDeviceInfoParam deviceInfo;
    BYTE *imageBuffer1;
    BYTE *imageBuffer2;
    BYTE *minutiaeBuffer1;
    BYTE *minutiaeBuffer2;
    char kbBuffer[100];
    FILE *fp = NULL;
    DWORD nfiq;
    DWORD quality;
    SGFingerInfo fingerInfo;
    DWORD templateSize, templateSizeMax;

    err = sgfplib->Init(SG_DEV_AUTO);
    if (err != SGFDX_ERROR_NONE)
    {
        printf("ERROR - Unable to initialize device.\n\n");
        return 0;
    }


    err = sgfplib->OpenDevice(0);
    if (err == SGFDX_ERROR_NONE)
    {
        err = sgfplib->SetLedOn(true);
        printf("Fingerprint Sensor LEDS should now be illuminated.\n\n");
        printf("Press <Enter> to turn fingerprint sensor LEDs off >> ");

        err = sgfplib->SetLedOn(false);
        
        printf("Capture 1. Please place [%s] on sensor and press <ENTER> ");
        
        deviceInfo.DeviceID = 0;
        err = sgfplib->GetDeviceInfo(&deviceInfo);
        /**
         * 300 X 400 = 500DPi
         **/
        imageBuffer1 = (BYTE*) malloc(deviceInfo.ImageHeight*deviceInfo.ImageWidth);
        imageBuffer2 = (BYTE*) malloc(deviceInfo.ImageHeight*deviceInfo.ImageWidth);
        DWORD timeout = 5000; //5000 milliseconds
        DWORD imageQuality = 60;   //60%
        err = sgfplib->GetImage(imageBuffer1);
        //err = sgfplib->GetImageEx(imageBuffer2, timeout, NULL, imageQuality);

        err = sgfplib->GetImageQuality(deviceInfo.ImageWidth, deviceInfo.ImageHeight, imageBuffer1, &quality);
        nfiq = sgfplib->ComputeNFIQ(imageBuffer1, deviceInfo.ImageWidth, deviceInfo.ImageHeight);

        err = sgfplib->GetMaxTemplateSize(&templateSizeMax);
        minutiaeBuffer2 = (BYTE*) malloc(templateSizeMax);
        fingerInfo.FingerNumber = SG_FINGPOS_UK;
        fingerInfo.ViewNumber = 1;
        fingerInfo.ImpressionType = SG_IMPTYPE_LP;
        fingerInfo.ImageQuality = quality; //0 to 100
        BOOL finger_matched = false;

        if (err == SGFDX_ERROR_NONE)
        {
            err = sgfplib->SetTemplateFormat(TEMPLATE_FORMAT_SG400);
            err = sgfplib->CreateTemplate(&fingerInfo, imageBuffer1, minutiaeBuffer1);
        
            

            //err = sgfplib->MatchTemplate(minutiaeBuffer1, minutiaeBuffer1, SL_NORMAL, &finger_matched);

            
            fp = fopen("./out/cap.raw","wb");
            fwrite (imageBuffer1 , sizeof (BYTE) , deviceInfo.ImageWidth*deviceInfo.ImageHeight , fp);
            fclose(fp);
        }

        
    }

    return err;
}

/*****************************************************************/
uint8_t *stringToByteArray(char* arr) {
    string ss(arr);
    int byteArrSize = ss.size() / 2;
    uint8_t *byteArr = (uint8_t *) calloc(sizeof(uint8_t *) *byteArrSize, 1);
    for (int i = 0; i < byteArrSize; i++) {
        string s = ss.substr(0,2);
        byteArr[i] = stoul(s, nullptr, 16);
        ss = ss.substr(2);
    }

    return byteArr;
}

