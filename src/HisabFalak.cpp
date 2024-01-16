/*
 Name:		HisabFalak.cpp
 Created:	12/23/2023 11:42:02 PM
 Author:	userwindows
 Editor:	http://www.visualmicro.com
*/

#include "HisabFalak.h"
#include <math.h>
#include <stdio.h>
// Time IDs
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif
#define INT(value) (value >= 0 ? (long)(value) : (long)(value-1))

// Trigonometry using degrees
#define SIND(value)   sin((value)*0.01745329251994329576923690768489)
#define COSD(value)   cos((value)*0.01745329251994329576923690768489)
#define TAND(value)   tan((value)*0.01745329251994329576923690768489)
#define ASIND(value)  (asin(value)*57.295779513082320876798154814105)
#define ACOSD(value)  (acos(value)*57.295779513082320876798154814105)
#define ATAND(value)  (atan(value)*57.295779513082320876798154814105)

//==============================================
//Fungsi Trigonometri============================
/*
double SIND(float nilai) {
	return (float)sin(nilai / 180 * PI);
}
double COSD(float nilai) {
	return (float)cos(nilai / 180 * PI);
}
double TAND(float nilai) {
	return (float)tan(nilai / 180 * PI);
}
double ASIND(float nilai) {
	return (float)asin(nilai) / PI * 180;
}
double ACOSD(float nilai) {
	return (float)acos(nilai) / PI * 180;
}
double ATAND(float nilai) {
	return (float)atan(nilai) / PI * 180;
}
*/
//===================================
static double fix_hour(double a) {
	return fmod(a, 24.0);
}
char Hisab::sholatN[][9]={ "IMSAK\0", "SUBUH\0", "SYURUQ", "DLUHA\0", "DHUHUR", "ASHAR\0", "MAGRIB", "ISYA\0\0", "IMSAK\0" };
void Hisab::get_float_time_parts(double time, uint8_t& hours, uint8_t& minutes, uint8_t& seconds) {
	hours = floor(time);
	minutes = floor((time - hours) * 60);
	seconds = floor(((time - hours) * 60 - minutes) * 60);
}
void Hisab::get_float_time_parts(double time, uint8_t& hours, uint8_t& minutes) {
	hours = floor(time);
	minutes = floor((time - hours) * 60);
}
//==============================================
Hisab::Hisab() {
	uint8_t defIhtiyat[] = { 2,2,2,2,2,2 };
	setLocation(-(7 + 15.0 / 60.0), (112 + 56.0 / 60.0), 10.0f, 7); //set default location
	setIhtiyat(defIhtiyat); //set default ihtiyat
	setDateTime(26, 12, 2023, 0, 0, 0); //set default waktu
	//Serial.begin(115200);
}
void Hisab::setLocation(double _latitude, double _longitude, float _altitude, uint8_t _timezone) {
	Latitude = _latitude;
	Longitude = _longitude;
	Altitude = _altitude;
	TimeZone = _timezone;
}
void Hisab::setIhtiyat(uint8_t _ihtiyat[]) {
	for (byte i = 0;i < 6;i++) ihtiyat[i] = _ihtiyat[i];
}

void Hisab::setDateTime(uint8_t date, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t second) {
	tglM = date;
	blnM = month;
	thnM = year;
	jam = hour;
	menit = minute;
	detik = second;
	JDBase = get_julian_date(thnM, blnM, tglM);
	//msh_to_hjr();
}

void Hisab::setInkhifad(double _inkhifad[]) {
	inkhifad[0] = _inkhifad[0];
	inkhifad[1] = _inkhifad[1];
}
/* compute mid-day (Dhuhr, Zawal) time */
double Hisab::compute_mid_day(double _t){
	double t = Data_Matahari(JDBase + _t - TimeZone/24.0, KWD);
	sholatT[Dhuhr] = fmod(12 - t, 24);
	return sholatT[Dhuhr];
}

double Hisab::compute_time(double angle, double _t) {
	double JD = JDBase + _t - TimeZone / 24.0;
	
	double decl, dft, SD;
	Data_Matahari(JD, decl, dft, SD);

	double kwd = dft + (double)(Longitude - TimeZone * 15.0f) / 15.0f;;
	//double Eq = Data_Matahari(JD, EoT);
	
	//double z = compute_mid_day(sholatT[Dhuhr]);
	double F = -TAND(Latitude) * TAND(decl);
	double G = COSD(Latitude) * COSD(decl);
	double w_istiwa = ACOSD(F + SIND(angle) / G)/15;
	
	double hasil = 12 + (angle > 90.0 ? -w_istiwa : w_istiwa);
	hasil -= kwd;
	return hasil;
}

