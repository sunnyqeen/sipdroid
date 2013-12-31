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

/*-----------------------------------------------------------------*
 *   Function Decod_ld8a                                           *
 *           ~~~~~~~~~~                                            *
 *   ->Main decoder routine.                                       *
 *                                                                 *
 *-----------------------------------------------------------------*/
void g729a_Decod_ld8a(
  g729a_decoder_state *state,
  Word16  parm[],      /* (i)   : vector of synthesis parameters
                                  parm[0] = bad frame indicator (bfi)  */
  Word16  synth[],     /* (o)   : synthesis speech                     */
  Word16  A_t[],       /* (o)   : decoded LP filter in 2 subframes     */
  Word16  *T2,         /* (o)   : decoded pitch lag in 2 subframes     */
  Word16 bad_lsf       /* (i)   : bad LSF indicator                    */
)
{
  Word16  *Az;                  /* Pointer on A_t   */
  Word16  lsp_new[M];           /* LSPs             */
  Word16  code[L_SUBFR];        /* ACELP codevector */

  /* Scalars */

  Word16  i, j, i_subfr;
  Word16  T0, T0_frac, index;
  Word16  bfi;
  Word32  L_temp;

  Word16 bad_pitch;             /* bad pitch indicator */
  //extern Word16 bad_lsf;        /* bad LSF indicator   */

  /* Test bad frame indicator (bfi) */

  bfi = *parm++;

  /* Decode the LSPs */

  D_lsp(state, parm, lsp_new, add(bfi, bad_lsf));
  parm += 2;

  /*
  Note: "bad_lsf" is introduce in case the standard is used with
         channel protection.
  */

  /* Interpolation of LPC for the 2 subframes */

  Int_qlpc(state->lsp_old, lsp_new, A_t);

  /* update the LSFs for the next frame */

  Copy(lsp_new, state->lsp_old, M);

/*------------------------------------------------------------------------*
 *          Loop for every subframe in the analysis frame                 *
 *------------------------------------------------------------------------*
 * The subframe size is L_SUBFR and the loop is repeated L_FRAME/L_SUBFR  *
 *  times                                                                 *
 *     - decode the pitch delay                                           *
 *     - decode algebraic code                                            *
 *     - decode pitch and codebook gains                                  *
 *     - find the excitation and compute synthesis speech                 *
 *------------------------------------------------------------------------*/

  Az = A_t;            /* pointer to interpolated LPC parameters */

  for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR)
  {

    index = *parm++;            /* pitch index */

    if(i_subfr == 0)
    {
      i = *parm++;              /* get parity check result */
      bad_pitch = add(bfi, i);
      if( bad_pitch == 0)
      {
        Dec_lag3(index, PIT_MIN, PIT_MAX, i_subfr, &T0, &T0_frac);
        state->old_T0 = T0;
      }
      else        /* Bad frame, or parity error */
      {
        T0  =  state->old_T0;
        T0_frac = 0;
        state->old_T0 = add( state->old_T0, 1);
        if( sub(state->old_T0, PIT_MAX) > 0) {
            state->old_T0 = PIT_MAX;
        }
      }
    }
    else                  /* second subframe */
    {
      if( bfi == 0)
      {
        Dec_lag3(index, PIT_MIN, PIT_MAX, i_subfr, &T0, &T0_frac);
        state->old_T0 = T0;
      }
      else
      {
        T0  =  state->old_T0;
        T0_frac = 0;
        state->old_T0 = add( state->old_T0, 1);
        if( sub(state->old_T0, PIT_MAX) > 0) {
            state->old_T0 = PIT_MAX;
        }
      }
    }
    *T2++ = T0;

   /*-------------------------------------------------*
    * - Find the adaptive codebook vector.            *
    *-------------------------------------------------*/

    Pred_lt_3(&state->exc[i_subfr], T0, T0_frac, L_SUBFR);

   /*-------------------------------------------------------*
    * - Decode innovative codebook.                         *
    * - Add the fixed-gain pitch contribution to code[].    *
    *-------------------------------------------------------*/

    if(bfi != 0)        /* Bad frame */
    {

      parm[0] = Random() & (Word16)0x1fff;     /* 13 bits random */
      parm[1] = Random() & (Word16)0x000f;     /*  4 bits random */
    }
    Decod_ACELP(parm[1], parm[0], code);
    parm +=2;

    j = shl(state->sharp, 1);          /* From Q14 to Q15 */
    if(sub(T0, L_SUBFR) <0 ) {
        for (i = T0; i < L_SUBFR; i++) {
          code[i] = add(code[i], mult(code[i-T0], j));
        }
    }

   /*-------------------------------------------------*
    * - Decode pitch and codebook gains.              *
    *-------------------------------------------------*/

    index = *parm++;      /* index of energy VQ */

    Dec_gain(index, code, L_SUBFR, bfi, &state->gain_pitch, &state->gain_code);

   /*-------------------------------------------------------------*
    * - Update pitch sharpening "sharp" with quantized gain_pitch *
    *-------------------------------------------------------------*/

    state->sharp = state->gain_pitch;
    if (sub(state->sharp, SHARPMAX) > 0) { state->sharp = SHARPMAX;  }
    if (sub(state->sharp, SHARPMIN) < 0) { state->sharp = SHARPMIN;  }

   /*-------------------------------------------------------*
    * - Find the total excitation.                          *
    * - Find synthesis speech corresponding to exc[].       *
    *-------------------------------------------------------*/

    for (i = 0; i < L_SUBFR;  i++)
    {
       /* exc[i] = gain_pitch*exc[i] + gain_code*code[i]; */
       /* exc[i]  in Q0   gain_pitch in Q14               */
       /* code[i] in Q13  gain_codeode in Q1              */

       L_temp = L_mult(state->exc[i+i_subfr], state->gain_pitch);
       L_temp = L_mac(L_temp, code[i], state->gain_code);
       L_temp = L_shl(L_temp, 1);
       state->exc[i+i_subfr] = round(L_temp);
    }

    Overflow = 0;
    Syn_filt(Az, &state->exc[i_subfr], &synth[i_subfr], L_SUBFR, state->mem_syn, 0);
    if(Overflow != 0)
    {
      /* In case of overflow in the synthesis          */
      /* -> Scale down vector exc[] and redo synthesis */

      for(i=0; i<PIT_MAX+L_INTERPOL+L_FRAME; i++)
        state->old_exc[i] = shr(state->old_exc[i], 2);

      Syn_filt(Az, &state->exc[i_subfr], &synth[i_subfr], L_SUBFR, state->mem_syn, 1);
    }
    else
      Copy(&synth[i_subfr+L_SUBFR-M], state->mem_syn, M);

    Az += MP1;    /* interpolated LPC parameters for next subframe */
  }

 /*--------------------------------------------------*
  * Update signal for next frame.                    *
  * -> shift to the left by L_FRAME  exc[]           *
  *--------------------------------------------------*/

  Copy(&state->old_exc[L_FRAME], &state->old_exc[0], PIT_MAX+L_INTERPOL);
}
