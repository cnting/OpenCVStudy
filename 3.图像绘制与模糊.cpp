#include "opencv2/opencv.hpp"
#include "iostream"

using namespace std;
using namespace cv;

/**
 * 绘制形状和文字
 */
void test1() {
    Mat src = imread("a.jpeg");
    //画线
    //LINE_AA：抗锯齿
//    line(src, Point(100, 100), Point(200, 200), Scalar(255, 0, 0), 2, LINE_4);

    //矩形
//    rectangle(src, Point(100, 100), Point(300, 300), Scalar(0, 0, 255), 2, LINE_8);

    //椭圆
//    ellipse(src, Point(src.cols / 2, src.rows / 2), Size(src.cols / 8, src.rows / 4), 180, 0, 360, Scalar(0, 255, 0),1);

    //圆
    //LINE_AA表示抗锯齿，https://blog.csdn.net/wenhao_ir/article/details/125535836
//    circle(src, Point(src.cols / 2, src.rows / 2), 100, Scalar(255, 255, 0), 3,LINE_AA);

    //填充多边形
//    Point pts[1][5];
//    pts[0][0] = Point(100, 100);
//    pts[0][1] = Point(100, 200);
//    pts[0][2] = Point(200, 300);
//    pts[0][3] = Point(200, 100);
//    pts[0][4] = Point(100, 100);
//
//    const Point *ptss[] = {pts[0]};
//    const int npts[] = {5};
//    fillPoly(src, ptss, npts, 1, Scalar(255, 255, 0), 8);

    //画字，基于左上角画
//    putText(src, "Hello OpenCV", Point(100, 100), FONT_HERSHEY_COMPLEX, 1, Scalar(255, 125, 0), 1, LINE_AA);

    //随机画，rng生成随机数
    RNG rng(time(NULL));
    //创建一张图，与src的宽高和颜色通道一致，所有点都是0
    Mat dst = Mat::zeros(src.size(), src.type());
    for (int i = 0; i < 1000; i++) {
        Point sp;
        sp.x = rng.uniform(0, dst.cols);
        sp.y = rng.uniform(0, dst.rows);

        Point ep;
        ep.x = rng.uniform(0, dst.cols);
        ep.y = rng.uniform(0, dst.rows);

        Scalar color(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        line(dst, sp, ep, color);

    }

    imshow("src", dst);
    waitKey(0);
}

/**
 * 卷积的概念：https://blog.csdn.net/chaipp0607/article/details/72236892
 *
 * 自己实现卷积操作
 * 边缘锐化
 * [ 0,-1,0
 *  -1,5,-1
 *  0,-1,0]
 */
void test2() {
    Mat src = imread("a.jpeg");
    Mat dst = Mat::zeros(src.size(), src.type());
    int cols = (src.cols - 1) * src.channels();
    int rows = src.rows;
    int offsetX = src.channels();
    for (int row = 1; row < rows - 1; row++) {
        uchar *previous = src.ptr<uchar>(row - 1);
        uchar *current = src.ptr<uchar>(row);
        uchar *next = src.ptr<uchar>(row + 1);
        uchar *output = dst.ptr<uchar>(row);
        for (int col = offsetX; col < cols; col++) {
            output[col] = saturate_cast<uchar>(
                    5 * current[col] -
                    (current[col - offsetX] + current[col + offsetX] + previous[col] + next[col]));
        }
    }
    imshow("src", dst);
    waitKey(0);
}

/**
 * opencv提供了掩膜操作，掩膜的概念：https://blog.csdn.net/bitcarmanlee/article/details/79132017
 * 简单来说掩膜就是两幅图像之间进行的各种位运算操作。
 */
void test3() {
    Mat src = imread("a.jpeg");
    Mat dst;
    Mat kernel = (Mat_<char>(3, 3) << 0, -1, 0,
            -1, 5, -1,
            0, -1, 0);

    /**
     * 第一个参数: 输入图像
       第二个参数: 输出图像，和输入图像具有相同的尺寸和通道数量
       第三个参数: 目标图像深度，输入值为-1时，目标图像和原图像深度保持一致。
       第四个参数: 卷积核，是一个矩阵
       第五个参数：内核的基准点(anchor)，其默认值为(-1,-1)说明位于kernel的中心位置。基准点即kernel中与进行处理的像素点重合的点。
       第五个参数: 在储存目标图像前可选的添加到像素的值，默认值为0
       第六个参数: 像素向外逼近的方法，默认值是BORDER_DEFAULT。
     */
    filter2D(src, dst, src.depth(), kernel);
    imshow("src", dst);
    waitKey(0);
}

/**
 * 模糊分为：
 * 1.均值模糊 blur。均值模糊里的卷积的kernel矩阵每个值都是1
 * 2.高斯模糊 GaussianBlur
 * 3.中值滤波，主要用来降噪，对所有考虑的点进行排序，取中间的那个值
 * 4.双边滤波，主要用来美容
 */
void test4() {
    Mat src = imread("a.jpeg");
    //均值模糊
    Mat dst;
    //size宽高只能是奇数
    blur(src, dst, Size(121, 121), Point(-1, -1));
    imshow("blur", dst);

    //高斯模糊
    Mat gaussian;
    //sigmaX、sigmaY:用于控制正态分布是否平缓
    //最后还有个参数borderType，控制 卷积操作时 边缘是如何填充的
    GaussianBlur(src, gaussian, Size(121, 121), 0);
    imshow("gaussian", gaussian);
    waitKey(0);
}

/**
 * 中值滤波，主要用来降噪，对所有考虑的点进行排序，取中间的那个值
 * https://blog.51cto.com/yang/4585251
 */
void test5() {
    //使用椒盐图片
    Mat src = imread("d.webp");
    Mat dst;
    medianBlur(src, dst, 7);
    imshow("dst", dst);
    waitKey(0);
}

/**
 * 双边滤波，主要用来美容。
 * 基于高斯，但能做到边缘的保护（使边缘明显一点）
 */
void test6() {
    Mat src = imread("e.jpeg");
    Mat dst;
    //这几个值是经验值
    bilateralFilter(src, dst, 15, 100, 5);
    imshow("dst", dst);

    //上一步做完会比较糊，再进行掩膜，锐化一下
    Mat final;
    Mat kernel = (Mat_<char>(3, 3) << 0, -1, 0,
            -1, 5, -1,
            0, -1, 0);
    filter2D(dst, final, dst.depth(), kernel);
    imshow("final", final);

    waitKey(0);
}

/**
 * 卷积操作时，处理边缘类型
 */
void test7() {
    Mat src = imread("a.jpeg");
    Mat dst;
    //BORDER_DEFAULT：用周边边缘来填充
    //BORDER_WRAP：用对边填充
    //BORDER_REFLECT:用周边像素来填充
    //BORDER_CONSTANT:用常数来填充
    copyMakeBorder(src, dst, src.rows / 8, src.rows / 8, src.cols / 8, src.cols / 8, BORDER_CONSTANT,
                   Scalar(255, 0, 0));
    imshow("dst", dst);
    waitKey(0);
}



int main() {
    test7();
}