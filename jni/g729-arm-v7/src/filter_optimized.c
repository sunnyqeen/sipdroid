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

/*-----------------------------------------------------*
 * procedure Syn_filt:                                 *
 *           ~~~~~~~~                                  *
 * Do the synthesis filtering 1/A(z).                  *
 *-----------------------------------------------------*/

/* ff_celp_lp_synthesis_filter */
Flag Syn_filt_overflow(
  Word16 a[],     /* (i) Q12 : a[m+1] prediction coefficients   (m=10)  */
  Word16 x[],     /* (i)     : input signal                             */
  Word16 y[],     /* (o)     : output signal                            */
  Word16 lg,      /* (i)     : size of filtering                        */
  Word16 mem[]    /* (i)     : memory associated with this filtering.   */
)
{
  Word16 i, j;
  Word32 s, t;
  Word16 tmp[100];     /* This is usually done by memory allocation (lg+M) */
  //Word16 *yy;

  /* Copy mem[] to yy[] */

  //yy = tmp;

  Copy(mem, tmp, M);
  //yy += M;

  /* Do the filtering. */
  for (i = 0; i < lg; i++)
  {
    s = x[i] * a[0];
    for (j = 1; j <= M; j++)
      //s -= a[j] * yy[-j];
      s -= a[j] * tmp[M+i-j];

    t = s << 4;
    if (t >> 4 != s)
    {
      // *yy++ = s & MIN_32 ? MIN_16 : MAX_16;
      return 1;
    }
    else
      // *yy++ = (t + 0x8000) >> 16;
      y[i] = tmp[M+i] = (t + 0x8000) >> 16;
  }

  //Copy(&tmp[M], y, lg);

  return 0;
}


/* ff_celp_lp_synthesis_filter */
void Syn_filt(
  Word16 a[],     /* (i) Q12 : a[m+1] prediction coefficients   (m=10)  */
  Word16 x[],     /* (i)     : input signal                             */
  Word16 y[],     /* (o)     : output signal                            */
  Word16 lg,      /* (i)     : size of filtering                        */
  Word16 mem[],   /* (i/o)   : memory associated with this filtering.   */
  Word16 update   /* (i)     : 0=no update, 1=update of memory.         */
)
{
  Word16 i, j;
  Word32 s, t;
  Word16 tmp[100];     /* This is usually done by memory allocation (lg+M) */

  /* Copy mem[] to yy[] */
  Copy(mem, tmp, M);

  /* Do the filtering. */
  for (i = 0; i < lg; i++)
  {
    s = x[i] * a[0];
    for (j = 1; j <= M; j++)
      s -= a[j] * tmp[M+i-j];

/*00065     for (n = 0; n < buffer_length; n++) {
00066         int sum = rounder;
00067         for (i = 1; i <= filter_length; i++)
00068             sum -= filter_coeffs[i-1] * out[n-i];
00069
00070         sum = ((sum >> 12) + in[n]) >> shift;
00071 
00072         if (sum + 0x8000 > 0xFFFFU) {
00073             if (stop_on_overflow)
00074                 return 1;
00075             sum = (sum >> 31) ^ 32767;
00076         }
00077         out[n] = sum;*/
      
    t = s << 4;
    if (t >> 4 != s)
    	y[i] = s & MIN_32 ? MIN_16 : MAX_16;
    	//y[i] = (s >> 31) ^ 32767;
    else
   		y[i] = (t + 0x8000) >> 16;
   	tmp[M+i] = y[i];
  }

 /* Update of memory if update==1 */
 if (update)
     Copy(&y[lg-M], mem, M);
}

/*-----------------------------------------------------------------------*
 * procedure Residu:                                                     *
 *           ~~~~~~                                                      *
 * Compute the LPC residual  by filtering the input speech through A(z)  *
 *-----------------------------------------------------------------------*/

void Residu(
  Word16 a[],    /* (i) Q12 : prediction coefficients                     */
  Word16 x[],    /* (i)     : speech (values x[-m..-1] are needed         */
  Word16 y[],    /* (o)     : residual signal                             */
  Word16 lg      /* (i)     : size of filtering                           */
)
{
  Word16 i, j;
  Word32 s;

  for (i = 0; i < lg; i++)
  {
    s = x[i] * a[0];
    for (j = 1; j <= M; j++)
      s += a[j] * x[i-j];

    y[i] = (s + 0x800) >> 12;
  }
}
