#include "opencv2/opencv.hpp"
#include "iostream"

using namespace std;
using namespace cv;

/**
 * https://zhuanlan.zhihu.com/p/67930839
 *
 * HSV更适合用于图像处理，可以非常直观的表达颜色的色调、鲜艳程度和明暗程度
 * H(Hue)：可以叫色相、颜色、色度。取值范围0~360度
 *
 * S(Sturation)：代表饱和度。取值范围0~100。饱和度减小，就是往光谱色种添加白色，饱和度减为0，整个颜色呈现白色
 *
 * V(Value)：亮度。取值范围0~100。亮度减小，就是往光谱色种添加黑色，亮度减为0，整个颜色呈现黑色
 *
 * OpenCV中 HSV的取值范围不一样：
 * cvtColor(src1, hsv1, COLOR_RGB2HSV);
 * 转换后 H的范围0~180， S的范围：0-255，V的范围：0-255
 *
 * cvtColor(src1, hsv1, COLOR_RGB2HSV_FULL);
 * 转换后 H的范围0~255， S的范围：0-255，V的范围：0-255
*/

int main(){
}