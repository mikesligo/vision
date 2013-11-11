#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

Mat count_red_spoons(Mat image)
{
    Mat hls, histImage;
    cvtColor(image, hls, CV_BGR2HLS);
    Mat red_only = image.clone();

    int red_count = 0;
    for (int row = 0; row < hls.rows; row ++){
        for (int col = 0; col < hls.cols; col ++){
            int hue = hls.at<Vec3b>(row,col)[0];
            int saturation = hls.at<Vec3b>(row,col)[2];
            if ( ((hue  <= 2 && hue >= 0) || hue >= 160) && (saturation >= 120 )){
                red_count ++;
            } else {
                for (int channels = 0; channels < red_only.channels(); channels ++){
                    red_only.at<Vec3b>(row,col)[channels] = 0;
                }
            }
        }
    }
    if (red_count < 500){
        printf("No spoons found!\tRed count is %d\n",red_count);
    } else if (red_count < 13000){
        printf("1 red spoon found!\tRed count is %d\n",red_count);
    } else {
        printf("2 red spoons found!\tRed count is %d\n",red_count);
    }
    return red_only;
}

int main( int argc, char** argv )
{
    Mat red_only, image;
    string filename;
    assert((argc >= 2) && "Not enough arguments");
    for (int i = 1; i < argc; i++){
        filename = argv[i];
        image = imread(filename, CV_LOAD_IMAGE_COLOR);
        red_only = count_red_spoons(image);

        namedWindow("Original", CV_WINDOW_AUTOSIZE );
        namedWindow("Spoons", CV_WINDOW_AUTOSIZE );
        moveWindow("Original", 700, 0);
        moveWindow("Spoons", 700, 700);
        imshow("Original", image );
        imshow("Spoons", red_only );
        waitKey(0);
        //imshow("Spoons", k_means );
    }
    return 0;



}
