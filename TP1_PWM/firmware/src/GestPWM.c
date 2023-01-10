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
#include "app.h"
#include "Mc32DriverAdc.h"
#include "Mc32DriverLcd.h"
#include "system_config/default/framework/driver/oc/drv_oc_static.h"
#include "peripheral/oc/plib_oc.h"

S_pwmSettings PWMData;      // pour les settings

void GPWM_Initialize()
{
   // Init les data 
    
   // Init état du pont en H
    BSP_EnableHbrige();
    
   // Lance les timers et OC
    DRV_TMR0_Start();
    DRV_TMR1_Start();
    DRV_TMR2_Start();
    DRV_TMR3_Start();
    DRV_OC0_Start();
    DRV_OC1_Start();
    
}

// Obtention vitesse et angle (mise a jour des 4 champs de la structure)
void GPWM_GetSettings()	
{
    /* Variables locales */
    static uint8_t Moving_Average_Memory = 0;
    uint8_t i;
    
    static uint16_t Moving_Average_Chan0[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    static uint16_t Moving_Average_Chan1[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    
    float Average_Chan0 = 0;
    float Average_Chan1 = 0;
    
    S_ADCResults Results;
    
    // Lecture de l'ADC
    Results = BSP_ReadAllADC();
    
    // Sauvegarde du résultat pour la moyenne glissante
    if(Moving_Average_Memory > 9)
    {
        Moving_Average_Memory = 0;
    }
    
    Moving_Average_Chan0[Moving_Average_Memory] = Results.Chan0;
    Moving_Average_Chan1[Moving_Average_Memory] = Results.Chan1;
    
    Moving_Average_Memory++;
    
    // Calcul de la moyenne glissante
    for (i = 0; i < 10; i++)
    {
        Average_Chan0 += Moving_Average_Chan0[i];
        Average_Chan1 += Moving_Average_Chan1[i];
    }
    
    Average_Chan0 = Average_Chan0 / 10;
    Average_Chan1 = Average_Chan1 / 10;
    
    // Conversion des valeurs
    PWMData.SpeedSetting = (Average_Chan0 * 198 / 1023) - 99;    // 5.166 = 1023 / 198
    PWMData.absSpeed = abs(PWMData.SpeedSetting);
    PWMData.absAngle = Average_Chan1 * 180 / 1023;               // 5.683 = 1023 / 180
    PWMData.AngleSetting = PWMData.absAngle - 90;
}


// Affichage des information en exploitant la structure
void GPWM_DispSettings()
{
    lcd_gotoxy(1, 1);
    printf_lcd("Tp1 PWM 2022-2023");
    lcd_gotoxy(1, 2);
    printf_lcd("SpeedSetting %3d", PWMData.SpeedSetting);
    lcd_gotoxy(1, 3);
    printf_lcd("absSpeed      %2d", PWMData.absSpeed);
    lcd_gotoxy(1, 4);
    printf_lcd("Angle        %3d", PWMData.AngleSetting);
}

// Execution PWM et gestion moteur à partir des info dans structure
void GPWM_ExecPWM()
{
    // Gestion du sens de direction du pont en H
    if(PWMData.SpeedSetting > 0)
    {
        AIN1_HBRIDGE_W = 1;
        AIN2_HBRIDGE_W = 0;
        STBY_HBRIDGE_W = 1;
    }
    else if(PWMData.SpeedSetting == 0)
    {
        STBY_HBRIDGE_W = 0;
    }
    else
    {
        AIN1_HBRIDGE_W = 0;
        AIN2_HBRIDGE_W = 1;
        STBY_HBRIDGE_W = 1;
    }
    
    // Calcul du nombre d'impulsions sur OC2
    PLIB_OC_PulseWidth16BitSet(OC_ID_2, PWMData.absSpeed * 1999 / 99);          // 20.19 = 1999 / 99
    
    // Calcul du nombre d'impulsions sur OC3
    PLIB_OC_PulseWidth16BitSet(OC_ID_3, PWMData.absAngle * 9000 / 180 + 2999);   // 58.33 = (13499 - 2999) / 180
}

// Execution PWM software
void GPWM_ExecPWMSoft()
{ 
    static uint8_t PWM_Counter = 0;
    static uint8_t PWM_Compare = 0;
    
    if (PWM_Counter == 0)
    {
        PWM_Compare = PWMData.absSpeed;
    }
    
    
    if (PWM_Counter < PWM_Compare)
    {
        BSP_LEDOn(BSP_LED_2);
    }
    else
    {
        BSP_LEDOff(BSP_LED_2);
    }

    
    PWM_Counter ++;
    
    if (PWM_Counter > 99)
    {
        PWM_Counter = 0;
    }
}


