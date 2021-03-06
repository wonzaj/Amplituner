//--------------------------------------------------------------
/* draw_display.c
 * contains operations that draw on the display
*/
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include <Display_OLED/draws_display.h>

//--------------------------------------------------------------
// Typedefs
//--------------------------------------------------------------
RTC_typeOfAlarm_t 	RTC_typeOfAlarm;
RTC_typeOfAlarm_t 	RTC_typeOfAlarm_A;
RTC_typeOfAlarm_t 	RTC_typeOfAlarm_B;
SSD1322_Screen_t 	SSD1322_Screen_State;
SSD1322_Screen_t 	SSD1322_Screen_State_Saved;
Clock_Data_Change_t 	Clock_Data_Time;
AlarmDataChange_t	AlarmDataChange;
led_power_state_t 	led_power_state;
display_mode_t 		display_mode;

extern SettingsUserMenu_t 	SettingsUserMenu;
//--------------------------------------------------------------
// Local Variables
//--------------------------------------------------------------
static char ConvertArrayCharTime[10];
static char TestingArray[40];
static char ConvertArrayCharLong[6];
static char display_gain_tab[10];


const uint16_t refresh_time_values[7] = {0,55001, 55000, 27500, 13750, 4580, 6}; //0 first element, 6 last element, 2 OFF
uint8_t settings_page = 0;
uint8_t saved_seconds = 0;
uint8_t saved_minutes = 0;
char user_name[10];
volatile _Bool is_display_on_standby_flag = false;

//--------------------------------------------------------------
// Const strings
//--------------------------------------------------------------
static const char *Poniedzialek = "Poniedzialek, "; //15
static const char *Wtorek 	= "Wtorek, ";       //9
static const char *Sroda        = "Sroda, ";        //8
static const char *Czwartek     = "Czwartek, ";     //11
static const char *Piatek       = "Piatek, ";       //9
static const char *Sobota       = "Sobota, ";       //9
static const char *Niedziela    = "Niedziela, ";    //12

static const char *Styczen      = "Styczen ";     //9
static const char *Luty         = "Luty ";        //6
static const char *Marzec       = "Marzec ";      //8
static const char *Kwiecien     = "Kwiecien ";    //10
static const char *Maj 	        = "Maj ";         //5
static const char *Czerwiec     = "Czerwiec ";    //10
static const char *Lipiec       = "Lipiec ";      //8
static const char *Sierpien     = "Sierpien ";    //10
static const char *Wrzesien     = "Wrzesien ";    //10
static const char *Pazdziernik  = "Pazdziernik "; //13
static const char *Listopad     = "Listopad ";    //10
static const char *Grudzien     = "Grudzien ";    //10

static const char *Jack_1     = "Jack 1";
static const char *Jack_2     = "Jack 2";
static const char *Radio      = "Radio" ;
static const char *Bluetooth  = "Bluetooth";
static const char *Microphone = "Microphone";

static const char *Power_OFF_str    = "1"; //zmienia jasno???? przy wy????czonym
static const char *Power_ON_str     = "2"; //zmienia jano???? przy w????czonym
static const char *Always_OFF_str   = "3"; //Zawsze wy????czone
static const char *Always_ON_str    = "4"; //zawsze w????czone
static const char *Change_brigh_str = "5"; //zawsze zmienia jasno????
					   //mo??e doda?? kontrole max jasno??ci

static const char *Disp_normal_str    = "Normal";
static const char *Disp_changing_str  = "Change";
static const char *Disp_standby_str   = "Standby"; 	//gasi wy??wietlacz po 1 min braku aktywno??ci
static const char *Disp_time_str      = "Time"; 	//co godzine pokazuje zegar przez minute , czyli drugi wyswietlacz

__attribute__((__unused__))static const char *FFT_front_left_str  = "Front left";
__attribute__((__unused__))static const char *FFT_front_right_str = "Front right";
__attribute__((__unused__))static const char *FFT_back_left_str   = "Back left";
__attribute__((__unused__))static const char *FFT_back_right_str  = "Back right";



//--------------------------------------------------------------
// Possible displayed screens
//--------------------------------------------------------------

void Refresh_display(const SSD1322_Screen_t SSD1322_Screen_State)
    {
	if(is_display_on_standby_flag == true)
	    {
		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

		if((saved_minutes + 1) == 60) saved_minutes = 0; 	// in this case it wait 2 min - work on it

		if((saved_seconds == sTime.Seconds) && ((saved_minutes + 1) == sTime.Minutes)) //do it one minute after last action with encoder or button
		    {
			SSD1322_API_sleep_on();
			return;
		    }
	    }

	switch (SSD1322_Screen_State)
	    {
	    case SSD1322_SCREEN_Welcome:
		SSD1322_Screen_Welcome(DisplayOLEDBuffer);
		break;
	    case SSD1322_SCREEN_TIME:
		SSD1322_Screen_Time(DisplayOLEDBuffer);
		break;
	    case SSD1322_SCREEN_RADIO:

		SSD1322_Screen_Radio(DisplayOLEDBuffer);
		break;
	    case SSD1322_SCREEN_WakeUp:

		SSD1322_Screen_WakeUp(DisplayOLEDBuffer);
		break;
	    case SSD1322_SCREEN_FFT:
		if(FFT_calc_done == 1)
		    {
			SSD1322_Screen_FFT(DisplayOLEDBuffer, OutFreqArray);
		    }
		break;
	    case SSD1322_SCREEN_UVMETER:
		if(UV_meter_front_back == UV_METER_BACK)
		    {
			SSD1322_Screen_UVMeter(DisplayOLEDBuffer, ADC_SamplesSUM[0], ADC_SamplesSUM[3], UV_meter_front_back);
		    }
		else if(UV_meter_front_back == UV_METER_FRONT)
		    {
			SSD1322_Screen_UVMeter(DisplayOLEDBuffer, ADC_SamplesSUM[2], ADC_SamplesSUM[1], UV_meter_front_back);
		    }
		break;
	    case SSD1322_SCREEN_OFF:
		SSD1322_Screen_OFF(DisplayOLEDBuffer);
		break;
	    case SSD1322_SCREEN_GoodBye:
		SSD1322_Screen_GoodBye(DisplayOLEDBuffer);
		break;
	    case SSD1322_SCREEN_SETCLOCK:
		SSD1322_Screen_SetClock(DisplayOLEDBuffer);
		break;
	    case SSD1322_SCREEN_SETALARM:
		SSD1322_Screen_SetAlarm(DisplayOLEDBuffer);
		break;
	    case SSD1322_SCREEN_SETTINGS:
		SSD1322_Screen_Settings(DisplayOLEDBuffer);
		break;
	    case SSD1322_SCREEN_USB:
		SSD1322_Screen_USB(DisplayOLEDBuffer);
		break;
	    case SSD1322_SCREEN_SETINPUT:
		SSD1322_Screen_SetInput(DisplayOLEDBuffer);
		break;
	    case SSD1322_SCREEN_TIME_BOUNCING:
		SSD1322_Screen_Time_Bouncing(DisplayOLEDBuffer);
		break;
	    case SSD1322_SCREEN_ENCODER_VOLUME_FRONT:
		SSD1322_Screen_Encoder_Volume_Front(DisplayOLEDBuffer);
		break;
	    case SSD1322_SCREEN_ENCODER_VOLUME_BACK:
		SSD1322_Screen_Encoder_Volume_Back(DisplayOLEDBuffer);
		break;
	    case SSD1322_SCREEN_ENCODER_LOUDNESS:
		SSD1322_Screen_Encoder_Loudness(DisplayOLEDBuffer);
		break;
	    case SSD1322_SCREEN_ENCODER_TREBLE:
		SSD1322_Screen_Encoder_Treble(DisplayOLEDBuffer);
		break;
	    case SSD1322_SCREEN_ENCODER_MIDDLE:
		SSD1322_Screen_Encoder_Middle(DisplayOLEDBuffer);
		break;
	    case SSD1322_SCREEN_ENCODER_BASS:
		SSD1322_Screen_Encoder_Bass(DisplayOLEDBuffer);
		break;
	    default:
		fill_buffer(DisplayOLEDBuffer, 0);
		send_buffer_to_OLED(DisplayOLEDBuffer, 0, 0);
		break;
	    }
    }

