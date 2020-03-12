/**
* \file
*
* \brief Empty user application template
*
*/

/**
* \mainpage User Application template doxygen documentation
*
* \par Empty user application template
*
* Bare minimum empty user application template
*
* \par Content
*
* -# Include the ASF header files (through asf.h)
* -# "Insert system clock initialization code here" comment
* -# Minimal main function that starts with a call to board_init()
* -# "Insert application code here" comment
*
*/

/*
* Include header files for all drivers that have been imported from
* Atmel Software Framework (ASF).
*/
/*
* Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
*/
#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

// Musicas adquiridas do github do Bruno Arthur Cesconeto (Toranja) https://github.com/brunoartc/Embarcados/
#include "musicas.h"

/****/
/* defines                                                              */
/****/
// Configurando LEDs
#define LED1_PIO			PIOA
#define LED1_PIO_ID			ID_PIOA
#define LED1_PIO_IDX		0
#define LED1_PIO_IDX_MASK	(1 << LED1_PIO_IDX)

#define LED2_PIO			PIOC
#define LED2_PIO_ID			ID_PIOC
#define LED2_PIO_IDX		30
#define LED2_PIO_IDX_MASK	(1 << LED2_PIO_IDX)

#define LED3_PIO			PIOB
#define LED3_PIO_ID			ID_PIOB
#define LED3_PIO_IDX		2
#define LED3_PIO_IDX_MASK	(1 << LED3_PIO_IDX)

#define LED4_PIO			PIOC
#define LED4_PIO_ID			ID_PIOC
#define LED4_PIO_IDX		8
#define LED4_PIO_IDX_MASK	(1 << LED4_PIO_IDX)


// Configuracoes do botoes
#define BUT1_PIO			PIOD
#define BUT1_PIO_ID			16
#define BUT1_PIO_IDX		28
#define BUT1_PIO_IDX_MASK	(1u << BUT1_PIO_IDX)

#define BUT3_PIO			PIOA
#define BUT3_PIO_ID			10
#define BUT3_PIO_IDX		19
#define BUT3_PIO_IDX_MASK	(1u << BUT3_PIO_IDX)

#define BUT2_PIO			PIOC
#define BUT2_PIO_ID			12
#define BUT2_PIO_IDX		31
#define BUT2_PIO_IDX_MASK	(1u << BUT2_PIO_IDX)

// Configuracoes Buzzer
#define BUZZ_PIO			PIOD
#define BUZZ_PIO_ID			ID_PIOD
#define BUZZ_PIO_IDX		30
#define BUZZ_PIO_IDX_MASK	(1u << BUZZ_PIO_IDX)

volatile char but1_flag;
volatile char but2_flag;
volatile char but3_flag;

void but1_callback(void);
void but2_callback(void);
void but3_callback(void);
void clear_leds(void);


/****/
/* Callback                                                         */
/****/
void but1_callback(void){
	but1_flag = 1;
}
void but2_callback(void){
	but2_flag = 1;
}
void but3_callback(void){
	but3_flag = 1;
}
/****/
/* Constants                                                         */
/****/

Pio *pio_list[] = {LED1_PIO,LED2_PIO,LED3_PIO};
int mask_list[] = {LED1_PIO_IDX_MASK, LED2_PIO_IDX_MASK,LED3_PIO_IDX_MASK};

/****/
/* Structs                                                         */
/****/
typedef struct{
	int n;
	int *notes;
	int *tempo;
	char nome[10];
} music_info;

/****/
/* funcoes                                                         */
/****/
void faz_buzz(float freq){
	float dc = (500000.0/(freq*2.0)); //tempo que cada ligada deve tomar
	pio_set(BUZZ_PIO, BUZZ_PIO_IDX_MASK);
	delay_us(dc);
	pio_clear(BUZZ_PIO, BUZZ_PIO_IDX_MASK);
	delay_us(dc);
}

void clear_leds(void){
	pio_set(pio_list[0], mask_list[0]);
	pio_set(pio_list[1], mask_list[1]);
	pio_set(pio_list[2], mask_list[2]);
}


