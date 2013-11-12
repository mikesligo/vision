#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;
using namespace cv;

Mat get_k_means(Mat image, int clusterCount, int iterations){
    Mat samples(image.rows * image.cols, 3, CV_32F);
    for( int y = 0; y < image.rows; y++ ){
        for( int x = 0; x < image.cols; x++ ){
            for( int z = 0; z < 3; z++){
                samples.at<float>(y + x*image.rows, z) = image.at<Vec3b>(y,x)[z];
            }   
        }   
    }

    Mat labels;
    Mat centers;
    kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001), iterations, KMEANS_PP_CENTERS, centers );

    Mat new_image( image.size(), image.type() );
    for( int y = 0; y < image.rows; y++ ){
        for( int x = 0; x < image.cols; x++ ){  
            int cluster_idx = labels.at<int>(y + x*image.rows,0);
            new_image.at<Vec3b>(y,x)[0] = centers.at<float>(cluster_idx, 0); 
            new_image.at<Vec3b>(y,x)[1] = centers.at<float>(cluster_idx, 1); 
            new_image.at<Vec3b>(y,x)[2] = centers.at<float>(cluster_idx, 2); 
        }   
    }
    return new_image;
}

// Split the image into interesting parts (bottom half, into 5 bottles) by using the known position of the bottles
vector<Mat> get_bottles(Mat image)
{
    vector<Mat> bottles;
    Rect roi(0,image.rows/2-1, image.cols, image.rows/2);
    Mat cropped = image(roi);
    int divider = cropped.cols/5;
    for (int i=0; i< cropped.cols-1; i = i + divider){
        Mat grey_scale, k_means, section;
        Rect bottle_pos(i, 0, divider, cropped.rows);
        section = cropped(bottle_pos);
        k_means = get_k_means(section,3,3);
        cvtColor(k_means, grey_scale, CV_BGR2GRAY);
        bottles.push_back(grey_scale);
    }
    return bottles;
}

int main( int argc, char** argv )
{
    Mat image;
    string filename;
    assert((argc >= 2) && "Not enough arguments");
    for (int i = 1; i < argc; i++){
        filename = argv[i];
        image = imread(filename, CV_LOAD_IMAGE_COLOR);
        vector<Mat> bottles = get_bottles(image);
        namedWindow("Lab2", CV_WINDOW_AUTOSIZE );
        moveWindow("Lab2", 500, 300);
        for (vector<Mat>::iterator it = bottles.begin(); it != bottles.end(); it++) {
            image = Mat(*it);
            imshow("Lab2", image );
            waitKey(0);
        }
    }
    return 0;



}
