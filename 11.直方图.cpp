#include "opencv2/opencv.hpp"
#include "iostream"

using namespace std;
using namespace cv;

/**
 * https://zhuanlan.zhihu.com/p/472316148
 * 图像直方图：对整个图像各个像素值统计次数，生成直方图。
 *
 * 直方图均衡化，目的在于增强图片对比度，适用于过于曝光或背光的图片
 *
 * 我的理解：直方图均衡化要传入灰度图，比如传入的一个灰度图颜色值范围在130~200，颜色就比较相近，对比度就比较低
 * 用直方图均衡化来拉伸到0~255，可选的灰度颜色就变多了，对比度就变高了
 */
void test1() {
    Mat src = imread("a.jpeg");

    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);
    imshow("gray", gray);

    Mat dst;
    equalizeHist(gray, dst);
    imshow("dst", dst);
    waitKey(0);
}

/**
 * 彩色图均衡化
 */
void test2() {
    Mat src = imread("a.jpeg");
    imshow("src", src);

    vector<Mat> bgr_s;
    //将图像分割成三个通道
    split(src, bgr_s);
    //对每个通道均衡化
    equalizeHist(bgr_s[0], bgr_s[0]);
    equalizeHist(bgr_s[1], bgr_s[1]);
    equalizeHist(bgr_s[2], bgr_s[2]);
    //合并通道
    Mat dst;
    merge(bgr_s, dst);
    imshow("dst", dst);

    waitKey(0);
}

/**
 * 统计图像的直方图
 *
 * bin的概念：将一个范围内的数值分成多少份。比如一个CV_8U的图像如果bin的尺寸设置为256，这样bin的宽度就为1，对应前面例子中在每一个像素值上统计；
 * 如果bin的尺寸设置为32，这样bin的宽度就为256/32=8，这样就会在每8个像素值上统计。
 *
 */
void test3() {
    Mat src = imread("a.jpeg");

    vector<Mat> bgr_s;
    //将图像分割成3个单通道图像
    split(src, bgr_s);

    /**
     * calcHist( const Mat* images, int nimages,
                          const int* channels, InputArray mask,
                          OutputArray hist, int dims, const int* histSize,
                          const float** ranges, bool uniform = true, bool accumulate = false );
     */
    /**
     * images:输入的图像
     * nimages:输入图像的个数
     * channels:需要统计直方图的第几通道
     * mask:掩膜，不需要掩膜的话就传Mat()
     * hist:输出
     * dims：需要统计的通道个数
     * histSize：直方图分成多少个区间 0~255
     * ranges：数据的范围，rgb是0~255，hsv是0~1
     * uniform：是否对得到的图片结果进行归一化处理
     * accumulate：在多个图像时，是否累计计算像素值的个数
     */
    int histSize = 256;
    float range[] = {0, 256};
    const float *ranges = {range};
    //存的是各个灰度值的个数
    Mat hist_b, hist_g, hist_r;
    calcHist(&bgr_s[0], 1, 0, Mat(), hist_b, 1, &histSize, &ranges, true, false);
    calcHist(&bgr_s[1], 1, 0, Mat(), hist_g, 1, &histSize, &ranges, true, false);
    calcHist(&bgr_s[2], 1, 0, Mat(), hist_r, 1, &histSize, &ranges, true, false);

    //归一化处理
    int hist_h = 400;
    int hist_w = 512;
    int bin_w = hist_w / histSize;//画笔的大小
    /**
     * normalize( InputArray src, InputOutputArray dst, double alpha = 1, double beta = 0,
                             int norm_type = NORM_L2, int dtype = -1, InputArray mask = noArray())
     */
    /**
     * alpha：最小值
     * beta：最大值
     * norm_type：
     *     NORM_MINMAX：数值缩放到一定区域
     */
    normalize(hist_b, hist_b, 0, hist_h, NORM_MINMAX, -1, Mat());
    normalize(hist_g, hist_g, 0, hist_h, NORM_MINMAX, -1, Mat());
    normalize(hist_r, hist_r, 0, hist_h, NORM_MINMAX, -1, Mat());

    //画到一张图中
    Mat histImage(hist_h, hist_w, CV_8UC3, Scalar());
    for (int i = 1; i < histSize; i++) {
        line(histImage,
             Point((i - 1) * bin_w, hist_h - hist_b.at<float>(i - 1)),
             Point(i * bin_w, hist_h - hist_b.at<float>(i)),
             Scalar(255, 0, 0), bin_w,
             LINE_AA);
        line(histImage,
             Point((i - 1) * bin_w, hist_h - hist_g.at<float>(i - 1)),
             Point(i * bin_w, hist_h - hist_g.at<float>(i)),
             Scalar(0, 255, 0), bin_w,
             LINE_AA);
        line(histImage,
             Point((i - 1) * bin_w, hist_h - hist_r.at<float>(i - 1)),
             Point(i * bin_w, hist_h - hist_r.at<float>(i)),
             Scalar(0, 0, 255), bin_w,
             LINE_AA);
    }

    imshow("histImage", histImage);

    waitKey(0);
}

