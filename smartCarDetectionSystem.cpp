#include "opencv.hpp"
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

void createLaneMask(Mat& input, Mat& mask) {
    mask = Mat::zeros(input.size(), CV_8UC1);
    Point trapezoid[1][4];

    trapezoid[0][0] = Point(input.cols * 0.1, input.rows);
    trapezoid[0][1] = Point(input.cols * 0.4, input.rows * 0.6);
    trapezoid[0][2] = Point(input.cols * 0.6, input.rows * 0.6);
    trapezoid[0][3] = Point(input.cols * 0.9, input.rows);

    const Point* ppt[1] = { trapezoid[0] };
    int npt[] = { 4 };

    fillPoly(mask, ppt, npt, 1, Scalar(255), 8);
}

bool getLaneLine(const vector<Vec4i>& lines, Vec2f& line_params) {
    if (lines.empty()) return false;

    double avg_m = 0, avg_b = 0;
    int count = 0;
    for (Vec4i l : lines) {
        double m = ((double)l[3] - (double)l[1]) / ((double)l[2] - (double)l[0] + 1e-6);
        double b = (double)l[1] - m * (double)l[0];

        if (abs(m) < 0.4) continue;

        avg_m += m;
        avg_b += b;
        count++;
    }

    if (count == 0) return false;

    avg_m /= count;
    avg_b /= count;

    line_params[0] = (float)avg_m;
    line_params[1] = (float)avg_b;

    return true;
}

int countingNonZeros(const Mat& mask) {

    int count = 0;

    for (int y = 0; y < mask.rows; y++) {
        const uchar* row_ptr = mask.ptr<uchar>(y);

        for (int x = 0; x < mask.cols; ++x) {
            if (row_ptr[x] != 0) {
                count++;
            }
        }
    }
    return count;
}


int main() {
    VideoCapture cap;

    if (!cap.open("Project2_video.mp4")) {
        cout << "No such file!" << endl;
        waitKey(0);
        return -1;
    }

    Ptr<BackgroundSubtractor> bg_model = createBackgroundSubtractorMOG2();

    Mat fgMask;
    Rect carROI(0, 0, 0, 0);

    Mat element = getStructuringElement(MORPH_ELLIPSE, Size(7, 7));

    Mat cannyImg, gray, blurred;
    Mat roiPolyMask;

    int departureFrames = 0;
    bool laneDepartureAlert = false;
    const int DEPARTURE_THRESHOLD = 8;

    int delay, fps;
    fps = cap.get(CAP_PROP_FPS);
    if (fps > 0) {
        delay = 1000 / fps;
    }
    else {
        delay = 33;
    }

    Mat frame, displayFrame;
    bool startMoving = false;

    Vec2f current_left_line(0, 0), current_right_line(0, 0);
    bool has_current_left = false, has_current_right = false;

    cap >> frame;
    if (frame.empty()) {
        cout << "Could not read the first frame from video." << endl;
        return -1;
    }

    int roi_width = frame.cols / 10;
    int roi_height = frame.rows / 8;

    int roi_x = (frame.cols * 0.52) - (roi_width / 2);

    int roi_y = (frame.rows * 0.62);

    carROI = Rect(roi_x, roi_y, roi_width, roi_height);

    createLaneMask(frame, roiPolyMask);


    while (true) {
        displayFrame = frame.clone();
        startMoving = false;

        int lane_departure_detector_left = (int)(frame.cols * 0.3);
        int lane_departure_detector_right = (int)(frame.cols * 0.7);


        cvtColor(frame, gray, CV_BGR2GRAY);
        GaussianBlur(gray, blurred, Size(7, 7), 1.5);
        Canny(blurred, cannyImg, 50, 150, 3);

        Mat laneMask = Mat::zeros(cannyImg.size(), CV_8UC1);
        cannyImg.copyTo(laneMask, roiPolyMask);

        vector<Vec4i> lines;

        HoughLinesP(laneMask, lines, 1, CV_PI / 180, 50, 60, 80);

        vector<Vec4i> left_lines, right_lines;
        for (int i = 0; i < lines.size(); i++) {
            Vec4i l = lines[i];
            double slope = ((double)l[3] - (double)l[1]) / ((double)l[2] - (double)l[0] + 1e-6);

            if (abs(slope) > 0.4) {
                if (slope < 0) left_lines.push_back(l);
                else right_lines.push_back(l);
            }
        }

        Vec2f left_line_params, right_line_params;
        bool found_left_this_frame = getLaneLine(left_lines, left_line_params);
        bool found_right_this_frame = getLaneLine(right_lines, right_line_params);

        if (found_left_this_frame) {
            current_left_line = left_line_params;
            has_current_left = true;
        }

        if (found_right_this_frame) {
            current_right_line = right_line_params;
            has_current_right = true;
        }

        bool departed_left = false;
        bool departed_right = false;

        if (has_current_left) {
            float m = current_left_line[0];
            float b = current_left_line[1];

            int y1 = frame.rows;
            int x1 = (y1 - b) / m;

            int left_line_x_at_bottom = x1;
            if (lane_departure_detector_left < left_line_x_at_bottom) {
                departed_left = true;
            }
        }

        if (has_current_right) {
            float m = current_right_line[0];
            float b = current_right_line[1];

            int y1 = frame.rows;
            int x1 = (y1 - b) / m;

            int right_line_x_at_bottom = x1;
            if (lane_departure_detector_right > right_line_x_at_bottom) {
                departed_right = true;
            }
        }

        if (departed_left || departed_right) {
            departureFrames++;
        }
        else if (has_current_left && has_current_right) {
            departureFrames = 0;
            laneDepartureAlert = false;
        }

        if (departureFrames > DEPARTURE_THRESHOLD) {
            laneDepartureAlert = true;
        }


        bg_model->apply(frame, fgMask);

        threshold(fgMask, fgMask, 240, 255, THRESH_BINARY);

        morphologyEx(fgMask, fgMask, MORPH_OPEN, element);

        Mat carMotionMask = fgMask(carROI);
        int carMotion = countingNonZeros(carMotionMask);

        Mat outsideMotionMask = fgMask.clone();
        outsideMotionMask(carROI).setTo(Scalar(0));
        int outsideMotion = countingNonZeros(outsideMotionMask);

        if (outsideMotion < 2000 && carMotion > 100) {
            startMoving = true;
        }

        if (laneDepartureAlert) {
            putText(displayFrame, "Lane Departure!", Point(50, 50), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 0, 255), 3);
        }
        if (startMoving) {
            putText(displayFrame, "Start Moving!", Point(50, 100), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 255, 0), 3);
        }

        imshow("Project2", displayFrame);

        if (waitKey(delay) == 27) {
            break;
        }

        cap >> frame;
        if (frame.empty()) {
            cout << "End of video." << endl;
            break;
        }
    }

    return 0;
}

