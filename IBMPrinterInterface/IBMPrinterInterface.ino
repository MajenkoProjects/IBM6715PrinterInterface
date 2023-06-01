#define PIN_DTR 2
#define PIN_DSR 3

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

void sendByte(uint8_t b) {
	Serial1.write(b);
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
char linebuffer[128] = {0};
uint32_t bufferpos = 0;

void initPrinter() {
	sendByte(0xA1);
	sendByte(0x00);
	sendByte(0xA4);
	sendByte(0x00);
	sendByte(0xA2);
	sendByte(0x00);
	sendByte(0x82);
	sendByte(0x1F);
	sendByte(0xC0);
	sendByte(0x78);

	sendByte(0x80);
	sendByte(12); // 1/10"

}

void lf() {
	sendByte(0xd0);
	sendByte(16);
}


void advance() {
	sendByte(0xc0);
	sendByte(12);
	linepos += 12;
}

void printCharacter(uint8_t c) {
	sendByte(ibmscii[c]);
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
		printCharacter(text[i]);
		advance();
	}

	cr();
	lf();
}

void setup() {
	Serial.begin(115200);
	Serial1.begin(4800);

	pinMode(PIN_DTR, INPUT);
	pinMode(PIN_DSR, OUTPUT);
	digitalWrite(PIN_DSR, LOW);

	pinMode(LED_BUILTIN, OUTPUT);

	initPrinter();
}

void loop() {

	if (Serial.available()) {
		char c = Serial.read();

		switch (c) {
			case '\n':
				printLine(linebuffer);
				bufferpos = 0;
				linebuffer[0] = 0;
				break;
			
			default:
				if (bufferpos < 100) {
					linebuffer[bufferpos++] = c;
					linebuffer[bufferpos] = 0;
				}
				break;
		}
	}
}

