/* - Staffan Piledahl
Driver for LCD Display
*/
#include "display.h"
#include "../rtc.h"
#include <math.h>
#include "../includes/at91sam3x8.h"
#include "../includes/common.h"

#define LONG_DELAY 100
#define SHORT_DELAY 20

/* Initializes PIO for display */
void DISPLAY_init(void){

  /* TODO INLINE FUNCTION GOES HERE enables clock for PIOC & PIOD */
  *AT91C_PMC_PCER = (3<<13);

  /*enables control of pins*/
  *AT91C_PIOC_PER =  (BIT_2_TO_9);

  /* initializes PIOD pin 2 and 0*/
  *AT91C_PIOD_PER = (1<<2)  | 1;
  *AT91C_PIOD_OER = (1<<2)  | 1;
  *AT91C_PIOD_SODR = (1<<2) | 1;

  /* initializes PIO port 2 3 4 5 as input*/
  *AT91C_PIOC_ODR = BIT_2_3_4_5;

    /* initializes PIO pin 12-17 and 18,19 as output*/
  *AT91C_PIOC_OER = BIT_12_TO_17 | BIT_18_19;

  /* Disable displaycontrol as standard */
  *AT91C_PIOC_SODR = (1<<12);

  /* Set areas etc. */
  DISPLAY_setDefaultState();

}

/*Writes to a cell, needs rewrite to take correct x and y
For help see display.h documentation!
*/
void DISPLAY_write(char *text, char x , char y){

  DISPLAY_writeData(x);
  DISPLAY_writeData(y);
  DISPLAY_writeCommand(0x24);//Set text coordinates

  //Go torugh each char in given string and write it using auto increment
  for(int i = 0; *(text + i) != 0x00 ; i++){
    DISPLAY_writeData( (*(text+i)-0x20) );
    DISPLAY_writeCommand(0xC0);
  }

}

/*Home screen that shows current values of everything*/
void DISPLAY_writeHomeScreen(char* temp, char* lux, char* air, char* date){

    DISPLAY_write(date,87,0); //Already includes wrapping string "Date: "

    DISPLAY_write("Temp: ",167,0);
    DISPLAY_write(temp,172,0);
    DISPLAY_write(" Cels",177,0);

    DISPLAY_write("Illu: ",207,0);
    DISPLAY_write(lux,212,0);
    DISPLAY_write(" Lux",218,0);

    DISPLAY_write("Air: ",247,0);
    DISPLAY_write(air,252,0);
    DISPLAY_write(" kPa",0,1);
}

/*Light follower screen, draws an arc and a sun*/
void DISPLAY_writeLightScreen(void){

  //Write out current servo pos
  DISPLAY_write("Sun position ",95,0);
  int reading = SERVO_getPos();
  reading = (reading)/44; //Turn into angle
  char *angle = malloc(12*sizeof(char *));
  if(angle == 0){
    //TODO Handle error
  }
  sprintf(angle, "%d degrees", reading);
  DISPLAY_write(angle,128,0);
  free(angle);

	//Draw suntracker painting.
	DISPLAY_drawArc(145, 135, 70);
	DISPLAY_drawSun(80, 110, 10); //TODO make this take the angle from servo
}

