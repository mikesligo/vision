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
            int greyness = thresholded.at<uchar>(row,col);
            if ( greyness == 0){
                for (int channels = 0; channels < no_black.channels(); channels ++){
                    no_black.at<Vec3b>(row,col)[channels] = 0;
                }
            } 
        }
    }
    return no_black;
}

void delete_white_border(Mat edge_image, int row, int col){
    if (row < 2 || col < 2 || row > edge_image.rows-2 || col > edge_image.rows -2) return;
    edge_image.at<uchar>(row, col) = 0;
    for (int i = row-1; i < row+2; i++){
        for (int j = col-1; j < col+2; j++){
            if (edge_image.at<uchar>(i, j)){
                delete_white_border(edge_image, i, j);
            }
        }
    }
}

bool plusOrMinus(double val, double range, double error){
    double Val = abs(val);
    double Range = abs(range);
    double Error = abs(error);
    
    double error_plus = Range + Error;
    double error_minus = Range - Error;
    if (Val < error_plus && Val > error_minus) return true;
    return false;
}

// Split the image into interesting parts (bottom half, into 5 bottles) by using the known position of the bottles
vector<Mat> get_bottles(Mat image)
{
    vector<Mat> bottles;
    Rect roi(0,image.rows/2-1, image.cols, image.rows/2);
    Mat cropped = image(roi);
    int divider = cropped.cols/5;
    for (int i=0; i< cropped.cols-cropped.cols%5; i = i + divider){
        Mat grey_scale, k_means, section, thresholded, just_bottle, grey_bottle, canny;

        // isolate bottle
        Rect bottle_pos(i, 0, divider, cropped.rows);
        section = cropped(bottle_pos);

        // remove background
        cvtColor(section, grey_scale, CV_BGR2GRAY);
        adaptiveThreshold(grey_scale, thresholded, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 151, 0);
        just_bottle = thresholdANDrgb(thresholded, section);

        // get edges with canny 
        cvtColor(just_bottle, grey_bottle, CV_BGR2GRAY);
        Canny(grey_bottle, canny, 100, 200);

        // Remove bottle edge recursively
        bool pixel_is_white; 
        for (int col = 0; col < canny.cols ; col ++){
            pixel_is_white = (bool)canny.at<uchar>((int)canny.rows*0.7,col);
            if (pixel_is_white) {
                delete_white_border(canny,(int) canny.rows*0.7, col);
                break;
            }
        }

        // Use hough to get lines

        vector<Vec2f> lines;
        HoughLines(canny, lines, 1, CV_PI/80.0, 50);

        Mat canny_bgr;
        cvtColor(canny, canny_bgr, CV_GRAY2BGR);
        
        bool found_vert = false;
        bool found_horiz = false;
        for( size_t i = 0; i < lines.size(); i++ )
        {
            float rho = lines[i][0];
            float theta = lines[i][1];
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho, y0 = b*rho;
            Point pt1(cvRound(x0 + 1000*(-b)), cvRound(y0 + 1000*(a)));
            Point pt2(cvRound(x0 - 1000*(-b)), cvRound(y0 - 1000*(a)));

            // filter out the lines that angles don't match horiz or vertical ish, also only look for 1 horiz and 1 vert
            double Angle = atan2(pt2.y - pt1.y,pt2.x - pt1.x) * 180.0 / CV_PI;
            if(Angle<0) Angle=Angle+360;
            if (plusOrMinus(Angle, 270.0, 3.0) ||plusOrMinus(Angle, 90.0, 3.0)) {
                found_vert = true;
                line( section, pt1, pt2, Scalar(0,0,255), 1, 8 );
            } else if (plusOrMinus(Angle, 0.0, 3.0) ||plusOrMinus(Angle, 180.0, 3.0)) {
                found_horiz = true;
                line( section, pt1, pt2, Scalar(0,0,255), 1, 8 );
            }
        }
        if (found_vert && found_horiz) printf("Label found!\n");
        else if (found_vert || found_horiz) printf("Label found but not well placed!\n");
        else {
            printf("Label not found!\n");
        }
        imshow("Lab2", section );
        waitKey(0);
        cout << endl;

        //bottles.push_back(canny_bgr);
    }
    return bottles;
}

int main( int argc, char** argv )
{
    Mat image;
    string filename;
    assert((argc >= 2) && "Not enough arguments");
    namedWindow("Lab2", CV_WINDOW_AUTOSIZE );
    moveWindow("Lab2", 500, 300);
    for (int i = 1; i < argc; i++){
        filename = argv[i];
        image = imread(filename, CV_LOAD_IMAGE_COLOR);
        vector<Mat> bottles = get_bottles(image);
        //for (vector<Mat>::iterator it = bottles.begin(); it != bottles.end(); it++) {
        //    image = Mat(*it);
        //    imshow("Lab2", image );
        //    waitKey(0);
        //}
    }
    return 0;
}
