#pragma once

#include <Windows.h>

typedef unsigned char u8;
typedef unsigned int u32;

typedef struct {
	unsigned int	time;
	unsigned int	time2;
	WORD	sid;
	BYTE	dlc;
	BYTE	data[8];
} CAN_MSG;

typedef struct {
	float yawrate;
	float gyro;
	unsigned int time;
	DWORD time_g;
	BYTE	gyro_raw[2];
} OUT_MSG;

typedef union {
	struct {
		unsigned char TargetNum;
		double	angle;
		double	range;
		double 	AbsLevel_db;
		double	x_range;
		double	y_range;
		double relatedSpeed;	// m/s
		unsigned char type;
		unsigned int group;
		unsigned char level;
	};
} RAW_OBJECT_STRUCT;

typedef union {
	struct {
		unsigned char number;
		double	x;
		double	y;
		double	Vx;
		double	Vy;
		unsigned char	lane;
	};
} TRACKING_OBJ;

typedef struct {
	u8		targetNum;
	double	x_range;
	double	y_range;
	double	x_speed;
	double	y_speed;
	unsigned long	time;
} TRACKING_OBJECT;