/**
 * 直方图比较，用来比较两个图片的相似度
 *
 * 一维直方图：只考虑一个特性，即像素的灰度值
 * 二维直方图：考虑两个特征，通常是色度和饱和度
 *
 * 对于二维直方图，需要将图像从RGB转HSV
 *
 */
void test4() {
    Mat src1 = imread("a.jpeg");
    Mat src2 = imread("openCvGray.jpeg");
    Mat src3 = imread("e.jpeg");

    //计算直方图
    //RGB 转 HSV，计算 H、S 的直方图
    Mat hsv1, hsv2, hsv3;
    cvtColor(src1, hsv1, COLOR_RGB2HSV); //转换后 H的范围0~180， S的范围：0-255，V的范围：0-255
    cvtColor(src2, hsv2, COLOR_RGB2HSV);
    cvtColor(src3, hsv3, COLOR_RGB2HSV);
    //因为需要同时处理H和S平面
    int channels[] = {0, 1};
    //Mat指二维矩阵，MatND是多维矩阵(>=3维)
    MatND hist1, hist2, hist3;
    //分成50份
    int h_bins = 50;
    int s_bins = 50;
    int hist_size[] = {h_bins, s_bins};
    float h_range[] = {0, 180};
    float s_range[] = {0, 255};
    const float *range[] = {h_range, s_range};
    calcHist(&hsv1, 1, channels, Mat(), hist1, 2, hist_size, range);
    calcHist(&hsv2, 1, channels, Mat(), hist2, 2, hist_size, range);
    calcHist(&hsv3, 1, channels, Mat(), hist3, 2, hist_size, range);

    //归一化 0~1
    normalize(hist1, hist1, 0, 1, NORM_MINMAX);
    normalize(hist2, hist2, 0, 1, NORM_MINMAX);
    normalize(hist3, hist3, 0, 1, NORM_MINMAX);

    /**
     * method:
     * 1.HISTCMP_CORREL，相关性比较。返回[-1,1]范围的值，1表示完全匹配，-1表示完全不匹配
     * 2.HISTCMP_CHISQR，卡方比较。返回[0,unbounded]范围的值，0表示完美匹配
     * 3.HISTCMP_INTERSECT，计算两个直方图的交集，如果直方图被归一化，则该指标返回范围[0,1]，1表示完全匹配，0表示完全不匹配
     * 4.HISTCMP_BHATTACHARYYA，巴氏距离。返回[0,1]范围的值，0是完美匹配，1是完全不匹配
     */
    double hist1_hist1 = compareHist(hist1, hist1, HISTCMP_CORREL);
    double hist1_hist2 = compareHist(hist1, hist2, HISTCMP_CORREL);
    double hist1_hist3 = compareHist(hist1, hist3, HISTCMP_CORREL);

    cout << hist1_hist1 << endl;
    cout << hist1_hist2 << endl;
    cout << hist1_hist3 << endl;

//    waitKey(0);
}

int main() {
    test3();
}
