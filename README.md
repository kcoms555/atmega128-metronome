# atmega128-metronome
A metronome on Atmega128

### 개요
  메트로놈은 음악 장비 중 하나로, 일정한 박자를 생성해주는 장비입니다. 분당 박자 수 BPM(Beats Per Minute)를 조절하여 각 박자가 시작하는 타이밍에 소리 내는 것이 주요 역할입니다. 더 나아가 LED를 활용하여 청각뿐만 아니라 시각적으로도 타이밍을 알 수 있게 만들었습니다. 해당 임베디드 시스템은 5가지로 구성되어있습니다. 타이머 장치, 박자 제어 장치, BPM 표시 장치, LED 표시 장치, 버저 장치입니다.
1. 타이머 장치                                                                                     
  	atmega128에서 지원하는 타이머 인터럽트를 이용하여 16ms 단위의 정확도를 가진 타이머 장치를 내부적으로 만듭니다.
2. 박자 제어 장치                                                                                     
  	외부에 버튼을 3개를 두어 박자를 제어합니다. 1개는 BPM 1 증가 버튼, 1개는 BPM 1 감소 버튼, 1개는 누르는 박자에 맞춰 BPM 설정해주는 버튼입니다. 마지막 BPM을 설정해주는 버튼은 타이머 장치와 큐를 이용하여 최근 버튼을 누른 시간 간격의 평균으로 BPM을 추정합니다.
3. BPM 표시 장치                                                                                     
  	atmega128에 내장되어있는 FND를 이용하여 현재 BPM을 표시합니다.
4. LED 표시 장치                                                                                     
  	LED를 이용하여 시각적으로 박자가 어디에 있는지 표시합니다. LED가 한 사이클을 다 돌면 한 박자가 진행된 것입니다.
5. 버저 장치                                                                                    
  	버저를 이용하여 매 박자마다 소리를 출력합니다.

### 실행
1. avr 기종 전용 c컴파일러 avr-gcc와 avr에 ROM과 EEPROM 이미지를 업로드하는 avrdude를 설치합니다.  
2. Makefile 을 연 후, flash 항목에서 flash 목적지 '/dev/...'를 atmega128 디바이스로 설정합니다.  
3. 명령어 'make && make flash' 를 실행합니다.  

### 결과
  60BPM 기준 완전히 1박자가 어긋나는데 걸리는 시간과 BPM 추정의 정확도를 측정하여 음악적으로 메트로놈으로서 사용할 수 있는 정도인지 신뢰성을 확인하였습니다.  
#### 1. 60BPM 기준 완전히 1박자가 어긋나는데 걸리는 시간 결과                                                                                
  직접 만든 메트로놈과 휴대폰의 메트로놈 어플리케이션을 이용하여 해당 시간을 측정한 결과, 4분 16초가 걸렸습니다. 이는 표기한 BPM과 실제 BPM의 오차가 약 0.39%이라는 것을 의미합니다. 실제로 연주할 음악 길이가 4분 길이 일때, 약 1초가 어긋난다는 것으로, 단순한 음악 연습용 메트로놈으로는 충분한 정확도를 보장합니다.  
#### 2. BPM 추정의 정확도 측정                                                                                    
 직접 만든 메트로놈을 이용하여 실제 음악 4곡의 BPM을 추정해보았습니다. 다양한 템포를 가진 곡들을 실제로 BPM을 측정하고, 실제 악보상 기입되어있는 BPM과 대조하여 정확도를 측정하였습니다.  
 1. 80 BPM 이하의 음악 - 좋니(윤종신) : 측정값 - 77 BPM, 실제 악보상 BPM - 78 BPM  
 2. 80 BPM ~ 120 BPM 의 음악 - 촛불하나(GOD) : 측정값 - 101 BPM, 실제 악보상 BPM - 100 BPM  
 3. 120 BPM ~ 180 BPM 의 음악 - 비밀번호 486(윤하) : 측정값 - 138 BPM, 실제 악보상 BPM - 137 BPM  
 4. 180 BPM 이상의 음악 - American Idiot(Green Day) : 측정값 - 184 BPM, 실제 악보상 BPM - 186 BPM  

실제로 측정한 결과, 재생 중인 음악의 BPM을 측정하는데 참값과 1~2 내외의 오차를 보이고, 해당 메트로놈에서 재생되는 박자가 실제 BPM과의 오차가 4분에 약 1초가 어긋나므로 연습용으로 충분히 활용 가능하다는 것을 확인하였습니다  

### 부품 목록
  연결에 사용되는 전선, 저항, 브레드보드 등 부품들은 제외하였습니다   
부저 : 아두이노 패시브 부저 모듈  
메인 LED : 아두이노 WS2812 풀컬러 16 링 LED 모듈  
보조 LED : 5파이 LED - 슈퍼고휘도  
버튼 : ITS-1105-5mm  
