/**
 *  g729a codec for iOS,...
 *  Copyright (C) 2009-2012 Samuel <samuelv0304@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/****************************************************************************************
Portions of this file are derived from the following ITU standard:
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
****************************************************************************************/

/*------------------------------------------------------------------------*
 * Function Pre_Process()                                                 *
 *                                                                        *
 * Preprocessing of input speech.                                         *
 *   - 2nd order high pass filter with cut off frequency at 140 Hz.       *
 *   - Divide input by two.                                               *
 *-----------------------------------------------------------------------*/
void g729a_Pre_Process(  g729a_pre_process_state *state,  Word16 sigin[],    /* input signal */  Word16 sigout[],   /* output signal */  Word16 lg)          /* length of signal    */
{
  Word16 i, x2;
  Word32 L_tmp;

  for(i=0; i<lg; i++)
  {
     x2 = state->x1;
     state->x1 = state->x0;
     state->x0 = sigin[i];

     /*  y[i] = b[0]*x[i]/2 + b[1]*x[i-1]/2 + b140[2]*x[i-2]/2  */
     /*                     + a[1]*y[i-1] + a[2] * y[i-2];      */

     L_tmp     = Mpy_32_16(state->y1_hi, state->y1_lo, a140[1]);
     L_tmp     = L_add(L_tmp, Mpy_32_16(state->y2_hi, state->y2_lo, a140[2]));
     L_tmp     = L_mac(L_tmp, state->x0, b140[0]);
     L_tmp     = L_mac(L_tmp, state->x1, b140[1]);
     L_tmp     = L_mac(L_tmp, x2, b140[2]);
     L_tmp     = L_shl(L_tmp, 3);      /* Q28 --> Q31 (Q12 --> Q15) */
     sigout[i] = round(L_tmp);

     state->y2_hi = state->y1_hi;
     state->y2_lo = state->y1_lo;
     L_Extract(L_tmp, &(state->y1_hi), &(state->y1_lo));
  }
}