void init(void){
	sysclk_init();
	
	WDT->WDT_MR=WDT_MR_WDDIS;
	// Ativa o PIO na qual o LED foi conectado
	// para que possamos controlar o LED.
	pmc_enable_periph_clk(LED1_PIO_ID);
	pmc_enable_periph_clk(LED2_PIO_ID);
	pmc_enable_periph_clk(LED3_PIO_ID);
	pmc_enable_periph_clk(LED4_PIO_ID);
	
	pio_configure(LED1_PIO, PIO_OUTPUT_0, LED1_PIO_IDX_MASK, PIO_DEFAULT);
	pio_configure(LED2_PIO, PIO_OUTPUT_0, LED2_PIO_IDX_MASK, PIO_DEFAULT);
	pio_configure(LED3_PIO, PIO_OUTPUT_0, LED3_PIO_IDX_MASK, PIO_DEFAULT);
	pio_configure(LED4_PIO, PIO_OUTPUT_0, LED4_PIO_IDX_MASK, PIO_DEFAULT);
	
	// Inicializa PIO do botao
	pmc_enable_periph_clk(BUT1_PIO_ID);
	pmc_enable_periph_clk(BUT2_PIO_ID);
	pmc_enable_periph_clk(BUT3_PIO_ID);
	
	pio_configure(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK, PIO_PULLUP);
	pio_configure(BUT2_PIO, PIO_INPUT, BUT2_PIO_IDX_MASK, PIO_PULLUP);
	pio_configure(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK, PIO_PULLUP);
	
	pio_handler_set(BUT1_PIO,
	BUT1_PIO_ID,
	BUT1_PIO_IDX_MASK,
	PIO_IT_FALL_EDGE,
	but1_callback);
	
	pio_handler_set(BUT2_PIO,
	BUT2_PIO_ID,
	BUT2_PIO_IDX_MASK,
	PIO_IT_FALL_EDGE,
	but2_callback);
	
	pio_handler_set(BUT3_PIO,
	BUT3_PIO_ID,
	BUT3_PIO_IDX_MASK,
	PIO_IT_FALL_EDGE,
	but3_callback);
	
	NVIC_EnableIRQ(BUT1_PIO_ID);
	NVIC_SetPriority(BUT1_PIO_ID, 4);
	
	NVIC_EnableIRQ(BUT2_PIO_ID);
	NVIC_SetPriority(BUT2_PIO_ID, 6);
	
	NVIC_EnableIRQ(BUT3_PIO_ID);
	NVIC_SetPriority(BUT3_PIO_ID, 5);
	
	pio_enable_interrupt(BUT1_PIO, BUT1_PIO_IDX_MASK);
	pio_enable_interrupt(BUT2_PIO, BUT2_PIO_IDX_MASK);
	pio_enable_interrupt(BUT3_PIO, BUT3_PIO_IDX_MASK);
	
	// Inicializa PIO do Buzzer
	pmc_enable_periph_clk(BUZZ_PIO);
	pio_set_output(BUZZ_PIO,BUZZ_PIO_IDX_MASK,0,0,0);
}

