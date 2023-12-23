#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/photo.hpp>
#include <tensorflow/lite/interpreter.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/model.h>
#include <string>
#include <memory>
#include <fcntl.h>

/**
 * [디렉토리 설명]
 * CMakeLists.txt       : C / C++프로젝트의 빌드를 위한 내용들을 입력해 주고,
 *                      의존성 문제를 편리하게 해결하기 위해 사용할 수 있다.
 * main.cpp             : 이 파일이 나중에 실행 파일이 된다. Hardware control, Detection(AI), OpenCV 등이 여기에 있음
 * tensorflow    [dir]  : tensorflow lite와 관련된 C라이브러리를 사용하기 위해 존재한다.
 * model         [dir]  : 이 파일에서 detection에 사용할 model.tflite가 들어 있다.
 * videos        [dir]  : 이 프로젝트에서 Labeling에 사용할 video들이 들어 있다.
 * installscript [dir]  : tensorflow lite를 C로 빌드해서 사용하려면 의존성 문제가 있었고, 이를 해결함.
 *                      flatbuffer의 버전 호환 X 버전에 맞게 가져오도록 스크립트 수정
*/

/**
 * [프로젝트 빌드 설명]
 * 0. "현재 디렉토리"에서 준비한다.
 * 1. Drivers 디렉토리에 들어가서, make를 한다.
 * 2. [sudo insmod]로 led, 7seg, button에 대한 driver module을 로드한다.
 * 3. 다시 "현재 디렉토리" 였던 곳으로 돌아온다 (이 main.py있는 디렉토리)
 * 4. [mkdir build]로 build 파일들을 만들 디렉토리를 만든다.
 * 5. [cd build && cmake ..] 커맨드를 입력해, CMakeLists.txt에 따라 프로젝트를 빌드한다.
 * 6. 그려면 build 디렉토리 안에 Makefile이 생긴다.
 * 7. build 디렉토리에 머문 채로, [make]를 실행해 주면 main.cpp가 컴파일되어, 'TermProject17' 실행 파일이 생긴다.
 * 8. [sudo ./TermProject17] 로 실행한다. (argv[1] = video file path)
 * 9. [sudo rmmod]로 driver module을 제거한다.
*/

const char* pwd = "/home/pi/working/ProjectCode/CTesting/";

// 이 모델이 예측할 수 있는 전체 라벨들
// 그러나 person에 대해서만 한정해서 생각
const std::vector<std::string> labels = {
  "???", "person", "bicycle", "car", "motorcycle",
  "airplane", "bus", "train", "truck", "boat",
  "traffic light", "fire hydrant", "???", "stop sign",
  "parking meter", "bench", "bird", "cat", "dog",
  "horse", "sheep", "cow", "elephant", "bear",
  "zebra", "giraffe", "???", "backpack", "umbrella",
  "???", "???", "handbag", "tie", "suitcase",
  "frisbee", "skis", "snowboard", "sports ball", "kite",
  "baseball bat", "baseball glove", "skateboard", "surfboard", "tennis racket",
  "bottle", "???", "wine glass", "cup", "fork",
  "knife", "spoon", "bowl", "banana", "apple",
  "sandwich", "orange", "broccoli", "carrot", "hot dog",
  "pizza","donut","cake","chair","couch",
  "potted plant","bed","???","dining table","???",
  "???","toilet","???","tv","laptop",
  "mouse","remote","keyboard","cell phone","microwave",
  "oven","toaster","sink","refrigerator","???",
  "book","clock","vase","scissors","teddy bear",
  "hair drier","toothbrush"
};


char seg_num[10] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xd8, 0x80, 0x90};
char seg_dnum[10] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x58, 0x00, 0x10};
const unsigned short LOOP_DELAY    = 100;

void displaySegment(int segDevice, int number) {
  const unsigned short SEGMENT_DELAY = 100;
  unsigned short DisplayBuff[4] = {0};

  DisplayBuff[0] = 0; // (seg_num[ number / 1000        ]   << 4) | 0x01; // first bit
  DisplayBuff[1] = 0; // (seg_num[(number % 1000) / 100 ]   << 4) | 0x02; // second bit
  DisplayBuff[2] = 0; // (seg_num[(number % 100) / 10   ]   << 4) | 0x04; // third bit
  DisplayBuff[3] = (seg_num[ number % 10          ]   << 4) | 0x08; // fourth bit

  for (unsigned char index = 0; index < 4; index++) {
    write(segDevice, &DisplayBuff[index], sizeof(unsigned short));
    usleep(SEGMENT_DELAY);
  }
}

