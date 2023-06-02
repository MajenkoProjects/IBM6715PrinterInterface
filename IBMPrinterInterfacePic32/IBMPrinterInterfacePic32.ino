#define PIN_DTR 9
#define PIN_DSR 10

USBFS usbDevice;
USBManager USB(usbDevice, 0xf055, 0x6715, "IBM", "ActionWriter 6715");
CDCACM uSerial;

const uint8_t descriptor[] = {
	9, 4, 0, 0, 2, 7, 1, 2, -0,
	7, 5, -0x01, 2, 0, 64, 0,
//	7, 5, -0x81, 2, 0, 8, 0
};

class USBPrinter : public USBDevice, public Stream {
	private:
		USBManager *_manager;
		uint8_t _ifBulk;
		uint8_t _epBulk;

        uint8_t _bulkRxA[64];
        uint8_t _bulkRxB[64];

		uint8_t _buffer[32768];
		volatile uint32_t _head;
		volatile uint32_t _tail;

	public:

		USBPrinter() : _head(0), _tail(0) {}

        bool getReportDescriptor(uint8_t __attribute__((unused)) ep, uint8_t __attribute__((unused)) target, uint8_t __attribute__((unused)) id, uint8_t __attribute__((unused)) maxlen)  { return false; }
        bool getStringDescriptor(uint8_t __attribute__((unused)) idx, uint16_t __attribute__((unused)) maxlen) { return false; }

		uint16_t getDescriptorLength() { return 9+7; }
		uint8_t getInterfaceCount() { return 1; }
		uint32_t populateConfigurationDescriptor(uint8_t *buf) {
			uint8_t i = 0;
			buf[i++] = 		9;
			buf[i++] = 		4;
			buf[i++] = 		0;
			buf[i++] = 		0;
			buf[i++] = 		2;
			buf[i++] = 		7;
			buf[i++] = 		1;
			buf[i++] = 		1; 
			buf[i++] = 		_ifBulk;

			buf[i++] = 		7;
			buf[i++] = 		5;
			buf[i++] = 		_epBulk;
			buf[i++] = 		2;
			buf[i++] = 		0x40;
			buf[i++] = 		0x00;
			buf[i++] = 		0;

			return i;
		}

		void initDevice(USBManager *manager) {
			_manager = manager;
			_ifBulk = _manager->allocateInterface();
			_epBulk = _manager->allocateEndpoint();
		}

		bool getDescriptor(uint8_t __attribute__((unused)) ep, uint8_t __attribute__((unused)) target, uint8_t __attribute__((unused)) id, uint8_t __attribute__((unused)) maxlen) {
    		return false;
		}

		void configureEndpoints() {
        	_manager->addEndpoint(_epBulk, EP_IN, EP_BLK, 64, _bulkRxA, _bulkRxB);
		}

		bool onSetupPacket(uint8_t ep, uint8_t __attribute__((unused)) target, uint8_t *data, uint32_t l) {
			if (ep == 0) {
				if (data[0] == 0xA1) {
					if (data[1] == 0) {
                		_manager->sendBuffer(0, NULL, 0);
						return true;
					}
					if (data[1] == 1) {
						uint8_t out = 0b00011000;
						_manager->sendBuffer(0, &out, 1);
						return true;
					}
				}
			}
			return false;
		}

		bool onInPacket(uint8_t ep, uint8_t target, uint8_t __attribute__((unused)) *data, uint32_t l) {
			if (ep == 0) {
				if (data[0] == 0xA1) {
					if (data[1] == 0) {
                		_manager->sendBuffer(0, NULL, 0);
						return true;
					}
					if (data[1] == 1) {
						uint8_t out = 0b00011000;
						_manager->sendBuffer(0, &out, 1);
						return true;
					}
				}
			}
    		return false;
		}

		bool onOutPacket(uint8_t ep, uint8_t target, uint8_t *data, uint32_t l) {

			if (ep == _epBulk) {

		        for (uint32_t i = 0; i < l; i++) {
        		    uint32_t bufIndex = (_head + 1) % 32768;
            		if (bufIndex != _tail) {
                		_buffer[_head] = data[i];
                		_head = bufIndex;
            		}
        		}

//        		int remaining = (_tail - _head + 32768) % 32768;
//
//        		if ((remaining < 2000) && (_head != _tail)) {
//            		_manager->haltEndpoint(_epBulk);
//        		}
        		return true;

			}

			return false;
		}

		size_t write(uint8_t b) {
			return 1;
		}

		int available() {
			return (32768 + _head - _tail) % 32768;
		}

		int availableForWrite() {
			return 0;
		}

		int read() {
		    if (_head == _tail) return -1;
    		int prevremaining = (_tail - _head + 32768) % 32768;

    		uint8_t ch = _buffer[_tail];
    		_tail = (_tail + 1) % 32768;

//    		int remaining = (_tail - _head + 32768) % 32768;
//
//    		if (prevremaining < 32000) {
//        		if ((remaining >= 32000) || (_head == _tail)) {
//            		_manager->resumeEndpoint(_epBulk);
//        		}
//    		}

    		return ch;
		}

