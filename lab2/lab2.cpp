#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;
using namespace cv;

Mat thresholdANDrgb (Mat thresholded, Mat rgb)
{
    Mat no_black = rgb.clone();
    for (int row = 0; row < thresholded.rows; row ++){
        for (int col = 0; col < thresholded.cols; col ++){
            int grayness = thresholded.at<uchar>(row,col);
            if ( grayness == 0){
                for (int channels = 0; channels < no_black.channels(); channels ++){
                    no_black.at<Vec3b>(row,col)[channels] = 0;
                }
            } 
        }
    }
    return no_black;
}

// Split the image into interesting parts (bottom half, into 5 bottles) by using the known position of the bottles
vector<Mat> get_bottles(Mat image)
{
    vector<Mat> bottles;
    Rect roi(0,image.rows/2-1, image.cols, image.rows/2);
    Mat cropped = image(roi);
    int divider = cropped.cols/5;
    for (int i=0; i< cropped.cols-cropped.cols%5; i = i + divider){
        Mat gray_scale, k_means, section, thresholded, just_bottle;

        // isolate bottle
        Rect bottle_pos(i, 0, divider, cropped.rows);
        section = cropped(bottle_pos);
        
        // remove background
        cvtColor(section, gray_scale, CV_BGR2GRAY);
        adaptiveThreshold(gray_scale, thresholded, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 151, 0);
        just_bottle = thresholdANDrgb(thresholded, section);

        bottles.push_back(k_means);
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
