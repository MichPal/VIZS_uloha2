#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>

using namespace cv;
using namespace std;

vector<Vec2f> HoughLines_u2(const Mat& img, float rho_step, float theta_step, int threshold, int linesMax);

int main(int argc, char** argv)
{
	const char* filename = "../Images/image.bmp";
	//const char* filename = "../Images/image2.bmp";

	Mat src = imread(filename, IMREAD_COLOR);
	if (src.empty())
	{
		cout << "can not open " << filename << endl;
		return -1;
	}

	Mat blured,dst, cdst;
	medianBlur(src, blured, 3);
	Canny(blured, dst, 60, 300, 3);
	vector<Vec2f> lines;


	lines = HoughLines_u2(dst, 1.0, CV_PI / 180.0, 150, 200);

	//zobrazenie ciar
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(src, pt1, pt2, Scalar(0, 255, 0), 2, CV_AA);
		
	}

	imshow("source", src);

	waitKey();

	return 0;
}



vector<Vec2f> HoughLines_u2(const Mat& img, float rho_step, float theta_step, int threshold, int max_lines)
{

	vector<Vec2f> lines;

	const uchar* image = img.ptr();
	int imwidth = img.cols;
	int imheight = img.rows;

	int theta_max = cvRound(CV_PI / theta_step);
	int rho_max = cvRound((imwidth + imheight) * 2 / rho_step);

	// Create accumulator
	int *acc = new int[theta_max * rho_max];
	memset(acc, 0, sizeof(acc[0]) * (theta_max  * rho_max));

	float ang = 0.0;

	// Fill accumulator
	for (int y = 0; y < imheight; y++)
	{
		for (int x = 0; x < imwidth; x++)
		{
			// Point detected
			if (image[y * imwidth + x])
			{
				ang = 0.0;
				for (int theta = 0; theta < theta_max; theta++)
				{
					int r = cvRound((x * cos(ang) + y * sin(ang)) / rho_step) + (rho_max / 2);
					ang += theta_step;
					acc[theta * rho_max + r]++;
				}
			}
		}
	}
	
	// Find lines
	int line_num = 0;
	for (int r = 0; r < rho_max; r++)
	{
		for (int theta = 0; theta < theta_max; theta++)
		{
			int acc_index = theta * rho_max + r;

			if (acc[acc_index] > threshold && line_num < max_lines)
			{
				line_num++;
				int theta_index = cvRound(acc_index / rho_max);
				int r_index = acc_index - theta_index * rho_max;

				float rho_line = (r_index - rho_max / 2) * rho_step;
				float angle_line = theta_index * theta_step;
				lines.push_back(Vec2f(rho_line, angle_line));
			}
		}
	}
	return lines;
}
