/*
 Name:		Hisab.ino
 Created:	12/23/2023 11:56:35 PM
 Author:	userwindows
*/

// the setup function runs once when you press reset or power the board
#include <HisabFalak.h>
Hisab hisab;

void setup() {
	hisab.setDateTime(26, 12, 2023, 8, 25, 0); //set waktu tanggal 26 Desember 2023 jam 08:25:00
	
	uint8_t ihtiyat[] = {2,2,2,2,2,2};	//Ihtiyat subuh, dluha, dhuhur, ashar, maghrib, isya
	hisab.setIhtiyat(ihtiyat);
	hisab.setLocation(-(7 + 42.0 / 60.0 + 17.34/3600.0), (112+49.0/60.0 + 43.04/3600.0), 64.0f, 7);
			//set lokasi : -7°42'17.34" LS, 112°49'43.04" BT, TZ=GMT+7
	double inkhifad[] = { -18.0, -20.0 };  //Inkhifad syams saat Isya' & Subuh
	hisab.setInkhifad(inkhifad);
	hisab.hitung_sholatT();
	char  BuffTime[10];

	for (uint8_t i = 0;i < hisab.TimesCount;i++) {
		uint8_t shour, sminute, ssecond;
		hisab.get_float_time_parts(hisab.sholatT[i], shour, sminute, ssecond);
		sprintf(BuffTime, "%02d:%02d:%02d", shour, sminute, ssecond);
		Serial.print(hisab.sholatN[i]);Serial.print(" : ");Serial.print(BuffTime);
		Serial.print(" / ");Serial.print(hisab.sholatTSec[i]);
		Serial.print(" / ");Serial.println(hisab.sholatT[i],6);
	}
	hisab.msh_to_hjr();

}

// the loop function runs over and over again until power down or reset
void loop() {
}
