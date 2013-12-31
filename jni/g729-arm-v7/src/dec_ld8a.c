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
 *   Functions Init_Decod_ld8a  and Decod_ld8a                     *
 *-----------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "ld8a.h"

#include "g729a_decoder.h"

        /* Lsp (Line spectral pairs) */
static Word16 lsp_old_i[M]={
             30000, 26000, 21000, 15000, 8000, 0, -8000,-15000,-21000,-26000};

/*-----------------------------------------------------------------*
 *   Function Init_Decod_ld8a                                      *
 *            ~~~~~~~~~~~~~~~                                      *
 *                                                                 *
 *   ->Initialization of variables for the decoder section.        *
 *                                                                 *
 *-----------------------------------------------------------------*/
void g729a_Init_Decod_ld8a(g729a_decoder_state *state)
{

  /* Initialize static pointer */

  state->exc = state->old_exc + PIT_MAX + L_INTERPOL;

  /* Static vectors to zero */

  Set_zero(state->old_exc, PIT_MAX+L_INTERPOL);
  Set_zero(state->mem_syn, M);

  Copy(lsp_old_i, state->lsp_old, M);
  state->sharp  = SHARPMIN;
  state->old_T0 = 60;
  state->gain_code = 0;
  state->gain_pitch = 0;

  Lsp_decw_reset(state);
}

#if defined(__EQUIVALENT_C__) && (__EQUIVALENT_C__!=0)
#  include "dec_ld8a_equivalent_c.c"
#else /* !__EQUIVALENT_C__ */
#  include "dec_ld8a_optimized.c"
#endif /* __EQUIVALENT_C__ */