void Hisab::day_portion(){
	for (int i = 0; i < TimesCount; ++i) sholatT[i] /= 24.0;
}

double Hisab::Data_Matahari(double JD, byte type) {
	double hasil;
	double T = (JD - 2451545.0) / 36525.0;
	double S = fmod((280.46645f + 36000.76983f * T), 360);
	double M = fmod((357.5291f + 35999.0503f * T), 360);
	double N = fmod((125.04 - 1934.136 * T), 360);
	double K[2], R[2];
	K[0] = (17.264 / 3600) * SIND(N) + (0.206 / 3600) * SIND(2 * N);
	K[1] = (-1.264 / 3600) * SIND(2 * S);
	R[0] = (9.23 / 3600) * COSD(N) - (0.09 / 3600) * COSD(2 * N);
	R[1] = (0.548 / 3600) * COSD(2 * S);
	double Qi = 23.439291111 + R[0] + R[1] - (46.8150 / 3600.0) * T;
	double E = (6898.06 / 3600) * SIND(M) + (72.095 / 3600.0) * SIND(2 * M) + (0.966 / 3600.0) * SIND(3 * M);
	double Si = fmod(S + E + K[0] + K[1] - 20.47 / 3600.0, 360);
	//DATA MATAHARI
	switch (type) {
		case MailS: hasil = ASIND(SIND(Si) * SIND(Qi)); break;
		case EoT: hasil = (-1.915 * SIND(M) + -0.02 * SIND(2 * M) + 2.466f * SIND(2 * Si) - 0.053f * SIND(4 * Si)) / 15.0;break;
		case SDM: hasil = 0.26f / (1 - 0.017 * COSD(M)); break;
		case KWD: hasil = (-1.915 * SIND(M) + -0.02 * SIND(2 * M) + 2.466f * SIND(2 * Si) - 0.053f * SIND(4 * Si)) / 15.0 + 
							(double)(Longitude - TimeZone * 15.0f) / 15.0f; break;//Selisih Wis-WIB
	}
	return hasil;
}

void Hisab::Data_Matahari(double JD, double& decl, double& dft, double& SD) {
	double T = (JD - 2451545.0) / 36525.0;
	double S = fmod((280.46645f + 36000.76983f * T), 360);
	double M = fmod((357.5291f + 35999.0503f * T), 360);
	double N = fmod((125.04 - 1934.136 * T), 360);
	double K[2], R[2];
	K[0] = (17.264 / 3600) * SIND(N) + (0.206 / 3600) * SIND(2 * N);
	K[1] = (-1.264 / 3600) * SIND(2 * S);
	R[0] = (9.23 / 3600) * COSD(N) - (0.09 / 3600) * COSD(2 * N);
	R[1] = (0.548 / 3600) * COSD(2 * S);
	double Qi = 23.439291111 + R[0] + R[1] - (46.8150 / 3600.0) * T;
	double E = (6898.06 / 3600) * SIND(M) + (72.095 / 3600.0) * SIND(2 * M) + (0.966 / 3600.0) * SIND(3 * M);
	double Si = fmod(S + E + K[0] + K[1] - 20.47 / 3600.0, 360);
	//DATA MATAHARI
	decl = ASIND(SIND(Si) * SIND(Qi));
	dft = (-1.915 * SIND(M) + -0.02 * SIND(2 * M) + 2.466f * SIND(2 * Si) - 0.053f * SIND(4 * Si)) / 15.0;
	SD = 0.26f / (1 - 0.017 * COSD(M));

}

