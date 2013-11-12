#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;
using namespace cv;

// Split the image into interesting parts (bottom half, into 5 bottles) by using the known position of the bottles
vector<Mat> get_bottles(Mat image)
{
    vector<Mat> bottles;
    Rect roi(0,image.rows/2-1, image.cols, image.rows/2);
    Mat cropped = image(roi);
    int divider = cropped.cols/5;
    for (int i=0; i< cropped.cols-1; i = i + divider){
        Mat grey_scale;
        Rect bottle_pos(i, 0, divider, cropped.rows);
        cvtColor(cropped(bottle_pos), grey_scale, CV_BGR2GRAY);
        bottles.push_back(grey_scale);
    }
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
        moveWindow("Lab2", 500, 300);
        for (vector<Mat>::iterator it = bottles.begin(); it != bottles.end(); it++) {
            image = Mat(*it);
            imshow("Lab2", image );
            waitKey(0);
        }
    }
    return 0;



}
