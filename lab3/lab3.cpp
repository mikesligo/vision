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
                delete_pixel_and_neighbours(binary, i, j);
            }
        }
    }
}

Mat get_border_dots(Mat image){
    Mat only_blue = image.clone();
    Mat hls, grey, binary, no_clusters, only_blue_bgr;

    cvtColor(image, hls, CV_BGR2HLS);

    for (int row = 0; row < hls.rows; row ++){
        for (int col = 0; col < hls.cols; col ++){
            int hue = hls.at<Vec3b>(row,col)[0];
            int lum = hls.at<Vec3b>(row,col)[1];
            int sat = hls.at<Vec3b>(row,col)[2];
            if ( hue  <= 110 && hue >= 90 && sat >= 100 && lum <= 160){
            } else {
                for (int channels = 0; channels < only_blue.channels(); channels ++){
                    only_blue.at<Vec3b>(row,col)[channels] = 0;
                }
            }
        }
    }
    cvtColor(only_blue, only_blue_bgr, CV_HLS2BGR);
    cvtColor(only_blue_bgr, grey, CV_BGR2GRAY);
    GaussianBlur( grey, grey, Size(9, 9), 2, 2 );
    threshold(grey, binary, 200, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

    remove_large_clusters(binary, 10);

    return binary;
}

Mat get_border_rectangle(Mat dots){
    Mat coloured, grey, binary;
    cvtColor(dots, coloured, CV_GRAY2BGR);

    vector<Vec4i> lines;
    HoughLinesP( dots, lines, 1, CV_PI/200, 5, 200, 200 );
    for( size_t i = 0; i < lines.size(); i++ )
    {
        line( coloured, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 1, 8 );
    }

    cvtColor(coloured, grey, CV_BGR2GRAY);
    GaussianBlur( grey, grey, Size(9, 9), 2, 2 );
    threshold(grey, binary, 200, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

    return binary;
}

Mat perspective_transformation(Mat image, Point2f * src){

    // apply standard transform
    Mat affine_matrix, transformed;
    //Point2f src[3];
    Point2f dst[3];

    //src[0] = Point2f(368,188);
    //src[0] = Point2f(1148,343);
    //src[1] = Point2f(543,530);
    //src[2] = Point2f(838,753);

    //dst[0] = Point2f(8,8);
    dst[0] = Point2f(398,8);
    dst[1] = Point2f(8,589);
    dst[2] = Point2f(398,589);

    affine_matrix = getAffineTransform(src, dst);
    warpAffine(image, transformed, affine_matrix, transformed.size());
    return transformed;
}

Point2f get_left_point(Mat binary){
    for (int i=300; i < binary.cols; i++){
        for (int j=0; j < binary.rows; j++){
            if (binary.at<uchar>(j, i)) {
                printf("Left - X: %d, Y:%d\n", j,i);
                return Point2f(j,i);
            }
        }
    }
}

Point2f get_bottom_point(Mat binary){
    for (int i= binary.rows-200; i > 0; i--){
        for (int j=0; j < binary.cols; j++){
            if (binary.at<uchar>(i, j)) {
                printf("Bottom - X: %d, Y:%d\n", i,j);
                return Point2f(i, j);
            }
        }
    }
}

Point2f get_right_point(Mat binary){
    for (int i=binary.cols-200; i >= 0; i--){
        for (int j=0; j < binary.rows; j++){
            // if a cluster
            if (binary.at<uchar>(j, i)) {
                printf("Right - X: %d, Y:%d\n", j,i);
                circle(binary, Point2f(j,i), 10, CV_RGB(0,0,255), 0);
                return Point2f(j,i);
            }
        }
    }
}

Point2f * get_right_bottom_left(Mat binary){
    // start on the left
    Point2f * corners = (Point2f*)malloc(sizeof(Point2f)*3); 
    corners[0] = get_right_point(binary);
    corners[1] = get_left_point(binary);
    corners[2]= get_bottom_point(binary);
    return corners;
    //  check that the next 2 clusters are roughly the standard distance away
    // if not then the next cluster is the starter
    // do the same for right and bot
    namedWindow("lol", CV_WINDOW_AUTOSIZE );
    imshow("lol", binary);
    waitKey();
}

int main( int argc, char** argv )
{
    Mat image, transformed, border_dots, houghed;
    string filename;
    assert((argc >= 2) && "Not enough arguments");
    namedWindow("Lab3", CV_WINDOW_AUTOSIZE );
    for (int i = 1; i < argc; i++){
        filename = argv[i];
        cout << filename << endl;
        image = imread(filename, CV_LOAD_IMAGE_COLOR);
        border_dots = get_border_dots(image);
        //houghed = get_border_rectangle(border_dots);
        //source = get_4_blue_dots(image);
        //        transformed = perspective_transformation(image);
        Point2f * points = get_right_bottom_left(border_dots);
        transformed = perspective_transformation(image, points);
        //imshow("Lab3", transformed);
        //waitKey();
    }
    return 0;
}