/*Temperature log screen, shows last 7 days worth of temp data as graphs*/
void DISPLAY_writeTempScreen(char* date){
  DISPLAY_write(date,87,0);
  
  //Prepare Graphing
  DISPLAY_drawAxis();
  DISPLAY_write("Tdy",57,2);
  DISPLAY_write("Ytd",61,2);
  DISPLAY_write("40",167,0);
  DISPLAY_write("20",71,1);
  DISPLAY_write("0",16,2);

  //Fetch last 7 days of temp data
  datestamp_t todays_datestamp = mem.temp->date; //TODO get date from RTC
  temp_t *tmp = mem.temp;
  char count = 0;
  while(tmp != NULL && count <7 ){
      DISPLAY_drawTempGraph(tmp, count); //Draw bar graph
      count++;
      tmp = tmp->next;
  }
}
/*Draws the initial set date screen on startup a lot of user input
The algorithm used to determine which entry user is at is done using integer
diviosn and modulu
*/
void DISPLAY_writeDateSetScreen(void){

  //Things needed to be filled in by user
  DISPLAY_write("Cent:              ",88,0);
	DISPLAY_write("Year:              ",128,0);
	DISPLAY_write("Month:              ",168,0);
	DISPLAY_write("Date:               ",208,0);
	DISPLAY_write("                    ",248,0);
  unsigned char date_entries_done = 0;
  unsigned char time_entries_done = 0;
  char cent = 0;
  char year = 0;
  char month = 0;
  char date = 0;
  char hr = 0;
  char sec = 0;
  char min = 0;

  /*Let user enter the 8 numbers needed and print them on screen accordingly
  After each number user must confirm and move to next number using star key
  */
  DISPLAY_write("_",94+((date_entries_done%2)*40),0);
  unsigned char pressed;
  while(date_entries_done < 8){
	  pressed = Keypad_Read();
    if(pressed != 0){

    	switch(pressed){
        case 1:
          DISPLAY_write("1",94+((date_entries_done%2)*40),0); //write the number at correct place
            date_entries_done++;
		    break;
        case 2:
          DISPLAY_write("2",94+((date_entries_done/2)*40)+(date_entries_done%2),0); //write the number at correct place
            date_entries_done++;
		    break;
        case 3:
          DISPLAY_write("3",94+((date_entries_done/2)*40)+(date_entries_done%2),0); //write the number at correct place
            date_entries_done++;
        break;
        case 4:
			   DISPLAY_write("4",94+((date_entries_done/2)*40)+(date_entries_done%2),0); //write the number at correct place
          date_entries_done++;
        break;
        case 5:
			DISPLAY_write("5",94+((date_entries_done/2)*40)+(date_entries_done%2),0); //write the number at correct place
            date_entries_done++;
        break;
        case 6:
			DISPLAY_write("6",94+((date_entries_done/2)*40)+(date_entries_done%2),0); //write the number at correct place
            date_entries_done++;
        break;
        case 7:
			DISPLAY_write("7",94+((date_entries_done/2)*40)+(date_entries_done%2),0); //write the number at correct place
            date_entries_done++;
        break;
        case 8:
			DISPLAY_write("8",94+((date_entries_done/2)*40)+(date_entries_done%2),0); //write the number at correct place
            date_entries_done++;
        break;
        case 9:
			DISPLAY_write("9",94+((date_entries_done/2)*40)+(date_entries_done%2),0); //write the number at correct place
            date_entries_done++;
        break;
        case 11:
			DISPLAY_write("0",94+((date_entries_done/2)*40)+(date_entries_done%2),0); //write the number at correct place
            date_entries_done++;

        break;
    }
/*Now save the number user entered, it is a two digit number hence *10*/
	if(!(pressed == 10 || pressed == 12)){
		if(pressed == 11){pressed = 0;} //Quickfix to make saving easier

		//Find out if its cent year month or date user is entering
		switch((date_entries_done-1)/2){
		  case 0:
			cent = (cent*10)+pressed;
		  break;
		  case 1:
			year = (year*10)+pressed;
		  break;
		  case  2:
			month = (month*10)+pressed;
		  break;
		  case 3:
			date = (date*10)+pressed;
		  break;
		}
	}

  //Press star to move to next item
  while(Keypad_Read() != 10){}
  DISPLAY_write("_",94+((date_entries_done/2)*40)+(date_entries_done%2),0);
	Delay(2000000);
    }
  }

  /*Same procedure for time*/
  DISPLAY_write("Hr:                ",88,0);
  DISPLAY_write("Min:               ",128,0);
  DISPLAY_write("Sec:              ",168,0);
  DISPLAY_write("                   ",208,0);
  DISPLAY_write("                    ",248,0);
  DISPLAY_write("_",94+((time_entries_done/2)*40)+(time_entries_done%2),0);
  while(time_entries_done < 6){
    pressed = Keypad_Read();
      if(pressed != 0){
        switch(pressed){
          case 1:
            DISPLAY_write("2",94+((time_entries_done/2)*40)+(time_entries_done%2),0); //write the number at correct place
              time_entries_done++;
        break;
          case 2:
            DISPLAY_write("2",94+((time_entries_done/2)*40)+(time_entries_done%2),0); //write the number at correct place
              time_entries_done++;
        break;
          case 3:
            DISPLAY_write("3",94+((time_entries_done/2)*40)+(time_entries_done%2),0); //write the number at correct place
              time_entries_done++;
          break;
          case 4:
        DISPLAY_write("4",94+((time_entries_done/2)*40)+(time_entries_done%2),0); //write the number at correct place
              time_entries_done++;
          break;
          case 5:
        DISPLAY_write("5",94+((time_entries_done/2)*40)+(time_entries_done%2),0); //write the number at correct place
              time_entries_done++;
          break;
          case 6:
        DISPLAY_write("6",94+((time_entries_done/2)*40)+(time_entries_done%2),0); //write the number at correct place
              time_entries_done++;
          break;
          case 7:
        DISPLAY_write("7",94+((time_entries_done/2)*40)+(time_entries_done%2),0); //write the number at correct place
              time_entries_done++;
          break;
          case 8:
        DISPLAY_write("8",94+((time_entries_done/2)*40)+(time_entries_done%2),0); //write the number at correct place
              time_entries_done++;
          break;
          case 9:
        DISPLAY_write("9",94+((time_entries_done/2)*40)+(time_entries_done%2),0); //write the number at correct place
              time_entries_done++;
          break;
          case 11:
        DISPLAY_write("0",94+((time_entries_done/2)*40)+(time_entries_done%2),0); //write the number at correct place
              time_entries_done++;
          break;
      }

      if(!(pressed == 10 || pressed == 12)){
        if(pressed == 11){pressed = 0;} //Quickfix to make saving easier
        //Find out if its hr min or sec or date user is entering
        switch((time_entries_done-1)/2){
          case 0:
            hr = (hr*10)+pressed;
          break;
          case 1:
            min = (min*10)+pressed;
          break;
          case  2:
            sec = (sec*10)+pressed;
          break;
        }
      }
      //Press star to move to next item
    while(Keypad_Read() != 10){}
    DISPLAY_write("_",94+((time_entries_done/2)*40)+(time_entries_done%2),0);
    Delay(2000000);
    }
  }
  //And write new date amd time to real time clock
  RTC_Init(sec, min,hr, cent, year, month, date, 1); //TODO do day aswell, hardcoded for now
}

