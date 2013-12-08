#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;
using namespace cv;

Mat get_only_blue(Mat image){
    Mat only_blue = image.clone();
    Mat hls;

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
    return only_blue;
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
    Mat image, transformed, only_blue;
    string filename;
    assert((argc >= 2) && "Not enough arguments");
    namedWindow("Lab3", CV_WINDOW_AUTOSIZE );
    for (int i = 1; i < argc; i++){
        filename = argv[i];
        image = imread(filename, CV_LOAD_IMAGE_COLOR);
        //only_blue = get_only_blue(image);
        //source = get_4_blue_dots(image);
        transformed = perspective_transformation(image);
        imshow("Lab3", transformed);
        waitKey();
    }
    return 0;
}
