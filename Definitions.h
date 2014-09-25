#ifndef DEFINITIONS_H
#define DEFINITIONS_H

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
#define TOO_MANY_ROI    5

#define MAX_PRODUCT_COUNT   10
#define MAX_ROI_COUNT   10

#include <QString>

#define EXTENDED_TYPE0 "Extended_Type0"

struct Inspection_Settings
{
    // basic parameters

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
    double upper_h;
    double upper_s;
    double lower_h;
    double lower_s;
    double mapping_ratio;
    int locator_threshold;
    double min_distance;
    double max_distance;
    int fail_threshold;
    int camera_shutter;
    int camera_gain;
    int vertical_flip;

    // extended parameters
    bool is_extended_type;
    QString extended_type;
    int roi_x_offset;
    int roi_x_offset_width;
    int minimum_x_distance_from_image_left_edge;
    int is_threshold_inverted;
    int aux_roi_count;
    int aux_roi_x_offset[MAX_ROI_COUNT];
    int aux_roi_y_offset[MAX_ROI_COUNT];
    int aux_roi_x_width[MAX_ROI_COUNT];
    int aux_roi_y_height[MAX_ROI_COUNT];
    int aux_roi_h_tolerance[MAX_ROI_COUNT];
    int aux_roi_s_tolerance[MAX_ROI_COUNT];
    double aux_minimum_width[MAX_ROI_COUNT];
    double aux_maximum_width[MAX_ROI_COUNT];
    double aux_minimum_height[MAX_ROI_COUNT];
    double aux_maximum_height[MAX_ROI_COUNT];


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
        upper_h = 0;
        upper_s = 0;
        lower_h = 0;
        lower_s = 0;
        mapping_ratio = 0.0;
        locator_threshold = 0;
        min_distance = 0;
        max_distance = 0;
        fail_threshold = 0;
        camera_shutter = 0;
        camera_gain = 0;
        vertical_flip = 0;

        is_extended_type = false;
        roi_x_offset = 0;
        roi_x_offset_width = 0;
        minimum_x_distance_from_image_left_edge = 0;
        is_threshold_inverted = 0;
        extended_type = "";


    }

};

struct result_ex_type0
{
    QList<double> result_ex;
    QList<double> average_aux_width;
    QList<double> average_aux_height;
};

#endif // DEFINITIONS_H
