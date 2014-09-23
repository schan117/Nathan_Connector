#pragma once

#include <QtTest/QTest>
#include "opencv2\core\core.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\highgui\highgui.hpp"
//#include <float.h>
#include <qmath.h>
#include "Definitions.h"
//#include <math.h>

using namespace cv;

class Measurement
{

public:
	Measurement(void);
	~Measurement(void);

	bool Load_Settings(int index, QString filename);
	bool Save_Settings(int index, QString filename);
	bool Set_Image(Mat image);

    bool Perform_Extraction(int calculated_index);
	
	Mat result_image;
	Mat internal_image;
	Mat front_roi_color;
	Mat front_roi_h;
	Mat front_roi_s;
	Mat back_roi_h;
	Mat back_roi_s;
	Mat image_hsv;
	Mat hsv[3];
    Mat aux_color_image[10];
    Mat aux_extraction_image[10];

    double result[100];
    QList<double> result_ex;

    Inspection_Settings is[MAX_PRODUCT_COUNT*2];

private:

	bool Perform_C1_Front(int thresh);
	bool Perform_C2_Front(int thresh);
	bool Perform_C3_Front(int thresh);

    bool Calculate_By_Locator_Method(int calculated_index, int thresh);
    bool Calculate_By_Locator_Method_Ex(int calculated_index, int thresh);
    bool Color_Blob_Extraction(Mat hsv_image, int min_h, int max_h, int min_s, int max_s, int min_v, int max_v, Rect* rect, Mat* in_range_image);

	void Calculate_Reference_HS(Rect roi, double* low_h, double* low_s, double* high_h, double* high_s, int avg_row);
    int Calculate_HS_Transition(int calculated_index, Rect roi, int avg_row);

};

