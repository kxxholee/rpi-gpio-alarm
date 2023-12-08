#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/photo.hpp>

using namespace std;
using namespace cv;

int main(int argc, char *argv[]) {
  VideoCapture cap;
  cap.open("/dev/video0", CAP_V4L2);
  if (!cap.isOpened()) {
    printf("Can't open Camera\n");
    return -1;
  }
  // Default resolutions of the frame are obtained. The default resolutions are
  // system dependent.
  int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
  int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

  /*
  * opencv Error -> Fixed from Stackoverflow code
  * https://stackoverflow.com/questions/59168477/cant-find-starting-number-in-the-name-of-file-when-trying-to-read-frames-fr/68278992#68278992
  */
  VideoWriter video("/home/pi/working/week11/opencv_example/outcpp01.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
                    10, Size(frame_width, frame_height));

  printf("Open Camera\n");
  Mat img;
  int count = 0;
  int max;

  if (argc > 1) {
    max = std::stoi(argv[1]);
  } else {
    max = 50;
  }

  while (count <= max) {
    cap.read(img);
    if (img.empty())
      break;
    video.write(img);
    count++;
  }

  cap.release();
  video.release();

  return 0;
}
