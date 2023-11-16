# Git 사진
[Git](../../.readme-images/git.png)

# week11 - *Error Issue!*
### OpcnCV 라이브러리의 VideoWriter 객체를 생성할 때, 비디오(.avi)파일의 경로와 파일 이름과 관련하여 예제 코드에 문제가 있음
* 오류 내용
  * g++ 뒤에 -o 가 없음!!
  * OpenCV에서 파일 이름을 읽어들이는 데 문제가 있음!!  
    - 실행 코드
      ```bash
      # g++ -o camera camera_example.cpp $(pkg-config opencv4 --libs --cflags)
      # ./camera
      ```
    - output
      ```
      [ERROR:0] global /home/pi/lib/opencv/opencv-4.5.1/modules/videoio/src/cap.cpp (561) open VIDEOIO(CV_IMAGES): raised OpenCV exception:
  
      OpenCV(4.5.1) /home/pi/lib/opencv/opencv-4.5.1/modules/videoio/src/cap_images.cpp:253: error: (-5:Bad argument) CAP_IMAGES: can't find starting number (in the name of file): outcpp.avi in function 'icvExtractPattern'
      
      
      Open Camera
      ```

* **오류 해결 출처** (Stack Overflow)
  ```
  https://stackoverflow.com/questions/59168477/cant-find-starting-number-in-the-name-of-file-when-trying-to-read-frames-fr/68278992#68278992
  ```
* **변경 내용**
  * 동영상 파일의 경로를 절대 경로 (`/`부터 시작하는 경로) 로 바꾸고, 파일의 이름에 번호를 붙여 줌
  * 문제 발생 환경을 여러 상황에서 실험해 본 결과, 절대 경로보다는 파일 이름에 번호가 붙지 않아 생기는 오류로 보임

* **기존 코드**
  ```cpp
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
  
    VideoWriter video("outcpp.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
                      10, Size(frame_width, frame_height));
  
    printf("Open Camera\n");
    Mat img;
    int count = 0;
    int max;
  
    if (argc > 1) {
      max = int(argv[1]);
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

  ```

* **수정된 코드**
  ```cpp
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
      max = int(argv[1]);
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
  ```
