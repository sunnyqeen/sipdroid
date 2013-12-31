/* ITU-T G.729 Software Package Release 2 (November 2006) */
/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/

/*------------------------------------------------------------------------*
 * Function Post_Process()                                                *
 *                                                                        *
 * Post-processing of output speech.                                      *
 *   - 2nd order high pass filter with cut off frequency at 100 Hz.       *
 *   - Multiplication by two of output speech with saturation.            *
 *-----------------------------------------------------------------------*/

/* acelp_high_pass_filter */
void g729a_Post_Process(
 g729a_post_process_state *state,
 Word16 sigin[],    /* input signal */
 Word16 sigout[],   /* output signal */
 Word16 lg          /* Length of signal    */
)
{  Word16 i;  Word32 L_tmp;  for(i=0; i<lg; i++)  {     /*  y[i] = b[0]*x[i]   + b[1]*x[i-1]   + b[2]*x[i-2]    */     /*                     + a[1]*y[i-1] + a[2] * y[i-2];      */     L_tmp  = ((Word32) state->y1_hi) * 15836;     L_tmp += (Word32)(((Word32) state->y1_lo * 15836) >> 15);     L_tmp += ((Word32) state->y2_hi) * (-7667);     L_tmp += (Word32)(((Word32) state->y2_lo * (-7667)) >> 15);     L_tmp += 7699 * (sigin[i] - 2*state->x1/*signal[i-1]*/ + state->x2/*signal[i-2]*/);    //L_tmp  = (y1 * 15836) >> 16;    //L_tmp += (y2 * -7667) >> 16;    //L_tmp += 7699 * (signal[i] - 2*x1/*signal[i-1]*/ + x2/*signal[i-2]*/);    L_tmp  = L_shl(L_tmp, 3);    state->x2 = state->x1;    state->x1 = sigin[i];     /* Multiplication by two of output speech with saturation. */    sigout[i] = round(L_shl(L_tmp, 1));    state->y2_hi = state->y1_hi;    state->y2_lo = state->y1_lo;    state->y1_hi = (Word16) (L_tmp >> 16);    state->y1_lo = (Word16)((L_tmp >> 1) - (state->y1_hi << 15));    //y2 = y1;    //y1 = L_tmp;  }}