// ??
void SSD1322_Screen_Welcome(uint8_t *const buffer)
    {
	draw_text(buffer, "Przyjemnosc ze sluchania ", 20, 15, 15);
	draw_text(buffer, "zapewnia ", 20, 36, 15);
	send_buffer_to_OLED(buffer, 0, 0);
	HAL_Delay(200);
	draw_char(buffer, 'M', 20, 55, 15);
	send_buffer_to_OLED(buffer, 0, 0);
	HAL_Delay(200);
	draw_char(buffer, 'A', 38, 55, 15);
	send_buffer_to_OLED(buffer, 0, 0);
	HAL_Delay(200);
	draw_char(buffer, 'C', 53, 55, 15);
	send_buffer_to_OLED(buffer, 0, 0);
	HAL_Delay(200);
	draw_char(buffer, 'I', 68, 55, 15);
	send_buffer_to_OLED(buffer, 0, 0);
	HAL_Delay(200);
	draw_char(buffer, 'E', 78, 55, 15);
	send_buffer_to_OLED(buffer, 0, 0);
	HAL_Delay(200);
	draw_char(buffer, 'J', 93, 55, 15);
	send_buffer_to_OLED(buffer, 0, 0);
	HAL_Delay(1000);
	fill_buffer(buffer, 0);
	send_buffer_to_OLED(buffer, 0, 0);
    }


void SSD1322_Screen_Time(uint8_t *const buffer)
    {
	// do zegara dodac wybor roznych czcionek
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	fill_buffer(buffer, DISPLAY_BLACK);
	select_font(&FreeSerifItalic24pt7b);
	//wywo??ywa?? tylko co sekunde i nie sprawdzac nie potrzebne innych warto??ci
	//aktualizacje czasu wywolywac timerem co sekunde
	/* Setting Time */
	ChangeDateToArrayCharTime(ConvertArrayCharTime, sTime.Hours, sTime.Minutes, sTime.Seconds, 0);
	draw_text(buffer, (char*) ConvertArrayCharTime, 2, 32, 5);
	/* Setting Date */
	select_font(&FreeSerifItalic9pt7b);
	ConvertDateToBuffer((2021 + sDate.Year), sDate.Month, sDate.WeekDay, sDate.Date);
	draw_text(buffer, (char*) TestingArray, 2, 60, 5);
	send_buffer_to_OLED(buffer, 0, 0);
    }

void SSD1322_Screen_Radio(uint8_t *const buffer)
    {
	static uint16_t freq_scaled = 0;
	static uint16_t rssi_scaled = 0;
	static char RDStext[32];

	if(RADIO_IS_ON_back_flag == true || RADIO_IS_ON_front_flag == true)
	    {
		RDA5807_Read();
	    }
	else
	    {
		RDA5807_RDSinit();
	    }

	freq_scaled = map(RDA5807_GetFrequency(), RADIO_MIN_FREQ, RADIO_MAX_FREQ, 20, 200);
	fill_buffer(buffer, 0);
	select_font(&FreeSerifBold9pt7b);

	ChangeDateToArrayChar(RDA5807_GetFrequency());
	draw_text(buffer, (char*) ConvertArrayCharLong, 20, 13, 5);

	/* draw radio info */
	select_font(&FreeSerifBold9pt7b);
	draw_text(buffer, (char *)StationName, 150, 13, 5);
	select_font(&MACIEK_FONT);
	if(prepare_RDS_text((char *)RDStext) == true)
	    {
		draw_text(buffer, (char *)RDStextbuffer, 20, 32, 5); //zwraca stringa
//		if(another string avaible == true)
//		    {
//			//draw in the same place after 2 sec
//			//zrobi?? wtedy zmian?? co 2 sec
//		    }
	    }

	/* Scale for frequency */
	draw_freq_scale(buffer, freq_scaled);

	/* Draw RSSI wska??nik */
	rssi_scaled = map(RDA5807_GetRSSI(), 0, 63, 0, 63);
	draw_rect_filled(buffer, 0, 62 - rssi_scaled, 10, 63, 5);
	/* Draw antenna */
	select_font(&Custon_chars);
	draw_char(buffer, '!', 0, 16, 5); // ! - anntena in custom chars

	send_buffer_to_OLED(buffer, 0, 0);
    }

void SSD1322_Screen_WakeUp(uint8_t *const buffer)
    {
	//zwi??kszanie g??o??no??ci podczas budzenia
	fill_buffer(buffer, DISPLAY_BLACK);
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	uint8_t Hours = sTime.Hours;
	uint8_t Minutes = sTime.Minutes;
	uint8_t Seconds = sTime.Seconds;

	uint8_t Mode = 0;

	select_font(&FreeSerifBoldItalic9pt7b);
	draw_text(buffer, "WSTAWAJ !!!", 2, 58, 5);
	ChangeDateToArrayCharTime(ConvertArrayCharTime, Hours, Minutes, Seconds, Mode);
	select_font(&FreeSerifBoldItalic24pt7b);
	draw_text(buffer, (char*) ConvertArrayCharTime, 2, 33, 5);
	//doda?? budzik kt??ry bedzie sie ruszal, czyli odswiezac i zmieniac go dwa razy na sekunde
	//poprzez togglowanie flagi
	//albo usunac napis wstawaj i dac tylko czas i animacje budzika
	send_buffer_to_OLED(buffer, 0, 0);
    }

void SSD1322_Screen_FFT(uint8_t *const buffer, __attribute__((__unused__)) uint8_t *const FFT_out_buffer)
    {
	FFT_calc_done = 0;
	select_font(&FreeSerif9pt7b);
	fill_buffer(buffer, 0);
//	switch (FFT_channel_source)
//	    {
//		case FFT_front_left:
//		    draw_text(buffer, (char*)FFT_front_left_str, 80, 12, 5);
//		break;
//		case FFT_front_right:
//		    draw_text(buffer, (char*)FFT_front_right_str, 80, 12, 5);
//		break;
//		case FFT_back_left:
//		    draw_text(buffer, (char*)FFT_back_left_str, 80, 12, 5);
//		break;
//		case FFT_back_right:
//		    draw_text(buffer, (char*)FFT_back_right_str, 80, 12, 5);
//		break;
//		default:
//		break;
//	    }
	uint8_t j = 10; //begin of first pr????ek

	for (uint8_t i = 0; i < 22; i++)
	    {
		if ((63 - OutFreqArray[i]) < 13) //set max heigh of pr????ek
		    {
			draw_rect_filled(buffer, j, 10, j + 5, 63, 5);
		    }
		else
		    {
			draw_rect_filled(buffer, j, 60 - OutFreqArray[i], j + 5, 63, 5);
		    }
		j = j + 10;
	    }

	select_font(&TomThumb);
	draw_text(buffer, "50", 10, 5, 5);
	draw_text(buffer, "200", 50, 5, 5);
	draw_text(buffer, "2k", 140, 5, 5);
	draw_text(buffer, "20k", 220, 5, 5);

	send_buffer_to_OLED(buffer, 0, 0);
    }

void SSD1322_Screen_UVMeter(uint8_t *const buffer, UV_meter_t left_channel, UV_meter_t right_channel, const uint8_t mode)
    {
	//efekt spadaj??cej warto??ci peak do w????czenia
	select_font(&FreeSerifItalic9pt7b);
	fill_buffer(buffer, 0);
	left_channel = map(left_channel, 20, 2100, 25, 254); //doda?? zabezpieczenie przed przekroczeniem zakres??w
	right_channel = map(right_channel, 20, 2100, 25, 254);

	if(mode == UV_METER_FRONT)
	    {
		//draw front label
	    }
	else if(mode == UV_METER_BACK)
	    {
		//draw back label
	    }

	draw_UV_rectangle_scale(buffer, left_channel, right_channel);
	draw_UV_lines_scale(buffer, left_channel, right_channel);

	draw_char(buffer, 'L', 2, 23, 10); // powinien by?? wy??wietlany czy to jest lewy front czy prawy front
	draw_char(buffer, 'P', 2, 55, 10);
	send_buffer_to_OLED(buffer, 0, 0);
    }

