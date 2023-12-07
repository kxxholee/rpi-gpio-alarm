#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/photo.hpp>

int main(int argc, char *argv[]) {
    VideoCapture video;
    video.open("/dev/video0", CAP_V4L2);
    if (!video.isOpened()) {
        printf("Camera Open Failure\n");
        return -1;
    }

    int frame_width = video.get(cv::CAP_PROP_FRAME_WIDTH);
}