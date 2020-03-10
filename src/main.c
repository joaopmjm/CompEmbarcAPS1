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

/********/
/* includes                                                             */
/********/

#include "asf.h"

// Musicas adquiridas do github do Bruno Arthur Cesconeto (Toranja) https://github.com/brunoartc/Embarcados/
#include "musicas.h"


/********/
/* defines                                                              */
/********/
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
#define LED3_PIO_IDX		3
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

/********/
/* constants                                                            */
/********/
#define MAX_SIZE 300

/********/
/* variaveis globais                                                    */
/********/
/********/
/* prototypes                                                           */
/********/

void init(void);
void faz_buzz(float freq);
/********/
/* interrupcoes                                                         */
/********/
/* Structs                                                         */
/********/
typedef struct{
	int notes[MAX_SIZE];
	int tempo[MAX_SIZE];
	int n;
} music_info;

/********/
/* funcoes                                                              */
/********/



void faz_buzz(float freq){
	float dc = (1.0/(freq*2.0))*1000000.0; //tempo que cada ligada deve tomar
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
	
	//Inicializa PC8 como saída
	pio_set_output(LED1_PIO, LED1_PIO_IDX_MASK, 1, 0, 0);
	pio_set_output(LED2_PIO, LED2_PIO_IDX_MASK, 1, 0, 0);
	pio_set_output(LED3_PIO, LED3_PIO_IDX_MASK, 1, 0, 0);
	
	// Inicializa PIO do botao
	pmc_enable_periph_clk(BUT1_PIO_ID);
	pmc_enable_periph_clk(BUT2_PIO_ID);
	pmc_enable_periph_clk(BUT3_PIO_ID);
	
	// configura pino ligado ao botão como entrada com um pull-up.
	pio_set_input(BUT1_PIO, BUT1_PIO_IDX_MASK,PIO_DEBOUNCE);
	pio_set_input(BUT2_PIO, BUT2_PIO_IDX_MASK,PIO_DEBOUNCE);
	pio_set_input(BUT3_PIO, BUT3_PIO_IDX_MASK,PIO_DEBOUNCE);
	
	pio_pull_up(BUT1_PIO,BUT1_PIO_IDX_MASK,1);
	pio_pull_up(BUT2_PIO,BUT2_PIO_IDX_MASK,1);
	pio_pull_up(BUT3_PIO,BUT3_PIO_IDX_MASK,1);
	
	// Inicializa PIO do Buzzer
	pmc_enable_periph_clk(BUZZ_PIO);
	pio_set_output(BUZZ_PIO,BUZZ_PIO_IDX_MASK,0,0,0);
}

	/********/
	/* Main                                                                 */
	/********/

	// Funcao principal chamada na inicalizacao do uC.
int main(void){
	init();

	char play = 0;
	int music = 0;
	int k = 0;

	music_info pirate_music;
	pirate_music.n = sizeof(pirate_notes)/sizeof(pirate_notes[0]);
	for(int i = 0; i < pirate_music.n; i++){
		pirate_music.notes[i] = pirate_notes[i];
		pirate_music.tempo[i] = pirate_tempo[i];
	}
	
	music_info imperial_music;
	imperial_music.n = sizeof(imperial_march_notes)/sizeof(imperial_march_notes[0]);
	for(int i = 0; i < imperial_music.n; i++){
		imperial_music.notes[i] = imperial_march_notes[i];
		imperial_music.tempo[i] = imperial_march_tempo[i];
	}

	//music_info mario_music;
	//mario_music.n = sizeof(mario_theme_notes)/sizeof(mario_theme_notes[0]);
	//for(int i = 0; i < mario_music.n; i++){
		//mario_music.notes[i] = mario_theme_notes[i];
		//mario_music.tempo[i] = mario_theme_tempo[i];
	//}
	
	Pio *pio_list[] = {LED1_PIO,LED2_PIO};
	int mask_list[] = {LED1_PIO_IDX_MASK, LED2_PIO_IDX_MASK};
	music_info music_list[] = {imperial_music, pirate_music};

	// super loop
	// aplicacoes embarcadas não devem sair do while(1).
	while (1){
		if (!pio_get(BUT1_PIO,PIO_DEFAULT,BUT1_PIO_IDX_MASK)){
			music--;
			if(music < 0) music = 1;
			k = 0;
			delay_s(1);
		}
		if (!pio_get(BUT3_PIO,PIO_DEFAULT,BUT3_PIO_IDX_MASK)){
			music++;
			if(music == 2) music = 0;
			k = 0;
			delay_s(1);
		}
		if (!pio_get(BUT2_PIO,PIO_DEFAULT,BUT2_PIO_IDX_MASK)){
			play = !play;
			delay_s(1);
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
			}
			//char k<music_list[music].n
			if (k<music_list[music].n){
				k++;
			}else{
				k = 0;
				music++;
				delay_s(1);
				if(music == 2) music = 0;
			}
		}
	}
	return 0;
}