void SSD1322_Screen_OFF(uint8_t *const buffer)
    {
	select_font(&FreeSerif9pt7b);
	fill_buffer(buffer, 0);
	draw_text(buffer, "Screen_OFF  (:", 5, 32, 5);
	send_buffer_to_OLED(buffer, 0, 0);
    }

void SSD1322_Screen_GoodBye(uint8_t *const buffer)
    {
	//doda?? argument z wy??wietlanym imieniem u??ytkownika i po??egnanie do niego
	fill_buffer(buffer, 0);
	select_font(&FreeSerif9pt7b);
	draw_text(buffer, "GoodBye  (:", 5, 32, 5);
	send_buffer_to_OLED(buffer, 0, 0);
    }

void SSD1322_Screen_SetClock(uint8_t *const buffer)
    {
	fill_buffer(buffer, 0);
	select_font(&FreeSerif9pt7b);
	draw_text(buffer, "SetClock  (:", 5, 32, 5);
	send_buffer_to_OLED(buffer, 0, 0);
    }

void SSD1322_Screen_SetAlarm(uint8_t *const buffer)
    {
	/* Alaways reset display buffer to zero*/
	fill_buffer(buffer, DISPLAY_BLACK);
	select_font(&FreeSerifItalic9pt7b);

	/* Checks if display mode is set to preview set alarms or modify them  */
	if (PreviewAlarm == true)
	    {
		/* Drawing a pointer to selected ALARM.  */
		if(alarm_set_A_or_B == true) 	draw_text(buffer, "--->", 135, 25, 5);
		if(alarm_set_A_or_B == false) 	draw_text(buffer, "--->", 135, 55, 5);

		/* Drawing filled rectangle if alarm A is SET otherwise just a rectangle */
		if(IS_ALARM_SET_A == false)	draw_rect(buffer, 240, 15, 250, 25, 5);
		if(IS_ALARM_SET_A == true)	draw_rect_filled(buffer, 240, 15, 250, 25, 5); //mo??e zmieni?? na X
		//draw_rect	(buffer, x0, y0, x1, y1, 15);

		/* Drawing filled rectangle if alarm B is SET otherwise just a rectangle */
		if(IS_ALARM_SET_B == false)	draw_rect(buffer, 240, 45, 250, 55, 5);
		else 				draw_rect_filled(buffer, 240, 45, 250, 55, 5); //mo??e zmieni?? na X

		/* Drawing a pointer to selected ALARM.  */
		draw_text(buffer, "Alarm A", 172, 25, 5);
		draw_text(buffer, "Alarm B", 172, 55, 5);

		/* Drawing a pointer to selected ALARM.  */
		select_font(&FreeSerifItalic24pt7b);
		ChangeDateToArrayCharTime(ConvertArrayCharTime, Alarm.AlarmTime.Hours, Alarm.AlarmTime.Minutes, 0, 1);
		draw_text(buffer, (char*) ConvertArrayCharTime, 5, 32, 5);

		/* Drawing alarm mode */
		select_font(&FreeSerifItalic9pt7b);
		Set_Alarm_Mode(RTC_typeOfAlarm);
		draw_text(buffer, (char*) AlarmMode, 5, 60, 5);
	    }
	else if(PreviewAlarm == false)
	    {
		if((IS_ALARM_SET_A == false) && (IS_ALARM_SET_B == false))
		    {
			select_font(&FreeMonoOblique24pt7b);
			draw_text(buffer, "ALARMY", 2, 30, 5);
			draw_text(buffer, "OFF", 2, 62, 5);
		    }
		else
		    {	//tutaj zmienic czcionk?? na ??adn??
			if (IS_ALARM_SET_A == true)
			    {
				select_font(&FreeSerif9pt7b);
				ChangeDateToArrayCharTime(ConvertArrayCharTime, Alarm_A.AlarmTime.Hours, Alarm_A.AlarmTime.Minutes, 0, 1);
				draw_text(buffer, (char*)buffer, 2, 30, 5);
				Set_Alarm_Mode(RTC_typeOfAlarm_A);
				draw_text(buffer, (char*) AlarmMode, 55, 30, 5);
			    }
			else if(IS_ALARM_SET_A == false)
			    {
				draw_text(buffer, "OFF", 2, 30, 5);
			    }
			if (IS_ALARM_SET_B == true)
			    {
				select_font(&FreeSerif9pt7b);
				ChangeDateToArrayCharTime(ConvertArrayCharTime, Alarm_B.AlarmTime.Hours, Alarm_B.AlarmTime.Minutes, 0, 1);
				draw_text(buffer, (char*)ConvertArrayCharTime, 2, 60, 5);
				Set_Alarm_Mode(RTC_typeOfAlarm_B);
				draw_text(buffer, (char*) AlarmMode, 55, 60, 5);
			    }
			else if(IS_ALARM_SET_B == false)
			    {
				draw_text(buffer, "OFF", 2, 60, 5);
			    }
			draw_text(buffer, "Alarm A", 190, 30, 5);
			draw_text(buffer, "Alarm B", 190, 60, 5);
		    }
	    }

	send_buffer_to_OLED(buffer, 0, 0);
    }

void SSD1322_Screen_Settings(uint8_t *const buffer)
    {
	select_font(&FreeMonoOblique9pt7b);
	fill_buffer(buffer, 0);

	if (settings_page == PAGE_SETTINGS_1)
	    {
		draw_text(buffer, "Screen time:", 15, 15, 5);
		draw_text(buffer, "Alarm A:", 15, 30, 5);
		draw_text(buffer, "Alarm B:", 15, 45, 5);
		draw_text(buffer, "User Name:", 15, 60, 5);

		draw_refreshTime(buffer);
		draw_alarmsSource(buffer);
		draw_text(buffer, (char *)user_name, 125, 60, 5);
	    }
	else if(settings_page == PAGE_SETTINGS_2)
	    {
		draw_text(buffer, "Display mode:", 15, 15, 5);
		draw_text(buffer, "Power LED:", 15, 30, 5);
		//another case for Instat power off/slow change volume to zero
		draw_powerLED(buffer);
		draw_displayMode(buffer);
//		draw_text(buffer, "Alarm B:", 15, 45, 5);
//		draw_text(buffer, "User Name:", 15, 60, 5);


	    }

	draw_pointer(buffer);
	send_buffer_to_OLED(buffer, 0, 0);
    }

void SSD1322_Screen_USB(uint8_t *const buffer)
    {
	select_font(&FreeSerif9pt7b);
	fill_buffer(buffer, 0);
	draw_text(buffer, "USB", 5, 32, 5);
	send_buffer_to_OLED(buffer, 0, 0);
    }

void SSD1322_Screen_Time_Bouncing(uint8_t *const buffer)
    {
	static uint32_t random_x_from = 0; 	//number beetwen 0...254
	static uint32_t random_y_from = 20; 	//nubmer beetwen 0...63
	static uint32_t random_x_to = 0; 	//number beetwen 0...254
	static uint32_t random_y_to = 0; 	//nubmer beetwen 0...63
	//static volatile uint8_t bouncing_flag;

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	select_font(&FreeSansBold9pt7b);

	if (get_random_coords(&random_x_to, &random_y_to) == true)
	    {
		make_array(buffer, random_x_from, random_y_from, random_x_to, random_y_to, 5);
		random_x_from = random_x_to;
		random_y_from = random_y_to;
	    }
    }

