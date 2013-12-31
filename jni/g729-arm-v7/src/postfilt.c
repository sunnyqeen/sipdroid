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
 *                         POSTFILTER.C                                   *
 *------------------------------------------------------------------------*
 * Performs adaptive postfiltering on the synthesis speech                *
 * This file contains all functions related to the post filter.           *
 *------------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "ld8a.h"
#include "oper_32b.h"

#include "g729a_decoder.h"

static void agc(  Word16 *sig_in,   /* (i)     : postfilter input signal  */  Word16 *sig_out,  /* (i/o)   : postfilter output signal */  Word16 l_trm      /* (i)     : subframe size            */);static void preemphasis(  Word16 *signal,  /* (i/o)   : input signal overwritten by the output */  Word16 g,        /* (i) Q15 : preemphasis coefficient                */  Word16 L         /* (i)     : size of filtering                      */);static void pit_pst_filt(    Word16 *signal,      /* (i)     : input signal                        */    Word16 *scal_sig,    /* (i)     : input signal (scaled, divided by 4) */    Word16 t0_min,       /* (i)     : minimum value in the searched range */    Word16 t0_max,       /* (i)     : maximum value in the searched range */    Word16 L_subfr,      /* (i)     : size of filtering                   */    Word16 *signal_pst   /* (o)     : harmonically postfiltered signal    */  );

/*---------------------------------------------------------------*
 *    Postfilter constant parameters (defined in "ld8a.h")       *
 *---------------------------------------------------------------*
 *   L_FRAME     : Frame size.                                   *
 *   L_SUBFR     : Sub-frame size.                               *
 *   M           : LPC order.                                    *
 *   MP1         : LPC order+1                                   *
 *   PIT_MAX     : Maximum pitch lag.                            *
 *   GAMMA2_PST  : Formant postfiltering factor (numerator)      *
 *   GAMMA1_PST  : Formant postfiltering factor (denominator)    *
 *   GAMMAP      : Harmonic postfiltering factor                 *
 *   MU          : Factor for tilt compensation filter           *
 *   AGC_FAC     : Factor for automatic gain control             *
 *---------------------------------------------------------------*/

/*---------------------------------------------------------------*
 * Procedure    Init_Post_Filter:                                *
 *              ~~~~~~~~~~~~~~~~                                 *
 *  Initializes the postfilter parameters:                       *
 *---------------------------------------------------------------*/

void g729a_Init_Post_Filter(g729a_post_filter_state *state)
{
  state->res2  = state->res2_buf + PIT_MAX;
  state->scal_res2  = state->scal_res2_buf + PIT_MAX;

  Set_zero(state->mem_syn_pst, M);
  Set_zero(state->res2_buf, PIT_MAX+L_SUBFR);
  Set_zero(state->scal_res2_buf, PIT_MAX+L_SUBFR);
}

#if defined(__EQUIVALENT_C__) && (__EQUIVALENT_C__!=0)
#  include "postfilt_equivalent_c.c"
#else /* !__EQUIVALENT_C__ */
#  include "postfilt_optimized.c"
#endif /* __EQUIVALENT_C__ */
