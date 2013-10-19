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
            if ( (hue  < 0 || hue >= 162) && hue != 165 ){
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
    } else if (red_count < 10000){
        printf("1 red spoon found!\tRed count is %d\n",red_count);
    } else {
        printf("2 red spoons found!\tRed count is %d\n",red_count);
    }
    return red_only;
}

Mat get_k_means(string filename, int clusterCount, int iterations){
    Mat src = imread(filename, CV_LOAD_IMAGE_COLOR);   // Read the file
    Mat samples(src.rows * src.cols, 3, CV_32F);
    for( int y = 0; y < src.rows; y++ ){
        for( int x = 0; x < src.cols; x++ ){
            for( int z = 0; z < 3; z++){
                samples.at<float>(y + x*src.rows, z) = src.at<Vec3b>(y,x)[z];
            }
        }
    }

    Mat labels;
    Mat centers;
    kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001), iterations, KMEANS_PP_CENTERS, centers );

    Mat new_image( src.size(), src.type() );
    for( int y = 0; y < src.rows; y++ ){
        for( int x = 0; x < src.cols; x++ ){ 
            int cluster_idx = labels.at<int>(y + x*src.rows,0);
            new_image.at<Vec3b>(y,x)[0] = centers.at<float>(cluster_idx, 0);
            new_image.at<Vec3b>(y,x)[1] = centers.at<float>(cluster_idx, 1);
            new_image.at<Vec3b>(y,x)[2] = centers.at<float>(cluster_idx, 2);
        }
    }
    return new_image;
}

int main( int argc, char** argv )
{
    Mat red_only, k_means;
    string filename;
    assert((argc >= 2) && "Not enough arguments");
    for (int i = 1; i < argc; i++){
        filename = argv[i];
        k_means = get_k_means(filename,20,5);
        red_only = count_red_spoons(k_means);
        namedWindow("Spoons", CV_WINDOW_AUTOSIZE );
        imshow("Spoons", red_only );
        waitKey(0);
        //imshow("Spoons", k_means );
    }
    return 0;



}