void SSD1322_Screen_SetInput(uint8_t *const buffer)
    {
	select_font(&FreeSerif9pt7b);
	fill_buffer(buffer, 0);
	draw_text(buffer, "Front input:", 10, 20, 5);
	draw_text(buffer, "Back  input:", 10, 50, 5);
	switch (TDA7719_config.set_input_front)
	    {
		case JACK_1:
		    draw_text(buffer, (char *)Jack_1, 100, 20, 5);
		break;
		case JACK_2:
		    draw_text(buffer, (char *)Jack_2, 100, 20, 5);
		break;
		case RADIO:
		    draw_text(buffer, (char *)Radio, 100, 20, 5);
		break;
		case BLUETOOTH:
		    draw_text(buffer, (char *)Bluetooth, 100, 20, 5);
		break;
		case MICROPHONE:
		    draw_text(buffer, (char *)Microphone, 100, 20, 5);
		break;
		default:
		break;
	    }

	switch (TDA7719_config.set_input_back)
	    {
		case JACK_1:
		    draw_text(buffer, (char *)Jack_1, 100, 50, 5);
		break;
		case JACK_2:
		    draw_text(buffer, (char *)Jack_2, 100, 50, 5);
		break;
		case RADIO:
		    draw_text(buffer, (char *)Radio, 100, 50, 5);
		break;
		case BLUETOOTH:
		    draw_text(buffer, (char *)Bluetooth, 100, 50, 5);
		break;
		case MICROPHONE:
		    draw_text(buffer, (char *)Microphone, 100, 50, 5);
		break;
		default:
		break;
	    }
	//TDA7719_config.set_input_front = TDA7719_config.audio_source;

	send_buffer_to_OLED(buffer, 0, 0);
    }

void SSD1322_Screen_Encoder_Volume_Front(uint8_t *const buffer)
    {
	fill_buffer(buffer, 0);
	select_font(&TomThumb);
	draw_char(buffer, '0'  , 195, 53, 5);
	draw_text(buffer, "-80", 2  , 53, 5);
	draw_text(buffer, "+15", 225, 53, 5);
	select_font(&FreeSerifItalic9pt7b);
	draw_text(buffer, "dB", 235, 63, 5);
	draw_encoder_volume_front_scale(buffer);

	send_buffer_to_OLED(buffer, 0, 0);
    }

void SSD1322_Screen_Encoder_Volume_Back(uint8_t *const buffer)
    {
	fill_buffer(buffer, 0);
	select_font(&TomThumb);
	draw_char(buffer, '0'  , 195, 53, 5);
	draw_text(buffer, "-80", 2  , 53, 5);
	draw_text(buffer, "+15", 225, 53, 5);
	select_font(&FreeSerifItalic9pt7b);
	draw_text(buffer, "dB", 235, 63, 5);
	draw_encoder_volume_back_scale(buffer);

	send_buffer_to_OLED(buffer, 0, 0);
    }

void SSD1322_Screen_Encoder_Loudness(uint8_t *const buffer)
    {
	for (uint8_t i = 0; i < 10; ++i)
	    {
		display_gain_tab[i] = 0;
	    }

	select_font(&FreeSerif9pt7b);
	fill_buffer(buffer, 0);
	draw_text(buffer, "LOUDNESS", 70, 15, 5);
	select_font(&FreeSans9pt7b);
	draw_text(buffer, "Mid freq:", 25, 40, 5);
	draw_text(buffer, "Gain:", 50, 63, 5);
	draw_gain_and_freq_loudness(buffer);

	send_buffer_to_OLED(buffer, 0, 0);
    }

void SSD1322_Screen_Encoder_Treble(uint8_t *const buffer)
    {
	for (uint8_t i = 0; i < 10; ++i)
	    {
		display_gain_tab[i] = 0;
	    }

	select_font(&FreeSerif9pt7b);	//niby mo??na wy??wietla?? wykres
	fill_buffer(buffer, 0);
	draw_text(buffer, "TREBLE", 70, 15, 5);
	select_font(&FreeSans9pt7b);
	draw_text(buffer, "Mid freq:", 25, 40, 5);
	draw_text(buffer, "Gain:", 50, 63, 5);
	draw_gain_and_freq(encoderFilterTreble.gain - 16);
	draw_text(buffer, (char*)display_gain_tab, 100, 63, 5);

	switch (encoderFilterTreble.centerFreq)
	    {
		case 0:		//00 - flat
		    draw_text(buffer, "10 kHz", 100, 40, 5);
		break;
		case 1:		//01  - 400 Hz
		    draw_text(buffer, "12,5 kHz", 100, 40, 5);
		break;
		case 2:		//10  - 800 Hz
		    draw_text(buffer, "15 kHz", 100, 40, 5);
		break;
		case 3:		//11  - 2400 Hz
		    draw_text(buffer, "17.5 kHz", 100, 40, 5);
		break;
		default:
		break;
	    }
	send_buffer_to_OLED(buffer, 0, 0);
    }

void SSD1322_Screen_Encoder_Middle(uint8_t *const buffer)
    {

	for (uint8_t i = 0; i < 10; ++i)
	    {
		display_gain_tab[i] = 0;
	    }

	select_font(&FreeSerif9pt7b);	//niby mo??na wy??wietla?? wykres
	fill_buffer(buffer, 0);
	draw_text(buffer, "MIDDLE", 70, 15, 5);
	select_font(&FreeSans9pt7b);
	draw_text(buffer, "Q fact:", 25, 40, 5);
	draw_text(buffer, "Gain:", 50, 63, 5);
	draw_gain_and_freq(encoderFilterMiddle.gain - 16);
	draw_text(buffer, (char*)display_gain_tab, 100, 63, 5);

	switch (encoderFilterMiddle.centerFreq)
	    {
		case 0:
		    draw_text(buffer, "0.5", 100, 40, 5);
		break;
		case 1:
		    draw_text(buffer, "0.75", 100, 40, 5);
		break;
		case 2:
		    draw_text(buffer, "1", 100, 40, 5);
		break;
		case 3:
		    draw_text(buffer, "1.25", 100, 40, 5);
		break;
		default:
		break;
	    }
	send_buffer_to_OLED(buffer, 0, 0);
    }

void SSD1322_Screen_Encoder_Bass(uint8_t *const buffer)
    {

	for (uint8_t i = 0; i < 10; ++i)
	    {
		display_gain_tab[i] = 0;
	    }

	select_font(&FreeSerif9pt7b);	//niby mo??na wy??wietla?? wykres
	fill_buffer(buffer, 0);
	draw_text(buffer, "BASS", 70, 15, 5);
	select_font(&FreeSans9pt7b);
	draw_text(buffer, "Q fact:", 25, 40, 5);
	draw_text(buffer, "Gain:", 50, 63, 5);
	draw_gain_and_freq(encoderFilterBass.gain - 16);
	draw_text(buffer, (char*)display_gain_tab, 100, 63, 5);

	switch (encoderFilterBass.centerFreq)
	    {
		case 0:
		    draw_text(buffer, "1.0", 100, 40, 5);
		break;
		case 1:
		    draw_text(buffer, "1.25", 100, 40, 5);
		break;
		case 2:
		    draw_text(buffer, "1.5", 100, 40, 5);
		break;
		case 3:
		    draw_text(buffer, "2.0", 100, 40, 5);
		break;
		default:
		break;
	    }
	send_buffer_to_OLED(buffer, 0, 0);
    }

//SSD1322_Screen_draw_signal_oscyloscope();

//--------------------------------------------------------------
// Some random things to draw
//--------------------------------------------------------------

