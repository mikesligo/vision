#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <assert.h>

using namespace cv;
using namespace std;

Mat load_image(int argc, char**argv)
{
    assert((argc == 2) && "Not enough arguments");
    Mat image = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file
    assert((image.data) && "Could not open or find image");
    return image;
}

void show(Mat image)
{
    namedWindow( "Display window", CV_WINDOW_AUTOSIZE );
    imshow( "Display window", image );
}

int main( int argc, char** argv ) 
{
    Mat hsv_image;
    cvtColor(load_image(argc, argv), hsv_image, CV_BGR2HSV);

    show(hsv_image);
    waitKey(0);  
    return 0;
}
