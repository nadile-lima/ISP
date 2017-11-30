/*
	University Jean Monnet
	Introduction to Scientific Programming
	Student: Nadile Nunes de Lima
	Instructor: Philippe Colantoni
	Description: File that contains the class ImageBase. The functions for all the analysis are here
*/

#ifndef BASE_H
#define BASE_H

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <string>
#include <sstream>

using namespace std;
using namespace cv;

namespace cosi{
	class ImageBase: public Mat{

		int histSize;
		int hist_w; // width of the histogram and dominant colors area
		int hist_h; // height of the histogram and dominant colors area
		int meanStdDev_w; // width of the mean and standard deviation area
		int meanStdDev_h; // height of the mean and standard deviation area
		int bin_w;
		Mat pixelLoc; // Image with the data for the Pixel location

	private:
		static void mouseEventCallback (int evt, int x, int y, int flags, void* param){
			ImageBase* imgBase = (ImageBase*)param;
			imgBase->mouseEvent ( evt, x, y, flags );
		}
		void mouseEvent ( int evt, int x, int y, int flags );

		void installCallback ( void ){
			setMouseCallback("frames", mouseEventCallback, this );
		}

	public:
		ImageBase(void) : Mat(){ // Constructor
			histSize = 256;
			meanStdDev_h = 80;
			meanStdDev_w = this->cols / 2;
			hist_w = 312;
			hist_h = (this->rows + meanStdDev_h)/3;
			bin_w = cvRound((double)hist_w / histSize);
			installCallback();
		}

		~ImageBase(void){}; // Destructor

		Mat getPixelLoc(void) const{
			return pixelLoc;
		}

		void setHist_h(void){
			hist_h = (this->rows + meanStdDev_h)/3;
		}

		void setMeanStdDev_w(void){
			meanStdDev_w = this->cols / 2;
		}

