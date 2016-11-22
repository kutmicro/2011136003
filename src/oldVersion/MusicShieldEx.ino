#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX(수신), TX(송신) 
                                 // 아두이노는 시리얼 통신을 통해 다른 기기들과 데이터를 주고 받을 수 있음

byte note = 0;        // MIDI에 저장되어 있는 음계값
byte resetMIDI = 4;   // VS1053 Reset용 핀
byte ledPin = 13;     // MIDI 트래픽 표시용 핀
int  instrument = 0;  // MIDI에 저장되어 있는 악기종류

void setup() {
  Serial.begin(57600);  // PC와의 통신속도 설정
  
  // 미디컨트롤을 위한 소프트웨어 시리얼을 준비
  mySerial.begin(31250);  // 뮤직쉴드와의 통신속도 설정

  // VS1053(미디 음원 칩)을 초기화하고 사용할 준비
  pinMode(resetMIDI, OUTPUT);     // 아두이노의 digital I/O 핀을 사용하려면 먼저 pinMode()함수로 각각의 핀을 입력 또는 출력 모드로 사용하겠다고 선언
  digitalWrite(resetMIDI, LOW);   // 특정 핀을 HIGH(5V) 또는 LOW(0V)로 만드는 함수
  delay(100);
  digitalWrite(resetMIDI, HIGH);
  delay(100);
  talkMIDI(0xB0, 0x07, 120); 
  // talkMIDI(byte Byte1, byte Byte2, byte Byte3)
  
  // Byte1 : 메시지 타입으로써 이 메시지의 유형에 따라 다르게 실행된다.
  // 0xB0 : "control change", 악기를 포괄하는 단위인 "bank"를 선택하는 함수를 실행
  // 0xC0 : "program change", "bank" 내부의 악기를 선택하는 함수를 실행
  
  // Byte2 : Byte1의 입력값에 따라 실행결과가 다르다
  // Byte1의 값이 0xB0 이면, 127개의 controller가 있는데 이 값이 0이면 "bank selector"를 의미 (대부분 0)
  // Byte1의 값이 0xC0 이면, 원하는 악기의 번호를 넣으면 해당 악기로 변경

  // Byte3
  // 0xB0 : "bank selector"일 때 이 값에 해당하는 bank로 변경
  // 0xC0 : 0으로 고정
}

void loop() {
  Serial.println("Basic Instruments");
  talkMIDI(0xB0, 0, 0x00); // default 값으로 bank 설정 (GM1)

  // bank 내부의 악기종류 설정
  for(instrument = 0 ; instrument < 127 ; instrument++) {

    Serial.print(" Instrument: ");
    Serial.println(instrument, DEC);

    talkMIDI(0xC0, instrument, 0); // 설정된 bank 내부의 변수 instrument의 값에 해당하는 악기로 설정

    // 설정된 note값에 해당하는 MIDI 내부의 음계 출력
    for (note = 30 ; note < 40 ; note++) {
      Serial.print("N:");
      Serial.println(note, DEC);
      
      //Note on channel 1 (0x90), some note value (note), middle velocity (0x45):
      noteOn(0, note, 60);
      delay(50);

      //Turn off the note with a given off/release velocity
      noteOff(0, note, 60);
      delay(50);
    }

    delay(100); //Delay between instruments
  }
}


//channel ranges from 0-15

// noteOn(byte Byte1, byte Byte2, byte Byte3) : 해당 음계를 출력하는 함수
// Byte1 : 채널값으로 뮤직쉴드를 1개만 사용하므로 0으로 고정
// Byte2 : 해당하는 음계값 
// Byte3 : 소리의 세기를 설정
void noteOn(byte channel, byte note, byte attack_velocity) {
  talkMIDI( (0x90 | channel), note, attack_velocity);
}

// noteOff(byte Byte1, byte Byte2, byte Byte3) : 해당 음계의 음의 출력을 중단하는 함수
// noteOn과 정반대의 역할을 하는 함수
void noteOff(byte channel, byte note, byte release_velocity) {
  talkMIDI( (0x80 | channel), note, release_velocity);
}


void talkMIDI(byte cmd, byte data1, byte data2) {
  digitalWrite(ledPin, HIGH);
  mySerial.write(cmd);    // SoftwareSerial : write(data)
                          // prints data to the transmit pin of the software serial port as raw bytes. Works the same as the Serial.write() function
  mySerial.write(data1);

  
  if( (cmd & 0xF0) <= 0xB0)
    mySerial.write(data2);

  digitalWrite(ledPin, LOW);
}
