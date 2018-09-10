#ifndef	__CY_OWN_HEADER__
#define	__CY_OWN_HEADER__

namespace CY_OWN {
	struct DR_FILE_CAN_PKT {
		unsigned char reserve[4];
		unsigned int time;	//byte[7:4]
		unsigned char reserve1[4];
		unsigned short sid;	//byte[13:12]
		unsigned char reserve2[2];
		unsigned char dlc;	//byte[16]
		unsigned char reserve3[3];
		unsigned char data[8];	//byte[27:20]
		unsigned char reserve4[12];
	};

	typedef union {
		struct {
			unsigned char byte[2];
		};
		short int value;
	} YAW;

	struct GUARDRAIL_INFO {
		unsigned char leftEnable;
		double leftPos;
		unsigned char rightEnable;
		double rightPos;
	};

	struct ALERT_INFO {
		unsigned char master;
		unsigned char slave;
	};

	struct SYSTEM_STATUS {
		double velocity;
		double yawrate;
		struct GUARDRAIL_INFO guardrail;
		struct ALERT_INFO alert;
	};

	struct RAW_DATA_OBJECT {
		unsigned int time;
		unsigned char targetNo;
		double	threshold;
		double	angle;
		double	range;
		double 	AbsLevel_db;
		double	relatedSpeed;	// m/s
		unsigned char type;
	};

	struct TRACKING_DATA_OBJECT {
		double range_x;
		double range_y;
		double speed_x;
		double speed_y;
		unsigned char number;
	};
};

#endif	//__CY_OWN_HEADER__
