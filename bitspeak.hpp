#ifndef BITSPEAK_HPP
#define BITSPEAK_HPP

/*
Encoding:
     ******** ******** ********
     CCCCCCCV VVVVCCCC CCCVVVVV
-CCCCCCC ---VVVVV -CCCCCCC ---VVVVV

C: Consonant
V: Vowel

Decoding:
-******* ---***** -******* ---*****
-xxxxxxx ---xyyyy -yyyyzzz ---zzzzz
     xxxxxxxx yyyyyyyy zzzzzzzz

*/

#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>

namespace bitspeak
{

static const std::string vows[32] = {
	"a" ,"e" ,"i" ,"o" ,"u" ,"w" ,"y" ,"ae",
	"ai","ao","au","aw","ay","ea","ee","ei",
	"eo","eu","ey","ia","io","iu","oa","oi",
	"ou","ow","oy","ui","uo","uy","we","wi"
};
static const std::string cons[128] = {
	"b"  ,"c"  ,"d"  ,"f"  ,"g"  ,"h"  ,"j"  ,"k"  ,
	"l"  ,"m"  ,"n"  ,"p"  ,"q"  ,"r"  ,"s"  ,"t"  ,
	"v"  ,"x"  ,"z"  ,"bc" ,"bl" ,"br" ,"bs" ,"ch" ,
	"chs","cht","ck" ,"cl" ,"cr" ,"cs" ,"ct" ,"fl" ,
	"fr" ,"fs" ,"ft" ,"gl" ,"gm" ,"gr" ,"gs" ,"gz" ,
	"hd" ,"hf" ,"hr" ,"hs" ,"ht" ,"hv" ,"jr" ,"js" ,
	"jt" ,"kl" ,"kr" ,"kt" ,"lb" ,"lc" ,"ld" ,"lg" ,
	"lm" ,"ln" ,"lp" ,"ls" ,"lt" ,"lv" ,"mb" ,"mn" ,
	"mp" ,"mr" ,"nd" ,"nk" ,"nm" ,"np" ,"ns" ,"nt" ,
	"nv" ,"nz" ,"pl" ,"pr" ,"ps" ,"pt" ,"pz" ,"rb" ,
	"rc" ,"rch","rd" ,"rf" ,"rg" ,"rk" ,"rl" ,"rm" ,
	"rn" ,"rp" ,"rr" ,"rs" ,"rt" ,"rv" ,"rz" ,"sb" ,
	"sc" ,"sch","sd" ,"sf" ,"sh" ,"sl" ,"sm" ,"sn" ,
	"sp" ,"sq" ,"sr" ,"st" ,"sth","str","sv" ,"sz" ,
	"tch","th" ,"tl" ,"tn" ,"tr" ,"ts" ,"vl" ,"vr" ,
	"vs" ,"vz" ,"xc" ,"xz" ,"zd" ,"zl" ,"zp" ,"zt"
};
static const std::string vow_l = "aeiouwy";
static const std::string con_l = "bcdfghjklmnpqrstvxz";

std::vector<std::string> split(const char* str,char delim)
{
	std::stringstream ss;
	std::string str1 = str;
	std::vector<std::string> splitted;
	std::string rfs;
	
	// Remove delimiters from the start and the end of str
	while (str1[0] == delim) { str1.erase(0,1); }
	size_t se = str1.length();
	while (str1[se-1] == delim) { str1.erase(se-1,se); se = str1.length(); }
	
	// If str is of only delimiters, str1.length() will be 0, so it will do nothing
	if (str1.length() > 0)
	{
		// Leave a single delimiter (if delim is "," and str1 is "a,,,b,,,,,c", then it will be "a,b,c")
		for (int i=0;i<str1.length()-1;i++)
		{
			if (str1[i] == delim && str1[i+1] == delim)
				continue;
			else
				rfs += str1[i];
		}
		rfs += str1[str1.length()-1];
		
		// Split
		for (int i=0;i<rfs.length()+1;i++)
		{
			if (rfs[i] != delim && i != rfs.length())
				ss << rfs[i];
			else
			{
				splitted.push_back(ss.str());
				ss.str(std::string());
			}
		}
	}
	return splitted;
}

bool is_vowel(char x)     { return vow_l.find(x) != std::string::npos; }
bool is_consonant(char x) { return con_l.find(x) != std::string::npos; }

uint32_t string_find(const std::string *arr,std::string x,std::size_t size)
{
	std::size_t idx = -1;
	for (std::size_t i=0;i<size;i++)
		if (x == arr[i])
			return i;
}

void encode(const uint8_t *data,std::size_t length,std::string& out)
{
	std::size_t i = 0, j = 0;
	uint8_t ca3[3];
	uint8_t ca4[4];
	
	if (data == NULL)
	{
		std::cout << "[bitspeak::encode] ERROR: Could not encode data.\n";
		exit(EXIT_FAILURE);
	}
	
	while (length--)
	{
		ca3[i++] = *(data++);
		if (i == 3)
		{
			// Bit shift
			ca4[0] = (ca3[0]&0xFE) >> 1;
			ca4[1] = ((ca3[0]&0x01) << 4) | ((ca3[1]&0xF0) >> 4);
			ca4[2] = ((ca3[1]&0x0F) << 3) | ((ca3[2]&0xE0) >> 5);
			ca4[3] = ca3[2]&0x1F;
			
			// Put consonants and vowels (form is CVCV for each 3 bytes of data)
			for (i=0;i<4;i++)
			{
				if (i == 0) out += cons[ca4[0]];
				if (i == 1) out += vows[ca4[1]];
				if (i == 2) out += cons[ca4[2]];
				if (i == 3) out += vows[ca4[3]];
			}
			i = 0;
		}
	}
	
	if (i)
	{
		for (j=i;j<3;j++)
			ca3[j] = '\0';
		
		// Bit shift
		ca4[0] = (ca3[0]&0xFE) >> 1;
		ca4[1] = ((ca3[0]&0x01) << 4) | ((ca3[1]&0xF0) >> 4);
		ca4[2] = ((ca3[1]&0x0F) << 3) | ((ca3[2]&0xE0) >> 5);
		ca4[3] = ca3[2]&0x1F;
		
		for (j=0;j<(i+1);j++)
		{
			if (j == 0) out += cons[ca4[0]];
			if (j == 1) out += vows[ca4[1]];
			if (j == 2) out += cons[ca4[2]];
			if (j == 3) out += vows[ca4[3]];
		}
		
		// Put padding
		while ((i++ < 3))
			out += "?";
	}
}

void decode(const char *data,std::size_t length,std::string& out)
{
	std::size_t i = 0, j = 0, si = 0;
	uint8_t ca3[3];
	uint8_t ca4[4];
	std::string sa4[4];
	std::size_t slen = length;
	std::vector<std::string> vow_v; // Vowels
	std::vector<std::string> con_v; // Consonants
	std::vector<std::string> all_v; // All data to decode
	std::string vow_tmp, con_tmp, pad_tmp; // Placeholders for consonants, vowels and padding
	std::size_t padc = 0;
	
	if (data == NULL)
	{
		std::cout << "[bitspeak::decode] Could not encode data.\n";
		exit(EXIT_FAILURE);
	}
	
	while ((i < slen))
	{
		if (is_consonant(data[i]))
		{
			con_tmp += data[i];
			vow_tmp += '*'; pad_tmp += '*';
		}
		if (is_vowel(data[i]))
		{
			vow_tmp += data[i];
			con_tmp += '*'; pad_tmp += '*';
		}
		if (data[i] == '?')
		{
			pad_tmp += data[i];
			con_tmp += '*'; vow_tmp += '*';
			padc++;
		}
		i++;
	}
	
	// Split
	vow_v = split(vow_tmp.c_str(),'*');
	con_v = split(con_tmp.c_str(),'*');
	
	// Put consonants and vowels (form is CVCV for each 3 bytes of data)
	std::size_t np = vow_v.size()+con_v.size();
	for (i=0;i<np;i++)
	{
		if (i%2 == 0) all_v.push_back(con_v[i/2]);
		if (i%2 == 1) all_v.push_back(vow_v[i/2]);
	}
	
	// Put padding
	for (i=0;i<padc;i++)
		all_v.push_back("?");
	
	std::size_t vlen = all_v.size();
	i = 0;
	
	while (vlen-- && (all_v[si] != "?"))
	{
		// Find which position is the one that corresponds to cons. and vow.
		if (si%2 == 0) ca4[i++] = (uint8_t)string_find(cons,con_v[si/2],128);
		if (si%2 == 1) ca4[i++] = (uint8_t)string_find(vows,vow_v[si/2], 32);
		si++;
		
		if (i == 4)
		{
			// Bit shift
			ca3[0] = ((ca4[0]&0x7F) << 1) | ((ca4[1]&0x10) >> 4);
			ca3[1] = ((ca4[1]&0x0F) << 4) | ((ca4[2]&0x78) >> 3);
			ca3[2] = ((ca4[2]&0x07) << 5) | (ca4[3]&0x1F);
			
			for (i=0;i<3;i++)
			{
				if (i == 0) out += ca3[0];
				if (i == 1) out += ca3[1];
				if (i == 2) out += ca3[2];
			}
			
			i = 0;
		}
	}
	
	if (i)
	{
		for (j=i;j<4;j++)
			ca4[j] = 0;
		
		// Bit shift
		ca3[0] = ((ca4[0]&0x7F) << 1) | ((ca4[1]&0x10) >> 4);
		ca3[1] = ((ca4[1]&0x0F) << 4) | ((ca4[2]&0x78) >> 3);
		ca3[2] = ((ca4[2]&0x07) << 5) | (ca4[3]&0x1F);
		
		for (j=0;j<(i-1);j++)
		{
			if (j == 0) out += ca3[0];
			if (j == 1) out += ca3[1];
			if (j == 2) out += ca3[2];
		}
	}
}

} // end of "bitspeak" namespace

#endif
