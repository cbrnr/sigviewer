/*
% $Id: biosig.h,v 1.3 2008-04-08 13:47:25 waldesel Exp $
% Copyright (C) 2005,2006,2007,2008 Alois Schloegl <a.schloegl@ieee.org>
% This file is part of the "BioSig for C/C++" repository 
% (biosig4c++) at http://biosig.sf.net/ 


    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 3
    of the License, or (at your option) any later version.

 */

/* External API definitions */

/****************************************************************************/
/**                                                                        **/
/**                 DEFINITIONS, TYPEDEFS AND MACROS                       **/
/**                                                                        **/
/****************************************************************************/

#ifndef __BIOSIG_EXT_H__
#define __BIOSIG_EXT_H__



#ifdef _VCPP_DEF
#define __BYTE_ORDER  __LITTLE_ENDIAN
typedef unsigned __int64	uint64_t;
typedef __int64			int64_t;
typedef unsigned long		uint32_t;
typedef long			int32_t;
typedef unsigned short		uint16_t;
typedef short			int16_t;
typedef unsigned char		uint8_t;
typedef char			int8_t;

#else
#include <inttypes.h>

#endif

/* 
	Including ZLIB enables reading gzipped files (they are decompressed on-the-fly)  
	The output files can be are zipped, too. 
 */
#include <zlib.h>
//#include <bz2lib.h>

#include <stdio.h>


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	biosig_data_type    data type of  internal data format 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
typedef double biosig_data_type;


/****************************************************************************/
/**                                                                        **/
/**                CONSTANTS and Global variables                          **/
/**                                                                        **/
/****************************************************************************/


/* for error handling */ 
enum B4C_ERROR {
	B4C_NO_ERROR,
	B4C_FORMAT_UNKNOWN,
	B4C_FORMAT_UNSUPPORTED,
	B4C_CANNOT_OPEN_FILE,
	B4C_CANNOT_WRITE_FILE,
	B4C_INSUFFICIENT_MEMORY,
	B4C_ENDIAN_PROBLEM,
	B4C_CRC_ERROR,
	B4C_DATATYPE_UNSUPPORTED,
	B4C_SCLOSE_FAILED,
	B4C_DECOMPRESSION_FAILED,
	B4C_UNSPECIFIC_ERROR,
};


	/* list of file formats */
enum FileFormat {
	unknown, 
	ABF, ACQ, ACR_NEMA, AIFC, AIFF, AINF, alpha, AU, ASF, AVI,
	BKR, BCI2000, BDF, BMP, BrainVision, BZ2, 
	CDF, CFWB, CNT, 
	DICOM, DEMG, EDF, EEProbe, EGI, ETG4000, EVENT, EXIF, 
	FAMOS, FEF, FITS, FLAC, GDF, GDF1,
	GIF, GZIP, HL7aECG, JPEG, 
	Matlab, MFER, MIDI, NetCDF, NEX1, OGG, 
	PBMA, PBMN, PDF, PGMA, PGMB, PLEXON, PNG, PNM, POLY5, PPMA, PPMB, PS, 
	RIFF, SCP_ECG, SIGIF, SMA, SND, SVG, SXI,    
	TIFF, VRML, VTK, WAV, WMF, XML, XPM,
	Z, ZIP, ZIP2
};


extern int   B4C_ERRNUM;
extern const char *B4C_ERRMSG;
extern int   VERBOSE_LEVEL; 



/****************************************************************************/
/**                                                                        **/
/**                 DEFINITIONS, TYPEDEFS AND MACROS                       **/
/**                                                                        **/
/****************************************************************************/



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	Conversion of time formats between Unix and GDF format.

	The default time format in BIOSIG uses a 64-bit fixed point format with 
	reference date 01-Jan-0000 00h00m00s (value=0). 
	One unit indicates the 2^(-32) part of 1 day (ca 20 us). Accordingly, 
	the higher 32 bits count the number of days, the lower 32 bits describe 
	the fraction of a day.  01-Jan-1970 is the day 719529. 

      	time_t t0; 
      	t0 = time(NULL); 
      	T0 = (double)t0/86400.0;	// convert seconds in days since 1970-Jan-01
      	floor(T0) + 719529;		// number of days since 01-Jan-0000
      	floor(ldexp(T0-floor(T0),32));  // fraction x/2^32; one day is 2^32 
      	
      	The following macros define the conversions between the unix time and the 
      	GDF format. 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