void draw_encoder_volume_front_scale(uint8_t *const buffer)
    {
	static uint16_t Volume;
	static uint16_t Volume1;
	static uint16_t Volume2;
	static uint16_t Volume3;
	static uint8_t i;

	switch (encoderVolFront.audioOutputState)
	    {
	    case MASTER:
		Volume = map(encoderVolFront.volumeMaster, 0, 94, 5, 230);
		draw_text(buffer, "Volume Master", 70, 15, 5);
		draw_rect_filled(buffer, 5, 56, Volume, 62, 5);
		draw_speaker_left(buffer, 3, 20);
		draw_speaker_right(buffer, 243, 20);
		draw_speaker_left(buffer, 10, 43);
		draw_speaker_right(buffer, 236, 43);
		draw_nutka1(buffer, 21, 10);
//		draw_nutka3(buffer, 26, 18);
		draw_nutka2(buffer, 31, 8);
		draw_nutka4(buffer, 32, 25);
		for ( i = Volume; i < 230; i++)
		    {
			if (i%5 == 0)draw_vline(buffer, i, 56, 62, 3);
		    }
		break;
	    case MASTER_V2:
		Volume = map(encoderVolFront.volumeMaster, 0, 94, 5, 230);
		draw_text(buffer, "Volume Master", 70, 15, 5);
		draw_rect_filled(buffer, 5, 56, Volume, 62, 5);
		draw_speaker_left(buffer, 3, 20);
		draw_speaker_right(buffer, 243, 20);
		draw_speaker_left(buffer, 10, 43);
		draw_speaker_right(buffer, 236, 43);
		for ( i = Volume; i < 230; i++)
		    {
			if (i%5 == 0)draw_vline(buffer, i, 56, 62, 3);
		    }
		break;
	    case NORMAL:
		Volume3 = map(encoderVolFront.volumeLeftRight, 0, 94, 5, 230);
		draw_text(buffer, "Volume front", 70, 15, 5);
		draw_rect_filled(buffer, 5, 56, Volume3, 62, 5);
		draw_speaker_left(buffer, 3, 20);
		draw_speaker_right(buffer, 243, 20);
		for ( i = Volume3; i < 230; i++)
		    {
			if (i%5 == 0)draw_vline(buffer, i, 56, 62, 3);
		    }
		break;
	    case MUTE:
		draw_text(buffer, "Volume MUTED", 70, 15, 5);
		draw_rect_filled(buffer, 5, 56, 6, 62, 5);
		for ( i = 0; i < 230; i++)
		    {
			if (i%5 == 0)draw_vline(buffer, i, 56, 62, 3);
		    }
		break;
	    case ATTE_LEFT:
		Volume1 = map(encoderVolFront.volumeLeft, 0, 94, 5, 230);
		draw_text(buffer, "Volume front left", 70, 15, 5);
		draw_rect_filled(buffer, 5, 56, Volume1, 62, 5);
		draw_speaker_left(buffer, 3, 20);
		for ( i = Volume1; i < 230; i++)
		    {
			if (i%5 == 0)draw_vline(buffer, i, 56, 62, 3);
		    }
		break;
	    case ATTE_RIGHT:
		Volume2 = map(encoderVolFront.volumeRight, 0, 94, 5, 230);
		draw_text(buffer, "Volume front right", 70, 15, 5);
		draw_rect_filled(buffer, 5, 56, Volume2, 62, 5);
		draw_speaker_right(buffer, 243, 20);
		for ( i = Volume2; i < 230; i++)
		    {
			if (i%5 == 0)draw_vline(buffer, i, 56, 62, 3);
		    }
		break;
	    default:
		break;
	    }

    }


void draw_encoder_volume_back_scale(uint8_t *const buffer)
    {
	static uint16_t Volume1;
	static uint16_t Volume2;
	static uint16_t Volume3;
	static uint8_t i;

	switch (encoderVolBack.audioOutputState)
	    {
	    case NORMAL:
		Volume3 = map(encoderVolBack.volumeLeftRight, 0, 94, 5, 230);
		draw_text(buffer, "Volume back", 70, 15, 5);
		draw_rect_filled(buffer, 5, 56, Volume3, 62, 5);
		draw_speaker_left(buffer, 10, 43);
		draw_speaker_right(buffer, 236, 43);
		for ( i = Volume3; i < 230; i++)
		    {
			if (i%5 == 0)draw_vline(buffer, i, 56, 62, 3);
		    }
		break;
	    case NORMAL_V2:
		Volume3 = map(encoderVolBack.volumeLeftRight, 0, 94, 5, 230);
		draw_text(buffer, "Volume back", 70, 15, 5);
		draw_rect_filled(buffer, 5, 56, Volume3, 62, 5);
		draw_speaker_left(buffer, 10, 43);
		draw_speaker_right(buffer, 236, 43);
		for ( i = Volume3; i < 230; i++)
		    {
			if (i%5 == 0)draw_vline(buffer, i, 56, 62, 3);
		    }
		break;
	    case MUTE:
		draw_text(buffer, "Volume MUTED", 70, 15, 5);
		draw_rect_filled(buffer, 5, 56, 6, 62, 5);
		for ( i = 0; i < 230; i++)
		    {
			if (i%5 == 0)draw_vline(buffer, i, 56, 62, 3);
		    }
		break;
	    case ATTE_LEFT:
		Volume1 = map(encoderVolBack.volumeLeft, 0, 94, 5, 230);
		draw_text(buffer, "Volume back left", 70, 15, 5);
		draw_rect_filled(buffer, 5, 56, Volume1, 62, 5);
		draw_speaker_left(buffer, 10, 43);
		for ( i = Volume1; i < 230; i++)
		    {
			if (i%5 == 0)draw_vline(buffer, i, 56, 62, 3);
		    }
		break;
	    case ATTE_RIGHT:
		Volume2 = map(encoderVolBack.volumeRight, 0, 94, 5, 230);
		draw_text(buffer, "Volume Back right", 70, 15, 5);
		draw_rect_filled(buffer, 5, 56, Volume2, 62, 5);
		draw_speaker_right(buffer, 236, 43);
		for ( i = Volume2; i < 230; i++)
		    {
			if (i%5 == 0)draw_vline(buffer, i, 56, 62, 3);
		    }
		break;
	    default:
		break;
	    }

    }


void draw_gain_and_freq_loudness(uint8_t *const buffer)
    {
	if ((encoderFilterLoudness.gain > 0))
	    {
		if (encoderFilterLoudness.gain > 10)
		    {
			display_gain_tab[0] = ASCII_PLUS;
			display_gain_tab[1] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(1, encoderFilterLoudness.gain);
			display_gain_tab[2] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(0, encoderFilterLoudness.gain);
		    }
		else
		    {
			display_gain_tab[0] = ASCII_PLUS;
			display_gain_tab[1] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(0, encoderFilterLoudness.gain);
		    }
	    }
	else 	//gain == 0
	    {
		display_gain_tab[0] = ASCII_DIGIT_OFFSET;
	    }

	strcat(display_gain_tab, " dB");
	draw_text(buffer, (char*)display_gain_tab, 100, 63, 5);

	switch (encoderFilterLoudness.centerFreq)
	    {
		case 0:		//00 - flat
		    draw_text(buffer, "Flat", 100, 40, 5);
		break;
		case 1:		//01  - 400 Hz
		    draw_text(buffer, "400 Hz", 100, 40, 5);
		break;
		case 2:		//10  - 800 Hz
		    draw_text(buffer, "800 Hz", 100, 40, 5);
		break;
		case 3:		//11  - 2400 Hz
		    draw_text(buffer, "2400 Hz", 100, 40, 5);
		break;
		default:
		break;
	    }
    }


void draw_gain_and_freq(int8_t gainValue)
    {
	if (gainValue < 0)
	    {
		gainValue = abs(gainValue);
		if (gainValue > 10)
		    {
			display_gain_tab[0] = ASCII_MINUS;
			display_gain_tab[1] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(1, gainValue);
			display_gain_tab[2] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(0, gainValue);
		    }
		else
		    {
			display_gain_tab[0] = ASCII_MINUS;
			display_gain_tab[1] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(0, gainValue);
		    }
	    }
	else if((gainValue > 0))
	    {
		if (gainValue > 10)
		    {
			display_gain_tab[0] = ASCII_PLUS;
			display_gain_tab[1] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(1, gainValue);
			display_gain_tab[2] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(0, gainValue);
		    }
		else
		    {
			display_gain_tab[0] = ASCII_PLUS;
			display_gain_tab[1] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(0, gainValue);
		    }
	    }
	else 	//gain == 0
	    {
		display_gain_tab[0] = ASCII_DIGIT_OFFSET;
	    }

	strcat(display_gain_tab, " dB");
    }

//heigh - 6 pixels, width - 8 pixels
void draw_nutka1(uint8_t *const buffer, const int16_t begin_x, const int16_t begin_y)
    {
	draw_rect_filled(buffer, begin_x, begin_y, begin_x + 1, begin_y + 1, 5);			// x = 19   y = 19
	draw_rect_filled(buffer, begin_x + 4, begin_y, begin_x + 5 , begin_y + 1, 5);
	      draw_hline(buffer, begin_y - 5 , begin_x + 2, begin_x + 7, 5);
	      draw_vline(buffer, begin_x + 1 , begin_y - 1, begin_y - 4, 5);
	      draw_vline(buffer, begin_x + 5,  begin_y - 1, begin_y - 4, 5);
	      draw_pixel(buffer, begin_x + 2, begin_y - 4, 5);
    }

