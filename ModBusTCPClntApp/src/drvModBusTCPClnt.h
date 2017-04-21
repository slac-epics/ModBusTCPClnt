#ifndef	_drvModBusTCPClnt_H_
#define	_drvModBusTCPClnt_H_

#ifdef _WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

#ifdef	vxWorks
#include "semLib.h"
#define SEM_CREATE()    semMCreate(SEM_INVERSION_SAFE|SEM_DELETE_SAFE|SEM_Q_PRIORITY)
#define SEM_DELETE(x)   semDelete((SEMAPHORE)(x))
#define SEM_TAKE(x)     semTake((SEMAPHORE)(x),WAIT_FOREVER)
#define SEM_GIVE(x)     semGive((SEMAPHORE)(x))
#define	SEMAPHORE	SEM_ID
#else
#include <epicsVersion.h>
#if     EPICS_VERSION>=3 && EPICS_REVISION>=14
#include "epicsMutex.h"
#else
#error  "You need EPICS 3.14 or above because we need OSI support!"
#endif

#define SEM_CREATE()    epicsMutexMustCreate()
#define SEM_DELETE(x)   epicsMutexDestroy((SEMAPHORE)(x))
#define SEM_TAKE(x)     epicsMutexMustLock((SEMAPHORE)(x))
#define SEM_GIVE(x)     epicsMutexUnlock((SEMAPHORE)(x))
#define	SEMAPHORE	epicsMutexId
typedef	int	BOOL;
#endif

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/*
 * Internal data structures
 */

/* MBAP header */
typedef struct MBAP_HDR
{
        unsigned short int      tID;    /* transaction ID, here is big-endian */
        unsigned short int      pID;    /* protocol ID, here is 0 */
        unsigned short int      len;    /* # of following bytes, include UNIT and PDU size, big-endian */
        unsigned char           UNIT;   /* Slave identifier */
}__attribute__ ((packed))       MBAP_HDR;

/* The exception PDU */
typedef struct MBT_EXCPT_PDU
{
        unsigned char           fCode;  /* Function code with MSB set */
        unsigned char           errcode;   /* error code */
}__attribute__ ((packed))       MBT_EXCPT_PDU;

/***********************************************************************************************/
/*********************** Request PDU and Response PDU definition *******************************/
/* We use xXYYYoffset to name offset, x is b(bit) or w(word), X is R(read) or W(Write)         */
/* YYY is the combination of I(input image) O(output image) R(registers)                       */
/* Dworddata means 16 bits scalar value for Diag, and WByteData means byte array to write      */
/***********************************************************************************************/
/************* MBT Function 1, Read digital outputs setting ******************************/
typedef struct MBT_F1_REQ
{
        unsigned char           fCode;  /* Function code, will be hardcode to 1 */
        unsigned short int      bROoffset; /* bits offset in output memory image, big-endian */
        unsigned short int      RBitCount;  /* number of bits to read, big-endian */
}__attribute__ ((packed))       MBT_F1_REQ;

typedef struct MBT_F1_RES
{
        unsigned char           fCode;	/* Function code, must to 1 */
        unsigned char		RByteCount; /* How many bytes following */
        unsigned char		RByteData[1];  /* byte data */
}__attribute__ ((packed))       MBT_F1_RES;

/************* MBT Function 2, Read digital inputs *****************************************/
typedef struct MBT_F2_REQ
{
        unsigned char           fCode;  /* Function code, will be hardcode to 2 */
        unsigned short int      bRIoffset; /* bits offset in input memory image, big-endian */
        unsigned short int      RBitCount;  /* number of bits to read, big-endian */
}__attribute__ ((packed))       MBT_F2_REQ;
/* Response PDU of FUNC 2 is same as FUNC 1 */
typedef struct MBT_F1_RES	MBT_F2_RES;

/************* MBT Function 3, Read Analong inputs and outputs and registers ***************/
typedef struct MBT_F3_REQ
{
        unsigned char           fCode;  /* Function code, will be hardcode to 3 */
        unsigned short int      wRIORoffset; /* word 16 bits offset in both memory image and registers, big-endian */
        unsigned short int      RWordCount;  /* number of words to read, big-endian */
}__attribute__ ((packed))       MBT_F3_REQ;

typedef struct MBT_F3_RES
{
        unsigned char           fCode;  /* Function code, must be 3 */
        unsigned char           RByteCount; /* How many bytes following */
        unsigned short int	RWordData[1];  /* word data, big endian */
}__attribute__ ((packed))       MBT_F3_RES;

