#include "Measurement.h"
#include <QSettings>
#include <QDebug>


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

    // Load in basic parameters

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

    is[index].upper_h = set.value("Colors/Upper_H").toDouble(&ok);
	if (!ok) return false;

    is[index].upper_s = set.value("Colors/Upper_S").toDouble(&ok);
	if (!ok) return false;

    is[index].lower_h = set.value("Colors/Lower_H").toDouble(&ok);
	if (!ok) return false;

    is[index].lower_s = set.value("Colors/Lower_S").toDouble(&ok);
	if (!ok) return false;

	is[index].mapping_ratio = set.value("Mapping/Ratio").toDouble(&ok);
	if (!ok) return false;

	is[index].locator_threshold = set.value("Locator/Threshold").toInt(&ok);
	if (!ok) return false;

    is[index].min_distance = set.value("Decision/Min_Distance").toDouble(&ok);
	if (!ok) return false;

    is[index].max_distance = set.value("Decision/Max_Distance").toDouble(&ok);
    if (!ok) return false;

	is[index].fail_threshold = set.value("Decision/Fail_Threshold").toInt(&ok);
	if (!ok) return false;

	is[index].camera_shutter = set.value("Camera/Shutter").toInt(&ok);
	if (!ok) return false;

    is[index].camera_gain = set.value("Camera/Gain").toInt(&ok);
    if (!ok) return false;

    is[index].vertical_flip = set.value("Process/Vertical_Flip").toInt(&ok);
    if (!ok) return false;

    // load in extended settings

    QString qs = set.value("Main/Type").toString();

    if (qs == "Extended_Type0")
    {
        qDebug() << "Extended_Type0 detected!";
        // if it is really an extended parameter file, load them in

        is[index].extended_type = qs;

        is[index].is_extended_type = true;

        is[index].is_threshold_inverted = set.value("Locator_Ex_Type0/Is_Threshold_Inverted").toInt(&ok);
        if (!ok) return false;

        is[index].minimum_x_distance_from_image_left_edge = set.value("Locator_Ex_Type0/Minimum_X_Distance_From_Image_Left_Edge").toInt(&ok);
        if (!ok) return false;

        is[index].roi_x_offset = set.value("ROI_Ex_Type0/X_Offset").toInt(&ok);
        if (!ok) return false;

        is[index].roi_x_offset_width = set.value("ROI_Ex_Type0/X_Offset_Width").toInt(&ok);
        if (!ok) return false;

        is[index].aux_roi_count = set.value("Aux_ROI_Ex_Type0/Count").toInt(&ok);
        if (!ok) return false;

        for (int i=0; i< is[index].aux_roi_count; i++)
        {
            is[index].aux_roi_x_offset[i] = set.value(QString("Aux_ROI_Ex_Type0/Aux%1_X_Offset").arg(i)).toInt(&ok);
            if (!ok) return false;

            is[index].aux_roi_y_offset[i] = set.value(QString("Aux_ROI_Ex_Type0/Aux%1_Y_Offset").arg(i)).toInt(&ok);
            if (!ok) return false;

            is[index].aux_roi_x_width[i] = set.value(QString("Aux_ROI_Ex_Type0/Aux%1_X_Width").arg(i)).toInt(&ok);
            if (!ok) return false;

            is[index].aux_roi_y_height[i] = set.value(QString("Aux_ROI_Ex_Type0/Aux%1_Y_Height").arg(i)).toInt(&ok);
            if (!ok) return false;

            is[index].aux_roi_h_tolerance[i] = set.value(QString("Aux_ROI_Ex_Type0/Aux%1_H_Tolerance").arg(i)).toInt(&ok);
            if (!ok) return false;

            is[index].aux_roi_s_tolerance[i] = set.value(QString("Aux_ROI_Ex_Type0/Aux%1_S_Tolerance").arg(i)).toInt(&ok);
            if (!ok) return false;

            is[index].aux_minimum_width[i] = set.value(QString("Decision_Ex_Type0/Aux%1_Min_Width").arg(i)).toDouble(&ok);
            if (!ok) return false;

            is[index].aux_maximum_width[i] = set.value(QString("Decision_Ex_Type0/Aux%1_Max_Width").arg(i)).toDouble(&ok);
            if (!ok) return false;

            is[index].aux_minimum_height[i] = set.value(QString("Decision_Ex_Type0/Aux%1_Min_Height").arg(i)).toDouble(&ok);
            if (!ok) return false;

            is[index].aux_maximum_height[i] = set.value(QString("Decision_Ex_Type0/Aux%1_Max_Height").arg(i)).toDouble(&ok);
            if (!ok) return false;
        }

    }

    qDebug() << "Loaded settings for:" << name;

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
    set.setValue("Colors/Upper_H", is[index].upper_h);
    set.setValue("Colors/Upper_S",is[index].upper_s);
    set.setValue("Colors/Lower_H",is[index].lower_h);
    set.setValue("Colors/Lower_S",is[index].lower_s );
	set.setValue("Mapping/Ratio", is[index].mapping_ratio);
	set.setValue("Locator/Threshold", is[index].locator_threshold);
    set.setValue("Decision/Min_Distance", is[index].min_distance);
    set.setValue("Decision/Max_Distance", is[index].max_distance);
	set.setValue("Decision/Fail_Threshold", is[index].fail_threshold);
	set.setValue("Camera/Shutter", is[index].camera_shutter);

    if (is[index].is_extended_type)
    {
        if (is[index].extended_type == "Extended_Type0")
        {
            set.setValue("Decision_Ex_Type0/Aux0_Min_Width", is[index].aux_minimum_width[0]);
            set.setValue("Decision_Ex_Type0/Aux0_Max_Width", is[index].aux_maximum_width[0]);
            set.setValue("Decision_Ex_Type0/Aux1_Min_Width", is[index].aux_minimum_width[1]);
            set.setValue("Decision_Ex_Type0/Aux1_Max_Width", is[index].aux_maximum_width[1]);
            set.setValue("Decision_Ex_Type0/Aux1_Min_Height", is[index].aux_minimum_height[1]);
            set.setValue("Decision_Ex_Type0/Aux1_Max_Height", is[index].aux_maximum_height[1]);
        }
    }

    qDebug("Settings saved for calculated index %d!", index);
	
	return true;


}

