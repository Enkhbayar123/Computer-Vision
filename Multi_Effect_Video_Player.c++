#include "opencv.hpp"
#include<iostream>
#include<vector>
#include<cmath>
using namespace std;
using namespace cv;

void negative(Mat frame) {
	for (int i = 0; i < frame.rows; i++) {
		for (int j = 0; j < frame.cols; j++) {
			frame.at<uchar>(i, j) = 255 - frame.at<uchar>(i, j);
		}
	}
}
void gamma(Mat frame) {
	unsigned char pix[256];
	for (int i = 0; i < 256; i++) {
		pix[i] = saturate_cast<uchar>(pow((float)(i / 255.0), 2.5) * 255.0f);
	}
	
	for (int i = 0; i < frame.rows; i++) {
		for (int j = 0; j < frame.cols; j++) {
			frame.at<uchar>(i, j) = pix[frame.at<uchar>(i, j)];
		}
	}
}
void unsharp(Mat frame) {
	Mat original = frame.clone();
	Mat blur1 = frame.clone();
	blur(frame, blur1, Size(9, 9));
	Mat mask = original - blur1;
	add(original, mask, frame);
}
void white_balancing(Mat frame) {
	Mat channels[3];
	split(frame, channels);

	double avg;
	int sum;

	for (int c = 0; c < frame.channels(); c++) {
		sum = 0;
		avg = 0.0f;
		for (int i = 0; i < frame.rows; i++) {
			for (int j = 0; j < frame.cols; j++) {
				sum += channels[c].at<uchar>(i, j);
			}
		}
		avg = sum / (frame.rows * frame.cols);
		for (int i = 0; i < frame.rows; i++) {
			for (int j = 0; j < frame.cols; j++) {
				int temp = (128 / avg) * channels[c].at<uchar>(i, j);
				if (temp > 255) channels[c].at<uchar>(i, j) = 255;
				else channels[c].at<uchar>(i, j) = temp;
			}
		}
	}
	merge(channels, 3, frame);
}
int main() {
	char operation = 'r';
	VideoCapture cap;

	if (cap.open("video.mp4") == 0) {	
		cout << "no such file" << endl;
		waitKey(0);
	}
	int fps = cap.get(CAP_PROP_FPS);
	int delay = 1000 / fps;
	Mat frame;
	while (1) {
		cap >> frame;
		Mat f = frame.clone();
		if (frame.empty()) {
			cout << "end of video" << endl;
			break;
		}
		Mat changed;
		switch (operation) {
			case 'n': {
				vector<Mat> channels;
				split(f, channels);
				for (int i = 0; i < channels.size(); i++) {
					negative(channels[i]);
				}
				merge(channels, changed);
				break;
			}
			case 'g': {
				vector<Mat> channels;
				split(f, channels);
				for (int i = 0; i < 3; i++) {
					gamma(channels[i]);
				}
				
				merge(channels, changed);
				
				break;
			}
			case 'h': {
				vector<Mat> channels;
				Mat HSV;
				cvtColor(f, HSV, COLOR_BGR2HSV);
				split(HSV, channels);
				equalizeHist(channels[2], channels[2]);
				
				merge(channels, changed);
				cvtColor(changed, changed, COLOR_HSV2BGR);
				
				break;
			}
			case 's': {
				vector<Mat> channels;
				Mat HSV;
				uchar* h;
				uchar* s;
				cvtColor(f, HSV, COLOR_BGR2HSV);
				split(HSV, channels);
				for (int i = 0; i < channels[0].rows; i++) {
					h = channels[0].ptr<uchar>(i);
					s = channels[1].ptr<uchar>(i);
					for (int j = 0; j < channels[0].cols; j++) {
						if (h[j] > 9 && h[j] < 23) s[j] = s[j];
						else s[j] = 0;
					}
				}
			
				merge(channels, changed);
				cvtColor(changed, changed, COLOR_HSV2BGR);
			
				break;
			}
			case 'c': {
				vector<Mat> channels;
				Mat HSV;
				uchar* h;
				uchar* s;
				cvtColor(f, HSV, COLOR_BGR2HSV);
				split(HSV, channels);
				for (int i = 0; i < channels[0].rows; i++) {
					h = channels[0].ptr<uchar>(i);
					s = channels[1].ptr<uchar>(i);
					for (int j = 0; j < channels[0].cols; j++) {
						if (h[j] > 129) h[j] = h[j] - 129;
						else h[j] += 50;
					}
				}
				merge(channels, changed);
				cvtColor(changed, changed, COLOR_HSV2BGR);
				break;
			}
			case 'a': {
				vector<Mat> channels;
				split(f, channels);
				vector<Mat> result(3);
				for (int i = 0; i < 3; i++) {
					blur(channels[i], result[i], Size(9, 9));
				}
				merge(result, changed);
				
				break;
			}
			case 'u': {
				vector<Mat> channels;
				split(f, channels);
				for (int i = 0; i < 3; i++) {
					unsharp(channels[i]);
				}
				
				merge(channels, changed);
				break;
			}
			case 'w': {
				white_balancing(f);
				changed = f.clone();
				break;
			}
			case 'r':
			default: {
				changed = frame.clone();
				break;
			}
		}
		imshow("video", changed);	
		int key = waitKey(delay);

		if (key != -1) {
			operation = (char)key;
		}

		if (operation == 27) {
			break;
		}
	}
	return 0;
}