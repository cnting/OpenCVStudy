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
 * 彩色图均衡化，这里练习提升亮度
 */
void test2() {
    Mat src = imread("scene.jpeg");
    imshow("src", src);

    Mat hsv;
    cvtColor(src, hsv, COLOR_BGR2HSV);

    vector<Mat> bgr_s;
    //将图像分割成三个通道
    split(hsv, bgr_s);

    //对v通道均衡化
    equalizeHist(bgr_s[2], bgr_s[2]);
    //合并通道
    merge(bgr_s, hsv);

    //imshow是按BGR来显示的，所以要再转成BGR
    Mat dst;
    cvtColor(hsv, dst, COLOR_HSV2BGR);
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
     *
     * images:输入的图像
     * nimages:输入图像的个数
     * channels:需要统计直方图的第几通道
     * mask:掩膜，不需要掩膜的话就传Mat()
     * hist:输出
     * dims：需要统计的通道个数
     * histSize：直方图分成多少个区间
     * ranges：数据的范围
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
     * https://tony-laoshi.github.io/opencv%E6%95%B0%E6%8D%AE%E6%A0%87%E5%87%86%E5%8C%96%E5%92%8C%E5%BD%92%E4%B8%80%E5%8C%96/
     *
     * normalize( InputArray src, InputOutputArray dst, double alpha = 1, double beta = 0,
                             int norm_type = NORM_L2, int dtype = -1, InputArray mask = noArray())
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

/**
 * 直方图反向投影，用于目标检测
 * https://blog.csdn.net/shuiyixin/article/details/80331839
 */
Mat hueImage;
int bins = 2;
int max_bins = 180;

void hist_backprojection(int, void *) {
    Mat hist;
    int histSize = MAX(bins, 2);
    float hue_range[] = {0, 180};
    const float *ranges[] = {hue_range};
    calcHist(&hueImage, 1, 0, Mat(), hist, 1, &histSize, ranges);
    normalize(hist, hist, 0, 255, NORM_MINMAX);

    Mat backProject;
    calcBackProject(&hueImage, 1, 0, hist, backProject, ranges);
    imshow("SourceImage", backProject);
}

void test5() {
    Mat src = imread("clothes.jpeg");
    Mat hsv;
    cvtColor(src, hsv, COLOR_BGR2HSV);
    vector<Mat> hsv_s;
    split(hsv, hsv_s);
    hueImage = hsv_s[0];

    namedWindow("SourceImage");
    createTrackbar("backprojection", "SourceImage", &bins, max_bins, hist_backprojection);
    hist_backprojection(bins, 0);

    waitKey(0);
}

/**
 * 对图片亮度进行直方均衡
 */
void test6() {
    Mat src = imread("a.jpeg");
    imshow("src", src);

    Mat hsv;
    cvtColor(src, hsv, COLOR_BGR2HSV);

    vector<Mat> hsv_s;
    split(hsv, hsv_s);

    equalizeHist(hsv_s[2], hsv_s[2]);
    merge(hsv_s, hsv);

    cvtColor(hsv, src, COLOR_HSV2BGR);
    imshow("dst", src);
    waitKey(0);
}

/**
 * 手写直方图计算
 */
void calcHist(const Mat &gray, Mat &hist) {
    hist.create(1, 256, CV_32S);
    for (int i = 0; i < hist.cols; i++) {
        hist.at<int>(0, i) = 0;
    }
    for (int row = 0; row < gray.rows; row++) {
        for (int col = 0; col < gray.cols; col++) {
            int index = gray.at<uchar>(row, col);
            hist.at<int>(0, index) += 1;
        }
    }

}

/**
 * 手写归一化
 */
void normalize(const Mat &src, Mat &dst, int n_Max) {
    int max_value = 0;
    for (int row = 0; row < src.rows; row++) {
        for (int col = 0; col < src.cols; col++) {
            int value = src.at<int>(row, col);
            max_value = max(value, max_value);
        }
    }
    dst.create(src.size(), src.type());
    for (int row = 0; row < src.rows; row++) {
        for (int col = 0; col < src.cols; col++) {
            int value = src.at<int>(row, col);
            dst.at<int>(row, col) = value * 1.0 / max_value * n_Max;
        }
    }
}

/**
 * 手写直方均衡
 *
 * 1.直方图统计
 * 2.计算直方图中像素的概率
 * 3.生成一张映射表
 * 4.从映射表中查找赋值
 */
void equalizeHist(const Mat &src, Mat &dst) {
    Mat hist;
    //1.直方图统计，得到0~255之间每个灰度值所占的像素点个数
    calcHist(src, hist);

    //2.计算直方图中像素的概率
    Mat prob_mat(hist.size(), CV_32FC1);
    float image_size = src.rows * src.cols;
    float prob_sum = 0;
    for (int i = 0; i < hist.cols; i++) { //这个i是灰度值
        float prob = hist.at<int>(0, i) / image_size;
        prob_sum += prob;
        prob_mat.at<float>(0, i) = prob_sum;
    }
    //3.生成一张映射表
    Mat map(hist.size(), CV_32FC1);
    for (int i = 0; i < prob_mat.cols; i++) {  //这个i是灰度值
        float prob = prob_mat.at<float>(0, i);
        map.at<float>(0, i) = prob * 255;
    }
    dst.create(src.size(), src.type());
    for (int row = 0; row < src.rows; row++) {
        for (int col = 0; col < src.cols; col++) {
            uchar pixels = src.at<uchar>(row, col);
            dst.at<uchar>(row, col) =  map.at<float>(0, pixels);
        }
    }
}

void test7() {
    Mat src = imread("a.jpeg");

    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);

    //这样定义得到的是空数组
//    Mat hist;
//    calcHist(gray, hist);

    //归一
//    normalize(hist, hist, 255);

    //画图
//    int bin_w = 5;
//    Mat hist_mat(256, bin_w * 256, CV_8UC3);
//    for (int i = 0; i < 256; i++) {
//        Point start(i * bin_w, hist_mat.rows);
//        Point end(i * bin_w, hist_mat.rows - hist.at<int>(0, i));
//        line(hist_mat, start, end, Scalar(255, 0, 255), bin_w,LINE_AA);
//    }
//
//    imshow("hist", hist_mat);

    //直方均衡
    equalizeHist(gray, gray);
    imshow("gray", gray);

    waitKey(0);
}

int main() {
    test7();
}
