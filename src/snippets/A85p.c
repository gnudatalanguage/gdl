/* Copyright (c) 2010 MJ Rutter 
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA  02110-1301, USA.
 */

/* print a string ASCII85 encoded
 * MJR 4/01, 5/02
 */

#include<stdio.h>

/* Number of raw bytes to encode per line. Note actual line length will
 * be 1.25x this.  Sane values are 56 (70 characters) or 60 (75 chars) */
#define LINELEN 96 /* must be multiple of 4 */

unsigned int A85p(FILE* fout, unsigned char* in, int len)
{
  int i, j;
  unsigned int tmp, t, line, length = 0;
  unsigned int pow85[] = {52200625, 614125, 7225, 85, 1}; /* powers of 85 */
  unsigned char out[LINELEN / 4 * 5 + 4], *outp;

  while (len > 0) {
    line = (len > LINELEN) ? LINELEN : len;
    len = len - line;
    outp = out;
    for (i = 0; i < (line & 0xfffc); i += 4) { /* encode groups of four bytes */
      tmp = 0;
      for (j = 0; j < 4; j++) {
        tmp = tmp << 8;
        tmp += *(in++);
      }
      if (tmp) {
        for (j = 0; j < 4; j++) {
          t = tmp / pow85[j];
          tmp -= t * pow85[j];
          *(outp++) = t + '!';
        }
        *(outp++) = tmp + '!';
      } else
        *(outp++) = 'z'; /* zero is encoded as "z", not "!!!!!" */
    }

    if (line & 0x3) { /* Deal with any final group of <4 bytes */
      i = line & 0xfffc;
      tmp = 0;
      for (j = 0; j < 4; j++) {
        tmp = tmp << 8;
        if ((i + j) < line) tmp += *(in++);
      }
      for (j = 0; j < ((line & 0x3) + 1); j++) {
        t = tmp / pow85[j];
        tmp -= t * pow85[j];
        *(outp++) = t + '!';
      }
    }

    *outp = 0;
    /* Assist things which parse DSC comments by ensuring that any line which
     * would start %! or %% has a space prefixed.
     */
    if ((out[0] == '%')&&((out[1] == '%') || (out[1] == '!')))
      length += fprintf(fout, " %s\n", out);
    else length += fprintf(fout, "%s\n", out);
  }

  length += fprintf(fout, "~>\n");

  return length;

}
