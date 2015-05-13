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
	struct SYSTEM_STATUS {
		double velocity;
	};
	struct RAW_DATA_OBJECT {
		unsigned char targetNo;
		double	threshold;
		double	angle;
		double	range;
		double 	AbsLevel_db;
		double	relatedSpeed;	// m/s
		unsigned char type;
	};
};

#endif	//__CY_OWN_HEADER__
