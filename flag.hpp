#ifndef FLAG_HPP
#define FLAG_HPP

#include <vector>
#include <string>

#include "columns.hpp"

std::vector<std::string> string_split(std::string inputStr, char splitter);
entry_line process_line(std::string line,int split_index,char split_char);

struct flag{
    int split_index=0;//navigates which keys to display
    char split_char=' ';//decides what char to split on
    std::string title=":~";//defines a char to store the prompt or title
    bool single_output=true;//defines a string for the output format
    bool p=false;//p is for pipe input
    bool preserve_pipe=false;//used for preserve_pipe it is a bit hacky
    bool direct_input=false;
    char dl='\n';//delimeter to seperate pipe
    flag(int argc, char* argv[],std::vector<column>& columns);
};

#endif
