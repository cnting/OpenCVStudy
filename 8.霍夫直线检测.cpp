#include "opencv2/opencv.hpp"
#include "iostream"

using namespace std;
using namespace cv;

/**
 * https://www.bilibili.com/video/BV1Ay4y117HV/?p=242&spm_id_from=pageDriver&vd_source=cfa545ca14ba2d7782dd4c30ae22638e
 * https://blog.csdn.net/shuiyixin/article/details/104541495
 */
void test1() {
    Mat src = imread("road.jpeg");
    //1.Canny边缘检测
    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);

    Mat canny;
    Canny(gray, canny, 140, 250, 3, false);
    imshow("canny", canny);

    //2.
    //lines:信息里包含多条直线，每条直线两个点
    //rho:像素间隙扫描
    //theta:每次增加的角度,CV_PI/180表示增加1度
    //threshold:低阈值
    //minLineLength：线的最小长度
    //maxLineGap：点之间的间隙。在做canny时，原本是同一线的可能会断开出现一些间隙，这里指定maxLineGap以内的间隙可以认为忽略，也就是把这个间隙连起来
    vector<Vec4f> plines;
    HoughLinesP(canny, plines, 1, CV_PI / 360, 170, 30, 5);
    for (int i = 0; i < plines.size(); i++) {
        Vec4f pline = plines[i];
        //画线
        line(src, Point(pline[0], pline[1]), Point(pline[2], pline[3]), Scalar(0, 0, 255), 4, LINE_AA);
    }
    imshow("src", src);


    waitKey(0);
}

/**
 * https://www.bilibili.com/video/BV1Ay4y117HV/?p=243&spm_id_from=pageDriver&vd_source=cfa545ca14ba2d7782dd4c30ae22638e
 * 霍夫圆的检测计算量比较大
 * 可以用其他方式，根据梯度和梯度方向计算，步骤：
 * 1.梯度求取
 * 2.根据梯度方向投票选圆心
 * 3.根据圆心选择半径
 * 4.找出所有圆的点
 */
void test2() {
    Mat src = imread("circle.jpeg");
    Mat gray;
    medianBlur(src,src,5);
    cvtColor(src, gray, COLOR_BGR2GRAY);


    vector<Vec3f> circles;
    //circles:输出圆信息，每个矢量里包含3个元素（x,y,radius）
    //method:检测方法
    //dp:分辨率，如果dp= 1时，累加器和输入图像具有相同的分辨率。如果dp=2，累加器便有输入图像一半那么大的宽度和高度
    //minDist：两个圆心之间的最小距离，小于这个值就当做是同一个圆
    //parma1:表示传递给canny边缘检测算子的高阈值
    //parma2:低阈值
    //minRadius:圆最小半径
    //maxRadius:圆最大半径
    HoughCircles(gray, circles, HOUGH_GRADIENT, 1, 10, 100, 35, 5, 150);
    cout << "检测到：" << circles.size() << endl;
    for (int i = 0; i < circles.size(); i++) {
        Vec3f cc = circles[i];
        circle(src, Point(cc[0], cc[1]), cc[2], Scalar(255, 0, 0), 4, LINE_AA);
    }
    imshow("src", src);
    waitKey(0);
}

int main() {
    test1();
}
