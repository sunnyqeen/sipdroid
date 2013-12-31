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
void g729a_Post_Process(
 g729a_post_process_state *state,
 Word16 sigin[],    /* input signal */
 Word16 sigout[],   /* output signal */
 Word16 lg          /* Length of signal    */
)
{
  Word16 i, x2;
  Word32 L_tmp;

  for(i=0; i<lg; i++)
  {
     x2 = state->x1;
     state->x1 = state->x0;
     state->x0 = sigin[i];

     /*  y[i] = b[0]*x[i]   + b[1]*x[i-1]   + b[2]*x[i-2]    */
     /*                     + a[1]*y[i-1] + a[2] * y[i-2];      */

     L_tmp     = Mpy_32_16(state->y1_hi, state->y1_lo, a100[1]);
     L_tmp     = L_add(L_tmp, Mpy_32_16(state->y2_hi, state->y2_lo, a100[2]));
     L_tmp     = L_mac(L_tmp, state->x0, b100[0]);
     L_tmp     = L_mac(L_tmp, state->x1, b100[1]);
     L_tmp     = L_mac(L_tmp, x2, b100[2]);
     L_tmp     = L_shl(L_tmp, 2);      /* Q29 --> Q31 (Q13 --> Q15) */

     /* Multiplication by two of output speech with saturation. */
     sigout[i] = round(L_shl(L_tmp, 1));

     state->y2_hi = state->y1_hi;
     state->y2_lo = state->y1_lo;
     L_Extract(L_tmp, &state->y1_hi, &state->y1_lo);
  }
}
