#pragma once

//#define PROPOSED
//#define MANDALIA 1
//#define SCHLECHTRIEMEN 2
#define PROPOSED_PF 3

//#define US_101 0
#define I_80 1

// MODE
enum eMODE{ DEFAULT=0, LINE_EXTRACTOR, TRAINER, TESTER, APPROXIMATE_CURVES, KEEPING, CHANGING };
enum eData{ TRAFFIC=0, FEATURE, LABEL };
enum eMethod{ HMM=0, SVM };

// FILE, DIR
#if defined (US_101)
	#define LINE_FILE_PATH "../../../Dataset/us-101/line/"
	#define TRAFFIC_FILE_PATH "../../../Dataset/us-101/traffic/"
#elif (I_80)
	#define LINE_FILE_PATH "../../../Dataset/i-80/line/"
	#define TRAFFIC_FILE_PATH "../../../Dataset/i-80/traffic/"
#endif

// RETURN
#define FAIL -1
#define DONE 1

// DATA
#define NUM_LINE_FILES 250
#define NUM_LINE 5
#define MAX_LINE_PT 500
#define MAX_VEHICLE_NO 4000
#define T_MAX 3000
#define NUM_COLUMN 18

#define NUM_TRAFFIC_DATA 350

#define MAX_Y 3000 // feet
#define RESOLUTION_LINE_PINT 0.328084
#define SENSING_AREA 100 // meter
#define LANE_WIDTH 12.28

// Traffic data packet
#define DATA_PACKET_NO 0
#define DATA_PACKET_SCENE 1
#define DATA_PACKET_LABEL 2
#define DATA_PACKET_X 4
#define DATA_PACKET_Y 5
#define DATA_PACKET_LENGTH 8
#define DATA_PACKET_WIDTH 9
#define DATA_PACKET_VEL 11
#define DATA_PACKET_LANE 13
#define DATA_PACKET_PRECEDING_VEH 14
#define DATA_PACKET_FOLLOWING_VEH 15

// DATA INFO PACKET
#define DATA_INFO_PACKET_VEHICLE_NO 0
#define DATA_INFO_PACKET_DATA_LENGTH 1
#define DATA_INFO_PACKET_GROUND_TRUTH 2

// Filter
#define MOV_AVG_SPAN 25 // must be set the odd number
#define MED_FLT_SPAN 7
#define NUM_STATE 4 // used by Bayesian filter

#if defined (KUMAR)
	#define BAYESIAN_FILTER
#endif

// FEATURE VECTOR
#if defined (PROPOSED)
	#define FEATURE_VECTOR_DIMENSION 2
#elif (MANDALIA)
	#define FEATURE_VECTOR_DIMENSION 4
#elif (SCHLECHTRIEMEN)
	#define FEATURE_VECTOR_DIMENSION 3
#elif (PROPOSED_PF)
	#define FEATURE_VECTOR_DIMENSION 3
#else
	#define FEATURE_VECTOR_DIMENSION 3
#endif
#define FEATURE_PACKET_DISTANCE 0
#define FEATURE_PACKET_LAT_VELOCITY 1
#define FEATURE_PACKET_LEAD_GAP 2
#define FEATURE_PACKET_LEAD_REL_VEL 3
#define FEATURE_PACKET_LAG_GAP 4
#define FEATURE_PACKET_LAG_REL_VEL 5
#define FEATURE_PACKET_LEAD_PROB_GAP 2
#define FEATURE_PACKET_LAG_PROB_GAP 3
#define FEATURE_PACKET_PROB_GAP 2
#define FEATURE_PACKET_REL_VELOCITY 2

// LABEL
#define LANE_KEEPING 1
#define LANE_CHANGING 2
#define LANE_ADJUSTMENT 3

#define METER_TO_FEET 3.28
#define FEET_TO_METER 0.3048

// ADJACENT VEHICLE DATA
#define NUM_ADJACENT_COLUMN 6
#define MAX_ADJACENT 200
#define ADJ_DATA_PACKET_VEHICLE_NO 0
#define ADJ_DATA_PACKET_SCENE 1
#define ADJ_DATA_PACKET_X 2
#define ADJ_DATA_PACKET_Y 3
#define ADJ_DATA_PACKET_LANE 4
#define ADJ_DATA_PACKET_VEL 5

// HIDDEN MARKOV MODEL (HMM)
#define STATE_NUM 4
#define STD_DISTANCE 1.87
#define STD_VELOCITY 1.22
#define STD_REL_VELOCITY 8.3

// MANDALIA
#define MANDALIA_WINDOW_SIZE 12

// WINDOW SIZE for SVM
#if defined (MANDALIA)
	#define WINDOW_SIZE 1
#elif (SCHLECHTRIEMEN)
	#define WINDOW_SIZE 1
#else
	#define WINDOW_SIZE 10
#endif

// PREDICTION TIME LIMIT
#define PREDICTION_TIME_LIMIT 66

// MEASUREMENT TIME
#define MEASUREMENT_TIME 66

struct stHMM
{
    float INIT[STATE_NUM];
    float TRANS[STATE_NUM][STATE_NUM];
    float EMIS[STATE_NUM][FEATURE_VECTOR_DIMENSION][2];
    double likelihood;
};