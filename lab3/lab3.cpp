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

Mat perspective_transformation(Mat image, Point2f * source, Point2f * dest){
    Mat perspective_matrix, transformed;

    perspective_matrix = getPerspectiveTransform(source, dest);
    warpPerspective(image, transformed, perspective_matrix, transformed.size());
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
        only_blue = get_only_blue(image);
        //source = get_4_blue_dots(image);
        //transformed = perspective_transformation(image);
        imshow("Lab3", only_blue);
        waitKey();
    }
    return 0;
}