/*Shows the logged air pressure reading as bar graphs*/
void DISPLAY_writeAirScreen(char* date){
  
  //Prepare graphing
  DISPLAY_write(date,98,0);
  DISPLAY_drawAxis();
  DISPLAY_write("Hi",167,0);
  DISPLAY_write("--",71,1);
  DISPLAY_write("Lo",15,2);

  //fetch the last 7 days air pressure and draw it
  DISPLAY_drawAirGraph();
}

/*Draws out the settings screen*/
void DISPLAY_writeSettingsScreen(void){
  
  //First off, the avergaing N value status
  DISPLAY_write("N= ",88,0);
  char *N = malloc(5*sizeof(char *));
  if(N == 0){
    //TODO Handle error
  }
  sprintf(N, "%d", sta.n_avg);
  DISPLAY_write(N,90,0);
  free(N);

  //Second fast mode status
  DISPLAY_write("Fast: ",168,0);
  if(sta.mode > 0){
    DISPLAY_write("ENABLED",174,0);
  }else{
    DISPLAY_write("DISABLED",174,0);
  }

  //Lastly the upper and lower limits of temperature alarm
  DISPLAY_write("Alarm L:  ",248,0);
  char *alm_l = malloc(5*sizeof(char *));
  if(alm_l == 0){
    //TODO Handle error
  }
  sprintf(alm_l, "%d", sta.alm_l);
  DISPLAY_write(alm_l,1,1);
  free(alm_l);

  DISPLAY_write("Alarm H:  ",32,1);
  char *alm_h = malloc(5*sizeof(char *));
  if(alm_h == 0){
    //TODO Handle error
  }
  sprintf(alm_h, "%d", sta.alm_h);
  DISPLAY_write(alm_h,41,1);
  free(alm_h);
}
/*Shows the startup module test screen*/
void DISPLAY_writeTestingScreen(char temp_pass,char air_pass,char light_pass,char mem_pass){

  DISPLAY_write("Temp module: ",81,0);
	if(temp_pass ==1){
		DISPLAY_write("PASSED!",96,0); //TODO Implement
	}else{
	  DISPLAY_write("FAILED!",96,0);
	}

	DISPLAY_write("Air module: ",121,0);
	if(air_pass ==1){
    	DISPLAY_write("PASSED!",136,0); //TODO Implement
	}else{
    	DISPLAY_write("FAILED!",136,0); //TODO Implement
	}

	DISPLAY_write("Light module: ",161,0);
	if(light_pass ==1){
    	DISPLAY_write("PASSED!",176,0); //TODO Implement
	}else{
    	DISPLAY_write("FAILED!",176,0); //TODO Implement
	}

	DISPLAY_write("Mem check: ",201,0);
	if(mem_pass == 1){
    	DISPLAY_write("PASSED!",216,0); //TODO Implement
	}else{
    	DISPLAY_write("FAILED!",216,0); //TODO Implement
	}
}
/*Draws the bar graphs for one week three bars for every day min avg max*/
void DISPLAY_drawAirGraph(){

  int min;
  int avg;
  int max;
for (int j = 0; j < 7; j++){
	  min = mem.pres.min[j];
	  avg = mem.pres.avg[j];
	  max = mem.pres.max[j];

	  //Normalize
	  if(min>120000){
		 min = 120000;
	  }
	   if(avg>120000){
		 avg = 120000;
	  }
	   if(max>120000){
		 max = 120000;
	  }
	  if(min<90000){
		 min = 90000;
	  }
	   if(avg<90000){
		 avg = 90000;
	  }
	   if(max<90000){
		 max = 90000;
	  }

	  //Draw min bar, origin is at (62,100)
	  int start_pos = 61+(j*11)+0;
	  for(int i =0;i< ((min/1000)-90)*3;i++ ){
		DISPLAY_drawPixel(start_pos,110-i);
		DISPLAY_drawPixel(start_pos+1,110-i);
		DISPLAY_drawPixel(start_pos+2,110-i);
	  }
	  //Draw avg bar
	  start_pos = 61+(j*11)+3;
	  for(int i =0;i<((avg/1000)-90)*3;i++ ){
		DISPLAY_drawPixel(start_pos,110-i);
		DISPLAY_drawPixel(start_pos+1,110-i);
		DISPLAY_drawPixel(start_pos+2,110-i);
	  }
	  //Draw max bar
	  start_pos = 61+(j*11)+7;
	  /*Draw vertical line*/
	  for(int i =0;i<((max/1000-90))*3;i++ ){
		DISPLAY_drawPixel(start_pos,110-i);
		DISPLAY_drawPixel(start_pos+1,110-i);
		DISPLAY_drawPixel(start_pos+2,110-i);
	  }
	}
}
/*Draws the bar graphs for one week three bars for every day min avg max*/
void DISPLAY_drawTempGraph(temp_t* temp, char count){
 //TODO assign from temperature struct
 signed char min = temp->min;
 signed char avg = temp->avg;
 signed char max = temp->max;
//Normalize values to spec 0-40 degrees celsius
  if(min>40){
     min = 40;
  }
   if(avg>40){
     avg = 40;
  }
   if(max>40){
     max = 40;
  }
  if(min<-10){
     min = 0;
  }
   if(avg<-10){
     avg = 0;
  }
   if(max<-10){
     max = 0;
  }
  //Draw min bar, origin is at (62,100)
  int start_pos = 61+(count*11)+0;
  for(int i =0;i< min*2;i++ ){
    DISPLAY_drawPixel(start_pos,110-i);
    DISPLAY_drawPixel(start_pos+1,110-i);
    DISPLAY_drawPixel(start_pos+2,110-i);
  }
  //Draw avg bar
  start_pos = 61+(count*11)+3;
  for(int i =0;i<avg*2;i++ ){
    DISPLAY_drawPixel(start_pos,110-i);
    DISPLAY_drawPixel(start_pos+1,110-i);
    DISPLAY_drawPixel(start_pos+2,110-i);
  }
  //Draw max bar
  start_pos = 61+(count*11)+7;
  /*Draw vertical line*/
  for(int i =0;i<max*2;i++ ){
    DISPLAY_drawPixel(start_pos,110-i);
    DISPLAY_drawPixel(start_pos+1,110-i);
    DISPLAY_drawPixel(start_pos+2,110-i);
  }

}

