
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include <math.h>

//
// Function Prototypes
//
__interrupt void adc_isr(void);
void Adc_Config(void);

void clarkeDeger();
void clarkTransform();

void ParkDeger();
void ParkTransform();

Uint16 Va, Vb, Vc;
Uint16 Valfa, Vbeta, Vo;
Uint16 Vd, Vq, Voo;
Uint16 Teta;

double Clark[3][3];
double Park[3][3];

Uint16 VoltageA0;
Uint16 VoltageA1;
Uint16 VoltageA2;
Uint16 VoltageA3;
Uint16 VoltageA4;
Uint16 VoltageA5;
Uint16 VoltageA7;
Uint16 VoltageB0;
Uint16 VoltageB1;
Uint16 VoltageB2;
Uint16 VoltageB3;
Uint16 VoltageB4;
Uint16 VoltageB5;

Uint16 ConversionCount;
//Uint16 Voltage2[10];

//
// Main
//
void main(void)
{
    //
    // Step 1. Initialize System Control:
    // PLL, WatchDog, enable Peripheral Clocks
    // This example function is found in the F2806x_SysCtrl.c file.
    //
    InitSysCtrl();

    //
    // Step 2. Initialize GPIO:
    // This example function is found in the F2806x_Gpio.c file and
    // illustrates how to set the GPIO to it's default state.
    //
    // InitGpio();  // Skipped for this example

    //
    // Step 3. Clear all interrupts and initialize PIE vector table:
    // Disable CPU iterruptsn
    //
    DINT;

    //
    // Initialize the PIE control registers to their default state.
    // The default state is all PIE interrupts disabled and flags
    // are cleared.
    // This function is found in the F2806x_PieCtrl.c file.
    //
    InitPieCtrl();

    //
    // Disable CPU interrupts and clear all CPU interrupt flags:
    //
    IER = 0x0000;
    IFR = 0x0000;

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    // This will populate the entire table, even if the interrupt
    // is not used in this example.  This is useful for debug purposes.
    // The shell ISR routines are found in F2806x_DefaultIsr.c.
    // This function is found in F2806x_PieVect.c.
    //
    InitPieVectTable();

    //
    // Interrupts that are used in this example are re-mapped to
    // ISR functions found within this file.
    //
    EALLOW;  // This is needed to write to EALLOW protected register
    PieVectTable.ADCINT1 = &adc_isr;
    EDIS;    // This is needed to disable write to EALLOW protected registers

    //
    // Step 4. Initialize all the Device Peripherals:
    // This function is found in F2806x_InitPeripherals.c
    // InitPeripherals(); // Not required for this example
    //
    InitAdc();  // For this example, init the ADC
    AdcOffsetSelfCal();

    //
    // Step 5. User specific code, enable interrupts:
    //

    //
    // Enable ADCINT1 in PIE
    //
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1; // Enable INT 1.1 in the PIE
    IER |= M_INT1;                     // Enable CPU Interrupt 1
    EINT;                              // Enable Global interrupt INTM
    ERTM;                              // Enable Global realtime interrupt DBGM


    ConversionCount = 0;

    //
    // Configure ADC
    //
    EALLOW;
    AdcRegs.ADCCTL2.bit.ADCNONOVERLAP = 1; // Enable non-overlap mode

    //
    // ADCINT1 trips after AdcResults latch
    //
    AdcRegs.ADCCTL1.bit.INTPULSEPOS = 1;

    AdcRegs.INTSEL1N2.bit.INT1E     = 1;  // Enabled ADCINT1
    AdcRegs.INTSEL1N2.bit.INT1CONT  = 0;  // Disable ADCINT1 Continuous mode

    //
    // setup EOC1 to trigger ADCINT1 to fire
    //
    AdcRegs.INTSEL1N2.bit.INT1SEL   = 1;

    AdcRegs.ADCSOC0CTL.bit.CHSEL    = 0;   // set SOC0 channel select to ADCINA0
    AdcRegs.ADCSOC1CTL.bit.CHSEL    = 1;   // set SOC1 channel select to ADCINA1
    AdcRegs.ADCSOC2CTL.bit.CHSEL    = 2;   // set SOC2 channel select to ADCINA2
    AdcRegs.ADCSOC3CTL.bit.CHSEL    = 3;   // set SOC3 channel select to ADCINA3
    AdcRegs.ADCSOC4CTL.bit.CHSEL    = 4;   // set SOC4 channel select to ADCINA4
    AdcRegs.ADCSOC5CTL.bit.CHSEL    = 5;   // set SOC5 channel select to ADCINA5
    AdcRegs.ADCSOC7CTL.bit.CHSEL    = 7;   // set SOC7 channel select to ADCINA7

    AdcRegs.ADCSOC8CTL.bit.CHSEL    = 8;   // set SOC8 channel select to ADCINB0
    AdcRegs.ADCSOC9CTL.bit.CHSEL    = 9;   // set SOC9 channel select to ADCINB1
    AdcRegs.ADCSOC10CTL.bit.CHSEL    = 10; // set SOC10 channel select to ADCINB2
    AdcRegs.ADCSOC11CTL.bit.CHSEL    = 11; // set SOC11 channel select to ADCINB3
    AdcRegs.ADCSOC12CTL.bit.CHSEL    = 12; // set SOC12 channel select to ADCINB4
    AdcRegs.ADCSOC13CTL.bit.CHSEL    = 13; // set SOC13 channel select to ADCINB5

    //AdcRegs.ADCSOC1CTL.bit.CHSEL    = 2;  // set SOC1 channel select to ADCINA2

    //
    // set SOC0:SOC10 start trigger on EPWM1A, due to round-robin SOC0 converts
    // first then SOC1
    //
    AdcRegs.ADCSOC0CTL.bit.TRIGSEL  = 5;
    AdcRegs.ADCSOC1CTL.bit.TRIGSEL  = 5;
    AdcRegs.ADCSOC2CTL.bit.TRIGSEL  = 5;
    AdcRegs.ADCSOC3CTL.bit.TRIGSEL  = 5;
    AdcRegs.ADCSOC4CTL.bit.TRIGSEL  = 5;
    AdcRegs.ADCSOC5CTL.bit.TRIGSEL  = 5;
    AdcRegs.ADCSOC7CTL.bit.TRIGSEL  = 5;

    AdcRegs.ADCSOC8CTL.bit.TRIGSEL  = 5;
    AdcRegs.ADCSOC9CTL.bit.TRIGSEL  = 5;
    AdcRegs.ADCSOC10CTL.bit.TRIGSEL  = 5;
    AdcRegs.ADCSOC11CTL.bit.TRIGSEL  = 5;
    AdcRegs.ADCSOC12CTL.bit.TRIGSEL  = 5;
    AdcRegs.ADCSOC13CTL.bit.TRIGSEL  = 5;



    //
    // set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
    //
    AdcRegs.ADCSOC0CTL.bit.ACQPS    = 6;
    AdcRegs.ADCSOC1CTL.bit.ACQPS    = 6;
    AdcRegs.ADCSOC2CTL.bit.ACQPS    = 6;
    AdcRegs.ADCSOC3CTL.bit.ACQPS    = 6;
    AdcRegs.ADCSOC4CTL.bit.ACQPS    = 6;
    AdcRegs.ADCSOC5CTL.bit.ACQPS    = 6;
    AdcRegs.ADCSOC7CTL.bit.ACQPS    = 6;

    AdcRegs.ADCSOC8CTL.bit.ACQPS    = 6;
    AdcRegs.ADCSOC9CTL.bit.ACQPS    = 6;
    AdcRegs.ADCSOC10CTL.bit.ACQPS    = 6;
    AdcRegs.ADCSOC11CTL.bit.ACQPS    = 6;
    AdcRegs.ADCSOC12CTL.bit.ACQPS    = 6;
    AdcRegs.ADCSOC13CTL.bit.ACQPS    = 6;



    EDIS;

    //
    // Assumes ePWM1 clock is already enabled in InitSysCtrl();
    //
    EPwm1Regs.ETSEL.bit.SOCAEN  = 1;        // Enable SOC on A group
    EPwm1Regs.ETSEL.bit.SOCASEL = 4;        // Select SOC from CMPA on upcount
    EPwm1Regs.ETPS.bit.SOCAPRD  = 1;        // Generate pulse on 1st event
    EPwm1Regs.CMPA.half.CMPAHR  = 200;   // Set compare A value
    EPwm1Regs.TBPRD             = 0xFFFF;   // Set period for ePWM1
    EPwm1Regs.TBCTL.bit.CTRMODE = 0;        // count up and start

    //
    // Wait for ADC interrupt
    //

    clarkeDeger();

    ParkDeger();

    while(1)
    {
        clarkTransform();

        ParkTransform();
    }
}

