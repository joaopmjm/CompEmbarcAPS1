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

/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include "asf.h"

// Musicas adquiridas do github do Bruno Arthur Cesconeto (Toranja) https://github.com/brunoartc/Embarcados/
#include "musicas.h"


/************************************************************************/
/* defines                                                              */
/************************************************************************/
	// Configurando LEDs
#define LED1_PIO			PIOA        
#define LED1_PIO_ID			10
#define LED1_PIO_IDX		0
#define LED1_PIO_IDX_MASK	(1 << LED1_PIO_IDX)

#define LED2_PIO			PIOC
#define LED2_PIO_ID			12
#define LED2_PIO_IDX		30
#define LED2_PIO_IDX_MASK	(1 << LED2_PIO_IDX)

#define LED3_PIO			PIOB
#define LED3_PIO_ID			11
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
#define BUZZ_PIO			PIOA
#define BUZZ_PIO_ID			10
#define BUZZ_PIO_IDX		4
#define BUZZ_PIO_IDX_MASK	(1u << BUZZ_PIO_IDX)

/************************************************************************/
/* constants                                                            */
/************************************************************************/

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/
/************************************************************************/
/* prototypes                                                           */
/************************************************************************/

void init(void);
void faz_buzz(float freq, float tempo);
/************************************************************************/
/* interrupcoes                                                         */
/************************************************************************/

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

void faz_buzz(float freq, float tempo){
	float dc = 1000/(freq*2); //tempo que cada ligada deve tomar
	//float qtd = (tempo*1000)/(2*dc);
	// ---___
	//for(int n=0;n < tempo*1000/(dc*2);n++){
		pio_set(BUZZ_PIO,BUZZ_PIO_IDX_MASK);
		delay_ms(dc);
		pio_clear(BUZZ_PIO,BUZZ_PIO_IDX_MASK);
		delay_ms(dc);
	//}
}

// Função de inicialização do uC
void init(void)
{
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

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

// Funcao principal chamada na inicalizacao do uC.
int main(void)
{
  init();
  
  //char music_chosen =0;
  char play=0;

  // super loop
  // aplicacoes embarcadas não devem sair do while(1).
  while (1)
  {
	  if (!pio_get(BUT2_PIO,PIO_DEFAULT,BUT2_PIO_IDX_MASK)){
		  play = !play;
		  delay_s(1);
	  }
	  if(play != 0){ 
			faz_buzz(250,5);
		}
  }
  return 0;
}
