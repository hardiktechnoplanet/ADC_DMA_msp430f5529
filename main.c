#include <msp430.h>

unsigned int DMA_DST;						// ADC conversion result is stored in this variable
volatile unsigned int results[10];
int main(void)
{
  WDTCTL = WDTPW+WDTHOLD;                   // Hold WDT

  P1OUT &= ~BIT0;                           // P1.0 clear
  P1DIR |= BIT0;                            // P1.0 output
  P4OUT &= ~BIT7;                           // P1.0 clear
  P1DIR |= BIT7;                            // P1.0 output
 
  P6SEL |= BIT0;                            // Enable A/D channel A0

/////////////////////////////////////////////////////SET UP ADC_12////////////////////////////////////////////////////////////////////////////

    ADC12CTL0 = ADC12SHT03 + ADC12ON+ADC12MSC;         // Sampling time, ADC12 on
    ADC12CTL1 = ADC12SHS_0+ADC12CONSEQ_2;
    ADC12CTL1 |= ADC12SHP+ADC12DIV_4;                     // Use sampling timer
    ADC12CTL2 |= ADC12PDIV;
   // ADC12IE = 0x01;                           // Enable interrupt
    ADC12MCTL0 = ADC12SREF_2+ADC12INCH_0;
    ADC12CTL0 |= ADC12ENC;
    ADC12MCTL0 = ADC12SREF_2;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////Setup DMA0///////////////////////////////////////////////////////////////
  DMACTL0 = DMA0TSEL_24;                    // ADC12IFGx triggered
  DMACTL4 = DMARMWDIS;                      // Read-modify-write disable
  DMA0CTL &= ~DMAIFG;
  DMA0CTL = DMADT_4+DMAEN+DMADSTINCR_3+DMAIE; // Rpt single tranfer, inc dst, Int
  DMA0SZ = 1;                               // DMA0 size = 1
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   while (1)
    {
      ADC12CTL0 |= ADC12SC;                   // Start sampling/conversion
  __data16_write_addr((unsigned short) &DMA0SA,(unsigned long) &ADC12MEM0);
                                            // Source block address
  __data16_write_addr((unsigned short) &DMA0DA,(unsigned long) &DMA_DST);
                                            // Destination single address
  __bis_SR_register(GIE);       // LPM0 w/ interrupts
  __no_operation();                         // used for debugging
}}

//------------------------------------------------------------------------------
// DMA Interrupt Service Routine
//------------------------------------------------------------------------------
#pragma vector=DMA_VECTOR
__interrupt void DMA_ISR(void)
{
  static unsigned int i=0;
  switch(__even_in_range(DMAIV,16))
  {
    case 0: break;
    case 2:                                 // DMA0IFG = DMA Channel 0
    	      P1OUT ^= BIT0;                        // Toggle P1.0 - PLACE BREAKPOINT HERE AND CHECK DMA_DST VARIABLE

      results[i]=DMA_DST;
    	    i++;
    	    
      break;
    case 4: break;                          // DMA1IFG = DMA Channel 1
    case 6: break;                          // DMA2IFG = DMA Channel 2
    case 8: break;                          // DMA3IFG = DMA Channel 3
    case 10: break;                         // DMA4IFG = DMA Channel 4
    case 12: break;                         // DMA5IFG = DMA Channel 5
    case 14: break;                         // DMA6IFG = DMA Channel 6
    case 16: break;                         // DMA7IFG = DMA Channel 7
    default: break;
  }
}