//
// adc_isr -
//
__interrupt void
adc_isr(void)
{
    VoltageA0 = AdcResult.ADCRESULT0;  // ADCINA0
    VoltageA1 = AdcResult.ADCRESULT1;  // ADCINA1
    VoltageA2 = AdcResult.ADCRESULT2;  // ADCINA2
    VoltageA3 = AdcResult.ADCRESULT3;  // ADCINA3
    VoltageA4 = AdcResult.ADCRESULT4;  // ADCINA4
    VoltageA5 = AdcResult.ADCRESULT5;  // ADCINA5
    VoltageA7 = AdcResult.ADCRESULT7;  // ADCINA7

    VoltageB0 = AdcResult.ADCRESULT8;  // ADCINB0
    VoltageB1 = AdcResult.ADCRESULT9;  // ADCINB1
    VoltageB2 = AdcResult.ADCRESULT10; // ADCINB2
    VoltageB3 = AdcResult.ADCRESULT11; // ADCINB3
    VoltageB4 = AdcResult.ADCRESULT12; // ADCINB4
    VoltageB5 = AdcResult.ADCRESULT13; // ADCINB5


    AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE

    return;
}


void clarkeDeger()
{
    Clark[0][0] = 1;
    Clark[0][1] = cos((2 * M_PI)/3);
    Clark[0][2] = cos((4 * M_PI)/3);

    Clark[1][0] = 0;
    Clark[1][1] = sin((2 * M_PI)/3);
    Clark[1][2] = sin((4 * M_PI)/3);

    Clark[2][0] = 1 / sqrt(2);
    Clark[2][1] = 1 / sqrt(2);
    Clark[2][2] = 1 / sqrt(2);
}


