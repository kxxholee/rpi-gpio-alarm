#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/kernels/register.h"


int main() {
    // 모델 및 라벨 파일 설정
    std::string model_filename = "detect.tflite";
    std::string labelmap_filename = "labelmap.txt";
    float min_conf_threshold = 0.5f;
    bool show_results = true;
    bool save_results = false;
    std::string image_path = "test1.jpg";  // 이미지 경로 설정

    // 라벨맵 로드
    std::vector<std::string> labels;
    std::ifstream file(labelmap_filename);
    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) {
            labels.push_back(line);
        }
        file.close();
    }

    // 모델 로드
    std::unique_ptr<tflite::FlatBufferModel> model = tflite::FlatBufferModel::BuildFromFile(model_filename.c_str());
    tflite::ops::builtin::BuiltinOpResolver resolver;
    std::unique_ptr<tflite::Interpreter> interpreter;
    tflite::InterpreterBuilder(*model, resolver)(&interpreter);
    interpreter->AllocateTensors();

    // 입력 및 출력 텐서 정보
    int input = interpreter->inputs()[0];
    TfLiteIntArray* dims = interpreter->tensor(input)->dims;
    int wanted_height = dims->data[1];
    int wanted_width = dims->data[2];
    int wanted_channels = dims->data[3];

    // 이미지 로드 및 전처리
    cv::Mat image = cv::imread(image_path);
    cv::Mat image_rgb;
    cv::cvtColor(image, image_rgb, cv::COLOR_BGR2RGB);
    cv::Mat image_resized;
    cv::resize(image_rgb, image_resized, cv::Size(wanted_width, wanted_height));

    // 모델 실행
    interpreter->SetAllowFp16PrecisionForFp32(true);
    interpreter->SetNumThreads(4);  // 스레드 설정
    interpreter->SetInputs({input});
    interpreter->Invoke();

    // 결과 처리 및 표시
    int output = interpreter->outputs()[0];
    TfLiteIntArray* output_dims = interpreter->tensor(output)->dims;
    // 가정: 출력 텐서는 형태 [1, num_detections, 4]인 3차원 배열임
    int num_detections = output_dims->data[1];

    float* detection_boxes = interpreter->typed_tensor<float>(output);
    float* detection_classes = interpreter->typed_tensor<float>(interpreter->outputs()[1]);
    float* detection_scores = interpreter->typed_tensor<float>(interpreter->outputs()[2]);

    for (int i = 0; i < num_detections; ++i) {
        float score = detection_scores[i];
        // 점수가 임계값보다 크면 검출된 객체를 처리
        if (score > min_conf_threshold) {
            int ymin = static_cast<int>(detection_boxes[4 * i] * image.rows);
            int xmin = static_cast<int>(detection_boxes[4 * i + 1] * image.cols);
            int ymax = static_cast<int>(detection_boxes[4 * i + 2] * image.rows);
            int xmax = static_cast<int>(detection_boxes[4 * i + 3] * image.cols);

            cv::rectangle(image, cv::Point(xmin, ymin), cv::Point(xmax, ymax), cv::Scalar(0, 255, 0), 2);

            std::string label = labels[static_cast<int>(detection_classes[i])];
            label += ": " + std::to_string(static_cast<int>(score * 100)) + "%";

            int baseLine;
            cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

            int y = std::max(ymin, labelSize.height);
            cv::rectangle(image, cv::Point(xmin, y - labelSize.height), cv::Point(xmin + labelSize.width, y + baseLine), cv::Scalar(255, 255, 255), cv::FILLED);
            cv::putText(image, label, cv::Point(xmin, y), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
        }
    }

    // 결과 이미지 표시
    if (show_results) {
        cv::imshow("Object detector", image);
        cv::waitKey(0);
    }

    // 결과 저장 (필요한 경우)
    if (save_results) {
        std::string save_path = "path/to/save/image.jpg";
        cv::imwrite(save_path, image);
    }

    // Clean up
    cv::destroyAllWindows();


    return 0;
}
