#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/photo.hpp>

/**
* argv[0] : filename
* argv[1] : videopath
* argv[2] : frame
*/
int main(int argc, char* argv[]) {
    printf("Running VideoIO File : %s", argv[0]);
    if (argc > 1) printf("Save Video to : %s", argv[1]);
    if (argc > 2) printf("Video Max Frame : %s", argv[2]);


    VideoCapture videoDevice;
    videoDevice.open("/dev/video0", CAP_V4L2);
    if (!videoDevice.isOpened()) {
        printf("Camera Open Failure!");
        return -1;
    }

    int frame_width = videoDevice.get(cv::CAP_PROP_FRAME_WIDTH)
    int frame_height = videoDevice.get(cv::CAP_PROP_FRAME_HEIGHT)

    VideoWriter video(argv[1], cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
                        10, Size(frame_width, frame_height));

    Mat Image;
    int maxFrame = (argc > 2) ? (int(argv[2])) : 50;

    for (int count = 0; count < MaxFrame; ++count) {
        videoDevice.read(Image);
        if (Image.empty()) break;
        video.write(Image);
    }

    videoDevice.release();
    video.release();

    return 0;
}