#include "opencv2/opencv.hpp"
#include "iostream"

using namespace std;
using namespace cv;

/**
 * Mat的基本使用
 */
void test1() {
    Mat src = imread("a.jpeg");

    //判断读取是否正确
    if (src.empty()) {
        cout << "src imread error" << endl;
        return;
    }
    //也可以用这种方式判断
    //src.data是二维数组的头指针
    if (!src.data) {
        cout << "src imread error" << endl;
        return;
    }

    //获取信息
    int cols = src.cols;
    int rows = src.rows;
    int channels = src.channels();
    cout << cols << " " << rows << " " << channels << endl;

    /**
     * 创建Mat1
     * CV_8UC1 单颜色通道 0~255
     * CV_8UC2 2颜色通道 16位  对应RGB565
     * CV_8UC3 3颜色通道 24位
     * CV_8UC4 4颜色通道 32位  对应ARGB8888
     *
     * Scalar指定颜色，0是黑色，255是白色，打印出来是下面这样的
     * [255, 255, 255;
        255, 255, 255;
        255, 255, 255]
     */
//    Mat mat(3, 3, CV_8UC1, Scalar(255));
//    cout << mat << endl;

    //创建方式2
    Mat mat(Size(20, 20), CV_8UC3, Scalar(0, 0, 255));

    //创建方式3，拷贝构造函数，指向的是同一个区域，所以是不会拷贝内容的
    Mat mat1(src);

    //这种方式会拷贝内容
    Mat mat2;
    src.copyTo(mat2);

    //这种方式也会拷贝内容
    Mat mat3 = src.clone();

    imshow("mat", mat2);
    waitKey(0);
}

/**
 * 区域截取，不改变原图
 */
void test2() {
    Mat src = imread("a.jpeg");

    //区域截取，指向的还是src里的内容，并没有创建新的Mat
    Mat srcROI = src(Rect(20, 20, 400, 400));

    //这种方式会修改到src
    int rows = srcROI.rows;
    int cols = srcROI.cols;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            //获取像素，Mat中是BGR
            //Vec3b表示3个参数
            int b = srcROI.at<Vec3b>(i, j)[0];
            int g = srcROI.at<Vec3b>(i, j)[1];
            int r = srcROI.at<Vec3b>(i, j)[2];

            //修改像素
            srcROI.at<Vec3b>(i, j)[0] = 255 - b;
            srcROI.at<Vec3b>(i, j)[1] = 255 - g;
            srcROI.at<Vec3b>(i, j)[2] = 255 - r;
        }
    }

    imshow("mat", src);
    waitKey(0);
}

/**
 * 区域截取，改变原图
 */
void test3() {
    Mat src = imread("a.jpeg");

    //区域截取，指向的还是src里的内容，并没有创建新的Mat
    Mat srcROI = src(Rect(20, 20, 400, 400));
    Mat dstROI = srcROI.clone();

    int rows = dstROI.rows;
    int cols = dstROI.cols;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            //获取像素，Mat中是BGR
            //Vec3b表示3个参数
            int b = dstROI.at<Vec3b>(i, j)[0];
            int g = dstROI.at<Vec3b>(i, j)[1];
            int r = dstROI.at<Vec3b>(i, j)[2];

            //修改像素
            dstROI.at<Vec3b>(i, j)[0] = 255 - b;
            dstROI.at<Vec3b>(i, j)[1] = 255 - g;
            dstROI.at<Vec3b>(i, j)[2] = 255 - r;
        }
    }

    imshow("mat", src);
    waitKey(0);
}

/**
 * 灰度图，变一个通道了，要进行判断
 */
void test4() {
    Mat src = imread("a.jpeg");

    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);
    //灰度图是一个通道
    int channel = gray.channels();

    int rows = gray.rows;
    int cols = gray.cols;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (channel == 3) {
                //3个通道这么取
                int b = gray.at<Vec3b>(i, j)[0];
                int g = gray.at<Vec3b>(i, j)[1];
                int r = gray.at<Vec3b>(i, j)[2];

                gray.at<Vec3b>(i, j)[0] = 255 - b;
                gray.at<Vec3b>(i, j)[1] = 255 - g;
                gray.at<Vec3b>(i, j)[2] = 255 - r;
            } else if (channel == 1) {
                //一个通道要这么取，这里把它弄成底片效果
                uchar pixels = gray.at<uchar>(i, j);
                gray.at<uchar>(i, j) = 255 - pixels;
            }
        }
    }

    imshow("mat", gray);
    waitKey(0);
}

/**
 * 自己实现灰度图
 */
void test5() {
    Mat src = imread("a.jpeg");
    int cols = src.cols;
    int rows = src.rows;
    int channel = src.channels();

    Mat gray(rows, cols, CV_8UC1);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (channel == 3) {
                int b = src.at<Vec3b>(i, j)[0];
                int g = src.at<Vec3b>(i, j)[1];
                int r = src.at<Vec3b>(i, j)[2];

                gray.at<uchar>(i, j) = 0.11f * r + 0.59f * g + 0.30f * b;
            }
        }
    }

    imshow("mat", gray);
    waitKey(0);
}

/**
 * 图片混合
 */
void test6() {
    Mat src = imread("a.jpeg");
    if (src.empty()) {
        cout << "src imread error" << endl;
        return;
    }
    Mat cSrc = imread("c.jpeg");
    if (cSrc.empty()) {
        cout << "cSrc imread error" << endl;
        return;
    }
    Mat logo = imread("logo.jpeg");
    if (logo.empty()) {
        cout << "logo imread error" << endl;
        return;
    }
    Mat dst;
    //这种方式需要保证两张图片大小一致，而且效果不好
//    add(src, logo, dst);

    //需要2张图片大小一致
    //gamma是亮度
    //下面等同于 saturate_cast(src(x)*alpha + logo(x)*beta + gamma);
//    addWeighted(src, 0.8, cSrc, 0.2, 0.0, dst);
//    imshow("src", dst);

    //两张图片大小不一致的混合
    //思想：从原图截取一部分跟logo一样的大小进行混合
    Mat srcROI = src(Rect(0, 0, logo.cols, logo.rows));
    addWeighted(srcROI, 0.8, logo, 0.2, 0.0, srcROI);
    imshow("src", src);

    waitKey(0);
}

/**
 * 饱和度、亮度、对比度 （https://zhuanlan.zhihu.com/p/546259316）
 * 饱和度：R -> 1.4R，就更加饱和
 * 亮度：亮暗程度
 * 对比度：最亮的和最暗的比
 */
void test7() {
    Mat src = imread("a.jpeg");
    if (src.empty()) {
        cout << "src imread error" << endl;
        return;
    }
    //调alpha可以调饱和度和对比度，调beta可以调亮度
    //F(R) = alpha*R + beta;
    //F(G) = alpha*G + beta;
    //F(B) = alpha*B + beta;

    int cols = src.cols;
    int rows = src.rows;
    int channel = src.channels();

    float alpha = 1.5;
    int beta = -50;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (channel == 3) {
                int b = src.at<Vec3b>(i, j)[0];
                int g = src.at<Vec3b>(i, j)[1];
                int r = src.at<Vec3b>(i, j)[2];

                src.at<Vec3b>(i, j)[0] = saturate_cast<uchar>(b * alpha + beta);
                src.at<Vec3b>(i, j)[1] = saturate_cast<uchar>(g * alpha + beta);
                src.at<Vec3b>(i, j)[2] = saturate_cast<uchar>(r * alpha + beta);
            }
        }
    }
    imshow("src", src);

    waitKey(0);
}

int main() {
    test7();
}