void clarkTransform()
{
    Valfa = (sqrt(2/3)) * ((Clark[0][0] * Va) + (Clark[0][1] * Vb) + (Clark[0][2] * Vc));
    Vbeta = (sqrt(2/3)) * ((Clark[1][0] * Va) + (Clark[1][1] * Vb) + (Clark[1][2] * Vc));
    Vo = (sqrt(2/3)) * ((Clark[2][0] * Va) + (Clark[2][1] * Vb) + (Clark[2][2] * Vc));
}


void ParkDeger()
{
    Park[0][0] = cos(Teta);
    Park[0][1] = sin(Teta);
    Park[0][2] = 0;

    Park[1][0] = -sin(Teta);
    Park[1][1] = cos(Teta);
    Park[1][2] = 0;

    Park[2][0] = 0;
    Park[2][1] = 0;
    Park[2][2] = 1;
}

void ParkTransform()
{
    Vd = (sqrt(2/3)) * ((Park[0][0] * Valfa) + (Park[0][1] * Vbeta) + (Park[0][2] * Vo));
    Vq = (sqrt(2/3)) * ((Park[1][0] * Valfa) + (Park[1][1] * Vbeta) + (Park[1][2] * Vo));
    Voo = (sqrt(2/3)) * ((Park[2][0] * Valfa) + (Park[2][1] * Vbeta) + (Park[2][2] * Vo));
}

//
// End of File
//
