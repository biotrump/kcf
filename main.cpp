#include <stdlib.h>
#include <cstdio>
#include <opencv2/opencv.hpp>
using namespace cv;

#include "kcf.h"
#include "vot.hpp"

Mat frame;//captured frame

int fApply=0;
char *apply_button="apply";
Point VertexLeftTop(-1,-1);
Point VertexRightDown(-1,-1);

KCF_Tracker tracker;

int kcf(cv::Mat &image, cv::Rect init_rect)
{
    BBox_c bb;
    double avg_time = 0.;
    int frames = 0;
	double time_profile_counter = cv::getCPUTickCount();

	tracker.track(image);
	time_profile_counter = cv::getCPUTickCount() - time_profile_counter;
	//std::cout << "  -> speed : " <<  time_profile_counter/((double)cvGetTickFrequency()*1000) << "ms. per frame" << std::endl;
	avg_time += time_profile_counter/((double)cvGetTickFrequency()*1000);
	frames++;

	bb = tracker.getBBox();
	cv::rectangle(image, cv::Rect(bb.cx - bb.w/2., bb.cy - bb.h/2., bb.w, bb.h), 
					CV_RGB(0,255,0), 2);

    std::cout << "Average processing speed " << avg_time/frames <<  "ms. (" << 1./(avg_time/frames)*1000 << " fps)" << std::endl;

    return EXIT_SUCCESS;
}

void onMouse(int Event,int x,int y, int flags, void* param)
{
	cv::Rect *init_rect=(cv::Rect *)param;

    if(Event==CV_EVENT_LBUTTONDOWN){
        VertexLeftTop.x = x;
        VertexLeftTop.y = y;
		VertexRightDown.x = -1;//reset purpose
        VertexRightDown.y = -1;
		init_rect->x =x;
		init_rect->y =y; 
    }
    if(Event==CV_EVENT_LBUTTONUP){
        VertexRightDown.x = x;
        VertexRightDown.y = y;
		init_rect->width = x- init_rect->x + 1;
		init_rect->height = y - init_rect->y + 1;
		printf("%s:tracker init... \n",__func__);
		tracker.init(frame, *init_rect);//init kcf roi for later tracking
    }
}

void cbButton(int state, void *arg)
{
	printf(">>%s:%d\n", __func__, state);
	if(arg){
		printf("%s: %p, %s\n",__func__, arg, (char *)arg);
		if(strncmp((char *)arg, apply_button, strlen(apply_button)) == 0){
			//apply button
			fApply = !state;
		}
	}
	printf("<<%s:fApply=%d\n",__func__, fApply);

}

/**
 * 
 */
int main(int argc, char **argv)
{
	VideoCapture cap/*(argv[1])*/;
	if(argc >= 2){
		cap.open( atoi(argv[1]) );	//cap.open(int device_number)
	}else
		cap.open(0);

	if (!cap.isOpened()){
		printf("%s n , where n is /dev/videon \n", argv[0]);
        return -1;
    }
	cv::Rect init_rect;
 
	namedWindow("image",CV_WINDOW_AUTOSIZE);
    setMouseCallback("image", onMouse, &init_rect);
	createButton(apply_button, cbButton, apply_button, CV_PUSH_BUTTON, 0);

	while(true){
		cap >> frame;
		if(frame.empty()){
            break;
        }
        flip(frame, frame,1);

		if(fApply){
			kcf(frame, init_rect);
		}else{
			if(VertexLeftTop.x!=-1 && VertexRightDown.x!=-1){
				rectangle(frame, Rect(VertexLeftTop,VertexRightDown),Scalar(255,0,0),2);
			}
		}
        imshow("image", frame);
        char key = cvWaitKey(10);
        if(key==27|| key == 'q' || key == 'Q'){
            break;
        }
    }
	return 0;
}
