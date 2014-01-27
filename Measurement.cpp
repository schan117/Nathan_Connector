#include "Measurement.h"
#include <QSettings>


Measurement::Measurement(void)
{

}


Measurement::~Measurement(void)
{

}

bool Measurement::Load_Settings(int index, QString filename)
{
	QString name = "vision settings/" + filename;

	bool ok;

	QSettings set(name, QSettings::IniFormat);

	is[index].locator_height = set.value("Locator/Height").toInt(&ok);
	if (!ok) return false;

	is[index].locator_width = set.value("Locator/Width").toInt(&ok);
	if (!ok) return false;

	is[index].locator_level_tolerance = set.value("Locator/Level_Tolerance").toDouble(&ok);
	if (!ok) return false;

	is[index].minimum_area = set.value("Locator/Minimum_Area").toInt(&ok);
	if (!ok) return false;

	is[index].length_tolerance = set.value("Locator/Length_Tolerance").toDouble(&ok);
	if (!ok) return false;

	is[index].kernel = set.value("Smooth/Kernel").toInt(&ok);
	if (!ok) return false;

	is[index].sigma = set.value("Smooth/Sigma").toDouble(&ok);
	if (!ok) return false;

	is[index].pre_morph_count = set.value("Smooth/Pre_Morph_Count").toInt(&ok);
	if (!ok) return false;

	is[index].roi_width_reduction = set.value("ROI/Width_Reduction").toDouble(&ok);
	if (!ok) return false;

	is[index].roi_y_offset = set.value("ROI/Y_Offset").toDouble(&ok);
	if (!ok) return false;

	is[index].roi_y_offset_height= set.value("ROI/Y_Offset_Height").toDouble(&ok);
	if (!ok) return false;

	is[index].silver_h = set.value("Colors/Silver_H").toDouble(&ok);
	if (!ok) return false;

	is[index].silver_s = set.value("Colors/Silver_S").toDouble(&ok);
	if (!ok) return false;

	is[index].copper_h = set.value("Colors/Copper_H").toDouble(&ok);
	if (!ok) return false;

	is[index].copper_s = set.value("Colors/Copper_S").toDouble(&ok);
	if (!ok) return false;

	is[index].mapping_ratio = set.value("Mapping/Ratio").toDouble(&ok);
	if (!ok) return false;

	is[index].locator_threshold = set.value("Locator/Threshold").toInt(&ok);
	if (!ok) return false;

	is[index].min_silver = set.value("Decision/Min_Silver").toDouble(&ok);
	if (!ok) return false;

	is[index].fail_threshold = set.value("Decision/Fail_Threshold").toInt(&ok);
	if (!ok) return false;

	is[index].camera_shutter = set.value("Camera/Shutter").toInt(&ok);
	if (!ok) return false;

	return true;
}

bool Measurement::Save_Settings(int index, QString filename)
{
	QString name = "vision settings/" + filename;

	bool ok;

	QSettings set(name, QSettings::IniFormat);

	set.setValue("Locator/Height", is[index].locator_height);
	set.setValue("Locator/Width", is[index].locator_width);
	set.setValue("Locator/Level_Tolerance", is[index].locator_level_tolerance);
	set.setValue("Locator/Minimum_Area", is[index].minimum_area);
	set.setValue("Locator/Length_Tolerance",is[index].length_tolerance);
	set.setValue("Smooth/Kernel",is[index].kernel);
	set.setValue("Smooth/Sigma",is[index].sigma);
	set.setValue("Smooth/Pre_Morph_Count",is[index].pre_morph_count);
	set.setValue("ROI/Width_Reduction",is[index].roi_width_reduction);
	set.setValue("ROI/Y_Offset",is[index].roi_y_offset);
	set.setValue("ROI/Y_Offset_Height",is[index].roi_y_offset_height);
	set.setValue("Colors/Silver_H", is[index].silver_h);
	set.setValue("Colors/Silver_S",is[index].silver_s);
	set.setValue("Colors/Copper_H",is[index].copper_h);
	set.setValue("Colors/Copper_S",is[index].copper_s );
	set.setValue("Mapping/Ratio", is[index].mapping_ratio);
	set.setValue("Locator/Threshold", is[index].locator_threshold);
	set.setValue("Decision/Min_Silver", is[index].min_silver);
	set.setValue("Decision/Fail_Threshold", is[index].fail_threshold);
	set.setValue("Camera/Shutter", is[index].camera_shutter);

    qDebug("Settings saved for index %d!", index);
	
	return true;


}