double Hisab::get_julian_date(uint16_t year, uint8_t month, uint8_t day) {
	if (month < 3) {
		year -= 1;
		month += 12;
	}
	long A = floor(year / 100.0);
	long B = 2 - A + floor(A / 4);
	return floor(365.25 * (year + 4716)) + floor(30.6001 * (month + 1)) + day + B - 1524.5;
}
double Hisab::getKWD() {
	double JD = JDBase - TimeZone / 24.0 + jam / 24.0 + menit / 60.0 / 24.0 + detik / 3600.0 / 24.0;
	return Data_Matahari(JD, KWD);
}
void Hisab::hitung_sholatT() {
	uint8_t iter = 2;
	if (sholatT[Maghrib] > 0.0) iter = 1;
	for (uint8_t u = 0;u < iter;u++) {
		double h[3], Dip;
		Dip = 1.76 / 60.0 * sqrt(Altitude);
		//h[0] = (float)-(Data_Matahari(JDBase + sholatT[Thulu], SDM) + 34.5 / 60.0 + Dip + 0.0024);  //Thulu'
		//h[1] = (float)-(Data_Matahari(JDBase + sholatT[Maghrib], SDM) + 34.5 / 60.0 + Dip + 0.0024);  //Maghrib
		h[0] = (float)-(Data_Matahari(JDBase + sholatT[Thulu]/24.0f, SDM) + 34.5 / 60.0 + Dip + 0.0024);  //Thulu'
		h[1] = (float)-(Data_Matahari(JDBase + sholatT[Maghrib] / 24.0f, SDM) + 34.5 / 60.0 + Dip + 0.0024);  //Maghrib
		h[2] = (double)ATAND((1 / (TAND(fabs(Latitude - Data_Matahari(JDBase + sholatT[Ashr] / 24.0f, MailS))) + 1)));
		day_portion();

		sholatT[Shubuh] = compute_time(180.0 - inkhifad[1], sholatT[Shubuh]);
		sholatT[Thulu]  = compute_time(180.0 - h[0], sholatT[Thulu]);
		sholatT[Dluha]  = compute_time(180.0 - 4.50, sholatT[Dluha]);
						  compute_mid_day(sholatT[Dhuhr]);
		sholatT[Ashr]   = compute_time(h[2], sholatT[Ashr]);
		sholatT[Maghrib] = compute_time(h[1], sholatT[Maghrib]);
		sholatT[Isya]   = compute_time(inkhifad[0], sholatT[Isya]);
		sholatT[Imsak]  = sholatT[Shubuh] - 10.0f / 60.0f;
	}
	sholatT[Shubuh] += ihtiyat[0]/60.0;
	sholatT[Imsak] = sholatT[Shubuh] - 10.0f / 60.0f;
	sholatT[Thulu] -= ihtiyat[4] / 60.0;
	sholatT[Dluha] += ihtiyat[1] / 60.0;
	sholatT[Dhuhr] += ihtiyat[2] / 60.0;
	sholatT[Ashr] += ihtiyat[3] / 60.0;
	sholatT[Maghrib] += ihtiyat[4] / 60.0;
	sholatT[Isya] += ihtiyat[5] / 60.0;
	for (uint8_t i = 0;i < TimesCount;i++) sholatTSec[i] = sholatT[i] * 3600;
}



