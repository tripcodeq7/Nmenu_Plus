#include "unicode_to_ascii.hpp"

#include <utf8proc.h>

extern "C" {
    #include "anyascii.h"
}

using namespace std;

string ascii_only_filter(const string &str){
    // Finally filter to strict ASCII.
    string out="";
    bool found_space=false;
    unsigned char c;
    for(size_t i=0;i<str.size();i++){
    c=static_cast<unsigned char>(str[i]);
    //Handles spaces condences it into one
    if((c==' ' || c=='\t' || c=='\n' || c=='\r' || c=='\f' || c=='\v') && !found_space){
	    out+=' ';
	    found_space=true;
	}
	//Askii characters
	else if(c>=33 && c<=126){
	    out.push_back((unsigned char)c);
	    found_space=false;
	}
    }
    //Remove starting and ending space tab or enter
    if(!out.empty()){
        if(out.back()==' ')
	    out.pop_back();
        if(out.front()==' ')
	    out.erase(0,1);
    }
    return out;
}

string utf8_to_ascii(const string &str){
	string out;
	const utf8proc_uint8_t* data=reinterpret_cast<const utf8proc_uint8_t*>(str.data());
	utf8proc_ssize_t length=static_cast<utf8proc_ssize_t>(str.size());
	utf8proc_int32_t rune;
	//Iterate through the utf8 data
	while(length>0){
		//Iterates through are unicode data
		utf8proc_ssize_t step=utf8proc_iterate(data, length, &rune);
		//Skip character with invalid UTF-8 sequence
		if(step<0){
			++data;//Advance to another position in data
			continue;
		}
		const char* askii_char=nullptr;
		/*anyascii returns the length of a cstring
		 *takes in a unicode character
		 *takes in a char pointer to store the data
		 *anyaskii potentially returns more then one char
		 *it is apparently addressed to readonly memory so no segfault*/
		size_t askii_length = anyascii(static_cast<uint_least32_t>(rune),&askii_char);

		//Add c string from anyascii
		if(askii_length > 0)
			out.append(askii_char,askii_char+askii_length);
		//Advance data pointer
		data+=step;
		//Keep track of the end of the unicode data
		length-=step;
	}
	return ascii_only_filter(out);
}
