#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <vector>
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <cmath>

using namespace std;
using namespace cv;

bool in_image_bounds(Mat& image, int x, int y){
    return (x >= 0 && x <= image.rows && y >= 0 && y <= image.cols);
}

void delete_pixel_and_neighbours(Mat& binary, int x, int y){
    if (!binary.at<uchar>(x, y)) return;
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

Point2f * get_border_rigt_bottom_left_top(vector<Point2f> points){
    Point2f * corners = (Point2f*)malloc(sizeof(Point2f)*4); 
    corners[0] = points[0]; // right
    corners[1] = points[0]; // bottom
    corners[2] = points[0]; // left
    corners[3] = points[0]; // top

    for (size_t i=0; i<points.size(); i++){
        Point2f point = points[i];
        int x = point.x;
        int y = point.y;
        if (x > corners[0].x){ // right
            corners[0] = point;
        }
        if (x < corners[1].x){ // bottom
            corners[1] = point;
        }
        if (y > corners[2].y){ // left
            corners[2] = point;
        }
        if (y < corners[3].y){ // top
            corners[3] = point;
        }
    }
    return corners;
}

Point2f * get_border(Mat binary, int radius, int min){
    vector<Point2f> points;
    vector<Point2f> valid_points;
    Mat top_corner, right;
    int border = 50;
    Rect roi_top_left(0,0,250,binary.rows);
    top_corner = binary(roi_top_left);
    top_corner.setTo(Scalar(0,0,0));

    Rect roi_right(binary.cols-300,0,300,binary.rows);
    right = binary(roi_right);
    right.setTo(Scalar(0,0,0));

    for (int row=border; row < binary.rows-border-min; row++){
        for (int col=border; col < binary.cols-border-min; col++){
            if (binary.at<uchar>(row,col)){
                points.push_back(Point2f(col,row));
            }
        }
    }
    for (size_t i=0; i<points.size(); i++){
        for (size_t j=0; j<points.size(); j++){
            if (j != i){
                int dX0 = points[i].x;
                int dY0 = points[i].y;
                int dX1 = points[j].x;
                int dY1 = points[j].y;
                int dist = sqrt((dX1 - dX0)*(dX1 - dX0) + (dY1 - dY0)*(dY1 - dY0));
                if (dist > min && dist < radius){
                    valid_points.push_back(points[i]);
                    break;
                }
            }
        }
    }
    Point2f * border_points = get_border_rigt_bottom_left_top(valid_points);
    return border_points;
}

Mat get_border_dots(Mat image){
    Mat only_blue = image.clone();
    Mat hls, hsv,grey, binary, no_clusters, only_blue_bgr;
    
    cvtColor(image, hls, CV_BGR2HLS);
    cvtColor(image, hsv, CV_BGR2HSV);

    for (int row = 0; row < hls.rows; row ++){
        for (int col = 0; col < hls.cols; col ++){
            int hue = hls.at<Vec3b>(row,col)[0];
            int lum = hls.at<Vec3b>(row,col)[1];
            int sat = hls.at<Vec3b>(row,col)[2];
            int val = hsv.at<Vec3b>(row,col)[2];
            if ( hue  <= 110 && hue >= 90 && sat >= 50 && lum <= 160 && val > 120){
            } else {
                for (int channels = 0; channels < only_blue.channels(); channels ++){
                    only_blue.at<Vec3b>(row,col)[channels] = 0;
                }
            }
        }
    }
    cvtColor(only_blue, only_blue_bgr, CV_HLS2BGR);
    cvtColor(only_blue_bgr, grey, CV_BGR2GRAY);
    threshold(grey, binary, 200, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

    remove_large_clusters(binary, 20);

    return binary;
}

Mat get_border_rectangle(Mat dots){
    Mat coloured, grey, binary;
    cvtColor(dots, coloured, CV_GRAY2BGR);

    vector<Vec4i> lines;
    HoughLinesP( dots, lines, 1, CV_PI/200, 5, 200, 200 );
    for( size_t i = 0; i < lines.size(); i++ ) {
        line( coloured, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 1, 8 );
    }

    cvtColor(coloured, grey, CV_BGR2GRAY);
    GaussianBlur( grey, grey, Size(9, 9), 2, 2 );
    threshold(grey, binary, 200, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

    return binary;
}

Mat perspective_transformation(Mat image, Point2f * src){

    Mat matrix, transformed;

    Point2f dst[4];
    dst[0] = Point2f(398,8);
    dst[1] = Point2f(8,589);
    dst[2] = Point2f(398,589);
    dst[3] = Point2f(8,8);

    matrix = getPerspectiveTransform(src, dst);
    warpPerspective(image, transformed, matrix, transformed.size());

    circle(transformed, dst[0], 50, CV_RGB(0,0,255), 0);
    circle(transformed, dst[1], 50, CV_RGB(0,255,0), 0);
    circle(transformed, dst[2], 50, CV_RGB(255,0,0), 0);
    circle(transformed, dst[3], 50, CV_RGB(200,200,200), 0);

    Rect roi(0,0, 407,597);
    Mat cropped = transformed(roi);
    return cropped;
}


Mat sharpen_image(Mat image){
    Mat sharpened = image.clone();
    GaussianBlur(image, sharpened, Size(0,0), 3);
    addWeighted(image, 1.5, sharpened, -0.5, 0, sharpened);
    return sharpened;
}

void count_pixels_in_rows(Mat image, vector<string> templates){
    int divisor = 5;
    int max = image.rows - image.rows%divisor;
    int increment = image.rows/divisor;
    int closest_count = 10000000;
    string closest_file;
    for (size_t cnt =0; cnt < templates.size(); cnt ++){
        int difference =0;
        Mat tmpl = imread(templates[cnt], CV_LOAD_IMAGE_COLOR);
        for (int row=0; row < max; row += increment){
            Rect roi(0, row, image.cols, increment);
            Mat cropped_img = image(roi);
            Mat cropped_tmpl = tmpl(roi);

            Mat grey, binary;
            int black_1, black_2;

            cvtColor(cropped_img, grey, CV_BGR2GRAY);
            threshold(grey, binary, 200, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
            black_1 = binary.rows*binary.cols - countNonZero(binary);

            cvtColor(cropped_tmpl, grey, CV_BGR2GRAY);
            threshold(grey, binary, 200, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
            black_2 = binary.rows*binary.cols - countNonZero(binary);

            difference = abs(difference + (black_1-black_2));
        }
        if (difference < closest_count){
            closest_count = difference;
            closest_file = templates[cnt];
        }
        cout << "Final difference for " << templates[cnt] << ": " << difference << endl;
    }
    cout << "Closest is " << closest_file << ", with " << closest_count << " pixels difference" << endl;
    Mat tmpl = imread(closest_file, CV_LOAD_IMAGE_COLOR);
    namedWindow("Lab3_2", CV_WINDOW_AUTOSIZE);
    imshow("Lab3", image);
    imshow("Lab3_2", tmpl);
}

int main( int argc, char** argv )
{
    Mat image, transformed, border_dots, houghed, sharpened, compared;
    string filename;

    vector<string> templates;
    for (int i=1; i< argc; i++){
        filename = argv[i];
        if (filename.substr(0,14).compare("pageimagefiles") == 0){
            templates.push_back(filename);
        }
    }

    assert((argc >= 2) && "Not enough arguments");
    namedWindow("Lab3", CV_NORMAL );

    for (int i = 1; i < argc; i++){
        filename = argv[i];
        if (filename.substr(0,14).compare("pageimagefiles") != 0){
            cout << "Filename: " << filename << endl;
            image = imread(filename, CV_LOAD_IMAGE_COLOR);
            border_dots = get_border_dots(image);
            Point2f * points = get_border(border_dots, 200, 40);
            transformed = perspective_transformation(image, points);
            sharpened = sharpen_image(transformed);
            count_pixels_in_rows(sharpened, templates);
            imshow("Lab3", sharpened);
            waitKey();
        }
    }
    return 0;
}