		// Create an image with the histogram of the current frame
		Mat rgbHistogram(void){
			Mat bgr[3];
			Mat b_hist, g_hist, r_hist;
			float range[] = { 0, 256 };
			const float* histRange = { range };
			int titleSpace = 20;
			
			split(this->clone(), bgr);//split original image into BGR  
			calcHist(&bgr[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, true, false);
			calcHist(&bgr[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, true, false);
			calcHist(&bgr[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, true, false);
			
			// Draw the histograms for B, G and R
			Mat histRGB(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

			// Normalize the values obtained, to fit in the area for the histogram
			normalize(b_hist, b_hist, 0, histRGB.rows - titleSpace, NORM_MINMAX, -1, Mat());
			normalize(g_hist, g_hist, 0, histRGB.rows - titleSpace, NORM_MINMAX, -1, Mat());
			normalize(r_hist, r_hist, 0, histRGB.rows - titleSpace, NORM_MINMAX, -1, Mat());

			for (int i = 1; i < histSize; i++)
			{
				line(histRGB, Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
					Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
					Scalar(255, 0, 0), 2, 8, 0); // Blue channel histogram
				line(histRGB, Point(bin_w*(i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
					Point(bin_w*(i), hist_h - cvRound(g_hist.at<float>(i))),
					Scalar(0, 255, 0), 2, 8, 0); // Green channel histogram
				line(histRGB, Point(bin_w*(i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
					Point(bin_w*(i), hist_h - cvRound(r_hist.at<float>(i))),
					Scalar(0, 0, 255), 2, 8, 0); // Red channel histogram
			}
			// Title
			putText(histRGB, "RGB Histogram", cvPoint(histRGB.cols/3,10), FONT_HERSHEY_COMPLEX_SMALL, 0.6, cvScalar(200,200,250), 1, CV_AA);

			return histRGB;
		};

		// Create an image with the HSV histogram of the current frame
		Mat hsvHistogram(void){
			Mat hsvImg;
			Mat hsv[3];
			Mat h_hist, s_hist, v_hist;

			// The ranges for each channel
			float h_ranges[] = { 0, 256 };
			float s_ranges[] = { 0, 180 };
			float v_ranges[] = { 0, 256 };

			const float* histRange[] = { h_ranges, s_ranges, v_ranges };

			int titleSpace = 20;

			cvtColor(this->clone(), hsvImg, COLOR_BGR2HSV); // convert image to HSV
			split(hsvImg, hsv);//split HSV image into HSV

			calcHist(&hsv[0], 1, 0, Mat(), h_hist, 1, &histSize, &histRange[0], true, false);
			calcHist(&hsv[1], 1, 0, Mat(), s_hist, 1, &histSize, &histRange[1], true, false);
			calcHist(&hsv[2], 1, 0, Mat(), v_hist, 1, &histSize, &histRange[2], true, false);

			// Draw the lines
			// Normalize the values obtained, to fit in the area for the histogram
			Mat histHSV(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));
			normalize(h_hist, h_hist, 0, histHSV.rows - titleSpace, NORM_MINMAX, -1, Mat());
			normalize(s_hist, s_hist, 0, histHSV.rows - titleSpace, NORM_MINMAX, -1, Mat());
			normalize(v_hist, v_hist, 0, histHSV.rows - titleSpace, NORM_MINMAX, -1, Mat());
			
			for (int i = 1; i < histSize; i++)
			{
				line(histHSV, Point(bin_w*(i - 1), hist_h - cvRound(h_hist.at<float>(i - 1))),
					Point(bin_w*(i), hist_h - cvRound(h_hist.at<float>(i))),
					Scalar(220, 20, 60), 2, 8, 0); // Hue channel histogram (crimson)
				line(histHSV, Point(bin_w*(i - 1), hist_h - cvRound(s_hist.at<float>(i - 1))),
					Point(bin_w*(i), hist_h - cvRound(s_hist.at<float>(i))),
					Scalar(173, 255, 47), 2, 8, 0); // Saturation channel histogram (green yellow)
				line(histHSV, Point(bin_w*(i - 1), hist_h - cvRound(v_hist.at<float>(i - 1))),
					Point(bin_w*(i), hist_h - cvRound(v_hist.at<float>(i))),
					Scalar(0, 0, 128), 2, 8, 0); // Value/Brightness channel histogram (navy)
			}

			// Title
			putText(histHSV, "HSV Histogram", cvPoint(histHSV.cols/3,10), FONT_HERSHEY_COMPLEX_SMALL, 0.6, cvScalar(200,200,250), 1, CV_AA);
			return histHSV;
		};

		// Create an image with the dominant colors of the current frame
		Mat dominantColors(void){
			Mat samples(this->rows * this->cols, 3, CV_32F);
			for( int y = 0; y < this->rows; y++ )
				for( int x = 0; x < this->cols; x++ )
					for( int z = 0; z < 3; z++)
					samples.at<float>(y + x*this->rows, z) = this->at<Vec3b>(y,x)[z];

			int clusterCount = 4; // Number of clusters. Consequently, the number of dominant colors
			Mat labels;
			int attempts = 5;
			Mat centers; // The values for the centers will be here
			kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers );

			vector<Mat> dColors;
			int titleSpace = 20;// The 20 pixels is for the name "Dominant colors" title
			int quadrantRows = (hist_h - titleSpace)/2;
			int quadrantCols = hist_w/2;


			for( int x = 0; x < clusterCount; x++ ){
				// Each channel of each center
				int B = centers.at<float>(x,0);
				int G = centers.at<float>(x,1);
				int R = centers.at<float>(x,2);

				dColors.push_back(Mat(quadrantRows, quadrantCols, CV_8UC3, Scalar(B, G, R)));
			}
			
			// Now, the dominant colors will be put it side by side in one image
			int nRows = (dColors.at(0).rows*(dColors.size()/2)) + titleSpace; 
			Mat imgResult(nRows,2*dColors.at(0).cols,CV_8UC3, Scalar(0, 0, 0));

			Mat roiImgResult_TopLeft = imgResult(Rect(0,titleSpace,dColors.at(0).cols,dColors.at(0).rows)); 
			Mat roiImgResult_TopRight = imgResult(Rect(dColors.at(0).cols,titleSpace,dColors.at(1).cols,dColors.at(1).rows)); 

			Mat roiImgResult_BottomLeft = imgResult(Rect(0,dColors.at(0).rows+titleSpace,dColors.at(2).cols,dColors.at(2).rows)); 
			Mat roiImgResult_BottomRight = imgResult(Rect(dColors.at(2).cols,dColors.at(0).rows+titleSpace,dColors.at(3).cols,dColors.at(3).rows)); 

			dColors.at(0).copyTo(roiImgResult_TopLeft); 
			dColors.at(1).copyTo(roiImgResult_TopRight);
			dColors.at(2).copyTo(roiImgResult_BottomLeft); 
			dColors.at(3).copyTo(roiImgResult_BottomRight);

			// Title
			putText(imgResult, "Dominant Colors", cvPoint(imgResult.cols/3,15), FONT_HERSHEY_COMPLEX_SMALL, 0.6, cvScalar(200,200,250), 1, CV_AA);
			return imgResult;
		};

		// Returns a Mat with a black background and a text on it with the values for the Mean and Standard Deviation
		Mat meanStandarDeviation(void){
			Scalar mean;
			Scalar stddev;

			meanStdDev(this->clone(), mean, stddev); // Calculate the mean and Standard Deviation

			Mat result(meanStdDev_h, meanStdDev_w, CV_8UC3, Scalar(0, 0, 0));

			ostringstream oss;
			string text;
			
			oss << "Mean: " << mean.val[0];
			text = oss.str();
			putText(result, text, cvPoint(20,meanStdDev_h/4), FONT_HERSHEY_COMPLEX_SMALL, 0.5, cvScalar(200,200,250), 1, CV_AA);
			
			oss.str("");
			oss.clear();
			
			oss << "Standard Deviation: " << stddev.val[0];
			text = oss.str();
			putText(result, text, cvPoint(20,meanStdDev_h/2), FONT_HERSHEY_COMPLEX_SMALL, 0.5, cvScalar(200,200,250), 1, CV_AA);

			return result;
		}

	};
}

void cosi::ImageBase::mouseEvent( int evt, int x, int y, int flags ){
	this->pixelLoc = Mat(meanStdDev_h, meanStdDev_w, CV_8UC3, Scalar(100, 100, 0));

	ostringstream oss;
	string text;

	// If the mouse is in the image area, will write the position and RGB values, 
	// otherwise will write a message saying that the mouse is not on the image
	if((x < this->cols) && (y < this->rows)){
		Vec3b pix = this->at<Vec3b>(y, x);

		int B = pix.val[0];
		int G = pix.val[1];
		int R = pix.val[2];
			
		oss << "Pixel Loc: " << x << " " << y;
		text = oss.str();
		putText(pixelLoc, text, cvPoint(20,30), FONT_HERSHEY_COMPLEX_SMALL, 0.5, cvScalar(200,200,250), 1, CV_AA);
			
		oss.str("");
		oss.clear();
			
		oss << "R: " << R ;
		text = oss.str();
		putText(pixelLoc, text, cvPoint(20,40), FONT_HERSHEY_COMPLEX_SMALL, 0.5, cvScalar(200,200,250), 1, CV_AA);

		oss.str("");
		oss.clear();
			
		oss << "G: " << G ;
		text = oss.str();
		putText(pixelLoc, text, cvPoint(20,50), FONT_HERSHEY_COMPLEX_SMALL, 0.5, cvScalar(200,200,250), 1, CV_AA);

		oss.str("");
		oss.clear();
			
		oss << "B: " << B ;
		text = oss.str();
		putText(pixelLoc, text, cvPoint(20,60), FONT_HERSHEY_COMPLEX_SMALL, 0.5, cvScalar(200,200,250), 1, CV_AA);
	}else{
		putText(pixelLoc, "Not in the image", cvPoint(20,30), FONT_HERSHEY_COMPLEX_SMALL, 0.5, cvScalar(200,200,250), 1, CV_AA);
	}

		namedWindow("Mouse", WINDOW_AUTOSIZE);
		imshow("Mouse", pixelLoc);
}

#endif