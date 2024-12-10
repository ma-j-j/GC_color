#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "color.h"
#include "color_opencv.h"

using namespace cv;
using namespace std;

int main()
{
	detect_color = 4;
	color a;
	a.color_detect();
	while (1)
	{
		a.ma_color();
	}
	return 0;
}
