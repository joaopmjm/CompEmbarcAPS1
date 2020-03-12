/**
* 5 semestre - Eng. da Computação - Insper
* Rafael Corsi - rafael.corsi@insper.edu.br
*
* Projeto 0 para a placa SAME70-XPLD
*
* Objetivo :
*  - Introduzir ASF e HAL
*  - Configuracao de clock
*  - Configuracao pino In/Out
*
* Material :
*  - Kit: ATMEL SAME70-XPLD - ARM CORTEX M7
*/

/****/
/* includes                                                             */
/****/

#include "asf.h"

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

/****/
/* constants                                                            */
/****/

volatile char but1_flag;
volatile char but2_flag;
volatile char but3_flag;

/****/
/* variaveis globais                                                    */
/****/

/****/
/* prototypes                                                           */
/****/

void init(void);
void faz_buzz(float freq);
/****/
/* interrupcoes                                                         */
/****/
/* Structs                                                         */
/****/
typedef struct{
	int n;
	int *notes;
	int *tempo;
	char nome[13];
} music_info;

/****/
/* funcoes                                                              */
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

void faz_buzz(float freq){
	float dc = (1000000.0/(freq*2.0)); //tempo que cada ligada deve tomar
	pio_set(BUZZ_PIO,BUZZ_PIO_IDX_MASK);
	delay_us(dc);
	pio_clear(BUZZ_PIO,BUZZ_PIO_IDX_MASK);
	delay_us(dc);
}

// Função de inicialização do uC
void init(void){
	sysclk_init();
	WDT->WDT_MR=WDT_MR_WDDIS;
	// Ativa o PIO na qual o LED foi conectado
	// para que possamos controlar o LED.
	pmc_enable_periph_clk(LED1_PIO_ID);
	pmc_enable_periph_clk(LED2_PIO_ID);
	pmc_enable_periph_clk(LED3_PIO_ID);

	pio_configure(LED1_PIO, PIO_OUTPUT_0, LED1_PIO_IDX_MASK, PIO_DEFAULT);
	pio_configure(LED2_PIO, PIO_OUTPUT_0, LED2_PIO_IDX_MASK, PIO_DEFAULT);
	pio_configure(LED3_PIO, PIO_OUTPUT_0, LED3_PIO_IDX_MASK, PIO_DEFAULT);

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

/****/
/* Main                                                                 */
/****/

// Funcao principal chamada na inicalizacao do uC.
int main(void){
	init();

	char play = 0;
	int music = 0;
	int k = 0;

	music_info pirate_music;
	pirate_music.n = sizeof(pirate_notes)/sizeof(pirate_notes[0]);
	pirate_music.nome = "Piratas C";
	pirate_music.notes = &pirate_notes;
	pirate_music.tempo = &pirate_tempo;
	
	music_info imperial_music;
	imperial_music.n = sizeof(imperial_march_notes)/sizeof(imperial_march_notes[0]);
	imperial_music.nome = "Darth Vader";
	imperial_music.notes = &imperial_march_notes;
	imperial_music.tempo = &imperial_march_tempo;

	music_info underworld_music;
	underworld_music.n = sizeof(underworld_melody)/sizeof(underworld_melody[0]);
	underworld_music.nome = "Underworld";
	underworld_music.notes = &underworld_melody;
	underworld_music.tempo = &underworld_tempo;

	music_info music_list[] = {imperial_music, pirate_music, underworld_music};
	
	Pio *pio_list[] = {LED1_PIO,LED2_PIO,LED3_PIO};
	int mask_list[] = {LED1_PIO_IDX_MASK, LED2_PIO_IDX_MASK,LED3_PIO_IDX_MASK};

	// super loop
	// aplicacoes embarcadas não devem sair do while(1).
	while (1){
		
		if(!play){
			pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
		}
		if (but1_flag){
			pio_clear(pio_list[music], mask_list[music]);
			music--;
			if(music < 0) music = 1;
			k = 0;
			but1_flag = 0;
			delay_s(1);
		}
		if (but3_flag){
			pio_clear(pio_list[music], mask_list[music]);
			music++;
			if(music == sizeof(music_list)/sizeof(music_list[0])) music = 0;
			k = 0;
			but3_flag = 0;
			delay_s(1);
		}
		if (but2_flag){
			play = !play;
			delay_s(1);
			but2_flag = 0;
		}
		if(play != 0){
			int tempo = music_list[music].tempo[k];
			if(tempo != 0){
				if (music_list[music].notes[k] < music_list[music].notes[k-1]){
					pio_set(pio_list[music], mask_list[music]);
					}else if(music_list[music].notes[k] > music_list[music].notes[k-1]){
					pio_clear(pio_list[music], mask_list[music]);
				}
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
				pio_clear(pio_list[music], mask_list[music]);
				music++;
				delay_s(2);
				if(music == sizeof(music_list)/sizeof(music_list[0])) music = 0;
			}
		}
	}
	return 0;
}