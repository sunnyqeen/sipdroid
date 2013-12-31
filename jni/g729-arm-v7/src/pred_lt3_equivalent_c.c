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
 * Function  Pred_lt_3()                                             *
 *           ~~~~~~~~~~~                                             *
 *-------------------------------------------------------------------*
 * Compute the result of long term prediction with fractional        *
 * interpolation of resolution 1/3.                                  *
 *                                                                   *
 * On return exc[0..L_subfr-1] contains the interpolated signal      *
 *   (adaptive codebook excitation)                                  *
 *-------------------------------------------------------------------*/

void Pred_lt_3(
  Word16   exc[],       /* in/out: excitation buffer */
  Word16   T0,          /* input : integer pitch lag */
  Word16   frac,        /* input : fraction of lag   */
  Word16   L_subfr      /* input : subframe size     */
)
{
  Word16   i, j, k;
  Word16   *x0, *x1, *x2, *c1, *c2;
  Word32  s;

  x0 = &exc[-T0];

  frac = negate(frac);
  if (frac < 0)
  {
    frac = add(frac, UP_SAMP);
    x0--;
  }

  for (j=0; j<L_subfr; j++)
  {
    x1 = x0++;
    x2 = x0;
    c1 = &inter_3l[frac];
    c2 = &inter_3l[sub(UP_SAMP,frac)];

    s = 0;
    for(i=0, k=0; i< L_INTER10; i++, k+=UP_SAMP)
    {
      s = L_mac(s, x1[-i], c1[k]);
      s = L_mac(s, x2[i],  c2[k]);
    }

    exc[j] = round(s);
  }
}
