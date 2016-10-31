#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include "bitspeak.hpp"

int main(int argc,char **argv)
{
	std::string tests[12] = {
		"Bitspeak test",
		"Technological progress is like an axe in the hands of a pathological criminal.",
		"0",
		"00",
		"000",
		"123456789abcdef",
		"X          X         X        X       X      X     X    X   X  X XX",
		"XX X  X   X    X     X      X       X        X         X          X",
		" ((( d(O_o)b ))) ",
		"\"double quotes\"/\'single quotes\'",
		"((2+1)*3-5)*2 = 8",
		"\n"
		"...d8b...\n"
		"..dd8bb..\n"
		".dd8.8bb.\n"
		"888.X.888\n"
		".qq8.8pp.\n"
		"..qq8pp..\n"
		"...q8p...\n"

	};
	int cnt_errors = 0;
	
	std::cout << ".....--------============= Bitspeak test =============--------.....\n";
	
	for (int i=0;i<12;i++)
	{
		std::string encoded, decoded;
		bool is_equal;
		
		std::cout << "Test string " << (i+1) << ": " << tests[i] << "\n";
		
		bitspeak::encode(
			(uint8_t*)tests[i].c_str(),
			tests[i].length(),
			encoded
		);
		std::cout << "Encoded:       " << encoded << "\n";
		
		bitspeak::decode(
			encoded.c_str(),
			encoded.length(),
			decoded
		);
		std::cout << "Decoded:       " << decoded << "\n";
		
		std::cout << "Are original and decoded strings equal? " << ((tests[i] == decoded)?"Yes.":"No.") << "\n\n";
		cnt_errors += (tests[i] != decoded)? 1 : 0;
	}
	
	std::cout << "Count of errors: " << cnt_errors << "\n";
}

