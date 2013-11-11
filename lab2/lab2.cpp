#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;
using namespace cv;

vector<Mat> get_bottles(Mat image)
{
    vector<Mat> bottles;
    bottles.push_back(image);
    return bottles;
}

int main( int argc, char** argv )
{
    Mat image;
    string filename;
    assert((argc >= 2) && "Not enough arguments");
    for (int i = 1; i < argc; i++){
        filename = argv[i];
        image = imread(filename, CV_LOAD_IMAGE_COLOR);
        vector<Mat> bottles = get_bottles(image);
        namedWindow("Lab2", CV_WINDOW_AUTOSIZE );
        moveWindow("Lab2", 300, 300);
        for (vector<Mat>::iterator it = bottles.begin(); it != bottles.end(); it++) {
            image = Mat(*it);
            imshow("Lab2", image );
        }
        waitKey(0);
    }
    return 0;



}
