#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <regex>
#ifdef _WIN32
#include "wingetopt.h"
#else
#include <getopt.h>
#endif
#include "Viewer.h"

#include <dirent.h>

#include <IDevice.h>
#include <IDeviceManager.h>

using namespace std;

/**
 * @brief _verbose True if user wants to get more information in standard output, like frame metata
 */
static bool _verbose = false;


string findFirstASHCamera() 
{
    IDeviceManager manager;

    vector<shared_ptr<IDevice>> heads = manager.detectDevices();

    string sensorName = heads[0]->getDeviceInformation()->getName();

    return sensorName;
}

bool compareString (std::string a, std::string b) 
{
    return a > b;
}

void deleteLastVideos()
{
    DIR *dr;
    struct dirent *en;
    dr = opendir("."); 
    vector<string> fileNames;
    if (dr) {
        while ((en = readdir(dr)) != NULL) {
            string fileName = en->d_name;
            if (fileName.find(".avi") != std::string::npos) {
                fileNames.push_back(fileName);
            }
        }

        closedir(dr);
    }
    std::sort(fileNames.begin(),fileNames.end(),compareString);
    if (fileNames.size() >= 4) {
        fileNames.erase(fileNames.begin(), fileNames.begin()+4);
        for (string file: fileNames) {
            char fileToDelete[file.length() + 1];
            strcpy(fileToDelete, file.c_str());
            remove(fileToDelete);
        }
    }
}

/********************************************************************************
 *  MAIN PROGRAM
 ********************************************************************************/

int main(int argc, char *argv[])
{

    deleteLastVideos();
    string hostname = findFirstASHCamera();
    
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d:%H-%M-%S");
    auto displayVideoName = oss.str().append("_display_video.avi");
    auto disparityVideoName = oss.str().append("_disparity_video.avi");

    int frameWidth = 828;
	int frameHeigth = 544;

    cv::VideoWriter displayVideoWriter(displayVideoName, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 20, cv::Size(frameWidth, frameHeigth));
    cv::VideoWriter disparityVideoWriter(disparityVideoName, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 20, cv::Size(frameWidth, frameHeigth), 1);
    
    /* Create and launch Viewer */
    Viewer viewer(hostname, argc, argv, _verbose, displayVideoName, displayVideoWriter, disparityVideoName, disparityVideoWriter);

    return viewer.exec();
}