typedef int64_t 		gdf_time; /* gdf time is represented in 64 bits */
#define t_time2gdf_time(t)	((gdf_time)round(ldexp(((double)(t))/86400.0 + 719529.0, 32)))
/* #define t_time2gdf_time(t)	((gdf_time)floor(ldexp(difftime(t,0)/86400.0 + 719529.0, 32)))  */
#define gdf_time2t_time(t)	((time_t)((ldexp((double)(t),-32) - 719529) * 86400))
#define tm_time2gdf_time(t) 	t_time2gdf_time(mktime(t))
/* #define gdf_time2tm_time(t)        gmtime(gdf_time2t_time(t)) */
#define	ntp_time2gdf_time(t)	((gdf_time)ldexp(ldexp(((double)(t)),-32)/86400 + 719529.0 - 70,32))
#define	gdf_time2ntp_time(t)	((int64_t)ldexp((ldexp(((double)(t)),-32) - 719529.0 + 70) * 86400,32))


/****************************************************************************/
/**                                                                        **/
/**                     TYPEDEFS AND STRUCTURES                            **/
/**                                                                        **/
/****************************************************************************/

/*
	This structure defines the header for each channel (variable header) 
 */
#define MAX_LENGTH_LABEL 	40 
#define MAX_LENGTH_TRANSDUCER 	80
#define MAX_LENGTH_PHYSDIM 	20
#define MAX_LENGTH_PID	 	80      // length of Patient ID: MFER<65, GDF<67, EDF/BDF<81, etc. 
#define MAX_LENGTH_RID		80	// length of Recording ID: EDF,GDF,BDF<80, HL7 ?  	
#define MAX_LENGTH_NAME 	128	// max length of personal name: MFER<=128
#define MAX_LENGTH_MANUF 	128	// max length of manufacturer field: MFER<128
typedef struct {
	char		OnOff; 		
	char		Label[MAX_LENGTH_LABEL+1]; 	/* Label of channel */
	uint16_t	LeadIdCode;	/* Lead identification code */ 
	char 		Transducer[MAX_LENGTH_TRANSDUCER+1];	/* transducer e.g. EEG: Ag-AgCl electrodes */
	char 		PhysDim[MAX_LENGTH_PHYSDIM+1];	/* physical dimension */
	uint16_t	PhysDimCode;	/* code for physical dimension */
	/* char* 	PreFilt;	// pre-filtering */

	float 		LowPass;	/* lowpass filter */
	float 		HighPass;	/* high pass */
	float 		Notch;		/* notch filter */
	float 		XYZ[3];		/* electrode position */
	float 		Impedance;   	/* in Ohm */
	
	double 		PhysMin;	/* physical minimum */
	double 		PhysMax;	/* physical maximum */
	double 		DigMin;		/* digital minimum */
	double	 	DigMax;		/* digital maximum */

	uint16_t 	GDFTYP;		/* data type */
	uint32_t 	SPR;		/* samples per record (block) */
	
	double		Cal;		/* gain factor */ 
	double		Off;		/* bias */ 
} CHANNEL_TYPE;


