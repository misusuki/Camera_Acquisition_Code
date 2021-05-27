/*
 * Program: IS_Camera_Capture_OpenGL.cpp
 * Author: Sushil RAUT
 * Email: robosushil@gmail.com
 * Description: This code executes ISCamera capture, Displays live images using opencv, record HFR images and data in excel file,execution time of each steps
 */
#include "parameter.h"


int main(int argc, char *argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB  | GLUT_DOUBLE );
	glutInitWindowPosition(glutGet(GLUT_SCREEN_WIDTH)/2 - graphicsWinWidth/2, glutGet(GLUT_SCREEN_HEIGHT)/2 - graphicsWinHeight/2);
	glutInitWindowSize(graphicsWinWidth, graphicsWinHeight);
	glutWindowHandle = glutCreateWindow("Input_Image");
	glutDisplayFunc(display);
	glutPostRedisplay();

	glutTimerFunc(REFRESH_DELAY, timerEvent, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	namedWindow("1", WINDOW_AUTOSIZE); 
	createTrackbar("Recording", "1", &record, record_max );
	
//---------threads initialization
	int thread_handler[5];
	thread_handler[1] = pthread_create(&thread_image[1], NULL, &processCapture, NULL);
//---------threads initialization
	glutMainLoop();
	return 0;
	}

char title [50];
void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2i(-1, 1);
	glPixelZoom(fZoom, -fZoom);//Will Zooom the image to complete glutWindow
	glDrawPixels(IMG_WIDTH, IMG_HEIGHT,  GL_RGB, GL_UNSIGNED_BYTE, aHead.data); //GL_LUMINANCE
	glutSwapBuffers();
	sprintf (title, "DisplayFPS==>%f",Acq_fps);
	glutSetWindowTitle(title);
	
	if(waitKey(16)!=27){
		//----------Image Display
		imshow("1", aHead);
		//----------Image Display
	}else{
	start_flag=false; 
	destroyAllWindows(); stop_capturing(); uninit_device();	close_device(); //terminate processes
	}	
}