int main(int argc, char* argv[]) {
  // >>> arguments check하기 >>>
  if (argc > 2) {
    std::cerr << "not available number of arguments!" << std::endl;
    return -1;
  }
  // <<< <<<

  // >>> device open하기
  int ButtonDevice  = open("/dev/my_button",  O_RDWR);
  int SegmentDevice = open("/dev/my_segment", O_RDWR);
  int LedDevice     = open("/dev/my_led",     O_RDWR);

  // Exception Handling
  if (SegmentDevice == -1) {
    printf("7 Segment Device access failure!\n");
    return -1;
  }
  else printf("7 Segment Device access successful!\n");

  if (ButtonDevice  == -1) {
    printf("Button Devices access failure!\n");
    return -1;
  }
  else printf("Button Devices access successful!\n");

  if (LedDevice     == -1) {
    printf("Led Device access failure!\n");
    return -1;
  }
  else printf("Led Device access successful!\n");
  // << device <<

  displaySegment(SegmentDevice, 3); // segment 0 으로 초기값

  // AI
  std::string modelname = pwd + std::string("model/model.tflite");

  // Load the model
  std::unique_ptr<tflite::FlatBufferModel> model =
    tflite::FlatBufferModel::BuildFromFile(modelname.c_str());

  // interpreter 빌드하기
  tflite::ops::builtin::BuiltinOpResolver resolver;
  std::unique_ptr<tflite::Interpreter> interpreter;
  tflite::InterpreterBuilder(*model, resolver)(&interpreter);
  interpreter->AllocateTensors();

  // Open video file
  cv::VideoCapture cap;
  if      (argc == 1) cap.open(pwd + std::string("videos/video3.mp4")); // 따로 입력없으면 video3
  else if (argc == 2) cap.open(pwd + std::string(argv[1]));             // 현재 디렉토리에 대한 상대경로로
                                                                        // video path 설정가능
  if (!cap.isOpened()) {
    std::cerr << "Error opening video file" << std::endl;
    return -1;
  }

  cv::Mat frame;
  unsigned int max_people = 0; // 화면에 잡혀도 되는 최대의 사람들 수
  unsigned char buttoninput = '0';
  unsigned char ledoutput = 'd';

  while (cap.read(frame)) {
    read(ButtonDevice, &buttoninput, sizeof(buttoninput));
    write(LedDevice, &ledoutput, sizeof(ledoutput));
    // 화면에서 너무 창이 크게 뜨는 문제 발생 : 이를 수정함
    double scale = 640.0 / frame.cols; // 가로 640픽셀 기준으로 스케일 계산
    int newHeight = cvRound(frame.rows * scale); // 종횡비 유지를 위한 높이 계산
    cv::resize(frame, frame, cv::Size(640, newHeight)); // 프레임 크기 조절

    cv::Mat cvt;
    cv::resize(frame, cvt, {300, 300});

    // tflite는 입력 데이터와 크기가 같은 데이터를 입력해 invoke할 수 있고
    // 이렇게 해 줘야 사용을 위한 준비가 완료된다.
    auto* input = interpreter->typed_input_tensor<unsigned char>(0);
    std::memcpy(input, cvt.data, 300*300*3);

    interpreter->Invoke();

    auto rects = interpreter->typed_output_tensor<float>(0);
    auto classes = interpreter->typed_output_tensor<float>(1);
    auto scores = interpreter->typed_output_tensor<float>(2);
    auto numDetect = interpreter->typed_output_tensor<float>(3);

    const auto size = interpreter->output_tensor(0)->dims->size;

    unsigned int nums_people = 0;
    for(int i=0; i<size; ++i){
      if (labels[std::floor(classes[i]+1.5)] != "person") continue;
      cv::rectangle(frame, // 감지된 사람들 주위에 사각형으로 감싼다
                    cv::Point2d{rects[i*4+1] * frame.cols, rects[i*4] * frame.rows},
                    cv::Point2d{rects[i*4+3] * frame.cols, rects[i*4+2] * frame.rows},
                    {255,0,0}, 2);

      char buf[10];
      std::sprintf(buf, "(%.1f%%)",scores[i]*100);
      cv::putText(frame, // 감지된 사람들의 위에 라벨링을 한다
                  labels[std::floor(classes[i]+1.5)] + std::string(buf),
                  cv::Point2d{rects[i*4+1] * frame.cols, rects[i*4] * frame.rows - 4},
                  cv::FONT_ITALIC, 0.4, {0,0,0}, 1);
      nums_people++; // 감지된 사람들을 센다
      }

      if (buttoninput == 'u') {
        if (max_people >= 9999) max_people = 0;
        else                    max_people++;
      }
      else if (buttoninput == 'd') {
        if (max_people <= 0)    max_people = 9999;
        else                    max_people--;
      }

    cv::putText(frame, // 화면에서 감지된 사람들의 수를 적는다
                std::to_string(nums_people) + "/" + std::to_string(max_people) + std::string("people detected"),
                cv::Point2d{0, 10}, cv::FONT_ITALIC, 0.4, {0,255, 0}, 1);
    
    displaySegment(SegmentDevice, max_people);
    buttoninput = '0';
    ledoutput = (nums_people >= max_people) ? 'u' : 'd';

    cv::imshow("Result", frame);
    if (cv::waitKey(30) >= 0) break;
  }

  write(SegmentDevice, (char*)NULL, sizeof(char) * 3);
  close(SegmentDevice);
  close(ButtonDevice);
  close(LedDevice);

  return 0;
}