bool Measurement::Perform_Extraction(int calculated_index)
{
    int locator_threshold = is[calculated_index].locator_threshold;

    if (is[calculated_index].is_extended_type)
    {
        if ( Calculate_By_Locator_Method_Ex_Type0(calculated_index, locator_threshold))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if ( Calculate_By_Locator_Method(calculated_index, locator_threshold) )
        {
            return true;
        }
        else
        {
            return false;
        }
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

int Measurement::Calculate_HS_Transition(int calculated_index, Rect roi, int avg_row)
{
	Mat h_roi = hsv[0](roi);
	Mat s_roi = hsv[1](roi);

	double avg_h = 0.0;
	double avg_s = 0.0;


	QList<int> y_list;
	QList<double> h_list;
	QList<double> s_list;
    QList<double> upper_dist;
    QList<double> lower_dist;

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

        upper_dist.append(sqrt( (avg_h-is[calculated_index].upper_h)*(avg_h-is[calculated_index].upper_h) + (avg_s-is[calculated_index].upper_s)*(avg_s-is[calculated_index].upper_s)));
        lower_dist.append(sqrt( (avg_h-is[calculated_index].lower_h)*(avg_h-is[calculated_index].lower_h) + (avg_s-is[calculated_index].lower_s)*(avg_s-is[calculated_index].lower_s)));
	}

    // attempt to find a y value in which HS coordinate is closer to the the silver than copper and use that as the transition value

	for (int i=0; i<y_list.size(); i++)
	{
        //qDebug("%f %f", silver_dist[i], copper_dist[i]);

        // if color is closer to upper color, than treat it as the upper color
        if (upper_dist[i] < lower_dist[i])
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

bool Measurement::Calculate_By_Locator_Method(int calculated_index, int thresh)
{
    // check if need to vertical flip the image
    if (is[calculated_index].vertical_flip)
    {
        flip(internal_image, internal_image, 0);  // flip around x axs
    }

    result_image = internal_image.clone();

    // swap r and b channel
    cvtColor(internal_image, internal_image, CV_RGB2BGR);

    GaussianBlur(internal_image, internal_image, Size(is[calculated_index].kernel,is[calculated_index].kernel), is[calculated_index].sigma);

    Mat element = getStructuringElement(MORPH_RECT, Size(3,3));

    for (int i=0; i<is[calculated_index].pre_morph_count; i++)
    {
        dilate(internal_image, internal_image, element);
    }

    for (int i=0; i<is[calculated_index].pre_morph_count; i++)
    {
        erode(internal_image, internal_image, element);
    }


    //imwrite("filtered1.bmp", internal_image);

    QString min_string = QString("Min: %1 x %2").arg(is[calculated_index].locator_width*(1-is[calculated_index].length_tolerance)).arg(is[calculated_index].locator_height*(1-is[calculated_index].length_tolerance));
    QString max_string = QString("Max: %1 x %2").arg(is[calculated_index].locator_width*(1+is[calculated_index].length_tolerance)).arg(is[calculated_index].locator_height*(1+is[calculated_index].length_tolerance));

    putText(result_image, "Locator Search" , Point(0, 1090), 0, 1, Scalar(255,0,0), 2);
    putText(result_image, min_string.toStdString(), Point(0, 1140), 0,1, Scalar(255,0,0), 2);
    putText(result_image, max_string.toStdString(), Point(0, 1190), 0,1, Scalar(255,0,0), 2);

    // draw locator search size to make easy for locator size tuning

    Rect small_rect(500, 1130, is[calculated_index].locator_width*(1-is[calculated_index].length_tolerance) , is[calculated_index].locator_height*(1-is[calculated_index].length_tolerance));
    Rect big_rect(650, 1130, is[calculated_index].locator_width*(1+is[calculated_index].length_tolerance) , is[calculated_index].locator_height*(1+is[calculated_index].length_tolerance));

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

        if (area > is[calculated_index].minimum_area)
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

        is_min_y_rect_valid = (min_y_rect.width > ( is[calculated_index].locator_width * (1-is[calculated_index].length_tolerance))) &&
                                (min_y_rect.width < ( is[calculated_index].locator_width * (1 +  is[calculated_index].length_tolerance))) &&
                                (min_y_rect.height > ( is[calculated_index].locator_height * (1-is[calculated_index].length_tolerance))) &&
                                (min_y_rect.height < ( is[calculated_index].locator_height * (1 +  is[calculated_index].length_tolerance)))  ;

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

    line(result_image, Point(0, min_y_rect.y - is[calculated_index].locator_level_tolerance), Point(result_image.cols, min_y_rect.y - is[calculated_index].locator_level_tolerance), Scalar(255,0,0), 2);
    line(result_image, Point(0, min_y_rect.y + is[calculated_index].locator_level_tolerance), Point(result_image.cols, min_y_rect.y + is[calculated_index].locator_level_tolerance), Scalar(255,0,0), 2);


    QList<int> connector_tops;
    QList<Rect> connector_tops_rects;

    QString index_string;
    QString area_string;
    double temp_area;

    for (int i=0; i< locator_candidates.size(); i++)
    {
        rect = boundingRect(contours[locator_candidates[i]]);

        if (	(rect.y > (min_y_rect.y - is[calculated_index].locator_level_tolerance)) &&
                (rect.y < (min_y_rect.y + is[calculated_index].locator_level_tolerance))  &&
                (rect.width > ( is[calculated_index].locator_width * (1-is[calculated_index].length_tolerance))) &&
                (rect.width < ( is[calculated_index].locator_width * (1 +  is[calculated_index].length_tolerance))) )
                //(rect.height > ( is[calculated_index].locator_height * is[calculated_index].length_tolerance)) &&
                //(rect.height < ( is[calculated_index].locator_height * 1 /  is[calculated_index].length_tolerance)) )

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
    // First check how many connectors are there

    if (connector_tops.count() > MAX_ROI_COUNT)
    {
        result[0] = TOO_MANY_ROI;
        return false;
    }

    cvtColor(internal_image, image_hsv, CV_BGR2HSV);
    split(image_hsv, hsv);

    // Now, construct a list of roi's

    QList<Rect> inspections_roi;

    int temp;

    Rect roi;

    for (int i=0; i<connector_tops_rects.size(); i++)
    {
        // make sure roi width is multiples of 4
        temp = connector_tops_rects[i].width * is[calculated_index].roi_width_reduction;
        temp = temp - (temp % 4);

        roi.width = temp;
        roi.height = is[calculated_index].roi_y_offset_height;
        roi.y = connector_tops_rects[i].y + is[calculated_index].roi_y_offset;
        roi.x = connector_tops_rects[i].x + ((double) connector_tops_rects[i].width) * (1-is[calculated_index].roi_width_reduction) / 2.0;

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
        transition_list.append(Calculate_HS_Transition(calculated_index, inspections_roi[i], 3));
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

        length = is[calculated_index].roi_y_offset + transition_list[i];
        result[i+7] = length * is[calculated_index].mapping_ratio;
        sum = sum + length * is[calculated_index].mapping_ratio;
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

bool Measurement::Calculate_By_Locator_Method_Ex_Type0(int calculated_index, int thresh)
{
    // check if need to vertical flip the image
    if (is[calculated_index].vertical_flip)
    {
        flip(internal_image, internal_image, 0);  // flip around x axs
    }

    result_image = internal_image.clone();

    // swap r and b channel
    cvtColor(internal_image, internal_image, CV_RGB2BGR);

    //GaussianBlur(internal_image, internal_image, Size(is[calculated_index].kernel,is[calculated_index].kernel), is[calculated_index].sigma);
    medianBlur(internal_image, internal_image, is[calculated_index].kernel);

    Mat element = getStructuringElement(MORPH_RECT, Size(3,3));

    for (int i=0; i<is[calculated_index].pre_morph_count; i++)
    {
        dilate(internal_image, internal_image, element);
    }

    for (int i=0; i<is[calculated_index].pre_morph_count; i++)
    {
        erode(internal_image, internal_image, element);
    }

    // first use locator condition to pinpoint connectors
    Mat internal_image_gray;
    cvtColor(internal_image, internal_image_gray, CV_RGB2GRAY);

    //imwrite("thresh.bmp", internal_image_gray);

    threshold(internal_image_gray, internal_image_gray, thresh, 255, CV_THRESH_BINARY_INV);

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(internal_image_gray, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    // if there are too many contours, do not do anything
    if (contours.size() > (MAX_ROI_COUNT * 3) )
    {
        result[0] = TOO_MANY_ROI;
        return false;
    }

    // locate the contour which has a minimum x coordinate AND greater than a certain value to further prevent ROIs getting out of the screen
    QList<int> locator_candidates;

    double area = 0;
    Rect rect;
    Rect min_x_rect;
    int min_x = internal_image_gray.cols;
    int min_x_index = 0;
    int min_index = 0;

    for (int i=0; i<contours.size(); i++)
    {
        // filter out small area contours
        area = contourArea(contours[i]);

        if (area > is[calculated_index].minimum_area)
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

    bool is_min_x_rect_valid;

    // search for min_y rect which satisfy size criteria
    do
    {
        min_x = internal_image_gray.cols;

        for (int i=0; i<locator_candidates.size(); i++)
        {
            rect = boundingRect(contours[locator_candidates[i]]);

            if (rect.x < min_x && rect.x > is[calculated_index].minimum_x_distance_from_image_left_edge)
            {
                min_x = rect.x;
                min_x_index = locator_candidates[i];
                min_x_rect = rect;
                min_index = i;
            }
        }

        is_min_x_rect_valid = (min_x_rect.width > ( is[calculated_index].locator_width * (1-is[calculated_index].length_tolerance))) &&
                                (min_x_rect.width < ( is[calculated_index].locator_width * (1 +  is[calculated_index].length_tolerance))) &&
                                (min_x_rect.height > ( is[calculated_index].locator_height * (1-is[calculated_index].length_tolerance))) &&
                                (min_x_rect.height < ( is[calculated_index].locator_height * (1 +  is[calculated_index].length_tolerance)))  ;

        if (!is_min_x_rect_valid)
        {
            locator_candidates.removeAt(min_index);
        }

        if (locator_candidates.size() == 0)
        {
            result[0] = LOCATOR_INVALID;
            return false;
        }

    } while (!is_min_x_rect_valid);

    double min_area = contourArea(contours[min_x_index]);

    drawContours(result_image, contours, min_x_index, Scalar(0,255,0), 2, 8);
    rectangle(result_image, min_x_rect, Scalar(255,0,0),2);
    // show width, length and area
    QString width_text = QString("Minimum-X Width: %1, Height: %2, Area: %3").arg(min_x_rect.width).arg(min_x_rect.height).arg(min_area);
    putText(result_image, width_text.toStdString(), Point(5,40), 0, 1, Scalar(255,0,0), 2);

    putText(result_image, "Locator Found", Point(min_x_rect.x, min_x_rect.y-90), 0, 1, Scalar(255,0,0), 2);

    // min_x_rect is locator bounding box at this point
    // next find all contours which have similar y coordinates and length ratios
    // use the centre of the locator hole for level tolerancing
    int centre = min_x_rect.y + min_x_rect.height / 2;


    line(result_image, Point(0, centre - is[calculated_index].locator_level_tolerance), Point(result_image.cols, centre - is[calculated_index].locator_level_tolerance), Scalar(255,0,0), 2);
    line(result_image, Point(0, centre + is[calculated_index].locator_level_tolerance), Point(result_image.cols, centre + is[calculated_index].locator_level_tolerance), Scalar(255,0,0), 2);

    QList<int> locator_holes;
    QList<Rect> locator_holes_rects;

    QString index_string;
    QString area_string;
    double temp_area;

    for (int i=0; i< locator_candidates.size(); i++)
    {
        rect = boundingRect(contours[locator_candidates[i]]);

        if (	(rect.y > (centre - is[calculated_index].locator_level_tolerance)) &&
                (rect.y < (centre + is[calculated_index].locator_level_tolerance))  &&
                (rect.width > ( is[calculated_index].locator_width * (1-is[calculated_index].length_tolerance))) &&
                (rect.width < ( is[calculated_index].locator_width * (1 +  is[calculated_index].length_tolerance))) &&
                (rect.x > is[calculated_index].minimum_x_distance_from_image_left_edge) )

                //(rect.height > ( is[calculated_index].locator_height * is[calculated_index].length_tolerance)) &&
                //(rect.height < ( is[calculated_index].locator_height * 1 /  is[calculated_index].length_tolerance)) )

        {
            locator_holes.append(locator_candidates[i]);
            locator_holes_rects.append(rect);

            area = contourArea(contours[locator_candidates[i]]);

            rectangle(result_image, rect, Scalar(0,0,255),2);
            index_string = QString("%1").arg(locator_holes.size());
            area_string = QString("%2").arg(area);
            putText(result_image, index_string.toStdString(), Point(rect.x, rect.y-50), 0, 1, Scalar(0,0,255), 2);
            putText(result_image, area_string.toStdString(), Point(rect.x, rect.y-10), 0, 1, Scalar(0,0,255), 2);

        }
    }

    // Now locator holes contain all contour indices that are needed for inspection
    // First check how many connectors are there

    if (locator_holes.count() > MAX_ROI_COUNT)
    {
        result[0] = TOO_MANY_ROI;
        return false;
    }

    cvtColor(internal_image, image_hsv, CV_BGR2HSV);
    split(image_hsv, hsv);

    // Now, construct a list of roi's

    QList<Rect> basic_inspections_roi;

    int temp;

    Rect roi;

    for (int i=0; i<locator_holes_rects.size(); i++)
    {
        // make sure roi width is multiples of 4
        temp = is[calculated_index].roi_x_offset_width;
        temp = temp - (temp % 4);

        roi.width = temp;
        roi.height = is[calculated_index].roi_y_offset_height;
        roi.y = locator_holes_rects[i].y + is[calculated_index].roi_y_offset;
        roi.x = locator_holes_rects[i].x + is[calculated_index].roi_x_offset;

        // make sure roi is valid inside image
        if  ( ((roi.x + roi.width) < internal_image.cols) &&
              ((roi.y + roi.height) < internal_image.rows) &&
               (roi.x > 0) &&
               (roi.y > 0) )
        {
            basic_inspections_roi.append(roi);
        }
    }

    if ( basic_inspections_roi.size() == 0)
    {
        result[0] = ROI_INVALID;
        return false;
    }

    // now perform morph for all roi

    Mat temp_h;
    Mat temp_s;

    for (int i=0; i<basic_inspections_roi.size(); i++)
    {
        temp_h = hsv[0](basic_inspections_roi[i]);
        temp_s = hsv[1](basic_inspections_roi[i]);

        //GaussianBlur(temp_h, temp_h, Size(3,3), 3);
        //GaussianBlur(temp_s, temp_s, Size(3,3), 3);
        medianBlur(temp_h, temp_h, 3);
        medianBlur(temp_s, temp_s, 3);


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
    int size = basic_inspections_roi.size() - 1;

    rectangle(result_image, basic_inspections_roi[size], Scalar(0,0,255),2);

    for (int i=0; i<size; i++)
    {
        rectangle(result_image, basic_inspections_roi[i], Scalar(0,255,0),2);
    }

    /////////////////////////////////////////////////////////////////////////////////////

    double low_h, low_s, high_h, high_s;

    Calculate_Reference_HS(basic_inspections_roi[size], &low_h, &low_s, &high_h, &high_s, 3);

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

    for (int i=0; i<basic_inspections_roi.size(); i++)
    {
        transition_list.append(Calculate_HS_Transition(calculated_index, basic_inspections_roi[i], 3));
    }

    // draw transition lines

    int actual_y;

    for (int i=0; i<transition_list.size(); i++)
    {
        actual_y = basic_inspections_roi[i].y + transition_list[i];
        line(result_image, Point(basic_inspections_roi[i].x, actual_y), Point(basic_inspections_roi[i].x+basic_inspections_roi[i].width, actual_y), Scalar(255,0,0), 2);
    }
    ////////////////////////////////////////////////////////////////////////

    // fill in measurement results

    result[6] = basic_inspections_roi.size();

    double length;
    double sum = 0.0;

    for (int i=0; i<basic_inspections_roi.size(); i++)
    {
        // calculate total length using top references

        length = qAbs(is[calculated_index].roi_y_offset) - transition_list[i]; // NOTE the minus sign
        result[i+7] = length * is[calculated_index].mapping_ratio;
        sum = sum + length * is[calculated_index].mapping_ratio;
    }

    // Also calculate the average height for reference

    result[7+basic_inspections_roi.size()] = sum / basic_inspections_roi.size();

    front_roi_color = internal_image(basic_inspections_roi[size]).clone();
    front_roi_h = hsv[0](basic_inspections_roi[size]);
    front_roi_s = hsv[1](basic_inspections_roi[size]);

    cvtColor(front_roi_h, front_roi_h, CV_GRAY2RGB);
    cvtColor(front_roi_s, front_roi_s, CV_GRAY2RGB);
    cvtColor(front_roi_color, front_roi_color, CV_BGR2RGB);

    line(front_roi_h, Point(0, transition_list[size]), Point(front_roi_h.cols, transition_list[size]), Scalar(0,0,255), 1);
    line(front_roi_s, Point(0, transition_list[size]), Point(front_roi_s.cols, transition_list[size]), Scalar(0,0,255), 1);
    line(front_roi_color, Point(0, transition_list[size]), Point(front_roi_s.cols, transition_list[size]), Scalar(0,0,255), 1);

    // now, perform extra area inspections
    // form extra inspection roi's

    QList<QList<Rect>> aux_roi;

    for (int i=0; i<locator_holes_rects.size(); i++)
    {
        QList<Rect> temp;

        for (int j=0; j<is[calculated_index].aux_roi_count; j++)
        {
            int x = locator_holes_rects[i].x + is[calculated_index].aux_roi_x_offset[j];
            int y = locator_holes_rects[i].y + is[calculated_index].aux_roi_y_offset[j];
            int w = is[calculated_index].aux_roi_x_width[j];
            int ww = w - (w % 4); // make sure w is multiple of 4
            int h = is[calculated_index].aux_roi_y_height[j];

            temp.append(cvRect(x, y, ww, h));

            if (i == 0)
            {
                rectangle(result_image, temp[temp.size() - 1], Scalar(0,0,255),2);
            }
            else
            {
                rectangle(result_image, temp[temp.size() - 1], Scalar(0,255,0),2);
            }
        }

        aux_roi.append(temp);
    }

    // go through the list and extract!
    m_result_ex_type0.result_ex.clear();
    m_result_ex_type0.average_aux_width.clear();
    m_result_ex_type0.average_aux_height.clear();
    // first, append the number of connector to the list
    // then, append the number of aux roi for each connector
    m_result_ex_type0.result_ex.append(locator_holes_rects.size());
    m_result_ex_type0.result_ex.append(is[calculated_index].aux_roi_count);

    double* width_sum  = new double[is[calculated_index].aux_roi_count];
    double* height_sum = new double[is[calculated_index].aux_roi_count];

    // initialize
    for (int i=0; i<(int) m_result_ex_type0.result_ex[1]; i++)
    {
        width_sum[i] = 0;
        height_sum[i] = 0;
    }

    for (int i=0; i<locator_holes_rects.size(); i++)
    {
        for (int j=0; j<is[calculated_index].aux_roi_count; j++)
        {
           Mat sub_hsv_image = image_hsv(aux_roi[i][j]);
           Mat in_range_image;
           Rect rect;
           double temp_width;
           double temp_height;
           if (Color_Blob_Extraction(sub_hsv_image,
                                 is[calculated_index].upper_h - is[calculated_index].aux_roi_h_tolerance[j],
                                 is[calculated_index].upper_h + is[calculated_index].aux_roi_h_tolerance[j],
                                 is[calculated_index].upper_s - is[calculated_index].aux_roi_s_tolerance[j],
                                 is[calculated_index].upper_s + is[calculated_index].aux_roi_s_tolerance[j],
                                 0,
                                 255,
                                 &rect,
                                 &in_range_image))
           {
               if (i == 0)
               {
                   // for the first work piece, get a sample image for the aux roi

                   // range image
                   cvtColor(in_range_image, aux_extraction_image[j], CV_GRAY2RGB);

                   // color image
                   Mat temp_image = internal_image(aux_roi[i][j]).clone();
                   cvtColor(temp_image, aux_color_image[j], CV_BGR2RGB);
                   rectangle(aux_color_image[j], Rect(rect.x, rect.y, rect.width, rect.height), Scalar(255,0,0), 1);
               }

               temp_width = rect.width * is[calculated_index].mapping_ratio;
               temp_height = rect.height * is[calculated_index].mapping_ratio;

               m_result_ex_type0.result_ex.append(temp_width);
               m_result_ex_type0.result_ex.append(temp_height);
           }
           else
           {
               temp_width = 0;
               temp_height = 0;

               m_result_ex_type0.result_ex.append(temp_width);
               m_result_ex_type0.result_ex.append(temp_height);
           }

           width_sum[j]  = width_sum[j] + temp_width;
           height_sum[j] = height_sum[j] + temp_height;
        }
    }

    // calculate the average width and height
    for (int i=0; i<is[calculated_index].aux_roi_count; i++)
    {
        m_result_ex_type0.average_aux_width.append(width_sum[i] / (double) m_result_ex_type0.result_ex[0]);
        m_result_ex_type0.average_aux_height.append(height_sum[i] / (double) m_result_ex_type0.result_ex[0]);
    }

    delete [] width_sum;
    delete [] height_sum;

    // All done!

    result[0] = CAL_OK;
    return true;
}

bool Measurement::Color_Blob_Extraction(Mat hsv_image, int min_h, int max_h, int min_s, int max_s, int min_v, int max_v, Rect* rect, Mat* in_range_image)
{

    rect->x = 0;
    rect->y = 0;
    rect->width = 0;
    rect->height = 0;


    Mat hsv_c = hsv_image.clone();
    medianBlur(hsv_c, hsv_c, 3);

    inRange(hsv_c, Scalar(min_h, min_s, min_v), Scalar(max_h, max_s, max_v), *in_range_image);

    Mat element = getStructuringElement(MORPH_RECT, Size(3,3));

    for (int i = 0; i<3; i++)
    {
        dilate(*in_range_image, *in_range_image, element);
    }

    for (int i = 0; i<3; i++)
    {
        erode(*in_range_image, *in_range_image, element);
    }

    Mat temp = in_range_image->clone();

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(temp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    if (contours.size() == 0)
    {
        return false;
    }

    int max_index;
    double max_area = 0;

    // find the largest contour
    for (int i=0; i<contours.size(); i++)
    {
        double area = contourArea(contours[i]);

        if (area > max_area)
        {
            max_index = i;
            max_area = area;
        }
    }

    // find the larget bounding box
    Rect r = boundingRect(contours[max_index]);

    rect->x = r.x;
    rect->y = r.y;
    rect->width = r.width;
    rect->height = r.height;

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