		void flush() {
		}

		operator int() {
			return 1;
		}

		int peek() {
			return -1;
		}

		void onEnumerated() {
		}

};



       
USBPrinter Printer;

		
/*
 * Daisy wheel character sequence
 * . , - v l m j w ² µ f Yen > Para + 1 2 3 4 5 6 7 8 9 0
 * E £ B F P S Z V & Y A T L $ R * C " D ? N T U ) W _ =
 * ; : M ' H ( K / O ! X SS Q J % ³ G º ¼ Cent ½ < Delta 
 * # t x q Ω ] @ [ y k p h c g n r s e a i d u b o z 
 */


// The character codes are arranged in the order of the characters
// on the daisywheel. 
const uint8_t ibmscii[256] = {
	// 0-31 - control codes, not used
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,

	0x00, 0x3e, 0x2b, 0x4c, 0x27, 0x43, 0x22, 0x0e,
	0x3a, 0x31, 0x29, 0x0f, 0x02, 0x03, 0x01, 0x3c,
	// v-- 0x19, but my 0 is broken
	0x3d, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
	0x17, 0x18, 0x36, 0x35, 0x48, 0x34, 0x0d, 0x2d,
	0x40, 0x24, 0x1c, 0x2a, 0x2c, 0x1a, 0x1d, 0x45,
	0x39, 0x2f, 0x42, 0x3b, 0x26, 0x37, 0x2e, 0x3d,
	0x1e, 0x41, 0x28, 0x1f, 0x25, 0x30, 0x21, 0x32,
	0x3f, 0x23, 0x20, 0x4b, 0x49, 0x47, 0x0c, 0x33,
	0x44, 0x5e, 0x62, 0x58, 0x60, 0x5d, 0x0b, 0x59,
	0x57, 0x5f, 0x07, 0x55, 0x05, 0x06, 0x5a, 0x63,
	0x56, 0x4f, 0x5b, 0x5c, 0x4d, 0x61, 0x04, 0x08,
	0x4e, 0x54, 0x64, 0x53, 0x52, 0x51, 0x50, 0x0a,
	0x46, 0x09, 0x38, 0x48, 0x50, 0x1b, 0x00, 0x00, 
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

// Send a single byte and then wait for the ack pulse to go high then low.
void sendByte(uint8_t b) {
	Serial.write(b);
	uint32_t ts = millis();
	while (digitalRead(PIN_DTR) == LOW) {
		if (millis() - ts > 10000) {
			Serial.println("Timeout Low");
			return;
		}
	}
	ts = millis();
	while (digitalRead(PIN_DTR) == HIGH) {
		if (millis() - ts > 10000) {
			Serial.println("Timeout High");
			return;
		}
	}
}

uint32_t linepos = 0;

void initPrinter() {
	sendByte(0xA1);
	sendByte(0x00);
	sendByte(0xA4);
	sendByte(0x00);
	sendByte(0xA2);
	sendByte(0x00);
	sendByte(0x82);
	sendByte(0x1F);

// These two will create a margin like the default typewriter.
	sendByte(0xC0);
	sendByte(0x78);

// Set to a 10-pitch font
	sendByte(0x80);
	sendByte(12); // 1/10"
}

void lf() {
	sendByte(0xd0);
	sendByte(16);
}


void advance() {
	sendByte(0xC0);
	sendByte(12);
	linepos += 12;
}

void backspace() {
	if (linepos < 12) {
		return;
	}
	sendByte(0xE0);
	sendByte(12);
	linepos -= 12;
}

void printCharacter(uint8_t c) {
	sendByte(ibmscii[c]);
	// We're using manual advancing
	sendByte(0x20);
}

void cr() {
	uint8_t l = linepos & 0xFF;
	uint8_t h = linepos >> 8;
	sendByte(0xE0 | h);
	sendByte(l);
	linepos = 0;
}

void printLine(char *text) {

	for (int i = 0; i < strlen(text); i++) {
	}

	cr();
	lf();
}

void setup() {
	Serial.begin(4800);

	USB.addDevice(Printer);
	USB.addDevice(uSerial);
	USB.begin();


	pinMode(PIN_DTR, INPUT);
	pinMode(PIN_DSR, OUTPUT);
	digitalWrite(PIN_DSR, LOW);

	pinMode(LED_BUILTIN, OUTPUT);

	initPrinter();
}

void loop() {

	if (Printer.available()) {
		char c = Printer.read();

		switch (c) {
			case '\n':
				lf();
				break;

			case '\r':
				cr();
				break;

			case 8:
				backspace();
				break;
			
			default:
				printCharacter(c);
				advance();
				break;
		}
	}
}