/*Writes out standard sidebar nav and then state dependent part*/
void DISPLAY_writeSidebar(){
  if(sta.state == 0){ //Startup sidebar
    DISPLAY_write("* Next",24+40,1);
    DISPLAY_write("1",40*2,0);
    DISPLAY_write("2",40*3,0);
    DISPLAY_write("3",40*4,0);
    DISPLAY_write("...",40*5,0);
    DISPLAY_write("Etc.",40*6,0);
  }else{
    //Regular sidebar part
    DISPLAY_write("1 Home",40*2,0);
    DISPLAY_write("2 Sun",40*3,0);
    DISPLAY_write("3 Temp",40*4,0);
    DISPLAY_write("4 Air",40*5,0);
    DISPLAY_write("5 Conf",40*6,0);
  }

  //state dependent sidebar part (Offset is y=1 x = 24)
  switch(sta.state){

    case 1: //Home screen

    break;

    case 2: //Light follower
    //TODO write start and stop buttons
      DISPLAY_write("* Strt",24+40,1);
      DISPLAY_write("# Stop",24+40*2,1);
    break;

    case 3: //Temperature history
      DISPLAY_write("7 -Wk",24+40,1);
      DISPLAY_write("8 +Wk",24+40*2,1);
    break;

    case 4: //Air pressure history
      //DISPLAY_write("* -Day",24+40,1);
      //DISPLAY_write("# +Day",24+40*2,1);
    break;

    case 5: //Conf Settings screen
      //DISPLAY_write("* =10",24+40,1);
      DISPLAY_write("9 SetA",24+40*0,1);
      DISPLAY_write("* Load",24+40*1,1);
      DISPLAY_write("0 SetN",24+40*2,1);
      DISPLAY_write("# Fast",24+40*3,1);
    break;
  }

  /*Draw vertical line*/
  for(int i =0;i<160;i++ ){
  DISPLAY_drawPixel(40,i);
  }
}
/* Displays the header bar containing warnings, title, clock*/
void DISPLAY_writeHeader(char warning_status, char* title, char* time){
  //Warning status dependent header
  switch(warning_status){
    case 1: //Temperature overload
      DISPLAY_write("T!",0,0);
    break;
    case 2: //Memory full - overwriting
      DISPLAY_write("M!",0,0); //nr 14 will be a space
    break;
    case 3:
      DISPLAY_write("T!M!",0,0);
    break;

  }
  //Write title and clock
  DISPLAY_write(title,8,0); // 10 chars nr  will be space
  DISPLAY_write(time,24,0); //5 chars

  /*Draw horizontal line*/
  DISPLAY_writeData(0x40);
  DISPLAY_writeData(0x41);
  DISPLAY_writeCommand(0x24);//Set coordinates
  for(int i =0;i<40;i++ ){
    DISPLAY_writeData(0xFF);
    DISPLAY_writeCommand(0xC0);
  }
}

