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
/*-----------------------------------------------------------*
 *  Function  Decod_ACELP()                                  *
 *  ~~~~~~~~~~~~~~~~~~~~~~~                                  *
 *   Algebraic codebook decoder.                             *
 *----------------------------------------------------------*/

void Decod_ACELP(  Word16 sign,      /* (i)     : signs of 4 pulses.                       */  Word16 index,     /* (i)     : Positions of the 4 pulses.               */  Word16 cod[]      /* (o) Q13 : algebraic (fixed) codebook excitation    */){  Word16 i, j;

  /* Decode the positions */
  /* decode the signs  and build the codeword */  Set_zero(cod, L_SUBFR);

  i      = index & (Word16)7;
  cod[5*i] = ((sign & (Word16)1) ? 8191 : -8192);  index  >>= 3;  i      = index & (Word16)7;
  cod[5*i+1] = ((sign & (Word16)2) ? 8191 : -8192);  index >>= 3;  i      = index & (Word16)7;
  cod[5*i+2] = ((sign & (Word16)4) ? 8191 : -8192);  index >>= 3;  j      = index & (Word16)1;  index >>= 1;  i      = index & (Word16)7;
  cod[5*i+3+j] = ((sign & (Word16)8) ? 8191 : -8192);}

