#include <xc.h>
#include <pic18f4550.h>
#include "config.h"

#include <stdio.h>             //Incluimos stdio para escribir cadenas  con formato en una variable
#include "lcd.h"               //Incluimos la libreria LCD
#define _XTAL_FREQ 8000000     //Cristal Externo de Alta Velocidad 8000000

int ADC1=0;                    //Variable entera 16 bits, almacena ADRESH y ADRESL.
int ADC2=0;                    //Variable entera 16 bits, almacena ADRESH y ADRESL.
float vol_linea=0;             //Variable de punto flotante 32 bits, almacena voltaje de la red, Resistencias.
float vol_sirena=0;            //Variable de punto flotante 32 bits, almacena voltaje de sirena.
unsigned char buffer_lcd[16];  //Buffer donde será almacenada la variable con formato a mostrar en en LCD.

void main(){
    lcd_init();                //Inicializamos módulo LCD 16x2.
    lcd_clear();               //Limpiamos pantalla LCD.
    lcd_gotoxy(1,1);           //Cursor LCD fila1, columna 1.
    lcd_putc(" SOLARIUM "); 
    lcd_gotoxy(2,1);           //Cursor LCD fila2, columna 1.
    lcd_putc("**PIC 18F4550**");
    __delay_ms(2000);          //Retardo de 2s.
    
    while(1)                   //Bucle Infinito.
    {
        ADCON0bits.CHS=0b0000; //Habilitamos el canal AN0.
        ADCON0bits.GO_DONE=1;  //Inicializamos la conversión 
        while(ADCON0bits.GO_DONE==1); //Esperamos ha que termine la conversión.
        ADC1=ADRESH;           //Cargamos ADC con los 8 bits del registro ADRESH
        ADC1=ADC1<<8;          //Desplazamos 8 bits a la izquierda del registro ADC.
        ADC1|=ADRESL;          //Realizamos una operación lógica OR entre ADC y ADRESL el resultado se almacena en ADC
        vol_linea=(5.0*ADC1)/1023.0;
        __delay_ms(20);
        ADCON0bits.CHS=0b0001; //Habilitamos el canal AN0.
        ADCON0bits.GO_DONE=1;  //Inicializamos la conversión 
        while(ADCON0bits.GO_DONE==1); //Esperamos ha que termine la conversión.
        ADC2=ADRESH;           //Cargamos ADC con los 8 bits del registro ADRESH
        ADC2=ADC2<<8;          //Desplazamos 8 bits a la izquierda del registro ADC.
        ADC2|=ADRESL;          //Realizamos una operación lógica OR entre ADC y ADRESL el resultado se almacena en ADC
        vol_sirena=(5.0*ADC2)/1023.0;
        
        if(vol_sirena<0.210 && vol_linea>0.151 && vol_linea<=0.219)
        {
            LATB=0x00;         //Pines RB0 y RB1 a nivel bajo.
            lcd_clear();               //Limpiamos pantalla LCD.
            lcd_gotoxy(1,1);           //Cursor LCD fila1, columna 1.
            lcd_putc(" SOLARIUM "); 
            lcd_gotoxy(2,1);           //Cursor LCD fila2, columna 1.
            lcd_putc("*RUNNING SYSTEM*");
            __delay_ms(300);          //Retardo de 300 ms..
        }
        
        if(vol_linea<=0.059)   //Si el voltaje del circuito de linea es menor a 59 mv.
        {
            lcd_clear();       //Limpiamos pantalla LCD.
            lcd_gotoxy(1,1);   //Cursor LCD fila 1, columna 1.
            lcd_putc("*****ALARMA*****");
            lcd_gotoxy(2,1);   //Cursor LCD fina 2, columna 1.
            lcd_putc("****FALLA CC****");
            LATBbits.LB1=1;    //Ponemos a 1 el bit RB1 (led falla).
            __delay_ms(300);   //Retardo de 300 ms.
        }
        
        if(vol_linea>=0.210)   //Si el voltaje del circuito de linea es mayor a 210 mv.
        {
            lcd_clear();       //Limpiamos pantalla LCD.
            lcd_gotoxy(1,1);   //Cursor LCD fila 1, columna 1.
            lcd_putc("*****ALARMA*****");
            lcd_gotoxy(2,1);   //Cursor LCD fina 2, columna 1.
            lcd_putc("FALLA EN LA RED");
            LATBbits.LB1=1;    //Ponemos a 1 el bit RB1 (led falla).
            __delay_ms(300);   //Retardo de 300 ms.
        } 
    }
    
    return 0;
}