/*Overwrites standard graphic mem with 0
NOTE: Expensive, see loop
*/
void DISPLAY_clearGraphics(){
  //Clear graphics
  DISPLAY_writeData(0x00);
  DISPLAY_writeData(0x40);
  DISPLAY_writeCommand(0x24);//Set coordinates

  for(int i =0;i<6840;i++ ){
    DISPLAY_writeData(0x00);
    DISPLAY_writeCommand(0xC0);
  }

}

/* Clears display */
void DISPLAY_clearText(){
  //clear text
  //DISPLAY_writeData(text_home_adress);
   DISPLAY_writeData(0x00);
  DISPLAY_writeData(0x00);
  DISPLAY_writeCommand(0x24);//Set coordinates

  for(int i =0;i<=640;i++ ){
    DISPLAY_writeData(0x00);
    DISPLAY_writeCommand(0xC0);
  }

}

//Fill one pixel
void DISPLAY_drawPixel(int x, int y){
    int nXRest;   // rest vid modulo
    int nXPos;
    int nBit = 6;
    int move1;
    int move2;

    //find position in x
    nXPos = x/nBit;
    nXPos = (y-1)*40 + nXPos;

    nXRest = x%nBit;
    nXRest = nBit - nXRest;

    if(nXRest == nBit){
      nXPos--;
      nXRest = 0;
    }

    move1 = nXPos & 0xFF;
    move2 = (nXPos & 0xFF00)>>8;

        DISPLAY_writeData(move1);
        DISPLAY_writeData(0x40 + move2); //0x40 is our graphic mem start adress
        DISPLAY_writeCommand(0x24); //move cursor

        DISPLAY_writeCommand(0xF8 + nXRest);

}