//heigh - 5 pixels, width - 4 pixels
void draw_nutka2(uint8_t *const buffer, const int16_t begin_x, const int16_t begin_y)
    {
	draw_rect_filled(buffer, begin_x, begin_y, begin_x + 1, begin_y + 1, 5);
	draw_vline(buffer, begin_x + 1, begin_y - 1, begin_y - 3, 5);
	draw_hline(buffer, begin_y - 4, begin_x + 1, begin_x + 4, 5);
	draw_pixel(buffer, begin_x + 4, begin_y - 3, 5);
    }

//heigh - 7 pixels, width - 6 pixels
void draw_nutka3(uint8_t *const buffer, const int16_t begin_x, const int16_t begin_y)
    {
	draw_rect_filled(buffer, begin_x, begin_y, begin_x + 2, begin_y + 2, 5);
	draw_vline(buffer, begin_x + 2, begin_y - 1, begin_y - 6, 5);
	draw_hline(buffer, begin_y - 6, begin_x + 2, begin_x + 5, 5);
	draw_pixel(buffer, begin_x + 5, begin_y - 4, 5);
	draw_pixel(buffer, begin_x + 5, begin_y - 5, 5);
    }

//heigh - 8 pixels, width - 11 pixels
void draw_nutka4(uint8_t *const buffer, const int16_t begin_x, const int16_t begin_y)
    {
	draw_rect_filled(buffer, begin_x + 1, begin_y, begin_x + 2, begin_y + 2, 5);
	draw_rect_filled(buffer, begin_x + 6, begin_y, begin_x + 8, begin_y + 2, 5);
	draw_vline(buffer, begin_x + 2, begin_y - 1, begin_y - 6, 5);
	draw_vline(buffer, begin_x + 8, begin_y - 1, begin_y - 7, 5);
	draw_hline(buffer, begin_y - 7, begin_x + 3, begin_x + 10, 5);
	draw_pixel(buffer, begin_x + 3, begin_y - 6, 5);
	draw_pixel(buffer, begin_x + 10, begin_y - 6, 5);
	draw_pixel(buffer, begin_x, begin_y + 1, 5);
	draw_pixel(buffer, begin_x, begin_y + 2, 5);
    }

//heigh - 8 pixels, width - 6 pixels
void draw_nutka5(uint8_t *const buffer, const int16_t begin_x, const int16_t begin_y)

    {
	draw_rect_filled(buffer, begin_x, begin_y, begin_x + 2, begin_y + 2, 5);
	draw_vline(buffer, begin_x + 2, begin_y - 1, begin_y - 6, 5);
	draw_hline(buffer, begin_y - 6, begin_x + 2, begin_x + 4, 5);
	draw_hline(buffer, begin_y - 5, begin_x + 4, begin_x + 5, 5);

    }

//heigh - 11 pixels, width - 19 pixels
void draw_speaker_left(uint8_t *const buffer, const int16_t begin_x, const int16_t begin_y)
    {
	draw_hline(buffer, begin_y, begin_x + 2, begin_x + 11, 5);
	draw_hline(buffer, begin_y - 16, begin_x + 3, begin_x  + 11, 5);
	draw_vline(buffer, begin_x + 3, begin_y , begin_y - 16, 5);
	draw_vline(buffer, begin_x + 12, begin_y, begin_y - 16, 5);
	draw_pixel(buffer, begin_x + 1, begin_y - 1, 5);
	draw_vline(buffer, begin_x, begin_y - 2, begin_y - 19, 5);
	draw_hline(buffer, begin_y - 19, begin_x, begin_x + 9 , 5);
	draw_pixel(buffer, begin_x + 10, begin_y - 18, 5);
	draw_pixel(buffer, begin_x + 11, begin_y - 17, 5);
	draw_pixel(buffer, begin_x + 1, begin_y - 18, 5);
	draw_pixel(buffer, begin_x + 2, begin_y  - 17, 5);
	draw_circle(buffer,begin_x + 8, begin_y - 5, 2, 5);
	draw_circle(buffer,begin_x + 8, begin_y - 12, 1, 5);
    }

//heigh - 11 pixels, width - 19 pixels
void draw_speaker_right(uint8_t *const buffer,const int16_t begin_x, const int16_t begin_y)
    {
	draw_hline(buffer, begin_y, begin_x, begin_x + 8, 5);
	draw_hline(buffer, begin_y - 16, begin_x, begin_x + 8, 5);
	draw_vline(buffer, begin_x, begin_y, begin_y - 16, 5);
	draw_vline(buffer, begin_x + 9, begin_y, begin_y - 16, 5);
	draw_pixel(buffer, begin_x + 1, begin_y - 17, 5);
	draw_pixel(buffer, begin_x + 2, begin_y - 18, 5);
	draw_hline(buffer, begin_y - 19, begin_x + 3, begin_x + 11, 5);
	draw_pixel(buffer, begin_x + 9 , begin_y - 17, 5);
	draw_pixel(buffer, begin_x + 10, begin_y - 18, 5);
	draw_vline(buffer, begin_x + 12, begin_y - 3, begin_y - 19, 5);
	draw_pixel(buffer, begin_x + 10, begin_y - 1, 5);
	draw_pixel(buffer, begin_x + 11, begin_y - 2, 5);
	draw_circle(buffer,begin_x + 4, begin_y - 5, 2, 5);
	draw_circle(buffer,begin_x + 4, begin_y - 12, 1, 5);
    }


void draw_UV_rectangle_scale(uint8_t *const buffer, const UV_meter_t left_channel, const UV_meter_t right_channel)
    {
	for (uint16_t a = 20; a < 254; a = a + 18)
	    {
		if (left_channel < (a + 18))
		    {
			draw_rect_filled(buffer, a, 16, a + 14, 26, 1);
		    }
		else
		    {
			draw_rect_filled(buffer, a, 16, a + 14, 26, 10);
		    }

		if(right_channel < (a + 18))
		    {
			draw_rect_filled(buffer, a, 40, a + 14, 50, 1);
		    }
		else
		    {
			draw_rect_filled(buffer, a, 40, a + 14, 50, 10);
		    }
	    }
    }


void draw_UV_lines_scale(uint8_t *const buffer, const UV_meter_t left_channel, const UV_meter_t right_channel)
    {
	for (uint16_t b = 20; b < 254; b = b + 3)
	    {
		if (left_channel < b)
		    {
			draw_vline(buffer, b, 10, 3, 1);
		    }
		else
		    {
			draw_vline(buffer, b, 10, 3, 10);
		    }
		if(right_channel < b)
		    {

			draw_vline(buffer, b, 63, 56, 1);
		    }
		else
		    {

			draw_vline(buffer, b, 63, 56, 10);
		    }
	    }
    }


void draw_freq_scale(uint8_t *const buffer,uint16_t freq_scaled)
    {
	for (uint8_t i = 40; i < 220; i++)
	    {
		if (i % 5 == 0)
		    {
			draw_vline(buffer, i, 63, 60, 5);
		    }

		if (i % 15 == 0)
		    {
			draw_vline(buffer, i, 57, 63, 5);
		    }
		if (freq_scaled == i - 20)
		    {
			//draw_vline(buffer, i, 53, 63, 5);
			draw_rect_filled(buffer, i, 52, i+1, 63, 10);
		    }
	    }

	select_font(&TomThumb);
	draw_text(buffer, "87,0", 33, 51, 5);
	draw_text(buffer, "108,0", 210, 51, 5);
    }


