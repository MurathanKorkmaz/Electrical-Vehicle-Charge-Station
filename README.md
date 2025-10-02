# Electrical-Vehicle-Charge-Station
AC/DC Converter Software for Electrical Vehicle Charge Station

---

## ADC_CLARK_PARK Subproject

This subfolder is designed for measuring and processing three-phase currents and voltages in electric vehicle charging stations, using ADC (Analog-to-Digital Converter) readings and Clark/Park vector transformations. The project is developed for the Texas Instruments TMS320F28069 microcontroller and is intended to be used with Code Composer Studio.

### Project Purpose
In electric vehicle charging stations, it is essential to accurately measure three-phase currents and voltages from the grid and process these signals for control algorithms. For this purpose, analog signals are digitized via the ADC, and Clark/Park transformations are applied to convert phase currents and voltages into reference frames suitable for control algorithms.

### Main Code
All core algorithms and application flow are implemented in the `ADC_CLARK_PARK/main.c` file. This file includes:
- ADC configuration and initialization,
- Reading measurement values via ADC interrupt,
- Creation and application of Clark and Park transformation matrices,
- PWM triggering and continuous data processing in the main loop.

#### Key Functions:
- **adc_isr:** When the ADC interrupt occurs, values read from the ADCRESULT registers are assigned to relevant variables. ADCRESULT0-ADCRESULT13 registers hold sampled results from different analog inputs.
- **clarkeDeger & clarkTransform:** The Clark transformation matrix is created, and phase currents/voltages are converted to α-β-0 axes. This transformation maps the three-phase system into a two-dimensional vector space.
- **ParkDeger & ParkTransform:** The Park transformation matrix is created, and α-β axis values are converted to d-q-0 axes. This transformation is used to move the data into a reference frame suitable for control algorithms (e.g., vector control).

#### Registers and Their Roles:
- **AdcRegs.ADCCTL1, ADCCTL2:** Configure ADC mode and trigger position.
- **AdcRegs.INTSEL1N2:** Set up ADC interrupt and trigger. INT1E enables ADCINT1, INT1CONT determines if the interrupt is continuous, INT1SEL selects which SOC triggers the interrupt.
- **AdcRegs.ADCSOCxCTL:** For each SOC (Start of Conversion), sets channel selection (CHSEL), trigger source (TRIGSEL), and acquisition period (ACQPS).
- **AdcRegs.ADCINTFLGCLR:** Clears the ADC interrupt flag.
- **PieCtrlRegs.PIEIER1, PIEACK:** Enables ADCINT1 interrupt in the PIE (Peripheral Interrupt Expansion) module and clears the flag after interrupt.
- **EPwm1Regs:** Configures the PWM module, used for ADC triggering.

### Flow
1. System and ADC are initialized.
2. Data is acquired from analog inputs via ADC interrupt.
3. Data is processed using Clark and Park transformations.
4. ADC is triggered by PWM, and continuous data processing occurs in the main loop.

This structure forms the basis for current/voltage measurement and vector control algorithms in electric vehicle charging stations.

---

For more technical details, please refer to the `ADC_CLARK_PARK/main.c` file.