/*Draw a smal blob*/
//TODO Take illu factor and create rays of  appr length
void DISPLAY_drawSun(int xw, int yw, int rw){
    int h;
    int x;
    int y;
  //Draw the circle
    for (x = -rw; x < rw ; x++){
        h = (int)sqrt(rw * rw - x * x);
        for (y = -h; y < h; y++){
            DISPLAY_drawPixel(x + xw, y + yw);
        }
    }
  //Draw the lines
      for (y = -h; y < h; y++){
            DISPLAY_drawPixel(x + xw, y + yw);
        }
}
/*Draw the arc the sun should follow*/
void DISPLAY_drawArc(int xw, int yw, int rw){
  int f = 1 - rw;
  int ddF_x = 1;
  int ddF_y = -2 * rw;
  int x = 0;
  int y = rw;

  DISPLAY_drawPixel(xw, yw+rw);
  DISPLAY_drawPixel(xw, yw-rw);
  DISPLAY_drawPixel(xw+rw, yw);
  DISPLAY_drawPixel(xw-rw, yw);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    DISPLAY_drawPixel(xw + x, yw + y);
    DISPLAY_drawPixel(xw - x, yw + y);
    DISPLAY_drawPixel(xw + x, yw - y);
    DISPLAY_drawPixel(xw - x, yw - y);

    DISPLAY_drawPixel(xw + y, yw + x);
    DISPLAY_drawPixel(xw - y, yw + x);
    DISPLAY_drawPixel(xw + y, yw - x);
    DISPLAY_drawPixel(xw - y, yw - x);
  }

}
/*Draws the axis for showing graph*/
void DISPLAY_drawAxis(){
  for (int i = 0; i < 130; i++)
  {
    DISPLAY_drawPixel(60+i,110);
  if(i<80){
      DISPLAY_drawPixel(60,110-i);
  }

  }

}
/*Draws the axis for showing graph*/
void DISPLAY_drawBorders(){
   /*Draw vertical line*/
  for(int i =0;i<160;i++ ){
   DISPLAY_drawPixel(40,i);
  }
  
  /*Draw horizontal line*/
  DISPLAY_writeData(0x40);
  DISPLAY_writeData(0x41);
  DISPLAY_writeCommand(0x24);//Set coordinates
  for(int i =0;i<40;i++ ){
    DISPLAY_writeData(0xFF);
    DISPLAY_writeCommand(0xC0);
  }

}
/************************************************************************/
/* Internal!                                                            */
/************************************************************************/
/*
    sets display to default state
*/
void DISPLAY_setDefaultState(void){
  *AT91C_PIOD_CODR = 1;         //Clear Reset display
  Delay(LONG_DELAY);            //LONG DELAY <------------------
  *AT91C_PIOD_SODR = 1;         //Set Reset display
  //*AT91C_PIOC_CODR = 5<<14;
  DISPLAY_writeData(0x00);
  DISPLAY_writeData(0x00);
  DISPLAY_writeCommand(0x40);//Set text home address
  DISPLAY_writeData(0x00);
  DISPLAY_writeData(0x40); //(0x40 standard)
  DISPLAY_writeCommand(0x42); //Set graphic home address
  DISPLAY_writeData(0x28); //standard was 1e
  DISPLAY_writeData(0x00);
  DISPLAY_writeCommand(0x41); // Set text area
  DISPLAY_writeData(0x28);
  DISPLAY_writeData(0x00);
  DISPLAY_writeCommand(0x43); // Set graphic area (same as text)
  DISPLAY_writeCommand(0x80); // text mode OR (0x80)
  DISPLAY_writeCommand(0x9C); // Enable both text and graphic 0x9c     Text on graphic off(0x94)
  //set cusor to upper left
  DISPLAY_writeData(0x00);
  DISPLAY_writeData(0x00);
  DISPLAY_writeCommand(0x24);
  //clear screen
  DISPLAY_clearText();
  DISPLAY_clearGraphics();
  //set cusor to upper left
  DISPLAY_writeData(0x00);
  DISPLAY_writeData(0x00);
  DISPLAY_writeCommand(0x24);

  //clear pin 45 and 44 for font select and fr
  *AT91C_PIOC_CODR = BIT_18_19;
}

