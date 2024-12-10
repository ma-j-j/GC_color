#include "color_opencv.h"


color_opencv::color_opencv(/* args */)
{
}

color_opencv::color_opencv(int dev)
{
    get_cap(dev);
}

//判断打开摄像头
bool color_opencv::get_cap(int dev)
{
    cap.open(dev);
    if (!cap.isOpened())  // if not success, exit program
	{
		std::cout << "Cannot open the web cam" << std::endl;
		return 0;
	}
    return 1;
}

bool color_opencv::get_imgOriginal()
{
    bool bSuccess = cap.read(imgOriginal); // read a new frame from video
    if (!bSuccess) //if not success, break loop
    {
        std::cout << "Cannot read a frame from video stream" << std::endl;
        return 0;
    }
    return 1;
}

void color_opencv::imgOriginal_Clone()
{
    imgClone = imgOriginal.clone();
}

void color_opencv::get_()
{
    cvtColor(imgOriginal, imgHSV, cv::COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
    split(imgHSV, hsvSplit);			//分类原图像的HSV三通道
    equalizeHist(hsvSplit[2], hsvSplit[2]);    //对HSV的亮度通道进行直方图均衡
    merge(hsvSplit, imgHSV);				   //合并三种通道
    cvtColor(imgHSV, imgBGR, cv::COLOR_HSV2BGR);    //将HSV空间转回至RGB空间，为接下来的颜色识别做准备
}


color_opencv::~color_opencv()
{
}