#pragma once

#include <QtTest/QTest>
#include "opencv2\core\core.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\highgui\highgui.hpp"
//#include <float.h>
#include <qmath.h>
//#include <math.h>

#define C1_FRONT	0
#define C2_FRONT	1
#define	C3_FRONT	2
#define	C1_BACK		3
#define	C2_BACK		4
#define	C3_BACK		5

#define	CAL_OK			1
#define	NO_LOCATOR		2
#define	LOCATOR_INVALID	3	
#define	ROI_INVALID		4			

#define MAX_PRODUCT_COUNT   10


struct Inspection_Settings
{
	int locator_width;
	int locator_height;
	double locator_level_tolerance;
	long minimum_area;
	double length_tolerance;
	int kernel;
	double sigma;
	int pre_morph_count;
	double roi_width_reduction;
	int roi_y_offset;
	int roi_y_offset_height;
	double silver_h;
	double silver_s;
	double copper_h;
	double copper_s;
	double mapping_ratio;
	int locator_threshold;
	double min_silver;
	int fail_threshold;
	int camera_shutter;

	Inspection_Settings()
	{
		locator_width = 0;
		locator_height = 0;
		locator_level_tolerance = 0.0;
		minimum_area = 0;
		length_tolerance = 0.0;
		kernel = 0;
		sigma = 0.0;
		pre_morph_count = 0;
		roi_width_reduction = 0.0;
		roi_y_offset = 0;
		roi_y_offset_height = 0;
		silver_h = 0;
		silver_s = 0;
	    copper_h = 0;
	    copper_s = 0;
		mapping_ratio = 0.0;
		locator_threshold = 0;
		min_silver = 0;
		fail_threshold = 0;
		camera_shutter = 0;


	}

};

using namespace cv;

class Measurement
{
public:
	Measurement(void);
	~Measurement(void);

	bool Load_Settings(int index, QString filename);
	bool Save_Settings(int index, QString filename);
	bool Set_Image(Mat image);

	bool Perform_Extraction(int index, int locator_threshold);
	
	Mat result_image;
	Mat internal_image;
	Mat front_roi_color;
	Mat front_roi_h;
	Mat front_roi_s;
	Mat back_roi_h;
	Mat back_roi_s;
	Mat image_hsv;
	Mat hsv[3];

	double result[20];

    Inspection_Settings is[MAX_PRODUCT_COUNT*2];

private:

	bool Perform_C1_Front(int thresh);
	bool Perform_C2_Front(int thresh);
	bool Perform_C3_Front(int thresh);

	void Calculate_Reference_HS(Rect roi, double* low_h, double* low_s, double* high_h, double* high_s, int avg_row);
	int Calculate_HS_Transition(int index, Rect roi, int avg_row);



};