/* Read display status*/
unsigned char DISPLAY_readStatus(void){
  unsigned char temp;

   *AT91C_PIOC_ODR =  BIT_2_TO_9; //set databus as input
   *AT91C_PIOC_SODR =  BIT_13 | BIT_12;    //Set direction as input  and activate chip
   //*AT91C_PIOC_CODR =  BIT_12;

   /* clear read */
   *AT91C_PIOC_SODR = (1<<17);     // set data
   *AT91C_PIOC_CODR  =3<<15;       // clear CS and rd
   //Delay
    Delay(SHORT_DELAY);
   //Read and save
    temp = (*AT91C_PIOC_PDSR);
    temp &= (3<<1);
    *AT91C_PIOC_SODR  =3<<15;        // set CS and rd
  //deactivate
   *AT91C_PIOC_SODR =  BIT_12;
   //Set Direction output
   *AT91C_PIOC_CODR =  BIT_13;
   return (temp>>1);
}

/* sends data to display */
void DISPLAY_writeData(unsigned char Data){
  while(DISPLAY_readStatus() != 3){
    Delay(SHORT_DELAY);
  }
  /* clear pins for output*/
  *AT91C_PIOC_CODR = BIT_2_TO_9;
  /* set databus to Data */
  *AT91C_PIOC_SODR = Data<<2;

  /* set direction on chip to output */
  *AT91C_PIOC_CODR =  BIT_13 | BIT_12;
  //*AT91C_PIOC_CODR =  BIT_12;    // enabling 74 chip

  *AT91C_PIOC_OER =  BIT_2_TO_9; // enable output on pins

  *AT91C_PIOC_CODR = 0xD<<14;
 /* *AT91C_PIOC_CODR = (1<<17);     // clear data
  *AT91C_PIOC_CODR  = 1<<16;        // clear CS
  *AT91C_PIOC_CODR = 1<<14;       // and WR*/
  Delay(SHORT_DELAY);             //SHORT DELAY
 // printf("datainput: %d", Data);
  *AT91C_PIOC_SODR = 0x5<<14;      // set CS  and wr

  *AT91C_PIOC_SODR =  BIT_12;     // disable 74 chip

  *AT91C_PIOC_ODR =  BIT_2_TO_9;  // enable input on pins
}

/* sends command to display */
void DISPLAY_writeCommand(unsigned char Command){
  while(DISPLAY_readStatus() != 3){
    Delay(SHORT_DELAY);
  }
  /* clear pins for output */
  *AT91C_PIOC_CODR = BIT_2_TO_9;
  /* set databus to command */
  *AT91C_PIOC_SODR = Command<<2;

  /* set direction on chip to output */
  *AT91C_PIOC_CODR =  BIT_13 | BIT_12; // set direction to output and enabling 74 chip
  //*AT91C_PIOC_CODR =  BIT_12;

  *AT91C_PIOC_OER =  BIT_2_TO_9; // enable output on pins

  *AT91C_PIOC_SODR = (1<<17);     // set data
  *AT91C_PIOC_CODR = 0x5<<14;     //clear CS and WR
  Delay(SHORT_DELAY);             //SHORT DELAY
 // printf("Command: %d\n", Command);
  *AT91C_PIOC_SODR = 0x5<<14;     //set CS and WR

  *AT91C_PIOC_SODR =  BIT_12;    // disable 74 chip

  *AT91C_PIOC_ODR =  BIT_2_TO_9; // enable input on pins

}

