#include <Wire.h>
#include <Button.h>
#include <LiquidCrystal.h>

#include <menu.h>
#include <RTClib.h>

LiquidCrystal _lcd(13, 12, 6, 5, 4, 3);

const int numRows = 2;
const int numCols = 16;

TimeProvider tp;
Lcd lcd(_lcd, numCols, numRows, ' ');

Button btnUp = Button(10, PULLDOWN);
Button btnDown = Button(9, PULLDOWN);
Button btnMenu = Button(11, PULLDOWN);
String menuItems[] = { String("Exit"), String("Alarm 1"), String("Alarm 2"), String("Time"), String("Summer time"), String("Date") };

bool printMenu(MenuInternals* internals) {
	String s = String(internals->currentIndex) + internals->items[internals->currentIndex];
	lcd.showString(s, 1);
	return false;
}

MenuCallback* callbacks[] = { new ExitMenuCallback(), new SimpleMenuCallback(printMenu), new SimpleMenuCallback(printMenu), new SimpleMenuCallback(printMenu), new SimpleMenuCallback(printMenu), new SimpleMenuCallback(printMenu), };

class SerialLogger : public Logger {
public:
	virtual bool isLogEnabled() {
		return true;
	}
	virtual void log(const char* msg, bool newLine = true) {
		Serial.print("log: const char* size:");
		if (msg != NULL) {
			int len = strlen(msg);
			Serial.print(len);
		}
	}

	virtual void log(const __FlashStringHelper* msg, bool newLine = true) {
		Serial.print("log: const __FlashStringHelper* size:");
		if (msg != NULL) {
			if (newLine) {
				//Serial.println(msg);
			} else {
				//Serial.print(msg);
			}
		}
	}

	virtual void logInt(int value, bool newLine = true) {
		if (newLine) {
			Serial.println(value);
		} else {
			Serial.print(value);
		}
	}

	virtual void logLong(long value, bool newLine = true) {
		if (newLine) {
			Serial.println(value);
		} else {
			Serial.print(value);
		}
	}
	virtual void logFloat(float value, bool newLine = true) {
		if (newLine) {
			Serial.println(value);
		} else {
			Serial.print(value);
		}
	}
	virtual void logDouble(double value, bool newLine = true) {
		if (newLine) {
			Serial.println(value);
		} else {
			Serial.print(value);
		}
	}
};

SerialLogger logger;
Menu m = Menu(logger, tp, lcd, btnUp, btnDown, btnMenu, menuItems, 6, 15, NULL, callbacks);

//Menu m = Menu(tp, lcd, btnUp, btnDown, btnMenu, menuItems, 6, 15, NULL, 1, printMenu, printMenu);

RTC_DS1307 rtc;
String timeSeparator(":");
String dateSeparator("/");

void beepKey() {
	tone(2, 2000, 100);
}

void setup() {
	Serial.begin(57600);

	pinMode(9, INPUT);
	pinMode(10, INPUT);
	pinMode(11, INPUT);
	pinMode(2, OUTPUT);

	_lcd.begin(numCols, numRows);
	_lcd.setCursor(0, 0);
	// _lcd.print("Press key");
	//_lcd.print("1234567891123456789212345678931234567894");
	Wire.begin();
	rtc.begin();

	if (!rtc.isrunning()) {
		Serial.println("RTC is NOT running!");
		// following line sets the RTC to the date & time this sketch was compiled
		//RTC.adjust(DateTime(__DATE__, __TIME__));
		beepKey();
	}

	beepKey();

	logger.log("Clock started");
}

void leadingZero(String& dest, int v, int digits = 2) {
	String s(v);
	while (s.length() < digits) {
		s = String("0") + s;
	}
	dest += s;
}

void loop() {
	DateTime now = rtc.now();

	String timeStr;
	leadingZero(timeStr, now.hour());
	timeStr += timeSeparator;
	leadingZero(timeStr, now.minute());
	timeStr += timeSeparator;
	leadingZero(timeStr, now.second());

	String dateStr;
	leadingZero(dateStr, now.day());
	dateStr += dateSeparator;
	leadingZero(dateStr, now.month());
	dateStr += dateSeparator;
	leadingZero(dateStr, now.year(), 4);

	//lcd.animate("+*", 500, 2);
	if (btnMenu.uniquePress()) {
		Serial.println("Menu button pressed.");
		beepKey();
		while (btnMenu.isPressed()) {
			// wait for button to be unpressed
			Serial.println("Waiting for Menu button to be unpressed.");
		}
		Serial.println("Showing menu.");
		m.show();
		Serial.println("Exiting menu");
	} else {
		lcd.showString(dateStr, 0);
		lcd.showString(timeStr, 1);
	}
}

