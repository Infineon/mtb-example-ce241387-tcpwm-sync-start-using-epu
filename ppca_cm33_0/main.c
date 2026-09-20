/******************************************************************************
* File Name:   main.c
*
* Description: PPCA Core 0 application for the TCPWM Synchronize Start using
*              EPU example. Configures an EPU T2 unit and combiner to generate
*              a start trigger every 0.5 seconds from a TCPWM counter overflow,
*              and routes that trigger to PWM1 and PWM2 simultaneously for a
*              synchronised start. Enters an idle loop once configured.
*
*
********************************************************************************
* (c) 2026, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
*
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
*******************************************************************************/

#include "cy_pdl.h"
#include "cycfg.h"
#include <stdio.h>

/*******************************************************************************
* Macros
********************************************************************************/

/*******************************************************************************
* Global Variables
********************************************************************************/


/*******************************************************************************
* Function Prototypes
********************************************************************************/


/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function for PPCA Core 0. It configures the EPU and TCPWM
* peripherals for synchronised PWM start, then enters an idle loop:
*    1. Enable EPU exclusive access and T2 unit for the 0.5 s trigger
*    2. Configure combiner 13 to route the trigger to both PWM channels
*    3. Initialise and start TCPWM counter, PWM1, and PWM2
*    4. Idle loop – all further operation is event-driven through EPU
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{

    /* --- EPU (Event Processing Unit) Configuration --- */
    /* Grant this PPCA core exclusive access to the EPU block */
    Cy_PPCA_EPU_EnableExclusiveAccess(EPU_BLK_HW, true);
    /* Enable the EPU block so processing units can route signals */
    Cy_PPCA_EPU_Enable(EPU_BLK_HW);

    /* Configure T2 processing unit to generate a trigger pulse every 0.5 s
       from the timer overflow event – this pulse starts PWM1 and PWM2 */
    Cy_PPCA_EPU_PU_T2_Configure(put2_1_HW, put2_1_INDEX, &put2_1_put2_config);
    /* Enable PUT T2 processing unit 1  */
    Cy_PPCA_EPU_PU_T2_Enable(put2_1_HW, put2_1_INDEX, put2_1_ENABLE_MODE);
    /* Configure combiner 13 to combine EPU signals and route the start
       trigger simultaneously to both PWM channels */
    Cy_PPCA_EPU_Combo_Configure(combiner13_HW, combiner13_INDEX, &combiner13_combo_config);

    /* --- TCPWM Initialization --- */
    /* Initialize the free-running counter that drives the 0.5 s trigger */
    Cy_TCPWM_Counter_Init(timer0_HW, timer0_NUM, &timer0_config);
    /* Initialize PWM1 and PWM2 – both will be started by the EPU trigger */
    Cy_TCPWM_PWM_Init(PWM1_HW, PWM1_NUM, &PWM1_config);
    Cy_TCPWM_PWM_Init(PWM2_HW, PWM2_NUM, &PWM2_config);

    /* Enable all three TCPWM blocks so they respond to start triggers */
    Cy_TCPWM_Counter_Enable(timer0_HW, timer0_NUM);
    Cy_TCPWM_PWM_Enable(PWM1_HW, PWM1_NUM);
    Cy_TCPWM_PWM_Enable(PWM2_HW, PWM2_NUM);

    /* Start the counter running – it will periodically fire the EPU trigger
       which in turn synchronously starts PWM1 and PWM2 every 0.5 s */
    Cy_TCPWM_TriggerStart_Single(timer0_HW, timer0_NUM);

    /* PPCA Core 0 has no further work to do; all operation is event-driven */
     for(;;)
     {
     }
}
