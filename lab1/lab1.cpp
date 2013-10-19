#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

Mat count_red_spoons(string filename)
{
    Mat hls, histImage, original;
    
    original = imread(filename, CV_LOAD_IMAGE_COLOR);   // Read the file
    cvtColor(original, hls, CV_BGR2HLS);

    Mat red_only = imread(filename, CV_LOAD_IMAGE_COLOR);   // Read the file

    int red_count = 0;
    for (int row = 0; row < hls.rows; row ++){
        for (int col = 0; col < hls.cols; col ++){
            int hue = hls.at<Vec3b>(row,col)[0];
            if ( (hue  < 0 || hue >= 162) && hue != 165 ){
                red_count ++;
            } else {
                for (int channels = 0; channels < red_only.channels(); channels ++){
                    red_only.at<Vec3b>(row,col)[channels] = 0;
                }
            }
        }
    }
    printf("Red count is %d\n",red_count);
    return red_only;
}


int main( int argc, char** argv )
{
    Mat red_only;
    string filename;
    assert((argc >= 2) && "Not enough arguments");
    for (int i = 1; i < argc; i++){
        filename = argv[i];
        red_only = count_red_spoons(filename);
        namedWindow(filename, CV_WINDOW_AUTOSIZE );
        imshow(filename, red_only );
        waitKey(0);
    }
    return 0;
}
