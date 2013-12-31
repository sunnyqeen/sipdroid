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

void Decod_ACELP(
  Word16 sign,      /* (i)     : signs of 4 pulses.                       */
  Word16 index,     /* (i)     : Positions of the 4 pulses.               */
  Word16 cod[]      /* (o) Q13 : algebraic (fixed) codebook excitation    */
)
{
  Word16 i, j;
  Word16 pos[4];


  /* Decode the positions */

  i      = index & (Word16)7;
  pos[0] = add(i, shl(i, 2));           /* pos0 =i*5 */

  index  = shr(index, 3);
  i      = index & (Word16)7;
  i      = add(i, shl(i, 2));           /* pos1 =i*5+1 */
  pos[1] = add(i, 1);

  index  = shr(index, 3);
  i      = index & (Word16)7;
  i      = add(i, shl(i, 2));           /* pos2 =i*5+1 */
  pos[2] = add(i, 2);

  index  = shr(index, 3);
  j      = index & (Word16)1;
  index  = shr(index, 1);
  i      = index & (Word16)7;
  i      = add(i, shl(i, 2));           /* pos3 =i*5+3+j */
  i      = add(i, 3);
  pos[3] = add(i, j);

  /* decode the signs  and build the codeword */

  for (i=0; i<L_SUBFR; i++) {
    cod[i] = 0;
  }

  for (j=0; j<4; j++)
  {

    i = sign & (Word16)1;
    sign = shr(sign, 1);

    if (i != 0) {
      cod[pos[j]] = 8191;      /* Q13 +1.0 */
    }
    else {
      cod[pos[j]] = -8192;     /* Q13 -1.0 */
    }
  }
}

