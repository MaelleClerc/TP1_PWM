/*--------------------------------------------------------*/
// GestPWM.c
/*--------------------------------------------------------*/
//	Description :	Gestion des PWM 
//			        pour TP1 2022-2023
//
//	Auteur 		: 	M.Clerc
//                  M.Santos
//
//	Version		:	V1.1
//	Compilateur	:	XC32 V1.42 + Harmony 1.08
//
/*--------------------------------------------------------*/



#include "GestPWM.h"
#include "bsp.h"

S_pwmSettings PWMData;      // pour les settings

void GPWM_Initialize(S_pwmSettings *pData)
{
   // Init les data 
    
   // Init état du pont en H
    BSP_EnableHbrige();
    
   // lance les timers et OC
    DRV_TMR0_Start();
    DRV_TMR1_Start();
    DRV_TMR2_Start();
    DRV_TMR3_Start();
    
}

// Obtention vitesse et angle (mise a jour des 4 champs de la structure)
void GPWM_GetSettings(S_pwmSettings *pData)	
{
    /* Variables locales */
    static uint8_t Moving_Average_Memory = 0;
    static uint8_t i;
    static uint8_t Moving_Average_Chan0[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    static uint8_t Moving_Average_Chan1[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    static uint8_t Average_Chan0 = 0;
    static uint8_t Average_Chan1 = 0;
    static uint8_t Convert_Speed = 0;
    static S_ADCResults Results;
    
    // Lecture de l'ADC
    Results = BSP_ReadAllADC();
    
    // Sauvegarde du résultat pour la moyenne glissante
    if(Moving_Average_Memory == 10)
    {
        Moving_Average_Memory = 0;
    }
    
    Moving_Average_Chan0[Moving_Average_Memory] = Results.Chan0;
    Moving_Average_Chan1[Moving_Average_Memory] = Results.Chan1;
    
    Moving_Average_Memory++;
    
    // Calcul de la moyenne glissante
    for (i = 0; i < 10; i++)
    {
        Average_Chan0 = Average_Chan0 + Moving_Average_Chan0[i];
        Average_Chan0 = Average_Chan1 + Moving_Average_Chan1[i];
    }
    
    Average_Chan0 = Average_Chan0 / 10;
    Average_Chan1 = Average_Chan1 / 10;
    
    // Conversion des valeurs
    Convert_Speed = Average_Chan0 / 5.166;
    PWMData.SpeedSetting = Convert_Speed - 99;
    PWMData.absSpeed = abs(PWMData.SpeedSetting);
    PWMData.AngleSetting = Average_Chan1 / 5.683;
    PWMData.absAngle = abs(PWMData.AngleSetting);
}


// Affichage des information en exploitant la structure
void GPWM_DispSettings(S_pwmSettings *pData)
{
    printf_lcd("Tp1 PWM 2022-2023");
    lcd_gotoxy(1, 2);
    printf_lcd("SpeedSetting %3d", PWMData.SpeedSetting);
    lcd_gotoxy(1, 3);
    printf_lcd("absSpeed      %2d", PWMData.absSpeed);
    lcd_gotoxy(1, 4);
    printf_lcd("Angle        %3d", PWMData.AngleSetting);
    lcd_bl_on();
}

// Execution PWM et gestion moteur à partir des info dans structure
void GPWM_ExecPWM(S_pwmSettings *pData)
{
    
}

// Execution PWM software
void GPWM_ExecPWMSoft(S_pwmSettings *pData)
{
    
}


