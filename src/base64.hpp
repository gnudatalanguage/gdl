#ifndef INCLUDED_Base64_H
#  define INCLUDED_Base64_H

// SA: based on:
// ----------------------------------------------------------------
// http://cvs.tekkotsu.org/viewvc/Tekkotsu/Shared/Base64.cc?view=co
// http://cvs.tekkotsu.org/viewvc/Tekkotsu/Shared/Base64.h?view=co
// ----------------------------------------------------------------
/* ...
 *     Copyright (c) 1999, Bob Withers - bwit@pobox.com
 *
 * This code may be freely used for any purpose, either personal
 * or commercial, provided the authors copyright notice remains
 * intact.
 *
 * Enhancements by Stanley Yamane:
 *     - reverse lookup table for the decode function 
 *     - reserve string buffer space in advance
 *
 * Modified by Ethan Tira-Thompson:
 *     - changed Base64 class to base64 namespace
 *     - modified functions to work on char[] instead of strings
 *     - added doxygen commenting
 *
 * author: Bob Withers - bwit@pobox.com (Creator)
 */

using namespace std;

namespace base64 {
	static const char fillchar = '='; //!< used to mark partial words at the end
	static const string::size_type np = string::npos; //!< shorthand for the "no position" return code from string
	
	//! this lookup table defines the base64 encoding
	const string Base64Table("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
	
	//! Decode Table gives the index of any valid base64 character in the Base64 table]
	/*! 65 == A, 97 == a, 48 == 0, 43 == +, 47 == / */
	const string::size_type DecodeTable[] = {
		np,np,np,np,np,np,np,np,np,np,  // 0 - 9
		np,np,np,np,np,np,np,np,np,np,  //10 -19
		np,np,np,np,np,np,np,np,np,np,  //20 -29
		np,np,np,np,np,np,np,np,np,np,  //30 -39
		np,np,np,62,np,np,np,63,52,53,  //40 -49
		54,55,56,57,58,59,60,61,np,np,  //50 -59
		np,np,np,np,np, 0, 1, 2, 3, 4,  //60 -69
		5, 6, 7, 8, 9,10,11,12,13,14,  //70 -79
		15,16,17,18,19,20,21,22,23,24,  //80 -89
		25,np,np,np,np,np,np,26,27,28,  //90 -99
		29,30,31,32,33,34,35,36,37,38,  //100 -109
		39,40,41,42,43,44,45,46,47,48,  //110 -119
		49,50,51,np,np,np,np,np,np,np,  //120 -129
		np,np,np,np,np,np,np,np,np,np,  //130 -139
		np,np,np,np,np,np,np,np,np,np,  //140 -149
		np,np,np,np,np,np,np,np,np,np,  //150 -159
		np,np,np,np,np,np,np,np,np,np,  //160 -169
		np,np,np,np,np,np,np,np,np,np,  //170 -179
		np,np,np,np,np,np,np,np,np,np,  //180 -189
		np,np,np,np,np,np,np,np,np,np,  //190 -199
		np,np,np,np,np,np,np,np,np,np,  //200 -209
		np,np,np,np,np,np,np,np,np,np,  //210 -219
		np,np,np,np,np,np,np,np,np,np,  //220 -229
		np,np,np,np,np,np,np,np,np,np,  //230 -239
		np,np,np,np,np,np,np,np,np,np,  //240 -249
		np,np,np,np,np,np               //250 -256
	};
	
	
	string encode(char buf[], unsigned int len) {
		string             ret;
		if(len==0)
			return "";
		ret.reserve((len-1)/3*4 + 4 + 1);
		
		for (string::size_type i = 0; i < len; ++i)
		{
			char c;
			c = (buf[i] >> 2) & 0x3f;
			ret.append(1, Base64Table[c]);
			c = (buf[i] << 4) & 0x3f;
			if (++i < len)
				c |= (buf[i] >> 4) & 0x0f;
			
			ret.append(1, Base64Table[c]);
			if (i < len)
			{
				c = (buf[i] << 2) & 0x3f;
				if (++i < len)
					c |= (buf[i] >> 6) & 0x03;
				
				ret.append(1, Base64Table[c]);
			}
			else
			{
				++i;
				ret.append(1, fillchar);
			}
			
			if (i < len)
			{
				c = buf[i] & 0x3f;
				ret.append(1, Base64Table[c]);
			}
			else
			{
				ret.append(1, fillchar);
			}
		}
		
		return(ret);
	}
	
	unsigned int decodeSize(const std::string& data) {
		unsigned int fills=0;
		unsigned int size=data.size();
		//skip any extra characters at the end (e.g. newlines or spaces)
		while(size>0 && DecodeTable[(unsigned char)data[size-1]]==np && data[size-1]!=fillchar)
			size--;
		if((size/4) * 4 != size) {
			Warning("base 64 decodeSize error: data size is not multiple of 4");
			return size/4*3 + 3;
		}
		//count fill characters ('=')
		while(fills<size && data[size-fills-1]==fillchar)
			fills++;
		if(fills>2) {
			Warning("base 64 decodeSize error: too many fill characters");
			size-=(fills/3) * 3;
			fills=fills%3;
		}
		//cout << "Size is " << size/4*3-fills << " with " << fills << " fills and " << data.size()-size << " extra" << endl;
		return size/4*3-fills;
	}

	bool decode(const std::string& data, char buf[], unsigned int len) {
		const string::size_type  size = data.length();
		string::size_type i = 0, p=0;
		while(true)
		{
			bool retry=false;
			string::size_type d=np;
			do {
				d=DecodeTable[(unsigned char)data[i++]];
				retry=false;
				if(d==np) {
					if(fillchar == data[i-1]) {
						Warning("base64 decode error: unexpected fill char -- offset read?");
						return false;
					}
					if(!(isspace(data[i-1]))) {
						//cerr << "base 64 decode error: illegal character '" << data[i-1] << "' (0x" << std::hex << (int)data[i-1] << std::dec << ")" << endl;
						Warning("base 64 decode error: illegal character");
						return false;
					}
					retry=true;
				}
			} while(retry);
			char c = (char) d;
			
			do {
				d=DecodeTable[(unsigned char)data[i++]];
				retry=false;
				if(d==np) {
					if(fillchar == data[i-1]) {
						Warning("base64 decode error: unexpected fill char -- offset read?");
						return false;
					}
					if(!(isspace(data[i-1]))) {
						//cerr << "base 64 decode error: illegal character '" << data[i-1] << '\'' << endl;
						Warning("base 64 decode error: illegal character");
						return false;
					}
					retry=true;
				}
			} while(retry);
			if(p>=len)
				return false;
			char c1 = (char) d;
			c = (c << 2) | ((c1 >> 4) & 0x3);
			buf[p++]=c;
			if(i>=size) {
				Warning("base64 decode error: Truncated data");
				return false;
			}

			do {
				d=DecodeTable[(unsigned char)data[i++]];
				retry=false;
				if(d==np) {
					if(fillchar == data[i-1]) {
						return true;
					}
					if((!isspace(data[i-1]))) {
						//cerr << "base 64 decode error: illegal character '" << data[i-1] << '\'' << endl;
						Warning("base 64 decode error: illegal character");
						return false;
					}
					retry=true;
				}
			} while(retry);
			if(p>=len)
				return false;
			c = (char) d;
			c1 = ((c1 << 4) & 0xf0) | ((c >> 2) & 0xf);
			buf[p++]=c1;
			if(i>=size) {
				Warning("base64 decode error: Truncated data");
				return false;
			}

			do {
				d=DecodeTable[(unsigned char)data[i++]];
				retry=false;
				if(d==np) {
					if(fillchar == data[i-1]) {
						return true;
					}
					if(!(isspace(data[i-1]))) {
						//cerr << "base 64 decode error: illegal character '" << data[i-1] << '\'' << endl;
						Warning("base 64 decode error: illegal character");
						return false;
					}
					retry=true;
				}
			} while(retry);
			if(p>=len)
				return false;
			c1 = (char) d;
			c = ((c << 6) & 0xc0) | c1;
			buf[p++]=c;
			if(i==size)
				return true;
		}
		//Warning("base64 decode error: unexpected break");
		//return false;
	}
}

#endif
