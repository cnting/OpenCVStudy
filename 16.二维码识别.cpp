#include "opencv2/opencv.hpp"
#include "iostream"

using namespace std;
using namespace cv;


/**
 * 仿射变换，把斜的二维码变正
 */
Mat warpTransform(const Mat &gray, const RotatedRect &rect) {
    int width = rect.size.width;
    int height = rect.size.height;
    Mat result(Size(width, height), gray.type());

    vector<Point> srcPoints;
    Point2f pts[4];
    rect.points(pts);
    for (int i = 0; i < 4; i++) {
        srcPoints.push_back(pts[i]);
    }
    vector<Point> dstPoints;
    dstPoints.push_back(Point(0, 0));
    dstPoints.push_back(Point(width, 0));
    dstPoints.push_back(Point(width, height));
    dstPoints.push_back(Point(0, height));

    Mat M = findHomography(srcPoints, dstPoints);
    warpPerspective(gray, result, M, result.size());
    return result;
}

bool isXCorner(const Mat &qrROI) {
    int cb = 0, lw = 0, rw = 0, lb = 0, rb = 0;
    //拿中间像素的值
    int width = qrROI.cols;
    int height = qrROI.rows;
    int cx = width / 2;
    int cy = height / 2;
    uchar pixels = qrROI.at<uchar>(cy, cx);
    //中间像素值一定是黑色的
    if (pixels == 255) {
        return false;
    }
    //求中心黑色
    int start = 0, end = 0, offset = 0;
    bool findLeft = false, findRight = false;
    while (true) {
        offset++;
        if ((cx - offset) <= 0 || (cx + offset) >= width - 1) {
            break;
        }
        //往左
        pixels = qrROI.at<uchar>(cy, cx - offset);
        if (!findLeft && pixels == 255) {
            start = cx - offset;
            findLeft = true;
        }
        //往右
        pixels = qrROI.at<uchar>(cy, cx + offset);
        if (!findRight && pixels == 255) {
            end = cx + offset;
            findRight = true;
        }
        if (findLeft && findRight) {
            break;
        }
    }
    if (start == 0 || end == 0) {
        return false;
    }
    cb = end - start;

    //找相间的白色
    for (int col = end; col < width - 1; col++) {
        pixels = qrROI.at<uchar>(cy, col);
        //遇到黑色跳出
        if (pixels == 0) {
            break;
        } else {
            rw++;
        }
    }
    for (int col = start; col > 0; col--) {
        pixels = qrROI.at<uchar>(cy, col);
        //遇到黑色跳出
        if (pixels == 0) {
            break;
        } else {
            lw++;
        }
    }
    if (rw == 0 || lw == 0) {
        return false;
    }
    //找两边的黑色
    for (int col = end + rw; col < width - 1; col++) {
        pixels = qrROI.at<uchar>(cy, col);
        if (pixels == 255) {
            break;
        } else {
            rb++;
        }
    }
    for (int col = start - lw; col > 0; col--) {
        pixels = qrROI.at<uchar>(cy, col);
        if (pixels == 255) {
            break;
        } else {
            lb++;
        }
    }
    if (rb == 0 || lb == 0) {
        return false;
    }

    float sum = cb + lb + rb + lw + rw;

    //求比例
    cb = cb / sum * 7.0 + 0.5;
    lb = lb / sum * 7.0 + 0.5;
    rb = rb / sum * 7.0 + 0.5;
    lw = lw / sum * 7.0 + 0.5;
    rw = rw / sum * 7.0 + 0.5;

    if ((cb == 3 || cb == 4)
        && (lb == rb)
        && (lw == rw)
        && (lb == lw)
        && (lw == 1)) {
        return true;
    }

    return false;
}

bool isYCorner(const Mat &qrROI) {
    int bp = 0, wp = 0;
    int width = qrROI.cols;
    int height = qrROI.rows;
    int cx = width / 2;
    uchar pixels;
    for (int row = 0; row < height; row++) {
        pixels = qrROI.at<uchar>(row, cx);
        if (pixels == 0) {
            bp++;
        } else if (pixels == 255) {
            wp++;
        }
    }
    if (bp == 0 || wp == 0) {
        return false;
    }
    if (wp * 2 > bp || bp > 4 * wp) {
        return false;
    }
    return true;
}

/**
 * 1.检测图片中是否包含二维码
 * 思路：
 * （1）图像转灰度
 * （2）图片二值化，自动阈值
 * （3）查找轮廓
 * （4）轮廓的筛选，轮廓大小>49，宽高比>0.8
 * （5）比对特征比例：7:3:1
 */
void test1() {
//    Mat src = imread("code5.png");
    Mat src = imread("code3.jpg");
//    Mat src = imread("code4.jpg");
    if (!src.data) {
        cout << "图片有误" << endl;
        return;
    }
    //转灰度图
    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);
    //二值化
    threshold(gray, gray, 0.0, 255, THRESH_BINARY | cv::THRESH_OTSU);
    //轮廓发现
    vector<vector<Point>> contours;
    findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
    int left = src.cols, top = src.rows, right = 0, bottom = 0;
    for (int i = 0; i < contours.size(); i++) {
        //过滤面积
        double area = contourArea(contours[i]);
        if (area < 49) {
            continue;
        }
        //过滤宽高比和宽高大小
        RotatedRect rect = minAreaRect(contours[i]);
        float w = rect.size.width;
        float h = rect.size.height;
        float ration = min(w, h) / max(w, h);//宽高比
        if (ration > 0.8 && w < gray.cols / 2 && h < gray.rows / 2) {
            //满足宽高大小
            Mat qrROI = warpTransform(gray, rect);
            if (isYCorner(qrROI) && isXCorner(qrROI)) {
                drawContours(src, contours, i, Scalar(0, 0, 255), 4);

                //找二维码边界
                for (auto &k: contours[i]) {
                    if (left > k.x) {
                        left = k.x;
                    }
                    if (right < k.x) {
                        right = k.x;
                    }
                    if (top > k.y) {
                        top = k.y;
                    }
                    if (bottom < k.y) {
                        bottom = k.y;
                    }
                }
            }
        }
    }

    imshow("src", src);

    //截取出二维码
    Mat qr = src(Rect(left, top, right - left, bottom - top));

    imshow("qr", qr);

    waitKey(0);
}


int main() {
    test1();
}
