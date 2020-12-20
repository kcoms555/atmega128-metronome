#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "queue.c"
#include "WS2811.h"

#define LEDNUM 17
#define COLORNUM 8

DEFINE_WS2811_FN(WS2811RGB, PORTE, 1)
RGB_t led[LEDNUM];
RGB_t colors[8];

#define BIT(B)           (0x01 << (uint8_t)(B))
#define SET_BIT_HI(V, B) (V) |= (uint8_t)BIT(B)
#define SET_BIT_LO(V, B) (V) &= (uint8_t)~BIT(B)

#ifndef ARRAYLEN
#define ARRAYLEN(A) (sizeof(A) / sizeof(A[0]))
#endif

static volatile unsigned int bpm;
static unsigned long time_gap; // 박자간 시간 간격(ms)
static double position = 0; // LED 위치 : 0~1 사이의 값
static int color = 4;
static unsigned long now_time = 0;
static unsigned long pos_time = 0;
static unsigned char fnd_digit[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7c, 0x07, 0x7f, 0x67};
static unsigned char fnd_sel[4] = {0x01, 0x02, 0x04, 0x08};
static queue * q = NULL;
#define TCNT_COUNT_VALUE 256-250;  //250 번 카운트 되면 인터럽트 발생
#define NUM_OF_EMBEDED_LED 8

void display_fnd(int);
void display_embeded_led(double);
void display_external_led(double);
void add_bpm();
void set_bpm();
void play_external_buzz(double);
void play_embeded_buzz(double);
void clear_time();
void init_colors();  
void display_external_circle_led(double position, int color);
void led_one(int target, int c);

void display_fnd(int number_4_digit) {
	int i, fnd[4];
	fnd[3] = (number_4_digit/1000)%10;
	fnd[2] = (number_4_digit/100)%10;
	fnd[1] = (number_4_digit/10)%10;
	fnd[0] = number_4_digit%10;
	for (i=0; i<4; i++) {
		PORTC = fnd_digit[fnd[i]];
		PORTG = fnd_sel[i];
		_delay_ms(2);
	}
}

void display_embeded_led(double position){
	PORTA = 1<<(int)(position * NUM_OF_EMBEDED_LED);
}

void display_external_led(double position){
	if(position < 0.04)
		PORTE |= 0x04;
	else
		PORTE &= ~0x04;
}

void add_bpm(int num){
	set_bpm(bpm + num);
}
void set_bpm(int num){
	if(num < 1) num = 1;
	if(num > 1200) num = 1200;
	bpm = num;
	time_gap = 60000/bpm;
}
void play_embeded_buzz(double position){
	if(position < 0.04){
		if(PORTB & 0x10)
			PORTB &= ~0x10;
		else
			PORTB |= 0x10;
	}
}
void play_external_buzz(double position){
	if(position < 0.04){
		if(PORTE & 0x01)
			PORTE &= ~0x01;
		else
			PORTE |= 0x01;
	}
}
void clear_time(){
	pos_time = 0;
}
void set_color(double position){
	color = (int)(COLORNUM * position);
}
void add_color(){
	color++;
	if(color > 7)
		color = 0;
}

//bpm 증가 버튼
ISR(INT4_vect) {
	add_bpm(1);
	clear_time();
}
//bpm 감소 버튼
ISR(INT5_vect) {
	add_bpm(-1);
	clear_time();
}
//bpm 자동 버튼
ISR(INT6_vect) {
	if(now_time - get_front(q) < 100) return;
	insert_queue(q, now_time);
	set_bpm( (60000 / get_timegap_average(q)) * 0.8); // 버튼 시간 간격은 ms 단위, 0.8은 보정계수
	clear_time();
}

//현재 타이머 주기 : 16ms 마다 인터럽트 발생
ISR(TIMER0_OVF_vect){
	now_time += 16;
	pos_time += 16;
	position = (double)((pos_time) % time_gap)/time_gap;
	TCNT0 = TCNT_COUNT_VALUE;
}

int main(void)
{
	DDRA = 0xff; // A 포트는 LED 포트
	DDRB = 0x10; // B 포트는 내장 buzzer 포트
	DDRC = 0xff; // C 포트는 FND 데이터 신호
	DDRG = 0x0f; // G 포트는 FND 선택 신호

 	SET_BIT_HI(DDRE, 1);
    SET_BIT_LO(PORTE, 1);
	init_colors();

	TIMSK = 0b00000001; // 0번 타이머 인터럽트 활성화
	TCCR0 = 0b00000111; // 1024분주 ( 64us 마다 TCNT0 1 증가 시킴)
	TCNT0 = TCNT_COUNT_VALUE; // 250번 카운트 되면 인터럽트 발생 (16ms 마다 인터럽트 발생)

	q = create_queue(5);

	DDRE = 0b10001111; //PE6, PE5, PE4 스위치 입력으로 허용
	EICRB = 0b00101010; //PE6, PE5, PE4 ascending edge 입력
	EIMSK = 0b01110000; //INT6, INT5, INT4 인터럽트 허용
	sei(); //전역 인터럽트 활성화

	bpm = 60;
	time_gap = 60000/bpm;
	while (1){
		set_color(position);
		display_fnd(bpm); //약 8ms 지연 발생
		display_embeded_led(position);
		display_external_led(position);
		display_external_circle_led(position, color);
		play_external_buzz(position);
		play_embeded_buzz(position);
	}
}

void led_one(int target, int c) {
	for (int i=0; i<LEDNUM; i++) {
		if (i == target) {
			led[i] = colors[c];    // set led on
		}
		else {
	    	led[i].r=0;led[i].g=0;led[i].b=0;    // set led off
		}
		WS2811RGB(led, ARRAYLEN(led));
	}
}

void init_colors() {
	for(int i=0; i<COLORNUM; i++){
		colors[i].r=30;
		colors[i].g=30;
		colors[i].b=30;
	}
	colors[0].r=90;
	colors[0].g=00;
	colors[0].b=00;

	colors[COLORNUM-1].r=00;
	colors[COLORNUM-1].g=90;
	colors[COLORNUM-1].b=00;

	colors[COLORNUM-2].r=45;
	colors[COLORNUM-2].g=45;
	colors[COLORNUM-2].b=00;
	/*
	colors[0].r=150; colors[0].g=150; colors[0].b=150;
    colors[1].r=255; colors[1].g=000; colors[1].b=000;//red
    colors[2].r=255; colors[2].g=100; colors[2].b=000;//orange
    colors[3].r=100; colors[3].g=255; colors[3].b=000;//yellow
    colors[4].r=000; colors[4].g=255; colors[4].b=000;//green
    colors[5].r=000; colors[5].g=100; colors[5].b=255;//light blue (turkis)
    colors[6].r=000; colors[6].g=000; colors[6].b=255;//blue
    colors[7].r=100; colors[7].g=000; colors[7].b=255;//violet
	*/
}

void display_external_circle_led(double position, int color) {
	led_one(LEDNUM * position, color);
}
