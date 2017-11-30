/*
	University Jean Monnet
	Introduction to Scientific Programming
	Student: Nadile Nunes de Lima
	Instructor: Philippe Colantoni
	Description: Main program for the analysis of an input video
*/

#include "ImageBase.h"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <string>

using namespace std;
using namespace cv;

using namespace cosi;

int main(int argc, char** argv){
	const string fileName = "smallKitten.mp4"; // Input file

	VideoCapture video(fileName);
	if (!video.isOpened())
	{
		cout << "Could not open the video: " << fileName << endl;
		return -1;
	}

	Mat src;
	// Get one frame from camera to know the frame size and type
    video >> src;
    if (src.empty()) {
        cout << "ERROR! blank frame grabbed" << endl;
        return -1;
    }
    bool isColor = (src.type() == CV_8UC3);

	int frames = video.get(CAP_PROP_FRAME_COUNT); // number of frames
	
	Mat image;
	Mat rgbHist;
	Mat hsvHist;
	Mat dominantColors;
	Mat meanStdDeviation;

    int codec = static_cast<int>(video.get(CAP_PROP_FOURCC));     // Get Codec Type- Int form
    double fps = video.get(CV_CAP_PROP_FPS); // frame rate of the created video - same as the input video
    string filename = "./analysisResult.avi"; // name of the output video file
	//VideoWriter writer(filename, CV_FOURCC('X','2','6','4'), fps, src.size(), true);

	VideoWriter output_cap("analysisResult.avi",  
                               video.get(CV_CAP_PROP_FOURCC),
                               video.get(CV_CAP_PROP_FPS), 
                               cv::Size(video.get(CV_CAP_PROP_FRAME_WIDTH), video.get(CV_CAP_PROP_FRAME_HEIGHT)));

	if (!output_cap.isOpened()){
        cout << "!!! Output video could not be opened" << std::endl;
        return -1;
    }

	// For each frame
	for (int i = 0; i < frames; i++){
		cosi::ImageBase imgBase;
		bool success = video.read(imgBase);

		if (!success){
			cout << "Cannot read  frame " << endl;
			break;
		}

		// This is used, so all images can be well placed on the final result
		imgBase.setHist_h(); // Set the height for the histograms and dominant color areas
		imgBase.setMeanStdDev_w(); // Set the width for the mean and standard deviation area

		rgbHist = imgBase.rgbHistogram();
		hsvHist = imgBase.hsvHistogram();
		dominantColors = imgBase.dominantColors();
		meanStdDeviation = imgBase.meanStandarDeviation();

		int nRows = imgBase.rows + meanStdDeviation.rows;
		int nCols = imgBase.cols + rgbHist.cols;

		Mat imgResult(nRows,nCols,CV_8UC3, Scalar(0, 0, 0)); // Result image

		Mat roiImgResult_TopLeft = imgResult(Rect(0, 0, imgBase.cols, imgBase.rows)); //Frame
		Mat roiImgResult_TopRight = imgResult(Rect(imgBase.cols, 0, rgbHist.cols, rgbHist.rows)); //RGB Histogram
		Mat roiImgResult_SecondRight = imgResult(Rect(imgBase.cols, rgbHist.rows, hsvHist.cols, hsvHist.rows)); //HSV Histogram
		Mat roiImgResult_BottomRight = imgResult(Rect(imgBase.cols, rgbHist.rows+hsvHist.rows, dominantColors.cols, dominantColors.rows)); // Dominant colors
		Mat roiImgResult_BottomLeft = imgResult(Rect(0, imgBase.rows, meanStdDeviation.cols, meanStdDeviation.rows)); // Mean and Standard Deviation
		Mat roiImgResult_BottomMiddle = imgResult(Rect(meanStdDeviation.cols, imgBase.rows, imgBase.getPixelLoc().cols, imgBase.getPixelLoc().rows)); // Pixel location

		// Copying the data to the final result Mat
		imgBase.copyTo(roiImgResult_TopLeft); 
		rgbHist.copyTo(roiImgResult_TopRight);
		hsvHist.copyTo(roiImgResult_SecondRight); 
		dominantColors.copyTo(roiImgResult_BottomRight); 
		meanStdDeviation.copyTo(roiImgResult_BottomLeft);
		imgBase.getPixelLoc().copyTo(roiImgResult_BottomMiddle);

		imshow("frames", imgResult);

		output_cap.write(imgResult);

		//writer.write(imgResult); // write the current frame in the output video

		if(waitKey(5) >= 0) break;
	}
	video.release();
	output_cap.release();
}