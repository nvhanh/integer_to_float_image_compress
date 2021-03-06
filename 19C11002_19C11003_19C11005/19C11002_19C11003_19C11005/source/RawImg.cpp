#include "RawImg.h"
#include "opencv2/opencv.hpp"

USG_NSP_APP();
using namespace std;
using namespace cv;


RawImg::RawImg() :
	buffer(nullptr),
	bufferLength(0),
	width(0),
	heigth(0) {
}

RawImg::RawImg(const RawImg& otherImg) {
	if (this == &otherImg)
		return;

	reset();

	bufferLength = otherImg.bufferLength;
	width = otherImg.width;
	heigth = otherImg.heigth;
	buffer = new uint8_t[bufferLength];

	if (bufferLength > 0)
		memcpy(buffer, otherImg.buffer, bufferLength);
}

RawImg::RawImg(const std::string& fileName) :
RawImg() {
	cv::Mat rawImg = imread(fileName, IMREAD_COLOR);
	if (rawImg.rows <= 0 || rawImg.cols <= 0)
		return;

	Mat rgbImg;
	rawImg.convertTo(rgbImg, CV_8UC3);

	width = rgbImg.cols;
	heigth = rgbImg.rows;
	bufferLength = rgbImg.total() * rgbImg.elemSize();
	
	buffer = new uint8_t[bufferLength];
	memcpy(buffer, rgbImg.data, bufferLength * sizeof(uint8_t));
}

RawImg::~RawImg() {
	reset();
}

void RawImg::reset() {
	if (!buffer)
		delete[] buffer;

	buffer = nullptr;
	bufferLength = 0;
	width = 0;
	heigth = 0;
}

bool RawImg::isValidImg() {
	return buffer != nullptr
		&& bufferLength > 0
		&& width > 0
		&& heigth > 0;
}

int32_t RawImg::show(const std::string& windowsName, int32_t maxWidth, int32_t maxHeigth) {
	if (!isValidImg()) {
		return ErrCode::kInvalidImg;
	}

	Mat img(cv::Size(width, heigth), CV_8UC3, buffer);

	if (width <= maxWidth && heigth <= maxHeigth)
		imshow(windowsName, img);
	else {
		cv::Mat tmp = img.clone();
		cv::Size curSize = tmp.size();
		cv::Size newSize = curSize.width / (double)maxWidth > curSize.height / (double)maxHeigth
			? cv::Size(maxWidth, curSize.height * maxWidth / curSize.width)
			: cv::Size(curSize.width * maxHeigth / curSize.height, maxHeigth);

		cv::resize(img, tmp, newSize);
		imshow(windowsName, tmp);
	}

	waitKey(0);

	return ErrCode::kSuccess;
}

int32_t RawImg::save(const std::string& fileName) {
	if (!isValidImg()) {
		return ErrCode::kInvalidImg;
	}

	Mat img(cv::Size(width, heigth), CV_8UC3, buffer);
	return imwrite(fileName, img) ? ErrCode::kSuccess : ErrCode::kSaveFileFailed;
}