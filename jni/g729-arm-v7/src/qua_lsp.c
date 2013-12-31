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

/*-------------------------------------------------------------------*
 * Function  Qua_lsp:                                                *
 *           ~~~~~~~~                                                *
 *-------------------------------------------------------------------*/
#include "typedef.h"
#include "basic_op.h"

#include "ld8a.h"
#include "tab_ld8a.h"

#include "g729a_encoder.h"

static void Lsp_qua_cs(
  g729a_encoder_state *state,
  Word16 flsp_in[M],    /* (i) Q13 : Original LSP parameters    */
  Word16 lspq_out[M],   /* (o) Q13 : Quantized LSP parameters   */
  Word16 *code          /* (o)     : codes of the selected LSP  */
);


/* static memory */
static Word16 freq_prev_reset[M] = {  /* Q13:previous LSP vector(init) */
  2339, 4679, 7018, 9358, 11698, 14037, 16377, 18717, 21056, 23396
};     /* PI*(float)(j+1)/(float)(M+1) */

static void Get_wegt(Word16 flsp[], Word16 wegt[]);
static void Lsp_pre_select(Word16 rbuf[], Word16 lspcb1[][M], Word16 *cand);
static void Lsp_get_tdist(Word16 wegt[], Word16 buf[], Word32 *L_tdist, Word16 rbuf[],  Word16 fg_sum[]);
static void Relspwed(Word16 lsp[], Word16 wegt[], Word16 lspq[], Word16 lspcb1[][M], 
					 Word16 lspcb2[][M], Word16 fg[MODE][MA_NP][M], Word16 freq_prev[MA_NP][M],
					 Word16 fg_sum[MODE][M], Word16 fg_sum_inv[MODE][M], Word16 code_ana[]);


void Qua_lsp(
  g729a_encoder_state *state,
  Word16 lsp[],       /* (i) Q15 : Unquantized LSP            */
  Word16 lsp_q[],     /* (o) Q15 : Quantized LSP              */
  Word16 ana[]        /* (o)     : indexes                    */
)
{
  Word16 lsf[M], lsf_q[M];  /* domain 0.0<= lsf <PI in Q13 */

  /* Convert LSPs to LSFs */
  Lsp_lsf2(lsp, lsf, M);

  Lsp_qua_cs(state, lsf, lsf_q, ana );

  /* Convert LSFs to LSPs */
  Lsf_lsp2(lsf_q, lsp_q, M);
}

void Lsp_encw_reset(
  g729a_encoder_state *state
)
{
  Word16 i;

  for(i=0; i<MA_NP; i++)
    Copy( &freq_prev_reset[0], &state->freq_prev[i][0], M );
}

void Lsp_qua_cs(
  g729a_encoder_state *state,
  Word16 flsp_in[M],    /* (i) Q13 : Original LSP parameters    */
  Word16 lspq_out[M],   /* (o) Q13 : Quantized LSP parameters   */
  Word16 *code          /* (o)     : codes of the selected LSP  */
)
{
  Word16 wegt[M];       /* Q11->normalized : weighting coefficients */

  Get_wegt( flsp_in, wegt );

  Relspwed( flsp_in, wegt, lspq_out, lspcb1, lspcb2, fg,
    state->freq_prev, fg_sum, fg_sum_inv, code);
}

#if defined(__EQUIVALENT_C__) && (__EQUIVALENT_C__!=0)
#  include "qua_lsp_equivalent_c.c"
#else /* !__EQUIVALENT_C__ */
#  include "qua_lsp_optimized.c"
#endif /* __EQUIVALENT_C__ */