void* processCapture(void *arg){
	//---------camera initialization
	double get_FPS;
	// camera initialization
	open_device("/dev/video2");
	init_device(IMG_WIDTH, IMG_HEIGHT);
	// v4l2-ctl -d /dev/video1 --list-formats-ext

	if (setFPS(FPS)) printf("Framerate set to : %d\n", FPS);
	start_capturing();
	//---------camera initialization
	
	//---------Data logger declaration
	ostringstream oss[10];
	char sz_filename[100];
	ofstream _index_log(sz_filename);
	_index_log.open("log_execution_file.csv");
	_index_log<<"Count,Time_Elapsed,Average_Time,Capture_Time,BayerToRGB,Vec_Rec_Time,Memcpy_Rec_Time,Image saving,TotalTime \n";
	//---------Data logger declaration

		
	//---------timer initialization
	MyTimer tm;
	tm.Start();n= 0;
	//---------timer initialization    
    
//---------[START]
	while(start_flag){
		timer[5].TStart();//----total time for main process
		//-----------Image Capture
		timer[0].TStart();
		memmove(gHead.data, snapFrame(), sizeof(char)*IMAGE_SIZE);
		timer[0].TStop();
		//-----------Image Capture
		
		//-----------Prepare Image for Display
		timer[1].TStart();
		cvtColor(gHead, aHead, COLOR_BayerBG2RGB); // Convert the image Gray to RGB
		timer[1].TStop();
		//-----------Prepare Image for Display
					
		//-----------Time and FPS count
		 total_time = (double) tm.Elapsed() / 1000000.0;
		 average_time = (double) total_time*1000 / n;
		 Execution[0] = timer[0].Execution_Time();
		 
		 if (cycle > 1000){
		 Acq_fps=1000.0f/(double) (average_time);
		 cycle=0;
		  }
		 
		// cout<<"Time Elapsed:"<<(double) total_time<<" sec."<<endl;
		// cout<<"Average Time: "<<(double) average_time<<" ms."<<endl;
		// cout<<"Execution[0]="<<(double) Execution[0]<<" ms."<<endl;
		// cout<<"FPS: "<<(double)Acq_fps<<" fps"<<endl;
		// cout<<""<<endl;
		
		 getFPS(get_FPS);

		 oss[0].str(""); oss[0] << "Time Elapsed [sec]:" << total_time;
		 putText(aHead, oss[0].str(), Point(5, 20), FONT_HERSHEY_SIMPLEX, 0.4, Scalar::all(255));
		 
		 oss[1].str(""); oss[1] << "Average Time [ms]:" << average_time;
		 putText(aHead, oss[1].str(), Point(250, 20), FONT_HERSHEY_SIMPLEX, 0.4, Scalar::all(255));
		 
		 oss[2].str(""); oss[2] << "Frame Rate [fps]:" << get_FPS;
		 putText(aHead, oss[2].str(), Point(450, 20), FONT_HERSHEY_SIMPLEX, 0.4, Scalar::all(255));
		 
		 //-----------Time and FPS count


		//-----------Data recording
		if(record==1){
		if (increment< SAVE_INPUT_Img){
			timer[2].TStart();
			cam_1.push_back(aHead.clone());
			timer[2].TStop();
			
			
			timer[3].TStart();
			memcpy(img_rec + increment*IMAGE_SIZE*3, aHead.data, IMAGE_SIZE*3);
			timer[3].TStop();
			
			exe.Time[0][increment]=(double)total_time;
			exe.Time[1][increment]=(double)average_time;

			for(int i=0;i<4;i++){
				exe.Execu[i][increment]= (double)timer[i].Execution_Time();
			}
			increment++;
			
		}
		
		if(increment== SAVE_INPUT_Img-1){cout<<"Image Recording Completed....."<<endl;}
		
	    }
	    //-----------Data recording
		timer[5].TStop();//----total time for main process
		exe.Execu[5][increment]= (double)timer[5].Execution_Time();
		Execution[5] = timer[5].Execution_Time();
		//cout<<"Total Time="<< Execution[5]<<endl;
	    n++;
		cycle++; // fps counter
	}
	
//---------[STOP]
//---------[OFFLINE_START]
	//----------Save file to HDD
		for (int j = 0; j < SAVE_INPUT_Img; j++) {
		_index_log<<j<<","<<exe.Time[0][j]<<","<<exe.Time[1][j]<<","<<exe.Execu[0][j]<<","<<exe.Execu[1][j]<<","<<exe.Execu[2][j]<<","<<exe.Execu[3][j]<<","<<exe.Execu[4][j]<<","<<exe.Execu[5][j]<<endl;
		}
	//----------Save file to HDD
	
	//----------Save images to HDD
		Mat img_1;
		char sz_1[200], sz_2[200];

		Mat img_2(IMG_HEIGHT, IMG_WIDTH, CV_8UC3);
		
		for (int j = 0; j < SAVE_INPUT_Img; j++) {
			//------------saving vector images
			timer[4].TStart();
			img_1 = cam_1[j];
			sprintf(sz_1, "%sV%04d.png", IMG_1_FOLDER, j);
			imwrite(sz_1, img_1);
			timer[4].TStop();
			Execution[4]=(double)timer[4].Execution_Time();
			exe.Execu[4][increment]= (double)timer[4].Execution_Time();
			std::cout << "saving i/p vector images==>" << j <<", Time:"<<Execution[4]<< std::endl;
			//------------saving vector images

			//------------saving memcpy images
			/*
			memcpy(img_2.data, img_rec + j* IMAGE_SIZE*3, IMAGE_SIZE*3);
			sprintf(sz_2, "%sM%04d.png", IMG_1_FOLDER, j);
			imwrite(sz_2, img_2);
			std::cout << "saving i/p memcpy images==>" << j << std::endl;
			*/
			//------------saving memcpy images

		}
		
		//----------Save images to HDD

		

	increment = 0; //reset counter
	cam_1.clear();// clear vector memory
//---------[OFFLINE_STOP]

	destroyAllWindows(); stop_capturing(); uninit_device(); close_device();
	}
	
	