void draw_alarmsSource(uint8_t *const buffer)
    {

	select_font(&FreeMonoOblique9pt7b);
	switch (SettingsUserMenu.AlarmSource_A)
	    {
	    case JACK_1:
		draw_text(buffer, (char*) Jack_1, 110, 30, 5);
		break;
	    case JACK_2:
		draw_text(buffer, (char*) Jack_2, 110, 30, 5);
		break;
	    case RADIO:
		draw_text(buffer, (char*) Radio, 110, 30, 5);
		break;
	    case BLUETOOTH:
		draw_text(buffer, (char*) Bluetooth, 110, 30, 5);
		break;
	    case MICROPHONE:
		draw_text(buffer, (char*) Microphone, 110, 30, 5);
		break;
	    default:
		break;
	    }

	select_font(&FreeMonoOblique9pt7b);
	switch (SettingsUserMenu.AlarmSource_B)
	    {
	    case JACK_1:
		draw_text(buffer, (char*) Jack_1, 110, 45, 5);
		break;
	    case JACK_2:
		draw_text(buffer, (char*) Jack_2, 110, 45, 5);
		break;
	    case RADIO:
		draw_text(buffer, (char*) Radio, 110, 45, 5);
		break;
	    case BLUETOOTH:
		draw_text(buffer, (char*) Bluetooth, 110, 45, 5);
		break;
	    case MICROPHONE:
		draw_text(buffer, (char*) Microphone, 110, 45, 5);
		break;
	    default:
		break;
	    }
    }


void draw_pointer(uint8_t *const buffer)
    {

	select_font(&FreeSerif24pt7b);
	switch (SettingsUserMenu.SETTINGS_USER_MENU)
	    {
	    case REFRESH_SCREEN_TIME:
		draw_text(buffer, ".", 2, 11, 5);
		break;
	    case ALARM_SOURCE_A:
		draw_text(buffer, ".", 2, 26, 5);
		break;
	    case ALARM_SOURCE_B:
		draw_text(buffer, ".", 2, 41, 5);
		break;
	    case USER_NAME:
		draw_text(buffer, ".", 2, 56, 5);
		break;
	    case DISPLAY_MODE_ON_OFF:
		draw_text(buffer, ".", 2, 11, 5);
		break;
	    case POWER_LED:
		draw_text(buffer, ".", 2, 26, 5);
		break;
	    default:
		break;
	    }
    }


void draw_refreshTime(uint8_t *const buffer)
    {
	switch (SettingsUserMenu.RefreshScreenTime)
	    {
	    case REFRESH_TIME_240s:
		draw_text(buffer, "240 s", 160, 15, 5);
		//also set double cnt flag
		break;
	    case REFRESH_TIME_120s:
		draw_text(buffer, "120 s", 160, 15, 5);
		break;
	    case REFRESH_TIME_60s:
		draw_text(buffer, "60 s", 160, 15, 5);
		break;
	    case REFRESH_TIME_30s:
		draw_text(buffer, "30 s", 160, 15, 5);
		break;
	    case REFRESH_TIME_10s:
		draw_text(buffer, "10 s", 160, 15, 5);
		break;
	    case 65535:
		draw_text(buffer, "OFF", 160, 15, 5);
		break;
	    default:
		break;
	    }
    }


void draw_powerLED(uint8_t *const buffer)
    {

	select_font(&FreeMonoOblique9pt7b);
	switch (SettingsUserMenu.Power_LED)
	    {
	    case POWER_OFF:
		draw_text(buffer, (char*) Power_OFF_str, 125, 30, 5);
		break;
	    case POWER_ON:
		draw_text(buffer, (char*) Power_ON_str, 125, 30, 5);
		break;
	    case Always_OFF:
		draw_text(buffer, (char*) Always_OFF_str, 125, 30, 5);
		break;
	    case Always_ON:
		draw_text(buffer, (char*) Always_ON_str, 125, 30, 5);
		break;
	    case Change_brigh:
		draw_text(buffer, (char*) Change_brigh_str, 125, 30, 5);
		break;
	    default:
		break;
	    }
    }


void draw_displayMode(uint8_t *const buffer)
    {
	select_font(&FreeMonoOblique9pt7b);
	switch (SettingsUserMenu.Display_mode)
	    {
	    case Disp_normal:
		draw_text(buffer, (char*) Disp_normal_str, 160, 15, 5);
		break;
	    case Disp_changing:
		draw_text(buffer, (char*) Disp_changing_str, 160, 15, 5);
		break;
	    case Disp_standby:
		draw_text(buffer, (char*) Disp_standby_str, 160, 15, 5);
		break;
	    case Disp_time:
		draw_text(buffer, (char*) Disp_time_str, 160, 15, 5);
		break;
	    default:
		break;
	    }
    }


//--------------------------------------------------------------
// Operation on display state fuctions
//--------------------------------------------------------------


void set_saved_display(TIM_HandleTypeDef *htim)
    {
	encoderVolFront.audioOutputState = MASTER;
	encoderVolBack.audioOutputState = NORMAL;
	SSD1322_Screen_State = SSD1322_Screen_State_Saved;
	SSD1322_Screen_State_Saved = 0;
	HAL_TIM_OC_Stop_IT(&htim15, TIM_CHANNEL_2);
	htim->Instance->CNT = 0;
    }


void save_current_display_state(SSD1322_Screen_t  Screen_State)
    {
	if (SSD1322_Screen_State != Screen_State)
	    {
		if (SSD1322_Screen_State != (11)
			&& SSD1322_Screen_State != 20 && SSD1322_Screen_State != 21
			&& SSD1322_Screen_State != 22 && SSD1322_Screen_State != 23
			&& SSD1322_Screen_State != 24 && SSD1322_Screen_State != 30
			&& SSD1322_Screen_State != 31 && SSD1322_Screen_State != 32
			&& SSD1322_Screen_State != 33 && SSD1322_Screen_State != 34
			&& SSD1322_Screen_State != 35 && SSD1322_Screen_State != 36)
		    {
			SSD1322_Screen_State_Saved = SSD1322_Screen_State;
		    }
		SSD1322_Screen_State = Screen_State;
	    }
    }


void change_display_state(TIM_HandleTypeDef *htim)
    {
	if (SettingsUserMenu.RefreshScreenTime != 65535) // change display if timer is set
	    {
		if ((SSD1322_Screen_State >= SSD1322_SCREEN_TIME)
			&& (SSD1322_Screen_State <= SSD1322_SCREEN_TIME_BOUNCING))
		    {
			SSD1322_Screen_State++;
			if (SSD1322_SCREEN_SETINPUT == SSD1322_Screen_State)
			    SSD1322_Screen_State = SSD1322_SCREEN_TIME;
			htim->Instance->CNT = 0;

			/* Check if ADC is needed */
			if ((SSD1322_Screen_State == SSD1322_SCREEN_UVMETER)
				|| (SSD1322_Screen_State == SSD1322_SCREEN_FFT))
			    {
				HAL_TIM_Base_Start(&htim6);
				HAL_ADC_Start_DMA(&hadc1, (uint32_t*) ADC_SamplesTEST, UV_meter_numb_of_chan);
				ADC_IS_ON_flag = true;
			    }
			else
			    {
				HAL_TIM_Base_Stop(&htim6);
				HAL_ADC_Stop_DMA(&hadc1);
				ADC_IS_ON_flag = false;
			    }
		    }
	    }
    }


void refresh_timer_state(void)
    {
	if (TIM_CHANNEL_STATE_GET(&htim15, TIM_CHANNEL_2) == HAL_TIM_CHANNEL_STATE_READY)
	    {
		HAL_TIM_OC_Start_IT(&htim15, TIM_CHANNEL_2);
	    }
	if (TIM_CHANNEL_STATE_GET(&htim15, TIM_CHANNEL_2) == HAL_TIM_CHANNEL_STATE_BUSY)
	    {
		htim15.Instance->CNT = 0;
	    }
    }


void ChangeDateToArrayChar(uint16_t frq)
    {
	if (frq >= RADIO_100MHZ_FREQ && frq <= RADIO_MAX_FREQ)
	    {
		ConvertArrayCharLong[4] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(0, frq);
		ConvertArrayCharLong[3] = PRZECINEK;
		ConvertArrayCharLong[2] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(1, frq);
		ConvertArrayCharLong[1] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(2, frq);
		ConvertArrayCharLong[0] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(3, frq);
		ConvertArrayCharLong[5] = ASCII_NULL;
	    }
	else if (frq < RADIO_100MHZ_FREQ && frq >= RADIO_MIN_FREQ)
	    {
		ConvertArrayCharLong[3] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(0, frq);
		ConvertArrayCharLong[2] = PRZECINEK;
		ConvertArrayCharLong[1] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(1, frq);
		ConvertArrayCharLong[0] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(2, frq);
		ConvertArrayCharLong[4] = ASCII_NULL;
	    }
	else //Invalid frq value
	    {
		for (uint8_t i = 0; i < 5; ++i)
		    {
			ConvertArrayCharLong[i] = 0;
		    }
	    }
    }


