#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/kernels/register.h"

/**
*   [IDEA]
*   1. Setup 
*       Open Camera Device && Start Live Streaming
*           i  ) Open device via cv::VideoCapture() Class
*           ii ) Handle possible errors
*           iii) Load the Model
*   2. Loop 
*       Read Each Frames && React
*           i  ) Capture each frames and store it into 'cv::Mat frame' variable
*           ii ) Handle possible errors from 'video.read()'
*           ii ) Use "Tensorflow Lite" Model to detect sleepy faces                     -> handleFrame(cv::Mat&)
*           iii) Detect the user motion && label the object with current status         -> handleFrame(cv::Mat&)
*           iv ) Show the labeled frame with new window
*           v  ) Create the window using cv::waitKey() and break this loop if necessary
*   3. CleanUp 
*       Close all windows and free all allocated memory && etc.
*           i  ) video.release()
*           ii ) cv::destroyAllWindows()
*/

// >>> OPENI EXISTING MODEL >>>
#define MODEL_PATH "path/to/your/model.tflite"

enum class ModelOpenStatus {
    INVALID_FILEPATH_ERROR,
    MODEL_LOAD_FAILURE_ERROR,
    MODEL_LOAD_SUCCESSFUL
};

ModelOpenStatus openModel(std::unique_ptr<tflite::FlatBufferModel>& model) {
    std::ifstream ifile(MODEL_PATH); // 오타 수정
    if (!ifile) return ModelOpenStatus::INVALID_FILEPATH_ERROR; // Enum 클래스 이름 사용
    
    model = tflite::FlatBufferModel::BuildFromFile(MODEL_PATH); // 함수 호출 수정
    if (!model) return ModelOpenStatus::MODEL_LOAD_FAILURE_ERROR;

    std::unique_ptr<tflite::Interpreter> interpreter;
    tflite::ops::builtin::BuiltinOpResolver resolver;
    tflite::InterpreterBuilder(*model, resolver)(&interpreter);

    return ModelOpenStatus::MODEL_LOAD_SUCCESSFUL;
}
// <<< OPEN EXIStING MODEL <<< 

// >>> HANDLE VIDEO FRAME >>>

// <<< HANDLE VIDEO FRAME <<< 

int main(int argc, char* argv[]) {
    // Open 'video0' device
    // Use VideoCapture Class to get frames 
    cv::VideoCapture video("/dev/video0", cv::CAP_V4L2);
    // Error Handling
    // return Error Code if Camera Opening Fails
    if (!video.isOpened()) {
        std::cerr << "Camera Open Failure" << std::endl;
        return -1;
    }
    // cv::Mat frame -> Variable to store Each frames of the video
    cv::Mat frame;
    while (true) {
        // initial Loop Condition 1
        // read frame each loop && break if frame is empty
        video.read(frame);
        if (frame.empty()) break;

        // Handle Each Frames Here
        handleFrame(frame);

        // Show the captured frame each loop
        // This function will create a window "Frame" and show the current frame
        // this function shows the window if cv::waitKey() is executed
        cv::imshow("Frame", frame);
        // keyboard input [q] break the loop 
        // cv::waitKey(1) returns current keyboard input
        if (cv::waitKey(1) == 'q') break;
    }
    // close all windows and free all allocated memory
    video.release();
    cv::destroyAllWindows();
    // return 0 -> no error exit code
    return 0;
}