/************* MBT Function 4, Read Analong inputs and registers ****************************/
typedef struct MBT_F4_REQ
{
        unsigned char           fCode;  /* Function code, will be hardcode to 4 */
        unsigned short int      wRIRoffset; /* word 16 bits offset in input memory image and registers, big-endian */
        unsigned short int      RWordCount;  /* number of words to read, big-endian */
}__attribute__ ((packed))       MBT_F4_REQ;
/* Response PDU of FUNC 4 is same as FUNC 3 */
typedef struct MBT_F3_RES       MBT_F4_RES;

/************* MBT Function 5, Set single digital output ************************************/
typedef struct MBT_F5_REQ
{
        unsigned char           fCode;  /* Function code, will be hardcode to 5 */
        unsigned short int      bWOoffset; /* bits offset in output memory image, big-endian */
        unsigned short int      Wworddata;  /* must be either on(0xFF00) or off(0x0000), big-endian */
}__attribute__ ((packed))       MBT_F5_REQ;
/* Response PDU for FUNC 5 is exact same as query */
typedef struct MBT_F5_REQ	MBT_F5_RES;

/************* MBT Function 6, Set single analog output *************************************/
typedef struct MBT_F6_REQ
{
        unsigned char           fCode;  /* Function code, will be hardcode to 6 */
        unsigned short int      wWORoffset; /* word offset in output memory image and register, big-endian */
        unsigned short int      Wworddata;  /* big-endian */
}__attribute__ ((packed))       MBT_F6_REQ;
/* Response PDU for FUNC 6 is exact same as query */
typedef struct MBT_F6_REQ	MBT_F6_RES;

/************* MBT Function 8, Diagnostics **************************************************/
typedef struct MBT_F8_REQ
{
        unsigned char           fCode;  /* Function code, will be hardcode to 8 */
        unsigned short int      subFunction; /* sub-function code, big-endian */
        unsigned short int      Dworddata;  /* big-endian, mostly 0 */
}__attribute__ ((packed))       MBT_F8_REQ;
/* Response PDU for FUNC 8 is exact same as query */
typedef struct MBT_F8_REQ	MBT_F8_RES;

/* MBT Function 15, set number of digital outputs */
typedef struct MBT_F15_REQ
{
        unsigned char           fCode;  /* Function code, will be hardcode to 15 */
        unsigned short int      bWOoffset; /* bit offset in output memory image, big-endian */
        unsigned short int      WBitCount;  /* number of bits to write, big-endian */
        unsigned char		WByteCount; /* bytes of data following, must be (WBitCount+7)/8 */
        unsigned char           WByteData[1];  /*bytes to write */
}__attribute__ ((packed))       MBT_F15_REQ;

typedef struct MBT_F15_RES
{
        unsigned char           fCode;  /* Function code, must be 15 */
        unsigned short int      bWOoffset; /* bit offset in output memory image, big-endian */
        unsigned short int      WBitCount;  /* number of bits to write, big-endian */
}__attribute__ ((packed))       MBT_F15_RES;

/* MBT Function 16, Write Analong outputs and registers */
typedef struct MBT_F16_REQ
{
        unsigned char           fCode;  /* Function code, will be hardcode to 16 */
        unsigned short int      wWORoffset; /* word offset in output image and registers, big-endian */
        unsigned short int      WWordCount;  /* number of words to write, big-endian */
        unsigned char		WByteCount; /* bytes of data following, must be WWordCount*2 */
        unsigned short int	WWordData[1];  /* words to write, big-endian */
}__attribute__ ((packed))       MBT_F16_REQ;

typedef struct MBT_F16_RES
{
        unsigned char           fCode;  /* Function code, must be 16 */
        unsigned short int      wWORoffset; /* word offset in output image and registers, big-endian */
        unsigned short int      WWordCount;  /* number of words to write, big-endian */
}__attribute__ ((packed))       MBT_F16_RES;

/* MBT Function 23, Read and Write Analong inputs and outputs and registers */
typedef struct MBT_F23_REQ
{
        unsigned char           fCode;  /* Function code, will be hardcode to 23 */
        unsigned short int      wRIRoffset; /* word offset in input memory image and registers, big-endian */
        unsigned short int      RWordCount;  /* number of words to read, big-endian */
        unsigned short int      wWORoffset; /* word offset in output memory image and registers, big-endian */
        unsigned short int      WWordCount;  /* number of words to write, big-endian */
	unsigned char		WByteCount;	 /* number of bytes to write, must be WWordCount*2 */
	unsigned short int	WWordData[1];/* beginning of data */
}__attribute__ ((packed))       MBT_F23_REQ;
/* Response PDU for FUNC 23 is same as FUNC 3 */
typedef struct MBT_F3_RES       MBT_F23_RES;


#ifdef __cplusplus
}
#endif  /* __cplusplus */


/***************************************************************************************************/

#endif
