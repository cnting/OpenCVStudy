#include "opencv2/opencv.hpp"
#include "iostream"

using namespace std;
using namespace cv;

/**
 * 轮廓查找
 */
void test1() {
    Mat src = imread("card1.jpeg");
    Mat binary;
    Canny(src, binary, 50, 150);
    imshow("binary", binary);

    vector<vector<Point>> contours;
    findContours(binary, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    Mat contours_mat = Mat::zeros(src.size(), CV_8UC3);
    Rect card_rect;
    for (int i = 0; i < contours.size(); i++) {
        //画轮廓
        Rect rect = boundingRect(contours[i]);
        if (rect.width > src.cols / 2 && rect.height > src.rows / 2) {
            drawContours(contours_mat, contours, i, Scalar(0, 0, 255), 1);
            card_rect = rect;

            rectangle(contours_mat, Point(rect.x, rect.y), Point(rect.x + rect.width, rect.y + rect.height),
                      Scalar(255, 255, 255), 2);
            break;
        }
    }

    imshow("dst", contours_mat);

    waitKey(0);
}

int main() {
    test1();
}
