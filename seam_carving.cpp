#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>
using namespace std;
using namespace cv;

void energyMatrix(Mat &image, float** energy){
    int height = image.rows;
    int width = image.cols;
    for(int i=1;i<height-1;i++){
        for(int j=1;j<width-1;j++){
            Vec3b leftpix=image.at<Vec3b>(i, j-1);
            Vec3b rightpix=image.at<Vec3b>(i, j+1);
            Vec3b toppix=image.at<Vec3b>(i-1, j);
            Vec3b bottompix=image.at<Vec3b>(i+1, j);
            float deltax=pow(abs(leftpix[0]-rightpix[0]),2)+pow(abs(leftpix[1]-rightpix[1]),2)+pow(abs(leftpix[2]-rightpix[2]),2);
            float deltay=pow(abs(toppix[0]-bottompix[0]),2)+pow(abs(toppix[1]-bottompix[1]),2)+pow(abs(toppix[2]-bottompix[2]),2);
            energy[i][j]=sqrt(deltax+deltay);
        }
    }
}

void findSeam(float** energy, int** path, int* seam, int height, int width){
    for(int j=0;j<width;j++){
        path[0][j]=energy[0][j];
    }
    for(int i=1;i<height;i++){
        for(int j=0;j<width;j++){
            path[i][j]=energy[i][j]+path[i-1][j];
            if(j>0 && j<width-1){
                if(path[i][j]>min(energy[i][j]+path[i-1][j-1], energy[i][j]+path[i-1][j+1])){
                    path[i][j]=min(energy[i][j]+path[i-1][j-1], energy[i][j]+path[i-1][j+1]);
                }
            }
            else if(j==width-1){
                if(path[i][j]>(energy[i][j]+path[i-1][j-1])){
                    path[i][j]=energy[i][j]+path[i-1][j-1];
                }
            }
            else if(j==0){
                if(path[i][j]>(energy[i][j]+path[i-1][j+1])){
                    path[i][j]=energy[i][j]+path[i-1][j+1];
                }
            }
        }
    }
    int min_index=0;
    for(int j=1;j<width;j++){
        if(path[height-1][j]<path[height-1][min_index]){
            min_index=j;
        }
    }
    seam[height-1]=min_index;
    for(int i=height-2;i>=0;i--){
        int seam_above=seam[i+1];
        seam[i]=seam_above;
        if(seam_above>0 && path[i][seam_above-1]<path[i][seam[i]]){
            seam[i]=seam_above-1;
        }
        if(seam_above<width-1 && path[i][seam_above+1]<path[i][seam[i]]){
            seam[i]=seam_above+1;
        }
    }
}
void removeSeam(Mat& image, int* seam, int height, int width, string& output, int flag){
    Mat output_image;
    //if(flag==1){
        output_image.create(height, width-1,image.type());
        for(int i=0;i<height;i++){
            int seamCol=seam[i];
            for(int j=0;j<seamCol;j++){
                output_image.at<Vec3b>(i,j)=image.at<Vec3b>(i,j);
            }
            for(int j=seamCol;j<width-1;j++){
                output_image.at<Vec3b>(i,j)=image.at<Vec3b>(i,j+1);
            }
        }
    //}
    
    // else{
    //     output_image.create(height-1, width,image.type());
    //     for(int j=0;j<width;j++){
    //         int seamRow=seam[j];
    //         for(int i=0;i<seamRow;i++){
    //             output_image.at<Vec3b>(i,j)=image.at<Vec3b>(i,j);
    //         }
    //         for(int i=seamRow;i<height-1;i++){
    //             output_image.at<Vec3b>(i,j)=image.at<Vec3b>(i+1,j);
    //         }
    //     }
    // }
    if(!imwrite(output, output_image)){
            cerr<<"Failed to save image"<<endl;
        }
    image=output_image;
}

int main(int argc, char** argv){
    string input=argv[1];
    string output=argv[2];
    int new_height=stoi(argv[4]);
    int new_width=stoi(argv[3]);
    Mat image=imread(input);

    int height=image.rows;
    int width=image.cols;
    while(width>new_width){
        float** energy=new float*[height];
        int** path=new int*[height];
        int* seam=new int[height];
        for(int i=0;i<height;i++){
            energy[i]=new float[width];
            path[i]=new int[width];
        }
        energyMatrix(image, energy);
        findSeam(energy, path, seam, height, width);
        removeSeam(image, seam, height, width, output, 1);
        for(int i=0;i<height;i++){
            delete[] energy[i];
            delete[] path[i];
        }
        delete[] energy;
        delete[] path;
        delete[] seam;
        width--;
    }
   // swap(height,width);

    // while(width>new_width){
    //     float** energy=new float*[height];
    //     int** path=new int*[height];
    //     int* seam=new int[height];
    //     for(int i=0;i<height;i++){
    //         energy[i]=new float[width];
    //         path[i]=new int[width];
    //     }
    //     energyMatrix(image, energy);
    //     findSeam(energy, path, seam, height, width);
    //     removeSeam(image, seam, height, width, output, 1);
    //     for(int i=0;i<height;i++){
    //         delete[] energy[i];
    //         delete[] path[i];
    //     }
    //     delete[] energy;
    //     delete[] path;
    //     delete[] seam;
    //     width--;
    // }
    while(height>new_height){
        transpose(image,image);
        float** energy=new float*[width];
        int** path=new int*[width];
        int* seam=new int[width];
        for(int i=0;i<width;i++){
            energy[i]=new float[height];
            path[i]=new int[height];
        }
        energyMatrix(image, energy);
        findSeam(energy, path, seam, width, height);
        removeSeam(image, seam, width, height, output, 1);
        for(int i=0;i<width;i++){
            delete[] energy[i];
            delete[] path[i];
        }
        delete[] energy;
        delete[] path;
        delete[] seam;
        height--;
        transpose(image,image);
    }
    imwrite(output, image);

    return 0;
}