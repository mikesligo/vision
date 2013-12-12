#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <vector>
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/features2d/features2d.hpp"

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
            if ( hue  <= 110 && hue >= 90 && sat >= 50 && lum <= 160){
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
    for( size_t i = 0; i < lines.size(); i++ ) {
        line( coloured, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 1, 8 );
    }

    cvtColor(coloured, grey, CV_BGR2GRAY);
    GaussianBlur( grey, grey, Size(9, 9), 2, 2 );
    threshold(grey, binary, 200, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

    return binary;
}

Mat perspective_transformation(Mat image, Point2f * src){

    // apply standard transform
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
    // imshow("Lab3", transformed);
    // waitKey();
    Rect roi(8,8, 398,589);
    Mat cropped = transformed(roi);
    return cropped;
}

// I don't know why, but the following methods only work if
// I return the point coordinates backwards to what I expect

Point2f get_left_point(Mat binary){
    for (int i=300; i < binary.cols; i++){
        for (int j=0; j < binary.rows; j++){
            if (binary.at<uchar>(j, i)) {
                printf("Left - X: %d, Y:%d\n", j,i);
                return Point2f(i,j);
            }
        }
    }   
    return Point2f(0,binary.rows/2);
}

Point2f get_bottom_point(Mat binary){
    for (int i= binary.rows-100; i > 0; i--){
        for (int j=0; j < binary.cols; j++){
            if (binary.at<uchar>(i, j)) {
                printf("Bottom - X: %d, Y:%d\n", i,j);
                return Point2f(j, i); 
            }
        }
    }   
    return Point2f(binary.cols/2,binary.rows);
}

Point2f get_right_point(Mat binary){
    for (int i=binary.cols-200; i >= 0; i--){
        for (int j=0; j < binary.rows; j++){
            if (binary.at<uchar>(j, i)) {
                printf("Right - X: %d, Y:%d\n", j,i);
                return Point2f(i,j);
            }
        }
    }   
    return Point2f(0,binary.rows/2);
}

Point2f get_top_point(Mat binary){
    for (int i=100; i < binary.rows; i++){
        for (int j=300; j < binary.cols; j++){
            if (binary.at<uchar>(i, j)) {
                printf("Top - X: %d, Y:%d\n", i,j);
                return Point2f(j,i);
            }
        }
    }
    return Point2f(binary.cols/2,0);
}

Point2f * get_right_bottom_left(Mat binary){
    // start on the left
    Point2f * corners = (Point2f*)malloc(sizeof(Point2f)*4); 
    corners[0] = get_right_point(binary);
    corners[1] = get_left_point(binary);
    corners[2]= get_bottom_point(binary);
    corners[3]= get_top_point(binary);
    //  check that the next 2 clusters are roughly the standard distance away
    // if not then the next cluster is the starter
    // do the same for right and bot
    //  Mat coloured;
    //  cvtColor(binary, coloured, CV_GRAY2BGR);
    //  circle(coloured, corners[0], 50, CV_RGB(0,0,255), 0);
    //  circle(coloured, corners[1], 50, CV_RGB(0,255,0), 0);
    //  circle(coloured, corners[2], 50, CV_RGB(255,0,0), 0);
    //  circle(coloured, corners[3], 50, CV_RGB(200,200,200), 0);
    //  imshow("Lab3", coloured);
    //  waitKey();
    return corners;
}

Mat sharpen_image(Mat image){
    Mat sharpened = image.clone();
    GaussianBlur(image, sharpened, Size(0,0), 3);
    addWeighted(image, 1.5, sharpened, -0.5, 0, sharpened);
    return sharpened;
}

Mat find_closest_match(Mat image, vector<string> templates){

    //-- Step 1: Detect the keypoints using SURF Detector
    Rect roi(0,0,image.cols,image.rows);

    Mat tmpl = imread(templates[0], CV_LOAD_IMAGE_COLOR)(roi);

    Mat img_1, img_2;
    vector<KeyPoint> keypoints_1, keypoints_2;
    int divisor = 2;
    int increment = image.cols/divisor;
    int max = image.cols - image.cols%divisor;
    Mat img_matches = Mat(image.cols/divisor, image.rows, CV_32F);

    printf("size: %d, %d\n", image.cols, image.rows);
    for (int cols=0; cols < max; cols += increment){
        Rect section (cols, 0, increment, image.rows);
        img_1 = image(section);
        img_2 = tmpl(section);
        int minHessian = 400;
    vector<DMatch> good_matches;

        SurfFeatureDetector detector( minHessian );

        detector.detect( img_1, keypoints_1 );
        detector.detect( img_2, keypoints_2 );

        //-- Step 2: Calculate descriptors (feature vectors)
        SurfDescriptorExtractor extractor;

        Mat descriptors_1, descriptors_2;

        extractor.compute( img_1, keypoints_1, descriptors_1 );
        extractor.compute( img_2, keypoints_2, descriptors_2 );

        //-- Step 3: Matching descriptor vectors using FLANN matcher
        FlannBasedMatcher matcher;
        vector<DMatch> matches;
        matcher.match(descriptors_1, descriptors_2, matches );

        double max_dist = 0; double min_dist = 100;

        //-- Quick calculation of max and min distances between keypoints
        for( int i = 0; i < descriptors_1.rows; i++ ) { 
            double dist = matches[i].distance;
            if( dist < min_dist ) min_dist = dist;
            if( dist > max_dist ) max_dist = dist;
        }

        //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist )
        //-- PS.- radiusMatch can also be used here.

        for( int i = 0; i < descriptors_1.rows; i++ ){ 
            if(matches[i].distance <= 2 * min_dist ){ 
                good_matches.push_back(matches[i]); 
            }
        }
        drawMatches( img_1, keypoints_1, img_2, keypoints_2, good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
           vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
        imshow("Lab3", img_matches);
        waitKey();

    }
    //cout << "Matches: " << good_matches.size() << endl;
    return img_matches;
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
    namedWindow("Lab3", CV_WINDOW_AUTOSIZE );

    for (int i = 1; i < argc; i++){
        filename = argv[i];
        if (filename.substr(0,14).compare("pageimagefiles") != 0){
            cout << "Filename: " << filename << endl;
            image = imread(filename, CV_LOAD_IMAGE_COLOR);
            border_dots = get_border_dots(image);
            houghed = get_border_rectangle(border_dots);
            Point2f * points = get_right_bottom_left(houghed);
            transformed = perspective_transformation(image, points);
            sharpened = sharpen_image(transformed);
            compared = find_closest_match(sharpened, templates);
           imshow("Lab3", compared);
            waitKey();
        }
    }
    return 0;
}