/*
	This structure defines the general (fixed) header  
*/
typedef struct {
	enum FileFormat TYPE; 		/* type of file format */
	float 		VERSION;	/* GDF version number */ 
	const char* 	FileName;
	
	struct {
		size_t 			size[2]; /* size {rows, columns} of data block	 */
		biosig_data_type* 	block; 	 /* data block */
	} data;

	uint32_t 	HeadLen;	/* length of header in bytes */
	uint16_t 	NS;		/* number of channels */
	uint32_t 	SPR;		/* samples per block (when different sampling rates are used, this is the LCM(CHANNEL[..].SPR) */
	uint64_t  	NRec;		/* number of records/blocks -1 indicates length is unknown. */	
	uint32_t 	Dur[2];		/* Duration of each block in seconds expressed in the fraction Dur[0]/Dur[1]  */
	double 		SampleRate;	/* Sampling rate */
	uint8_t 	IPaddr[6]; 	/* IP address of recording device (if applicable) */
	uint32_t  	LOC[4];		/* location of recording according to RFC1876 */
	gdf_time 	T0; 		/* starttime of recording */
	int16_t 	tzmin; 		/* time zone (minutes of difference to UTC */

	/* Patient specific information */
	struct {
		char		Name[MAX_LENGTH_NAME+1]; /* because for privacy protection it is by default not supported, support is turned on with FLAG.ANONYMOUS */
//		char*		Name;	/* because for privacy protection it is by default not supported, support is turned on with FLAG.ANONYMOUS */
		char		Id[MAX_LENGTH_PID+1];	/* patient identification, identification code as used in hospital  */
		uint8_t		Weight;		/* weight in kilograms [kg] 0:unkown, 255: overflow  */
		uint8_t		Height;		/* height in centimeter [cm] 0:unkown, 255: overflow  */
		gdf_time 	Birthday; 	/* Birthday of Patient */
		uint16_t	Headsize[3]; 	/* circumference, nasion-inion, left-right mastoid in millimeter;  */
		/* Patient classification */
		int	 	Sex;		/* 0:Unknown, 1: Male, 2: Female  */
		int		Handedness;	/* 0:Unknown, 1: Right, 2: Left, 3: Equal */
		int		Smoking;	/* 0:Unknown, 1: NO, 2: YES */
		int		AlcoholAbuse;	/* 0:Unknown, 1: NO, 2: YES */
		int		DrugAbuse;	/* 0:Unknown, 1: NO, 2: YES */
		int		Medication;	/* 0:Unknown, 1: NO, 2: YES */
		struct {
			int 	Visual;		/* 0:Unknown, 1: NO, 2: YES, 3: Corrected */
		} Impairment;
		
	} Patient; 
	
	struct {
		char		Recording[MAX_LENGTH_RID+1]; 	/* HL7, EDF, GDF, BDF replaces HDR.AS.RID */
		char* 		Technician; 	
		char* 		Hospital; 	
		uint64_t 	Equipment; 	/* identifies this software */
		struct {
			/* see 
				SCP: section1, tag14, 
				MFER: tag23:  "Manufacturer^model^version number^serial number"
			*/	
			char	_field[MAX_LENGTH_MANUF+1];	/* buffer */
			char*	Name;  
			char*	Model;
			char*	Version;
			char*	SerialNumber;
		} Manufacturer;  
		char 		EquipmentManufacturer[20];
		char		EquipmentModel[20]; 
		char		EquipmentSerialNumber[20];
	} ID;

	/* position of electrodes; see also HDR.CHANNEL[k].XYZ */
	struct {
		float		REF[3];	/* XYZ position of reference electrode */
		float		GND[3];	/* XYZ position of ground electrode */
	} ELEC;

	/*	EVENTTABLE */
	struct 
	{
		double  	SampleRate;	/* for converting POS and DUR into seconds  */
		uint32_t  	N;	/* number of events */
		uint16_t 	*TYP;	/* defined at http://cvs.sourceforge.net/viewcvs.py/biosig/biosig/t200/eventcodes.txt?view=markup */
		uint32_t 	*POS;	/* starting position [in samples] */
		uint32_t 	*DUR;	/* duration [in samples] */
		uint16_t 	*CHN;	/* channel number; 0: all channels  */
	} EVENT; 

	struct {	/* flags */
		char		OVERFLOWDETECTION; 	/* overflow & saturation detection 0: OFF, !=0 ON */
		char		UCAL; 		/* UnCalibration  0: scaling  !=0: NO scaling - raw data return  */
		char		ANONYMOUS; 	/* 1: anonymous mode, no personal names are processed */ 
		char		SWAP; 	        /* 1: endian swapping is needed */ 
	} FLAG; 

	struct {	/* File specific data  */
#ifdef ZLIB_H
		gzFile		gzFID;
#endif
#ifdef _BZLIB_H
//		BZFILE*		bzFID;
#endif
		FILE* 		FID;		/* file handle  */
		size_t 		POS;		/* current reading/writing position [in blocks] */
		uint8_t		OPEN; 		/* 0: closed, 1:read, 2: write */
		uint8_t		LittleEndian;
		uint8_t		COMPRESSION;   /* 0: no compression 9: best compression */
	} FILE; 

	/*	internal variables (not public)  */
	struct {
//		char 		PID[MAX_LENGTH_PID+1];	/* use HDR.Patient.Id instead */
//		char* 		RID;		/* recording identification */ 
		uint32_t 	spb;		/* total samples per block */
		uint32_t 	bpb;  		/* total bytes per block */
		uint32_t 	*bi;
		uint8_t*	Header; 
		uint8_t*	rawdata; 	/* raw data block */
	} AS;
	
	CHANNEL_TYPE *CHANNEL;  
	void *aECG;
	
} HDRTYPE;



/****************************************************************************/
/**                                                                        **/
/**                     EXPORTED FUNCTIONS                                 **/
/**                                                                        **/
/****************************************************************************/

HDRTYPE* nullhdr();
HDRTYPE* getfiletype(HDRTYPE* hdr);
HDRTYPE* create_default_hdr(const unsigned NS, const unsigned N_EVENT);
HDRTYPE* sopen(const char* FileName, const char* MODE, HDRTYPE* hdr);
int 	sclose(HDRTYPE* hdr);
size_t	sread(biosig_data_type* data, size_t start, size_t length, HDRTYPE* hdr);
size_t  swrite(const biosig_data_type *data, size_t nelem, HDRTYPE* hdr);
int	seof(HDRTYPE* hdr);
void	srewind(HDRTYPE* hdr);
int 	sseek(HDRTYPE* hdr, long int offset, int whence);
long int stell(HDRTYPE* hdr);
int 	serror();
int 	sflush_gdf_event_table(HDRTYPE* hdr);
int	hdr2ascii(HDRTYPE* hdr,FILE *fid, int verbosity);

/****************************************************************************/
/**                                                                        **/
/**                               EOF                                      **/
/**                                                                        **/
/****************************************************************************/

#endif	/* __BIOSIG_EXT_H__ */
