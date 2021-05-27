#include <iostream>
#include <fstream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "v4ldevice.cpp"
#include <ctime>
#include <GL/freeglut_std.h>
#include "Execution.h"
#include "mytimer.h"


using namespace std;
using namespace cv;

#define IMG_WIDTH	640
#define IMG_HEIGHT	480
#define _WIDTH	IMG_WIDTH/2
#define _HEIGHT	IMG_HEIGHT/2
#define IMAGE_SIZE IMG_WIDTH*IMG_HEIGHT
#define BUF_FRAME	60
#define BUF_LOG		10000
#define BUF_RECORD	100
#define SAVE_INPUT_Img 1000
#define NUM_BUFFERS 1 // Specify the number of buffers to be used.
#define T_INTERVAL 1
#define INPUT_FOLDER	"./record_image/"
#define IMG_1_FOLDER	"./Data/cam1/"  // Image Save Folder

// camera setup
#define GAIN 100
#define FPS 600 // maximum fps
#define EXPOSURE 1000 // us
#define BRIGHTNESS 300
#define HUE_RED 60
#define HUE_GREEN 50
#define HUE_BLUE 90

#define REFRESH_DELAY 16 //ms



pthread_t thread_image[5];

bool start_flag=true; 

typedef float TYPE;
char command[256]; // for parameter setting from shell
unsigned char* ImageBuffer = NULL;
unsigned int cycle=0;
int record =0, record_max=1, n=0,increment=0;
int fps_run=FPS, fps_run_max=800;
double total_time=0.0, average_time = 0.0, Acq_fps=0.0;


	
struct Execution_Time
{
	double Execu[30][SAVE_INPUT_Img];
	double Time[30][SAVE_INPUT_Img];
};
extern Execution_Time exe;
Execution_Time exe;



vector<Mat> cam_1;
unsigned char img_rec[IMAGE_SIZE*3*SAVE_INPUT_Img];


uchar **buf_image;
Mat input(IMG_HEIGHT, IMG_WIDTH, CV_8UC1);
Mat sHead(IMG_HEIGHT, IMG_WIDTH, CV_8UC1);
Mat gHead(IMG_HEIGHT, IMG_WIDTH, CV_8UC1);
Mat cHead(IMG_HEIGHT, IMG_WIDTH, CV_8UC3);
Mat aHead(IMG_HEIGHT, IMG_WIDTH, CV_8UC3);


//-------------OpenGL_Declaration

int glutWindowHandle;

int graphicsWinWidth = IMG_WIDTH ;
int graphicsWinHeight = IMG_HEIGHT ;
//int graphicsWinHeight = (int)(((float)XiWIDTH / (float)XiHEIGHT) * graphicsWinWidth);
float fZoom = 1;

int imgLoad = 0;


void keyboard(unsigned char, int, int);
void timerEvent(int value);
void OpenGL_Initialization(int argc, char **argv);
void display();
void reshape(int w, int h);
void idle(void);
void CalculateFrameRate();
static float drawFPS(int id, Mat img);
void saveimages();
void loadimages(Mat img);
void loadimagetrigger();

//-------------OpenGL_Declaration


void* processCapture(void *arg);

int set_props(char *device, int exposure){
	sprintf(command, "v4l2-ctl -d %s -c exposure_auto=1", device);
	system(command);
	sprintf(command, "v4l2-ctl -d %s -c exposure_time_us=%d", device, EXPOSURE);
	system(command);
	sprintf(command, "v4l2-ctl -d %s -c gain=%d", device, GAIN);
	system(command);
	sprintf(command, "v4l2-ctl -d %s -c brightness=%d", device, BRIGHTNESS);
	system(command);
	}

void timerEvent(int value)
{
	glutPostRedisplay();
	glutTimerFunc(REFRESH_DELAY, timerEvent, 0);
}