byte Hisab::Hisab_Awal_Bulan(byte bulan, int tahun) {
	byte HariAwal;
	if (bulan - 1 == 0) {
		bulan = 12;tahun -= 1;
	}
	else {
		bulan -= 1;
	}
	float Dip = 1.76f / 60.0f * sqrt(Altitude);
	float HY = tahun + (bulan * 29.53) / 354.3671;
	float K = floor(0.5 + (HY - 1410) * 12.0);
	float JA = K / 1200.00;
	double JD = 2447740.652 + (double)(29.53058868 * K) + (double)(0.0001178 * JA * JA);

	double M = fmod(207.9587074f + 29.10535608f * K - 0.00003333f * JA * JA, 360);
	double Mi = fmod(111.1791307f + 385.81691806f * K + 0.0107306f * JA * JA, 360);
	double F = fmod(164.2162296f + 390.67050646f * K - 0.0016528f * JA * JA, 360);
	double te = (0.1734f - 0.000395f * JA) * SIND(M) + 0.0021f * SIND(2 * M) - 0.4068f * SIND(Mi) + 
		0.0161f * SIND(2.0 * Mi) - 0.0004f * SIND(3 * Mi) + 0.0104f * SIND(2.0f * F) - 0.0051f * SIND(M + Mi) - 
		0.0074f * SIND(M - Mi) + 0.0004 * SIND(2.0f * F + M) - 0.0004f * SIND(2.0f * F - M) - 
		0.0006f * SIND(2.0 * F + Mi) + 0.001 * SIND(2.0f * F - Mi) + 0.0005 * SIND(M + 2.0f * Mi);
	double JD_Ijt = JD + 0.5f + te + (TimeZone / 24.0f);
	double WI_WD = (JD_Ijt - floor(JD_Ijt)) * 24.0f;
	double MailS, EoT, SDM;
	//double T = (float)(JD_Ijt - 2451545.0) / 36525.0f;

	Data_Matahari(JD_Ijt, MailS, EoT, SDM);
	float H = -(SDM + 0.29333f + Dip);
	float Maghrib_Istiwa = ACOSD(-TAND(Latitude)*TAND(MailS)+SIND(H)/COSD(Latitude)/COSD(MailS))/15.0f;
	float Maghrib_LMT = Maghrib_Istiwa + 12.0 - EoT;
	float Maghrib_WD = Maghrib_LMT + ((TimeZone * 15) - Longitude) / 15.0;

	//Cari Hari Rukyah
	byte KorH = (WI_WD > Maghrib_WD && WI_WD < 24) ? 1 : 0;
	if (WI_WD > Maghrib_WD && WI_WD < 24 || (Maghrib_WD - WI_WD) < 8)
		KorH = 2;
	else KorH = 1;
	HariAwal = fmod((floor(JD_Ijt) + KorH + 2), 7);
	return HariAwal;
}
void Hisab::msh_to_hjr() {
	//Konversi Masehi-Hijri sederhana
	double AH = JDBase - 1721423 - 227016+TimeZone/24.0f;
	
	byte ToUrfi, SelUrfi, UpgreadeUrfi, HariHij, HariTahqiqi = 0, HariTahqiqiNext = 0;
	pasaran = fmod(JDBase+0.5 + 1, 5);
	hari = fmod(JDBase+0.5f + 2, 7);
	
	HariHij = 0;
	float floatnow = jam + menit / 60.0f + detik / 3600.0f;
	if (floatnow >= sholatT[Maghrib]) {
		HariHij++;AH++;
	}
	HariHij += fmod(hari, 7);
	thnH = floor(AH / 354.3671f) + 1;
	blnH = floor(fmod(AH, 354.3671f) / 29.50f) + 1;
	tglH = floor(fmod(fmod(AH, 354.3671f), 29.50f) + 1);
	
	HariTahqiqi = Hisab_Awal_Bulan(blnH, thnH);         //Proses hitung hari awal bulan berdasarkan ijtima' & hilal di file Hisab.ino
	if(tglH>28) HariTahqiqiNext = Hisab_Awal_Bulan(blnH + 1, thnH);   //Proses hitung hari awal bulan selanjutnya berdasarkan ijtima' & hilal di file Hisab.ino


	ToUrfi = (int)fmod((HariTahqiqi + (tglH - 1)), 7); //To Urfi = hari yang sebenarnya berdasarkan ijtima' (ketika tanggal hasil konversi)

	if (ToUrfi != HariHij) { //cek selisih
		if (ToUrfi > 5 && HariHij < 3) HariHij = (int)(HariHij + 7);
		if (ToUrfi < 3 && HariHij > 5) ToUrfi = (int)(ToUrfi + 7);

		SelUrfi = (int)(HariHij - ToUrfi);
		SelUrfi -= SelUrfi > 4 ? 7 : 0;
		SelUrfi += SelUrfi < -4 ? 7 : 0;

		UpgreadeUrfi = (int)fmod(HariHij + SelUrfi, 7);
		UpgreadeUrfi += UpgreadeUrfi == 0 ? 7 : 0;

		if (ToUrfi == UpgreadeUrfi) SelUrfi = (int)SelUrfi;

		// Koreksi hari===============
		tglH += SelUrfi;
		if (tglH < 1) {
			tglH = 30;
			blnH -= 1;
			if (blnH < 1) {
				blnH += 12;
				thnH -= 1;
			}
		}
	}
	if (tglH == 31 || tglH == 30 && fmod(HariTahqiqi, 7) == fmod(HariTahqiqiNext - 1, 7)) {//usia 29
		tglH = 1;
		blnH += 1;
		if (blnH > 12) {
			blnH -= 12;
			thnH += 1;
		}
	}
	//char  BuffTgl[20];
	//sprintf(BuffTgl, "Tanggal: %02d-%02d-%04d", tglH, blnH, thnH);
	//Serial.println(BuffTgl);
}