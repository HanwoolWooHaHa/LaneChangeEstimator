#pragma once

/**
* @file	constant.h
* @version	1.00
* @author	Hanwool Woo
* @date	Creation date: 2015/12/23
* @brief	this file defines constant values
*/

// 1. Data packet
#define PACK_TIME 0
#define PACKET_OWN_GLOBAL_X 1 // 経度，Longitude
#define PACKET_OWN_GLOBAL_Y 2 // 緯度，Latitude
#define PACKET_OWN_GLOBAL_ATT 3 // 姿勢角，Attitude angle

// 2. Data file value
#define T_MAX 2000
#define FILE_COLUMN_MAX 26
#define MDATA_COLUMN_MAX 40
#define LINE_MAX_INDEX 26475
#define RECORD_COLUMN 10
#define NUM_LANE 4

// 3. Sensors setting
//#define LASER_SENS_AREA 80.0 // meter

// 4. Environment setting
#define LANE_WIDTH 3.5

// 5. Approximation setting
#define GAUSS_N 3
#define SENSING_RANGE 100.0
#define RESOLUTION_LINE_POINT 0.1

// 6. Measurement data version
#define ALL_VEHICLE 1
//#define SELECTED_VEHICLE 2
#define ADJ_VEH_MAX 30

// 7. Feature vector info
#define FEATURE_VECTOR_DIMENSION 3

#define FEATURE_PACKET_DISTANCE 0
#define FEATURE_PACKET_LAT_VELOCITY 1
#define FEATURE_PACKET_POTENTIAL 2

#define ADJ_AMOUNTS_LEAD_IDX 0
#define ADJ_AMOUNTS_LEAD_GAP 1
#define ADJ_AMOUNTS_LEAD_VEL 2
#define ADJ_AMOUNTS_REAR_IDX 3
#define ADJ_AMOUNTS_REAR_GAP 4
#define ADJ_AMOUNTS_REAR_VEL 5
#define ADJ_AMOUNTS_PRECED_IDX 6
#define ADJ_AMOUNTS_PRECED_GAP 7
#define ADJ_AMOUNTS_PRECED_VEL 8
#define ADJ_AMOUNTS_OWN_IDX 9
#define ADJ_AMOUNTS_OWN_GAP 10
#define ADJ_AMOUNTS_OWN_VEL 11

// 8. Normalization
#define STD_DISTANCE (0.5 * LANE_WIDTH) // [m]
#define STD_LATERAL_VELOCITY 1.22 // [m/s]


//  9. Hidden Markov Model (HMM)
#define NUM_STATE 4


//  10. Support Vector Machine
#define NUM_CLASS 4
#define WINDOW_SIZE 1

//  11. Estimation result packet
#define RESULT_PACKET_NUM 4
#define RESULT_PACKET_FLAG 0
#define RESULT_PACKET_CLASS 1

//  12. Filter definition
#define MOV_AVG_FLT_SPAN 25 // must be set the odd number
#define MED_FLT_SPAN 7

enum {LANE_KEEPING = 1, LANE_CHANGING, LANE_ARRIVAL, LANE_ADJUSTMENT};

struct stHMM
{
	float INIT[NUM_STATE];
	float TRANS[NUM_STATE][NUM_STATE];
	float EMIS[NUM_STATE][FEATURE_VECTOR_DIMENSION][2];
	double likelihood;
};