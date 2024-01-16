/*
 Name:		HisabFalak.h
 Created:	12/23/2023 11:42:02 PM
 Author:	userwindows
 Editor:	LFNU Kab. Pasuruan
*/

#ifndef _HisabFalak_h
#define _HisabFalak_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class Hisab {
public:Hisab();
	  enum TimeID {
		  Imsak, Shubuh, Thulu, Dluha, Dhuhr, Ashr, Maghrib, Isya, TimesCount
	  };
	  void setLocation(double _latitude, double _longitude, float _altitude, uint8_t _timezone);
	  void setIhtiyat(uint8_t _ihtiyat[]);
	  void setDateTime(uint8_t date, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t second);
	  void setInkhifad(double inkhifad[]);
	  double sholatT[8];
	  uint32_t sholatTSec[8];
	  void hitung_sholatT();
	  int thnH;
	  byte tglH, blnH, hari, pasaran;
	  void msh_to_hjr();
	  double getKWD();
	  void get_float_time_parts(double time, uint8_t& hours, uint8_t& minutes, uint8_t& seconds);
	  void get_float_time_parts(double time, uint8_t& hours, uint8_t& minutes);
	  char static sholatN[][9];
private:
	enum SunDataType {
		MailS, EoT, SDM, KWD
	};
	double Longitude, Latitude;
	float Altitude;
	uint16_t thnM;
	uint8_t tglM, blnM, jam, menit, detik, TimeZone;
	uint8_t ihtiyat[6];
	double inkhifad[2];
	double compute_time(double angle, double time);
	double Data_Matahari(double JD, uint8_t type);
	void Data_Matahari(double JD, double& decl, double& dft, double& SD);
	byte Hisab_Awal_Bulan(byte bulan, int tahun);
	double get_julian_date(uint16_t year, uint8_t month, uint8_t day);
	double compute_mid_day(double _t);
	double JDBase;
	void day_portion();
};

#endif