void ChangeDateToArrayCharTime(char *arrayChar, uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t mode)

    {
	if (hours < 9)
	    {
		arrayChar[0] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(0, hours);
		arrayChar[1] = ASCII_DWUKROPEK;
		arrayChar[2] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(1, minutes);
		arrayChar[3] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(0, minutes);
		if (0 == mode)
		    {
			arrayChar[4] = ASCII_DWUKROPEK;
			arrayChar[5] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(1, seconds);
			arrayChar[6] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(0, seconds);
			arrayChar[7] = ASCII_NULL;
		    }
		else if (1 == mode)
		    {
			arrayChar[4] = ASCII_NULL;
		    }

	    }
	else if (hours <= 24 && hours >= 10)
	    {
		arrayChar[0] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(1, hours);
		arrayChar[1] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(0, hours);
		arrayChar[2] = ASCII_DWUKROPEK;
		arrayChar[3] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(1, minutes);
		arrayChar[4] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(0, minutes);
		if (0 == mode)
		    {
			arrayChar[5] = ASCII_DWUKROPEK;
			arrayChar[6] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(1, seconds);
			arrayChar[7] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(0, seconds);
			arrayChar[8] = ASCII_NULL;
		    }
		else if (1 == mode)
		    {
			arrayChar[5] = ASCII_NULL;
		    }
	    }

    }

//
int16_t SplitNumberToDignits(int16_t position, int16_t number)
    {
	while(position--)
	    number /= 10;
	return number % 10;
    }

//
uint32_t map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min,
	uint32_t out_max)
    {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

void ConvertDateToBuffer(uint16_t Year, uint8_t Month, uint8_t WeekDay, uint8_t Date)
    {
	uint8_t temp = 0;
	for (uint8_t i = 0; i < 40; ++i)
	    {
		TestingArray[i] = 0;
	    }
	switch (WeekDay)
	    {
	    case 1:
		strcat(TestingArray, Poniedzialek);
		break;
	    case 2:
		strcat(TestingArray, Wtorek);
		break;
	    case 3:
		strcat(TestingArray, Sroda);
		break;
	    case 4:
		strcat(TestingArray, Czwartek);
		break;
	    case 5:
		strcat(TestingArray, Piatek);
		break;
	    case 6:
		strcat(TestingArray, Sobota);
		break;
	    case 7:
		strcat(TestingArray, Niedziela);
		break;
	    default:
		break;
	    }

	temp = strlen(TestingArray);
	TestingArray[temp] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(1, Date);
	TestingArray[temp + 1] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(0, Date);
	TestingArray[temp + 2] = PRZECINEK;

	switch (Month)
	    {
	    case 1:
		strcat(TestingArray, Styczen);
		break;
	    case 2:
		strcat(TestingArray, Luty);
		break;
	    case 3:
		strcat(TestingArray, Marzec);
		break;
	    case 4:
		strcat(TestingArray, Kwiecien);
		break;
	    case 5:
		strcat(TestingArray, Maj);
		break;
	    case 6:
		strcat(TestingArray, Czerwiec);
		break;
	    case 7:
		strcat(TestingArray, Lipiec);
		break;
	    case 8:
		strcat(TestingArray, Sierpien);
		break;
	    case 9:
		strcat(TestingArray, Wrzesien);
		break;
	    case 10:
		strcat(TestingArray, Pazdziernik);
		break;
	    case 11:
		strcat(TestingArray, Listopad);
		break;
	    case 12:
		strcat(TestingArray, Grudzien);
		break;
	    default:
		break;
	    }
	temp = strlen(TestingArray);
	TestingArray[temp] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(3, Year);
	TestingArray[temp + 1] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(2, Year);
	TestingArray[temp + 2] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(1, Year);
	TestingArray[temp + 3] = ASCII_DIGIT_OFFSET + SplitNumberToDignits(0, Year);
	TestingArray[temp + 4] = ASCII_NULL;
    }


_Bool prepare_RDS_text(char *RDS_text)
    {

	//if first chars are empty spaces then its not a good string to display
	if ((RDStextbuffer[0] == ' ') && (RDStextbuffer[1] == ' ') && (RDStextbuffer[2] == ' ') && (RDStextbuffer[3] == ' '))
	    {
		return false;
	    }
	if ((RDStextbuffer[0] == 0x00) && (RDStextbuffer[1] == 0x00) && (RDStextbuffer[2] == 0x00) && (RDStextbuffer[3] == 0x00))
	    {
		return false;
	    }
	//checks for end of the string
	for (uint8_t i = 0; i < 66; i++)
	    {
		if (RDStextbuffer[i] == ' ' && RDStextbuffer[i + 1] == ' ' && RDStextbuffer[i + 2] == ' '&& RDStextbuffer[i + 3] == ' '&& RDStextbuffer[i + 4] == ' '&& RDStextbuffer[i + 5] == ' ')
		    {

			RDStextbuffer[i] = 0x00; //add end of the string
			if(i > 40)
			    {
				RDS_text[i - 39] = RDStextbuffer[i];
			    }
		    }

	    }

	return true;
    }


_Bool  get_random_coords(uint32_t *random_x, uint32_t *random_y)
    {
	 uint32_t temp;
	 static uint32_t random2;

	if (hrng.ErrorCode == 2) //da?? inny warunek, uniwersalny
	    {
		HAL_RNG_DeInit(&hrng);
		HAL_RNG_Init(&hrng);
	    }

	HAL_RNG_GenerateRandomNumber(&hrng, &random2);
	if (hrng.Instance->SR == 1) //new random data ready
	    {
		temp = hrng.RandomNumber;
		*random_x = (uint8_t) temp; 	   //get 8 bits
		*random_y = (uint8_t)(temp >> 26); //get other 6 bits
		//don't let the clock be outside of display
		if(*random_x >= 195)
		    {
			*random_x -= 59;
		    }
		if(*random_y <=  13)
		    {
			*random_y += 13;
		    }
		return true;
	    }
	return false;
    }


void make_array(uint8_t *frame_buffer, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t brightness)
{
	int16_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep == 1)
	{
		uint16_t tmp = y0;
		y0 = x0;
		x0 = tmp;
		tmp = y1;
		y1 = x1;
		x1 = tmp;
	}

	if (x0 > x1) //okre??la kierunek rysowania linii
	{
		uint16_t tmp = x0;
		x0 = x1;
		x1 = tmp;
		tmp = y0;
		y0 = y1;
		y1 = tmp;
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1)
	{
		ystep = 1;
	}
	else
	{
		ystep = -1;
	}

	for (; x0 <= x1; x0++)
	{
		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

		if (steep)
		{	if(SSD1322_Screen_State != SSD1322_SCREEN_TIME_BOUNCING) return;
			fill_buffer(frame_buffer, 0);
			HAL_Delay(50);
			ChangeDateToArrayCharTime(ConvertArrayCharTime, sTime.Hours, sTime.Minutes, sTime.Seconds, 0);
			draw_text(frame_buffer, (char*) ConvertArrayCharTime, y0, x0, brightness);
			send_buffer_to_OLED(frame_buffer, 0, 0);
		}
		else
		{	if(SSD1322_Screen_State != SSD1322_SCREEN_TIME_BOUNCING) return;
			fill_buffer(frame_buffer, 0);
			HAL_Delay(50);
			ChangeDateToArrayCharTime(ConvertArrayCharTime, sTime.Hours, sTime.Minutes, sTime.Seconds, 0);
			draw_text(frame_buffer, (char*) ConvertArrayCharTime, x0, y0, brightness);
			send_buffer_to_OLED(frame_buffer, 0, 0);
			//mo??na doda?? flag?? volatile kt??ra jest sprawdzana je??eli zmieniono ekran
		}
		err -= dy;
		if (err < 0)
		{
			y0 += ystep;
			err += dx;
		}
	}
}

