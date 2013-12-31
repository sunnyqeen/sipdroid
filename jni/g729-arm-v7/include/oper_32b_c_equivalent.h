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

#ifndef __OPER_32B_C_EQUIVALENT_H__
#define __OPER_32B_C_EQUIVALENT_H__

#define __EQUIVALENT_C__ 1

/* Double precision operations */
void   L_Extract(Word32 L_32, Word16 *hi, Word16 *lo);
Word32 L_Comp(Word16 hi, Word16 lo);
Word32 Mpy_32(Word16 hi1, Word16 lo1, Word16 hi2, Word16 lo2);
Word32 Mpy_32_16(Word16 hi, Word16 lo, Word16 n);
Word32 Div_32(Word32 L_num, Word16 denom_hi, Word16 denom_lo);

#endif /* __OPER_32B_C_EQUIVALENT_H__ */
