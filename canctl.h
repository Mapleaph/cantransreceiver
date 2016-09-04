#ifndef __CAN_CTL_H
#define __CAN_CTL_H

/*command param for ioctl*/
#define	SET_CAN_BAUDRATE	0x80000000
#define GET_CAN_BAUDRATE	0x80000001
#define SET_CAN_ACC_CODE	0x80000002
#define GET_CAN_ACC_CODE	0x80000003
#define SET_CAN_ACM_CODE	0x80000004
#define GET_CAN_ACM_CODE	0x80000005
#define GET_REG_VALUE		0x80000006
#define GET_CAN_INFO		0x80000007

typedef struct
{
	unsigned int	FrameFormat;	/*0=std, 1=ext*/
	unsigned int	RemoteTransReq;	/*remote transmission request*/
	unsigned int	ID;				/*the destination node ID*/
	unsigned int	DataLenCode;	/*data length code(0-8)*/
	unsigned char	Data[8];		/*the data to trans/rec*/
}CAN_MESSAGE, *pCAN_MESSAGE;

/**************************************************
**
**current mode
**bit 7	:0-basicCAN modr,1-peliCAN mode
**bit 6-5 :reserved
**bit 4	:sleep mode
**bit 3	:acceptance filter mode,0-dual filter,1-single filter
**bit 2	:reserved
**bit 1	:listen only mode
**bit 0	:reserved
**
**interruptenable
**bit 7	:bus error interrupt enable
**bit 6	:arbitration lost interrupt enable
**bit 5	:error passive interrupt enable
**bit 4	:wake-up interrupt enable
**bit 3	:data overrun interrupt enable
**bit 2	:error warning interrupt enable
**bit 1	:transmit interrupt enable
**bit 0	:receive interrupt enable
**
**errorwarninglimit
**bit 7-0	:error warning limit value
**************************************************/

typedef struct
{
	unsigned char	CurrentMode;		/*can working mode*/
	unsigned char	InterruptEnable;	/*interrupt enable status*/
	unsigned char	ErrorWarningLimit;	/*max allowed tx/rx count*/
	unsigned int	tx_count;			/*transmit count*/
	unsigned int	rx_count;			/*receive count*/
	unsigned int	bus_err_count;		/*bus error count*/
	unsigned int	al_err_count;		/*arbitration lost error count*/
	unsigned int	ep_err_count;		/*error passive error count*/
	unsigned int	wu_count;			/*wake up count*/
	unsigned int	do_err_count;		/*data overrun error count*/
	unsigned int	ew_count;			/*error warning error count*/
	unsigned int	tx_err_count;		/*transmit error count*/
	unsigned int	rx_err_count;		/*receive error count*/
}CAN_INFORMATION, *pCAN_INFORMATION;

#endif