int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */
	
	board_init();
	sysclk_init();
	init();
	delay_init();
	
	
	char play = 0;
	int led_on = 0;
	int music = 0;
	int k = 0;

	music_info pirate_music;
	pirate_music.n = sizeof(pirate_notes)/sizeof(pirate_notes[0]);
	pirate_music.nome[0]  = 'P';
	pirate_music.nome[1]  = 'i';
	pirate_music.nome[2]  = 'r';
	pirate_music.nome[3]  = 'a';
	pirate_music.nome[4]  = 't';
	pirate_music.nome[5]  = 'a';
	pirate_music.nome[6]  = 's';
	pirate_music.nome[7]  = ' ';
	pirate_music.nome[8]  = 'C';
	pirate_music.nome[9]  = '.';
	pirate_music.nome[10] = '\0';
	
	pirate_music.notes = &pirate_notes;
	pirate_music.tempo = &pirate_tempo;
	
	music_info imperial_music;
	imperial_music.n = sizeof(imperial_march_notes)/sizeof(imperial_march_notes[0]);
	imperial_music.nome[0]  = 'D';
	imperial_music.nome[1]  = 'a';
	imperial_music.nome[2]  = 'r';
	imperial_music.nome[3]  = 't';
	imperial_music.nome[4]  = 'h';
	imperial_music.nome[5]  = ' ';
	imperial_music.nome[6]  = 'V';
	imperial_music.nome[7]  = 'a';
	imperial_music.nome[8]  = 'd';
	imperial_music.nome[9]  = 'e';
	imperial_music.nome[10]  = 'r';
	imperial_music.nome[11] = '\0';
	imperial_music.notes = &imperial_march_notes;
	imperial_music.tempo = &imperial_march_tempo;

	music_info underworld_music;
	underworld_music.n = sizeof(underworld_melody)/sizeof(underworld_melody[0]);
	underworld_music.nome[0] = 'U';
	underworld_music.nome[1] = 'd';
	underworld_music.nome[2] = 'e';
	underworld_music.nome[3] = 'r';
	underworld_music.nome[4] = 'w';
	underworld_music.nome[5] = 'o';
	underworld_music.nome[6] = 'r';
	underworld_music.nome[7] = 'l';
	underworld_music.nome[8] = 'd';
	underworld_music.nome[9] = '\0';
	underworld_music.notes = &underworld_melody;
	underworld_music.tempo = &underworld_tempo;

	music_info music_list[] = {pirate_music, imperial_music, underworld_music};
	
	clear_leds();

	gfx_mono_ssd1306_init();
	gfx_mono_draw_string(music_list[music].nome, 10, 10, &sysfont);

	/* Insert application code here, after the board has been initialized. */
	while(1) {
		
		if(!play){
			clear_leds();
			pio_clear(LED4_PIO,LED4_PIO_IDX_MASK);
			pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
		}
		if (but1_flag){
			music--;
			if(music < 0) music = 1;
			k = 0;
			gfx_mono_draw_string("           ", 10, 10, &sysfont);
			gfx_mono_draw_string(music_list[music].nome, 10, 10, &sysfont);
			but1_flag = 0;
			//delay_s(1);
		}
		if (but3_flag){
			music++;
			if(music == sizeof(music_list)/sizeof(music_list[0])) music = 0;
			gfx_mono_draw_string("           ", 10, 10, &sysfont);
			gfx_mono_draw_string(music_list[music].nome, 10, 10, &sysfont);
			k = 0;
			but3_flag = 0;
			delay_s(1);
		}
		if (but2_flag){
			play = !play;
			if (play){ 
				pio_set(LED4_PIO,LED4_PIO_IDX_MASK);
			}
			delay_s(1);
			but2_flag = 0;
		}
		if(play != 0){
			int tempo = music_list[music].tempo[k];
			if(tempo != 0){
				if (music_list[music].notes[k] < music_list[music].notes[k-1]){
					if(led_on < 2){
						pio_set(pio_list[led_on], mask_list[led_on]);
						led_on++;
					}
					}else if(music_list[music].notes[k] > music_list[music].notes[k-1]){
					if(led_on > 0) {
						pio_set(pio_list[led_on], mask_list[led_on]);
						led_on--;
					}
				}
				pio_clear(pio_list[led_on], mask_list[led_on]);
				for (int n=0;n<tempo;n++){
					if (music_list[music].notes[k] != 0){
						faz_buzz((float)music_list[music].notes[k]);
					}
				}
				delay_ms(100);
			}
			if (k<music_list[music].n){
				k++;
				}else{
				k = 0;
				clear_leds();
				music++;
				delay_s(2);
				if(music == sizeof(music_list)/sizeof(music_list[0])) music = 0;
				gfx_mono_draw_string("           ", 10, 10, &sysfont);
				gfx_mono_draw_string(music_list[music].nome, 10, 10, &sysfont);
			}
		}
	}
	return 1;
}
