#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;
using namespace cv;

bool in_image_bounds(Mat& image, int x, int y){
    return (x >= 0 && x <= image.rows && y >= 0 && y <= image.cols);
}

void delete_pixel_and_neighbours(Mat& binary, int x, int y){
    if (!binary.at<uchar>(x, y)) return;
    //printf("X is %d, Y is %d\n", x, y);
    binary.at<uchar>(x, y) = 0;
    if (in_image_bounds(binary, x+1,y)){
        delete_pixel_and_neighbours(binary, x+1, y);
    }
    if (in_image_bounds(binary, x-1,y)){
        delete_pixel_and_neighbours(binary, x-1, y);
    }
    if (in_image_bounds(binary, x,y+1)){
        delete_pixel_and_neighbours(binary, x, y+1);
    }
    if (in_image_bounds(binary, x,y-1)){
        delete_pixel_and_neighbours(binary, x, y-1);
    }
}

void remove_large_clusters(Mat binary, int radius){

    for (int i = radius; i < binary.rows - radius-1; i++){
        for (int j = radius; j < binary.cols-radius; j++){
            bool left, right, top, bottom;
            left = binary.at<uchar>(i-radius, j);
            right = binary.at<uchar>(i+radius, j);
            top = binary.at<uchar>(i, j-radius);
            bottom = binary.at<uchar>(i, j+radius);
            if (left && right && top && bottom){
                printf("Cluster found at %d, %d\n", i, j);
            }
            if (left && right && top && bottom){
                delete_pixel_and_neighbours(binary, i, j);
            }
        }
    }
}

Mat get_border_dots(Mat image){
    Mat only_blue = image.clone();
    Mat hls, grey, binary, no_clusters;

    cvtColor(image, hls, CV_BGR2HLS);

    for (int row = 0; row < hls.rows; row ++){
        for (int col = 0; col < hls.cols; col ++){
            int hue = hls.at<Vec3b>(row,col)[0];
            int sat = hls.at<Vec3b>(row,col)[2];
            if ( hue  <= 110 && hue >= 90 && sat >= 100){
            } else {
                for (int channels = 0; channels < only_blue.channels(); channels ++){
                    only_blue.at<Vec3b>(row,col)[channels] = 0;
                }
            }
        }
    }
    cvtColor(only_blue, grey, CV_BGR2GRAY);
    GaussianBlur( grey, grey, Size(9, 9), 2, 2 );
    threshold(grey, binary, 200, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    //adaptiveThreshold(grey, binary, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 3, 0);
    vector<Vec3f> circles;

    remove_large_clusters(binary, 10);
    
    return binary;
}

Mat get_border_rectangle(Mat dots){
    Mat coloured;
    cvtColor(dots, coloured, CV_GRAY2BGR);

    vector<Vec4i> lines;
    HoughLinesP( dots, lines, 2, CV_PI/200, 20, 250, 100 );
    for( size_t i = 0; i < lines.size(); i++ )
    {
        line( coloured, Point(lines[i][0], lines[i][1]),
                Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 1, 8 );
    }
    /* vector<Vec2f> lines;
       HoughLines(dots, lines, 1, CV_PI/60, 100, 0, 0 );

       for( size_t i = 0; i < lines.size(); i++ )
       {
       float rho = lines[i][0], theta = lines[i][1];
       Point pt1, pt2;
       double a = cos(theta), b = sin(theta);
       double x0 = a*rho, y0 = b*rho;
       pt1.x = cvRound(x0 + 1000*(-b));
       pt1.y = cvRound(y0 + 1000*(a));
       pt2.x = cvRound(x0 - 1000*(-b));
       pt2.y = cvRound(y0 - 1000*(a));
       line( coloured, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
       }
       */
    return coloured;
}

Mat perspective_transformation(Mat image){

    // apply standard transform
    Mat affine_matrix, transformed;
    Point2f src[3], dst[3];

    //src[0] = Point2f(368,188);
    src[0] = Point2f(1148,343);
    src[1] = Point2f(543,530);
    src[2] = Point2f(838,753);

    //dst[0] = Point2f(8,8);
    dst[0] = Point2f(398,8);
    dst[1] = Point2f(8,589);
    dst[2] = Point2f(398,589);

    affine_matrix = getAffineTransform(src, dst);
    warpAffine(image, transformed, affine_matrix, transformed.size());
    return transformed;
}

int main( int argc, char** argv )
{
    Mat image, transformed, border_dots, houghed;
    string filename;
    assert((argc >= 2) && "Not enough arguments");
    namedWindow("Lab3", CV_WINDOW_AUTOSIZE );
    for (int i = 1; i < argc; i++){
        filename = argv[i];
        image = imread(filename, CV_LOAD_IMAGE_COLOR);
        border_dots = get_border_dots(image);
        houghed = get_border_rectangle(border_dots);
        //source = get_4_blue_dots(image);
        //        transformed = perspective_transformation(image);
        imshow("Lab3", houghed);
        waitKey();
    }
    return 0;
}
