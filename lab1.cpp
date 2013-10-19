#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

Mat load_image(int argc, char**argv)
{
    assert((argc == 2) && "Not enough arguments");
    Mat image = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file
    assert((image.data) && "Could not open or find image");
    return image;
}

Mat calc_hist(Mat src)
{
    vector<Mat> bgr_planes;
    split( src , bgr_planes );

    /// Establish the number of bins
    int histSize = 256;

    float range[] = { 0, 256 } ;
    const float* histRange = { range };

    bool uniform = true; bool accumulate = false;

    Mat hue, luminance, saturation;

    /// Compute the histograms:
    calcHist( &bgr_planes[0], 1, 0, Mat(), hue, 1, &histSize, &histRange, uniform, accumulate );
    calcHist( &bgr_planes[1], 1, 0, Mat(), luminance, 1, &histSize, &histRange, uniform, accumulate );
    calcHist( &bgr_planes[2], 1, 0, Mat(), saturation, 1, &histSize, &histRange, uniform, accumulate );

    // Draw the histograms for B, G and R
    int hist_w = 512; 
    int hist_h = 400;
    int bin_w = cvRound( (double) hist_w/histSize );

    Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

    /// Normalize the result to [ 0, histImage.rows ]
    normalize(hue, hue, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
    normalize(luminance, luminance, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
    normalize(saturation, saturation, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

    /// Draw for each channel
    for( int i = 1; i < histSize; i++ )
    {

        if (i <= 7 || i >= 162) {
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(hue.at<float>(i-1)) ) ,
                    Point( bin_w*(i), hist_h - cvRound(hue.at<float>(i)) ),
                    Scalar( 255, 0, 0), 2, 8, 0  );
        }
        else {
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(hue.at<float>(i-1)) ) ,
                    Point( bin_w*(i), hist_h - cvRound(hue.at<float>(i)) ),
                    Scalar( 0, 0, 255), 2, 8, 0  );
        }
        /*line( histImage, Point( bin_w*(i-1), hist_h - cvRound(luminance.at<float>(i-1)) ) ,
          Point( bin_w*(i), hist_h - cvRound(luminance.at<float>(i)) ),
          Scalar( 0, 255, 0), 2, 8, 0  );
          line( histImage, Point( bin_w*(i-1), hist_h - cvRound(saturation.at<float>(i-1)) ) ,
          Point( bin_w*(i), hist_h - cvRound(saturation.at<float>(i)) ),
          Scalar( 0, 0, 255), 2, 8, 0  );*/
    }

    return histImage;
}

int main( int argc, char** argv )
{
    Mat src, histImage;

    cvtColor(load_image(argc, argv), src, CV_BGR2HLS);
    histImage = calc_hist(src);

    namedWindow(argv[1], CV_WINDOW_AUTOSIZE );
    imshow(argv[1], histImage );

    waitKey(0);

    return 0;
}