bool Measurement::Perform_Extraction(int index, int locator_threshold)
{
	switch (index)
	{
		case C1_FRONT:
		{
			if (Perform_C1_Front(locator_threshold))
			{
				return true;
			}
			else
			{
				return false;
			}
			
		}
		case C2_FRONT:
		{
			if (Perform_C2_Front(locator_threshold))
			{
				return true;
			}
			else
			{
				return false;
			}
			
		}
		case C3_FRONT:
		{
			if (Perform_C3_Front(locator_threshold))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		default:
			return false;
	}
}

bool Measurement::Perform_C1_Front(int thresh)
{
	result_image = internal_image.clone();

	// swap r and b channel
	cvtColor(internal_image, internal_image, CV_RGB2BGR);

	GaussianBlur(internal_image, internal_image, Size(is[C1_FRONT].kernel,is[C1_FRONT].kernel), is[C1_FRONT].sigma); 

	Mat element = getStructuringElement(MORPH_RECT, Size(3,3));
	
	for (int i=0; i<is[C1_FRONT].pre_morph_count; i++)
	{
		dilate(internal_image, internal_image, element);
	}

	for (int i=0; i<is[C1_FRONT].pre_morph_count; i++)
	{
		erode(internal_image, internal_image, element);
	}
	
	
	//imwrite("filtered1.bmp", internal_image);

	QString min_string = QString("Min: %1 x %2").arg(is[C1_FRONT].locator_width*(1-is[C1_FRONT].length_tolerance)).arg(is[C1_FRONT].locator_height*(1-is[C1_FRONT].length_tolerance));
	QString max_string = QString("Max: %1 x %2").arg(is[C1_FRONT].locator_width*(1+is[C1_FRONT].length_tolerance)).arg(is[C1_FRONT].locator_height*(1+is[C1_FRONT].length_tolerance));

	putText(result_image, "Locator Search" , Point(0, 1090), 0, 1, Scalar(255,0,0), 2);
	putText(result_image, min_string.toStdString(), Point(0, 1140), 0,1, Scalar(255,0,0), 2);
	putText(result_image, max_string.toStdString(), Point(0, 1190), 0,1, Scalar(255,0,0), 2);
	
	// draw locator search size to make easy for locator size tuning

	Rect small_rect(500, 1130, is[C1_FRONT].locator_width*(1-is[C1_FRONT].length_tolerance) , is[C1_FRONT].locator_height*(1-is[C1_FRONT].length_tolerance));
	Rect big_rect(650, 1130, is[C1_FRONT].locator_width*(1+is[C1_FRONT].length_tolerance) , is[C1_FRONT].locator_height*(1+is[C1_FRONT].length_tolerance));

	rectangle(result_image, small_rect, Scalar(255, 255, 255), -1);
	rectangle(result_image, big_rect, Scalar(255, 255, 255), -1);


	// first use locator condition to pinpoint connectors

	Mat internal_image_gray;
	cvtColor(internal_image, internal_image_gray, CV_RGB2GRAY);

	
	
	//imwrite("thresh.bmp", internal_image_gray);

	threshold(internal_image_gray, internal_image_gray, thresh, 255, CV_THRESH_BINARY);

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(internal_image_gray, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	// locate the contour which has the smallest y coordinate

	QList<int> locator_candidates;

	double area = 0;
	Rect rect;
	Rect min_y_rect;
	int min_y = internal_image_gray.rows;
	int min_y_index = 0;
	int min_index = 0;

	for (int i=0; i<contours.size(); i++)
	{
		// filter out small area contours
		area = contourArea(contours[i]);

		if (area > is[C1_FRONT].minimum_area)
		{
			locator_candidates.append(i);
			
		}
	}

	// check if there are any valid contours at all
	if (locator_candidates.size() == 0)
	{
		result[0] = NO_LOCATOR;
		return false;
	}

	// there will be at least 1 valid contours for considers at this point

	bool is_min_y_rect_valid;

	// search for min_y rect which satisfy size criteria
	do 
	{
		min_y = internal_image_gray.rows;

		for (int i=0; i<locator_candidates.size(); i++)
		{
			rect = boundingRect(contours[locator_candidates[i]]);

			if (rect.y < min_y)
			{
				min_y = rect.y;
				min_y_index = locator_candidates[i];
				min_y_rect = rect;
				min_index = i;
			}
		}

	    is_min_y_rect_valid = (min_y_rect.width > ( is[C1_FRONT].locator_width * (1-is[C1_FRONT].length_tolerance))) &&
								(min_y_rect.width < ( is[C1_FRONT].locator_width * (1 +  is[C1_FRONT].length_tolerance))) &&
								(min_y_rect.height > ( is[C1_FRONT].locator_height * (1-is[C1_FRONT].length_tolerance))) &&
								(min_y_rect.height < ( is[C1_FRONT].locator_height * (1 +  is[C1_FRONT].length_tolerance)))  ;

		if (!is_min_y_rect_valid)
		{
			locator_candidates.removeAt(min_index);
		}

		if (locator_candidates.size() == 0)
		{
			result[0] = LOCATOR_INVALID;
			return false;
		}

	} while (!is_min_y_rect_valid);

	double min_area = contourArea(contours[min_y_index]);
	
	drawContours(result_image, contours, min_y_index, Scalar(0,255,0), 2, 8);
	rectangle(result_image, min_y_rect, Scalar(255,0,0),2);
	// show width, length and area
	QString width_text = QString("Minimum-Y Width: %1, Height: %2, Area: %3").arg(min_y_rect.width).arg(min_y_rect.height).arg(min_area);
	putText(result_image, width_text.toStdString(), Point(5,40), 0, 1, Scalar(255,0,0), 2);

	putText(result_image, "Locator Found", Point(min_y_rect.x, min_y_rect.y-90), 0, 1, Scalar(255,0,0), 2);

	// min_y_rect is locator bounding box at this point
	// next find all contours which have similar y coordinates and length ratios

	line(result_image, Point(0, min_y_rect.y - is[C1_FRONT].locator_level_tolerance), Point(result_image.cols, min_y_rect.y - is[C1_FRONT].locator_level_tolerance), Scalar(255,0,0), 2);
	line(result_image, Point(0, min_y_rect.y + is[C1_FRONT].locator_level_tolerance), Point(result_image.cols, min_y_rect.y + is[C1_FRONT].locator_level_tolerance), Scalar(255,0,0), 2);


	QList<int> connector_tops;
	QList<Rect> connector_tops_rects;

	QString index_string;
	QString area_string;
	double temp_area;

	for (int i=0; i< locator_candidates.size(); i++)
	{
		rect = boundingRect(contours[locator_candidates[i]]);

		if (	(rect.y > (min_y_rect.y - is[C1_FRONT].locator_level_tolerance)) &&
			    (rect.y < (min_y_rect.y + is[C1_FRONT].locator_level_tolerance))  &&
				(rect.width > ( is[C1_FRONT].locator_width * (1-is[C1_FRONT].length_tolerance))) &&
				(rect.width < ( is[C1_FRONT].locator_width * (1 +  is[C1_FRONT].length_tolerance))) )
				//(rect.height > ( is[C1_FRONT].locator_height * is[C1_FRONT].length_tolerance)) &&
				//(rect.height < ( is[C1_FRONT].locator_height * 1 /  is[C1_FRONT].length_tolerance)) )
				
		{
			connector_tops.append(locator_candidates[i]);
			connector_tops_rects.append(rect);

			area = contourArea(contours[locator_candidates[i]]);

			rectangle(result_image, rect, Scalar(0,0,255),2);
			index_string = QString("%1").arg(connector_tops.size());
			area_string = QString("%2").arg(area);
			putText(result_image, index_string.toStdString(), Point(rect.x, rect.y-50), 0, 1, Scalar(0,0,255), 2);
			putText(result_image, area_string.toStdString(), Point(rect.x, rect.y-10), 0, 1, Scalar(0,0,255), 2);

		}
	}

	// Now connector tops contain all contour indices that are needed for inspection
	

	cvtColor(internal_image, image_hsv, CV_BGR2HSV);
	split(image_hsv, hsv);

	// Now, construct a list of roi's 

	QList<Rect> inspections_roi;

	int temp;

	Rect roi;

	for (int i=0; i<connector_tops_rects.size(); i++)
	{
		// make sure roi width is multiples of 4
		temp = connector_tops_rects[i].width * is[C1_FRONT].roi_width_reduction;
		temp = temp - (temp % 4);

		roi.width = temp;
		roi.height = is[C1_FRONT].roi_y_offset_height;
		roi.y = connector_tops_rects[i].y + is[C1_FRONT].roi_y_offset;
		roi.x = connector_tops_rects[i].x + ((double) connector_tops_rects[i].width) * (1-is[C1_FRONT].roi_width_reduction) / 2.0;

		// make sure roi is valid inside image
		if  ( ((roi.x + roi.width) < internal_image.cols) &&
			  ((roi.y + roi.height) < internal_image.rows) )
		{
			inspections_roi.append(roi);
		}
	}

	if ( inspections_roi.size() == 0)
	{
		result[0] = ROI_INVALID;
		return false;
	}

	// now perform morph for all roi

	Mat temp_h;
	Mat temp_s;

	for (int i=0; i<inspections_roi.size(); i++)
	{
		temp_h = hsv[0](inspections_roi[i]);
		temp_s = hsv[1](inspections_roi[i]);

		GaussianBlur(temp_h, temp_h, Size(3,3), 3);
		GaussianBlur(temp_s, temp_s, Size(3,3), 3);

		for (int j=0; j<5; j++)
		{
			dilate(temp_h, temp_h, element);
			dilate(temp_s, temp_s, element);
		}

		for (int j=0; j<3; j++)
		{
			//erode(temp_h, temp_h, element);
			//erode(temp_s, temp_s, element);
		}

	}

	// use the last in the list as display
	int size = inspections_roi.size() - 1;
	
	rectangle(result_image, inspections_roi[size], Scalar(0,0,255),2);

	for (int i=0; i<size; i++)
	{
		rectangle(result_image, inspections_roi[i], Scalar(0,255,0),2);
	}

	/////////////////////////////////////////////////////////////////////////////////////

	double low_h, low_s, high_h, high_s;

	Calculate_Reference_HS(inspections_roi[size], &low_h, &low_s, &high_h, &high_s, 3);

	result[1] = low_h;
	result[2] = low_s;
	result[3] = high_h;
	result[4] = high_s;

	// Calculate color discrepancy for current image
	/////////////////////////////////////////////////////////////////////////////////

	double max_dis = sqrt(180.0*180.0+255.0*255.0);
	double current_dis = sqrt( (low_h-high_h)*(low_h-high_h) + (low_s-high_s)*(low_s-high_s) ) / max_dis;

	// fill in some results
	result[5] = current_dis;

	QList<int> transition_list;

	for (int i=0; i<inspections_roi.size(); i++)
	{
		transition_list.append(Calculate_HS_Transition(C1_FRONT, inspections_roi[i], 3));
	}

	// draw transition lines

	int actual_y;

	for (int i=0; i<transition_list.size(); i++)
	{
		actual_y = inspections_roi[i].y + transition_list[i];
		line(result_image, Point(inspections_roi[i].x, actual_y), Point(inspections_roi[i].x+inspections_roi[i].width, actual_y), Scalar(255,0,0), 2);
	}
	////////////////////////////////////////////////////////////////////////

	// fill in measurement results

	result[6] = inspections_roi.size();

	double length;
	double sum = 0.0;

	for (int i=0; i<inspections_roi.size(); i++)
	{
		// calculate total length using top references

		length = is[C1_FRONT].roi_y_offset + transition_list[i];
		result[i+7] = length * is[C1_FRONT].mapping_ratio;
		sum = sum + length * is[C1_FRONT].mapping_ratio;
	}

	// Also calculate the average height for reference

	result[7+inspections_roi.size()] = sum /inspections_roi.size();
	
	front_roi_color = internal_image(inspections_roi[size]).clone();
	front_roi_h = hsv[0](inspections_roi[size]);
	front_roi_s = hsv[1](inspections_roi[size]);

	cvtColor(front_roi_h, front_roi_h, CV_GRAY2RGB);
	cvtColor(front_roi_s, front_roi_s, CV_GRAY2RGB);
	cvtColor(front_roi_color, front_roi_color, CV_BGR2RGB);

	line(front_roi_h, Point(0, transition_list[size]), Point(front_roi_h.cols, transition_list[size]), Scalar(0,0,255), 1);
	line(front_roi_s, Point(0, transition_list[size]), Point(front_roi_s.cols, transition_list[size]), Scalar(0,0,255), 1);
	line(front_roi_color, Point(0, transition_list[size]), Point(front_roi_s.cols, transition_list[size]), Scalar(0,0,255), 1);


	result[0] = CAL_OK;
	return true;
}

void Measurement::Calculate_Reference_HS(Rect roi, double* low_h, double* low_s, double* high_h, double* high_s, int avg_row)
{
	Mat h_roi = hsv[0](roi);
	Mat s_roi = hsv[1](roi);

	double lh = 0.0;
	double ls = 0.0;
	double hh = 0.0;
	double hs = 0.0;

	for (int i=0; i<h_roi.cols; i++)
	{
		for (int j=h_roi.rows-1; j>h_roi.rows-(avg_row+1); j--)
		{
			lh = lh + h_roi.at<uchar>(Point(i, j));
			ls = ls + s_roi.at<uchar>(Point(i, j));
		}

		for (int j=0; j<avg_row; j++)
		{
			hh = hh + h_roi.at<uchar>(Point(i, j));
			hs = hs + s_roi.at<uchar>(Point(i, j));
		}
	}

	*low_h = lh / (h_roi.cols * avg_row);
	*low_s = ls / (s_roi.cols * avg_row);
	*high_h = hh / (h_roi.cols * avg_row);
	*high_s = hs / (s_roi.cols* avg_row);

}

int Measurement::Calculate_HS_Transition(int index, Rect roi, int avg_row)
{
	Mat h_roi = hsv[0](roi);
	Mat s_roi = hsv[1](roi);

	double avg_h = 0.0;
	double avg_s = 0.0;


	QList<int> y_list;
	QList<double> h_list;
	QList<double> s_list;
	QList<double> silver_dist;
	QList<double> copper_dist;

	for (int y=h_roi.rows-1; y>=avg_row-1; y--)
	{
		avg_h = 0.0;
		avg_s = 0.0;

		for (int yy=y; yy>=(y-(avg_row-1)); yy--)
		{
			for (int x=0; x<h_roi.cols; x++)
			{
					avg_h = avg_h + h_roi.at<uchar>(Point(x, yy));
					avg_s = avg_s + s_roi.at<uchar>(Point(x, yy));
			}
		}

		avg_h = avg_h / (h_roi.cols * avg_row);
		avg_s = avg_s / (s_roi.cols * avg_row);

		y_list.append(y);
		h_list.append(avg_h);
		s_list.append(avg_s);

		// calculate distances

		silver_dist.append(sqrt( (avg_h-is[index].silver_h)*(avg_h-is[index].silver_h) + (avg_s-is[index].silver_s)*(avg_s-is[index].silver_s)));
		copper_dist.append(sqrt( (avg_h-is[index].copper_h)*(avg_h-is[index].copper_h) + (avg_s-is[index].copper_s)*(avg_s-is[index].copper_s)));
	}

    // attempt to find a y value in which HS coordinate is closer to the the silver than copper and use that as the transition value

	for (int i=0; i<y_list.size(); i++)
	{
        //qDebug("%f %f", silver_dist[i], copper_dist[i]);

		// if color is closer to silver, than treat it as silver
		if (silver_dist[i] < copper_dist[i])
		{
            //qDebug("%d, %d", i, y_list[i]);
			return y_list[i];
		}
	}

	// if reach here, there probably is no transition at all, return the highest y value

	return y_list[y_list.size()-1];

	/*
	QFile qf("hs.txt");

	qf.open(QFile::WriteOnly);

	QTextStream qs(&qf);

	for (int i=0; i<y_list.size(); i++)
	{
        qs << y_list[i] << "\t" << copper_dist[i] << "\t" << silver_dist[i] << "";
	}

	qf.close();
	*/
	return true;




}

bool Measurement::Perform_C2_Front(int thresh)
{
	result_image = internal_image.clone();

	// swap r and b channel
	cvtColor(internal_image, internal_image, CV_RGB2BGR);

	GaussianBlur(internal_image, internal_image, Size(is[C2_FRONT].kernel,is[C2_FRONT].kernel), is[C2_FRONT].sigma); 

	Mat element = getStructuringElement(MORPH_RECT, Size(3,3));
	
	for (int i=0; i<is[C2_FRONT].pre_morph_count; i++)
	{
		dilate(internal_image, internal_image, element);
	}

	for (int i=0; i<is[C2_FRONT].pre_morph_count; i++)
	{
		erode(internal_image, internal_image, element);
	}
	
	
	//imwrite("filtered1.bmp", internal_image);


	// first use locator condition to pinpoint connectors

	Mat internal_image_gray;
	cvtColor(internal_image, internal_image_gray, CV_RGB2GRAY);

	threshold(internal_image_gray, internal_image_gray, thresh, 255, CV_THRESH_BINARY);

	//imwrite("thresh.bmp", internal_image_gray);

	QString min_string = QString("Min: %1 x %2").arg(is[C2_FRONT].locator_width*(1-is[C2_FRONT].length_tolerance)).arg(is[C2_FRONT].locator_height*(1-is[C2_FRONT].length_tolerance));
	QString max_string = QString("Max: %1 x %2").arg(is[C2_FRONT].locator_width*(1+is[C2_FRONT].length_tolerance)).arg(is[C2_FRONT].locator_height*(1+is[C2_FRONT].length_tolerance));

	putText(result_image, "Locator Search" , Point(0, 1090), 0, 1, Scalar(255,0,0), 2);
	putText(result_image, min_string.toStdString(), Point(0, 1140), 0,1, Scalar(255,0,0), 2);
	putText(result_image, max_string.toStdString(), Point(0, 1190), 0,1, Scalar(255,0,0), 2);
	
	// draw locator search size to make easy for locator size tuning

	Rect small_rect(500, 1120, is[C2_FRONT].locator_width*(1-is[C2_FRONT].length_tolerance) , is[C2_FRONT].locator_height*(1-is[C2_FRONT].length_tolerance));
	Rect big_rect(650, 1120, is[C2_FRONT].locator_width*(1+is[C2_FRONT].length_tolerance) , is[C2_FRONT].locator_height*(1+is[C2_FRONT].length_tolerance));

	rectangle(result_image, small_rect, Scalar(255, 255, 255), -1);
	rectangle(result_image, big_rect, Scalar(255, 255, 255), -1);


	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(internal_image_gray, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	//drawContours(result_image, contours, -1, Scalar(0,0,255),2);
		
	// locate the contour which has the smallest y coordinate

	QList<int> locator_candidates;

	double area = 0;
	Rect rect;
	Rect min_y_rect;
	int min_y = internal_image_gray.rows;
	int min_y_index = 0;
	int min_index = 0;

	for (int i=0; i<contours.size(); i++)
	{
		// filter out small area contours
		area = contourArea(contours[i]);


		if (area > is[C2_FRONT].minimum_area)
		{
			locator_candidates.append(i);
		}
	}

	
	// check if there are any valid contours at all
	if (locator_candidates.size() == 0)
	{
		result[0] = NO_LOCATOR; // locator cannot be found, all locator contour area does not meet requirements
		return false;
	}

    //qDebug("locator: %d %d", locator_candidates.size(), is[C2_FRONT].minimum_area);

	// there will be at least 1 valid contours for considers at this point

	bool is_min_y_rect_valid;

	// search for min_y rect which satisfy size criteria
	do 
	{
		min_y = internal_image_gray.rows;

		for (int i=0; i<locator_candidates.size(); i++)
		{
			rect = boundingRect(contours[locator_candidates[i]]);

			if (rect.y < min_y)
			{
				min_y = rect.y;
				min_y_index = locator_candidates[i];
				min_y_rect = rect;
				min_index = i;
			}
		}

	    is_min_y_rect_valid = (min_y_rect.width > ( is[C2_FRONT].locator_width * (1-is[C2_FRONT].length_tolerance))) &&
								(min_y_rect.width < ( is[C2_FRONT].locator_width * (1 +  is[C2_FRONT].length_tolerance))) &&
								(min_y_rect.height > ( is[C2_FRONT].locator_height * (1-is[C2_FRONT].length_tolerance))) &&
								(min_y_rect.height < ( is[C2_FRONT].locator_height * (1 +  is[C2_FRONT].length_tolerance)))  ;

		if (!is_min_y_rect_valid)
		{
			locator_candidates.removeAt(min_index);
		}

		if (locator_candidates.size() == 0)
		{
			result[0] = LOCATOR_INVALID;
			return false;
		}

	} while (!is_min_y_rect_valid);

	double min_area = contourArea(contours[min_y_index]);
	
	drawContours(result_image, contours, min_y_index, Scalar(0,255,0), 2, 8);
	rectangle(result_image, min_y_rect, Scalar(255,0,0),2);
	// show width, length and area
	QString width_text = QString("Minimum-Y Width: %1, Height: %2, Area: %3").arg(min_y_rect.width).arg(min_y_rect.height).arg(min_area);
	putText(result_image, width_text.toStdString(), Point(5,40), 0, 1, Scalar(255,0,0), 2);

	putText(result_image, "Locator Found", Point(min_y_rect.x, min_y_rect.y-90), 0, 1, Scalar(255,0,0), 2);

	// min_y_rect is locator bounding box at this point
	// next find all contours which have similar y coordinates and length ratios

	line(result_image, Point(0, min_y_rect.y - is[C2_FRONT].locator_level_tolerance), Point(result_image.cols, min_y_rect.y - is[C2_FRONT].locator_level_tolerance), Scalar(255,0,0), 2);
	line(result_image, Point(0, min_y_rect.y + is[C2_FRONT].locator_level_tolerance), Point(result_image.cols, min_y_rect.y + is[C2_FRONT].locator_level_tolerance), Scalar(255,0,0), 2);


	QList<int> connector_tops;
	QList<Rect> connector_tops_rects;

	QString index_string;
	QString area_string;
	double temp_area;

	for (int i=0; i< locator_candidates.size(); i++)
	{
		rect = boundingRect(contours[locator_candidates[i]]);
		

		if (	(rect.y > (min_y_rect.y - is[C2_FRONT].locator_level_tolerance)) &&
			    (rect.y < (min_y_rect.y + is[C2_FRONT].locator_level_tolerance)) &&
				(rect.width > ( is[C2_FRONT].locator_width * (1-is[C2_FRONT].length_tolerance))) &&
				(rect.width < ( is[C2_FRONT].locator_width * (1 +  is[C2_FRONT].length_tolerance))) ) 
				//(rect.height > ( is[C2_FRONT].locator_height * is[C2_FRONT].length_tolerance)) &&
				//(rect.height < ( is[C2_FRONT].locator_height * 1 /  is[C2_FRONT].length_tolerance)) )
				
		{
			connector_tops.append(locator_candidates[i]);
			connector_tops_rects.append(rect);

			temp_area = contourArea(contours[locator_candidates[i]]);

			rectangle(result_image, rect, Scalar(0,0,255),2);
			index_string = QString("%1").arg(connector_tops.size());
			area_string = QString("%1").arg(temp_area);
			putText(result_image, index_string.toStdString(), Point(rect.x, rect.y-50), 0, 1, Scalar(0,0,255), 2);
			putText(result_image, area_string.toStdString(), Point(rect.x, rect.y-10), 0, 1, Scalar(0,0,255), 2);

		}
	}

	// Now connector tops contain all contour indices that are needed for inspection
	

	cvtColor(internal_image, image_hsv, CV_BGR2HSV);
	split(image_hsv, hsv);

	// Now, construct a list of roi's 

	QList<Rect> inspections_roi;

	int temp;

	Rect roi;

	for (int i=0; i<connector_tops_rects.size(); i++)
	{
		// make sure roi width is multiples of 4
		temp = connector_tops_rects[i].width * is[C2_FRONT].roi_width_reduction;
		temp = temp - (temp % 4);

		roi.width = temp;
		roi.height = is[C2_FRONT].roi_y_offset_height;
		roi.y = connector_tops_rects[i].y + is[C2_FRONT].roi_y_offset;
		roi.x = connector_tops_rects[i].x + ((double) connector_tops_rects[i].width) * (1-is[C2_FRONT].roi_width_reduction) / 2.0;

		// make sure roi is valid inside image
		if  ( ((roi.x + roi.width) < internal_image.cols) &&
			  ((roi.y + roi.height) < internal_image.rows) )
		{
			inspections_roi.append(roi);
		}
	}

	if ( inspections_roi.size() == 0)
	{
		result[0] = ROI_INVALID; 
		return false;
	}

	// now perform morph for all roi

	Mat temp_h;
	Mat temp_s;

	for (int i=0; i<inspections_roi.size(); i++)
	{
		temp_h = hsv[0](inspections_roi[i]);
		temp_s = hsv[1](inspections_roi[i]);

		GaussianBlur(temp_h, temp_h, Size(3,3), 3);
		GaussianBlur(temp_s, temp_s, Size(3,3), 3);

		for (int j=0; j<5; j++)
		{
			dilate(temp_h, temp_h, element);
			dilate(temp_s, temp_s, element);
		}

		for (int j=0; j<3; j++)
		{
			//erode(temp_h, temp_h, element);
			//erode(temp_s, temp_s, element);
		}

	}

	// use the last in the list as display
	int size = inspections_roi.size() - 1;
	
	rectangle(result_image, inspections_roi[size], Scalar(0,0,255),2);

	for (int i=0; i<size; i++)
	{
		rectangle(result_image, inspections_roi[i], Scalar(0,255,0),2);
	}

	/////////////////////////////////////////////////////////////////////////////////////

	double low_h, low_s, high_h, high_s;

	Calculate_Reference_HS(inspections_roi[size], &low_h, &low_s, &high_h, &high_s, 3);

	result[1] = low_h;
	result[2] = low_s;
	result[3] = high_h;
	result[4] = high_s;

	// Calculate color discrepancy for current image
	/////////////////////////////////////////////////////////////////////////////////

	double max_dis = sqrt(180.0*180.0+255.0*255.0);
	double current_dis = sqrt( (low_h-high_h)*(low_h-high_h) + (low_s-high_s)*(low_s-high_s) ) / max_dis;

	// fill in discrepancy result
	result[5] = current_dis;

	///////////////////////////////////////////////////////////////////////////////////

	QList<int> transition_list;

	for (int i=0; i<inspections_roi.size(); i++)
	{
		transition_list.append(Calculate_HS_Transition(C2_FRONT, inspections_roi[i], 3));
	}

	// draw transition lines

	int actual_y;

	for (int i=0; i<transition_list.size(); i++)
	{
		actual_y = inspections_roi[i].y + transition_list[i];
		line(result_image, Point(inspections_roi[i].x, actual_y), Point(inspections_roi[i].x+inspections_roi[i].width, actual_y), Scalar(255,0,0), 2);
	}

	////////////////////////////////////////////////////////////////////////

	// fill in measurement results

	result[6] = inspections_roi.size();

	double length;
	double sum = 0.0;

	for (int i=0; i<inspections_roi.size(); i++)
	{
		// calculate total length using top references

		length = is[C2_FRONT].roi_y_offset + transition_list[i];
		result[i+7] = length * is[C2_FRONT].mapping_ratio;
		sum = sum + length * is[C2_FRONT].mapping_ratio;
	}

	result[7+inspections_roi.size()] = sum /inspections_roi.size();

	front_roi_color = internal_image(inspections_roi[size]).clone();
	front_roi_h = hsv[0](inspections_roi[size]);
	front_roi_s = hsv[1](inspections_roi[size]);

	cvtColor(front_roi_h, front_roi_h, CV_GRAY2RGB);
	cvtColor(front_roi_s, front_roi_s, CV_GRAY2RGB);
	cvtColor(front_roi_color, front_roi_color, CV_BGR2RGB);

	line(front_roi_h, Point(0, transition_list[size]), Point(front_roi_h.cols, transition_list[size]), Scalar(0,0,255), 1);
	line(front_roi_s, Point(0, transition_list[size]), Point(front_roi_s.cols, transition_list[size]), Scalar(0,0,255), 1);
	line(front_roi_color, Point(0, transition_list[size]), Point(front_roi_s.cols, transition_list[size]), Scalar(0,0,255), 1);

	result[0] = CAL_OK;
	return true;
}

bool Measurement::Perform_C3_Front(int thresh)
{
	result_image = internal_image.clone();

	// swap r and b channel
	cvtColor(internal_image, internal_image, CV_RGB2BGR);

	GaussianBlur(internal_image, internal_image, Size(is[C3_FRONT].kernel,is[C3_FRONT].kernel), is[C3_FRONT].sigma); 

	Mat element = getStructuringElement(MORPH_RECT, Size(3,3));
	
	for (int i=0; i<is[C3_FRONT].pre_morph_count; i++)
	{
		dilate(internal_image, internal_image, element);
	}

	for (int i=0; i<is[C3_FRONT].pre_morph_count; i++)
	{
		erode(internal_image, internal_image, element);
	}
	
	
	//imwrite("filtered1.bmp", internal_image);

	QString min_string = QString("Min: %1 x %2").arg(is[C3_FRONT].locator_width*(1-is[C3_FRONT].length_tolerance)).arg(is[C3_FRONT].locator_height*(1-is[C3_FRONT].length_tolerance));
	QString max_string = QString("Max: %1 x %2").arg(is[C3_FRONT].locator_width*(1+is[C3_FRONT].length_tolerance)).arg(is[C3_FRONT].locator_height*(1+is[C3_FRONT].length_tolerance));

	putText(result_image, "Locator Search" , Point(0, 1090), 0, 1, Scalar(255,0,0), 2);
	putText(result_image, min_string.toStdString(), Point(0, 1140), 0,1, Scalar(255,0,0), 2);
	putText(result_image, max_string.toStdString(), Point(0, 1190), 0,1, Scalar(255,0,0), 2);
	
	// draw locator search size to make easy for locator size tuning

	Rect small_rect(500, 1130, is[C3_FRONT].locator_width*(1-is[C3_FRONT].length_tolerance) , is[C3_FRONT].locator_height*(1-is[C3_FRONT].length_tolerance));
	Rect big_rect(650, 1130, is[C3_FRONT].locator_width*(1+is[C3_FRONT].length_tolerance) , is[C3_FRONT].locator_height*(1+is[C3_FRONT].length_tolerance));

	rectangle(result_image, small_rect, Scalar(255, 255, 255), -1);
	rectangle(result_image, big_rect, Scalar(255, 255, 255), -1);


	// first use locator condition to pinpoint connectors

	Mat internal_image_gray;
	cvtColor(internal_image, internal_image_gray, CV_RGB2GRAY);

	threshold(internal_image_gray, internal_image_gray, thresh, 255, CV_THRESH_BINARY);

	//imwrite("thresh.bmp", internal_image_gray);

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(internal_image_gray, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	//drawContours(result_image, contours, -1, Scalar(0,0,255),2);
		
	// locate the contour which has the smallest y coordinate

	QList<int> locator_candidates;

	double area = 0;
	Rect rect;
	Rect min_y_rect;
	int min_y = internal_image_gray.rows;
	int min_y_index = 0;
	int min_index = 0;

	for (int i=0; i<contours.size(); i++)
	{
		// filter out small area contours
		area = contourArea(contours[i]);


		if (area > is[C3_FRONT].minimum_area)
		{
			locator_candidates.append(i);
		}
	}

	
	// check if there are any valid contours at all
	if (locator_candidates.size() == 0)
	{
		result[0] = NO_LOCATOR; // locator cannot be found, all locator contour area does not meet requirements
		return false;
	}

    //qDebug("locator: %d %d", locator_candidates.size(), is[C3_FRONT].minimum_area);

	// there will be at least 1 valid contours for considers at this point

	bool is_min_y_rect_valid;

	// search for min_y rect which satisfy size criteria
	do 
	{
		min_y = internal_image_gray.rows;

		for (int i=0; i<locator_candidates.size(); i++)
		{
			rect = boundingRect(contours[locator_candidates[i]]);

			if (rect.y < min_y)
			{
				min_y = rect.y;
				min_y_index = locator_candidates[i];
				min_y_rect = rect;
				min_index = i;
			}
		}

	    is_min_y_rect_valid = (min_y_rect.width > ( is[C3_FRONT].locator_width * (1-is[C3_FRONT].length_tolerance))) &&
								(min_y_rect.width < ( is[C3_FRONT].locator_width * (1 +  is[C3_FRONT].length_tolerance))) &&
								(min_y_rect.height > ( is[C3_FRONT].locator_height * (1-is[C3_FRONT].length_tolerance))) &&
								(min_y_rect.height < ( is[C3_FRONT].locator_height * (1 +  is[C3_FRONT].length_tolerance)))  ;

		if (!is_min_y_rect_valid)
		{
			locator_candidates.removeAt(min_index);
		}

		if (locator_candidates.size() == 0)
		{
			result[0] = LOCATOR_INVALID;
			return false;
		}

	} while (!is_min_y_rect_valid);

	double min_area = contourArea(contours[min_y_index]);
	
	drawContours(result_image, contours, min_y_index, Scalar(0,255,0), 2, 8);
	rectangle(result_image, min_y_rect, Scalar(255,0,0),2);
	// show width, length and area
	QString width_text = QString("Minimum-Y Width: %1, Height: %2, Area: %3").arg(min_y_rect.width).arg(min_y_rect.height).arg(min_area);
	putText(result_image, width_text.toStdString(), Point(5,40), 0, 1, Scalar(255,0,0), 2);

	putText(result_image, "Locator Found", Point(min_y_rect.x, min_y_rect.y-90), 0, 1, Scalar(255,0,0), 2);

	// min_y_rect is locator bounding box at this point
	// next find all contours which have similar y coordinates and length ratios

	line(result_image, Point(0, min_y_rect.y - is[C3_FRONT].locator_level_tolerance), Point(result_image.cols, min_y_rect.y - is[C3_FRONT].locator_level_tolerance), Scalar(255,0,0), 2);
	line(result_image, Point(0, min_y_rect.y + is[C3_FRONT].locator_level_tolerance), Point(result_image.cols, min_y_rect.y + is[C3_FRONT].locator_level_tolerance), Scalar(255,0,0), 2);


	QList<int> connector_tops;
	QList<Rect> connector_tops_rects;

	QString index_string;
	QString area_string;
	double temp_area;

	for (int i=0; i< locator_candidates.size(); i++)
	{
		rect = boundingRect(contours[locator_candidates[i]]);
		

		if (	(rect.y > (min_y_rect.y - is[C3_FRONT].locator_level_tolerance)) &&
			    (rect.y < (min_y_rect.y + is[C3_FRONT].locator_level_tolerance)) &&
				(rect.width > ( is[C3_FRONT].locator_width * (1 - is[C3_FRONT].length_tolerance))) &&
				(rect.width < ( is[C3_FRONT].locator_width * (1 +  is[C3_FRONT].length_tolerance))) ) 
				//(rect.height > ( is[C3_FRONT].locator_height * is[C3_FRONT].length_tolerance)) &&
				//(rect.height < ( is[C3_FRONT].locator_height * 1 /  is[C3_FRONT].length_tolerance)) )
				
		{
			connector_tops.append(locator_candidates[i]);
			connector_tops_rects.append(rect);

			temp_area = contourArea(contours[locator_candidates[i]]);

			rectangle(result_image, rect, Scalar(0,0,255),2);
			index_string = QString("%1").arg(connector_tops.size());
			area_string = QString("%1").arg(temp_area);
			putText(result_image, index_string.toStdString(), Point(rect.x, rect.y-50), 0, 1, Scalar(0,0,255), 2);
			putText(result_image, area_string.toStdString(), Point(rect.x, rect.y-10), 0, 1, Scalar(0,0,255), 2);

		}
	}

	// Now connector tops contain all contour indices that are needed for inspection
	

	cvtColor(internal_image, image_hsv, CV_BGR2HSV);
	split(image_hsv, hsv);

	// Now, construct a list of roi's 

	QList<Rect> inspections_roi;

	int temp;

	Rect roi;

	for (int i=0; i<connector_tops_rects.size(); i++)
	{
		// make sure roi width is multiples of 4
		temp = connector_tops_rects[i].width * is[C3_FRONT].roi_width_reduction;
		temp = temp - (temp % 4);

		roi.width = temp;
		roi.height = is[C3_FRONT].roi_y_offset_height;
		roi.y = connector_tops_rects[i].y + is[C3_FRONT].roi_y_offset;
		roi.x = connector_tops_rects[i].x + ((double) connector_tops_rects[i].width) * (1-is[C3_FRONT].roi_width_reduction) / 2.0;

		// make sure roi is valid inside image
		if  ( ((roi.x + roi.width) < internal_image.cols) &&
			  ((roi.y + roi.height) < internal_image.rows) )
		{
			inspections_roi.append(roi);
		}
	}

	if ( inspections_roi.size() == 0)
	{
		result[0] = ROI_INVALID; 
		return false;
	}

	// now perform morph for all roi

	Mat temp_h;
	Mat temp_s;

	for (int i=0; i<inspections_roi.size(); i++)
	{
		temp_h = hsv[0](inspections_roi[i]);
		temp_s = hsv[1](inspections_roi[i]);

		GaussianBlur(temp_h, temp_h, Size(3,3), 3);
		GaussianBlur(temp_s, temp_s, Size(3,3), 3);

		for (int j=0; j<5; j++)
		{
			dilate(temp_h, temp_h, element);
			dilate(temp_s, temp_s, element);
		}

		for (int j=0; j<3; j++)
		{
			//erode(temp_h, temp_h, element);
			//erode(temp_s, temp_s, element);
		}

	}

	// use the last in the list as display
	int size = inspections_roi.size() - 1;
	
	rectangle(result_image, inspections_roi[size], Scalar(0,0,255),2);

	for (int i=0; i<size; i++)
	{
		rectangle(result_image, inspections_roi[i], Scalar(0,255,0),2);
	}

	/////////////////////////////////////////////////////////////////////////////////////

	double low_h, low_s, high_h, high_s;

	Calculate_Reference_HS(inspections_roi[size], &low_h, &low_s, &high_h, &high_s, 3);

	result[1] = low_h;
	result[2] = low_s;
	result[3] = high_h;
	result[4] = high_s;

	// Calculate color discrepancy for current image
	/////////////////////////////////////////////////////////////////////////////////

	double max_dis = sqrt(180.0*180.0+255.0*255.0);
	double current_dis = sqrt( (low_h-high_h)*(low_h-high_h) + (low_s-high_s)*(low_s-high_s) ) / max_dis;

	// fill in discrepancy result
	result[5] = current_dis;

	///////////////////////////////////////////////////////////////////////////////////

	QList<int> transition_list;

	for (int i=0; i<inspections_roi.size(); i++)
	{
		transition_list.append(Calculate_HS_Transition(C3_FRONT, inspections_roi[i], 3));
	}

	// draw transition lines

	int actual_y;

	for (int i=0; i<transition_list.size(); i++)
	{
		actual_y = inspections_roi[i].y + transition_list[i];
		line(result_image, Point(inspections_roi[i].x, actual_y), Point(inspections_roi[i].x+inspections_roi[i].width, actual_y), Scalar(255,0,0), 2);
	}

	////////////////////////////////////////////////////////////////////////

	// fill in measurement results

	result[6] = inspections_roi.size();

	double length;
	double sum = 0.0;

	for (int i=0; i<inspections_roi.size(); i++)
	{
		// calculate total length using top references

		length = is[C3_FRONT].roi_y_offset + transition_list[i];
		result[i+7] = length * is[C3_FRONT].mapping_ratio;
		sum = sum + length * is[C3_FRONT].mapping_ratio;
	}

	result[7+inspections_roi.size()] = sum /inspections_roi.size();

	front_roi_color = internal_image(inspections_roi[size]).clone();
	front_roi_h = hsv[0](inspections_roi[size]);
	front_roi_s = hsv[1](inspections_roi[size]);

	cvtColor(front_roi_h, front_roi_h, CV_GRAY2RGB);
	cvtColor(front_roi_s, front_roi_s, CV_GRAY2RGB);
	cvtColor(front_roi_color, front_roi_color, CV_BGR2RGB);

	line(front_roi_h, Point(0, transition_list[size]), Point(front_roi_h.cols, transition_list[size]), Scalar(0,0,255), 1);
	line(front_roi_s, Point(0, transition_list[size]), Point(front_roi_s.cols, transition_list[size]), Scalar(0,0,255), 1);
	line(front_roi_color, Point(0, transition_list[size]), Point(front_roi_s.cols, transition_list[size]), Scalar(0,0,255), 1);

	result[0] = CAL_OK;
	return true;
}

bool Measurement::Set_Image(Mat image)
{
	if (image.data == NULL)
	{
		return false;
	}

	internal_image = image;

	return true;
}






