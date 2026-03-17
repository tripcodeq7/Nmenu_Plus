#include "flag.hpp"

#include <string.h>
#include <iostream>

using namespace std;

std::vector<std::string> string_split(std::string inputStr, char splitter) {
        std::vector<std::string> strings;
        for(size_t i=0; i<inputStr.length(); i++){
                std::string temp = "";
                for(; i < inputStr.length() && inputStr[i] != splitter; i++)
                        temp += inputStr[i];
		//Treats things like -- as a single -
		if(temp.length()!=0){
                	strings.push_back(temp);
		}
        }
        return strings;
}

int char_to_int(const char* input){
    bool is_negative=false;
    int i=0;
    if(input[0]=='-'){
	    i++;
	    is_negative=true;
    }
    int out=0;
    for(;input[i]>='0' && input[i]<='9';i++){
	out*=10;
	out+=input[i]-'0';
    }
    if(is_negative)
	out*=-1;
    return out;
}

//Re-wrote to use an entry_line struct
entry_line process_line(string line,int split_index,char split_char){
    	    //This section handles -1 and -2 which means that it is a half
    	    //split aka field 1 and 2 only ckecks the deliminator once
	    //The default case is just a regular split
	    //Turn back into while loop
    	    string tmp="";
	    size_t pos=line.find(split_char); 
    	    switch (split_index){
	    	case 0:
			tmp=line;
	    		break;
    	    	case -1:
	    		tmp=line.substr(0,pos);
	    		break;
    	    	case -2:
			//Maybe add an error message for in an else statement
			if(pos!=string::npos)
	    			tmp=line.substr(pos+1);
	    		break;
	    	default:
			//Protection statement in case user enters a negative value
			//or another out of index problem
			//index starts at 1 because 0 is default -1 and -2 are special options
			vector<string> tmp_vector=string_split(line,split_char);
			if(split_index>0 && split_index-1<tmp_vector.size())
	    			tmp=tmp_vector[split_index-1];
			else{
				tmp="";
			}
    	    }
	    //Program will exit if the pipe has an empty line
	    if(line.empty()){
		cerr<<"Entry_line is empty!\n";
		exit(1);
	    }
	    //Program continues on formatting error
    	    if(tmp.empty()){
		cerr<<"Entry_line missing char for split: "<<line<<endl;
		return entry_line(line,line);
	    }
	    //This is the normal condition for split
	    else
		return entry_line(tmp,line); 
}



flag::flag(int argc, char* argv[],vector<column>& columns){
    //Set translit to true for entry line objects as a default
    //entry_line::translit=true;
    //exits if there are no parameters          
    if(argc==0 || argc==1)
        exit(1);
    for(int i=1;i<argc;i++){
        //title option enabled
        if(strcmp(argv[i],"-t")==0){
		if(i+1 < argc)
			title=argv[++i];
		else{
			cerr << "-t requires a title\n";
			exit(1);
		}
        }
	//pipe option enabled
	else if(strcmp(argv[i],"-p")==0){
        	p=true;
	}
	//preserve pipe column
	else if(strcmp(argv[i],"-preserve")==0){
        	p=true;
		preserve_pipe=true;
	}
	//Allows you to enter incomplete strings
	else if(strcmp(argv[i],"-direct_input")==0){
		direct_input=true;
	}
	//disable askii to unicode
	else if(strcmp(argv[i],"-no_translit")==0){
		entry_line::translit=true;
	}
	//change deliminator to string terminator
	else if(strcmp(argv[i],"-0")==0){
        	dl='\0';
	}
        //change the deliminator
	else if(strcmp(argv[i],"-dl")==0){
		if(i+1 < argc)
			dl=argv[++i][0];
		else{
			cerr << "-dl needs a delimiter\n";
			exit(1);
		}
        }
	//multicolumn output
	else if(strcmp(argv[i],"-m")==0){
            single_output=false;
	}
	else if(strcmp(argv[i],"-split")==0){
		if(i+2 < argc)
			i++;
		else{
			cerr << "This argument takes 2 parameters\n";
			exit(0);
		}
		split_index=char_to_int(argv[i]);
		split_char=argv[++i][0];
		if(split_index==0){
			cerr << "This argument takes any number except 0\n";
			exit(0);
		}
	}
        //populate the rest of the buffers
	else if(strcmp(argv[i],"-c")==0 || strcmp(argv[i],"-c1")==0){
        	column temp_column=column();
		//set format column to true or false dependent on -c1 call
		bool format=false;
		if(!strcmp(argv[i],"-c1"))
			format=true;
		//Push back the column
		for(;i<argc-1 && strcmp(argv[i+1],"-c") && strcmp(argv[i+1],"-c1");i++){
			if(format)
				temp_column.buffer.push_back(process_line(argv[i+1],split_index,split_char));
			else
				temp_column.buffer.push_back(entry_line(argv[i+1],argv[i+1]));
		}
		if(temp_column.buffer.size()==0){
			cerr << "column is empty please put perameters after -c\n";
			exit (1);
		}

        	//make sure the column isn't empty
        	if(temp_column.buffer.size()!=0)
        		columns.push_back(temp_column);
        }
	else{
		cerr << argv[i] << " isn't a valid parameter.\n";
		exit(1);
	}
    }

    //populate buffer_a with arguments
    string line;
    if(p){
    	column temp_column=column();
        //This section if it gets more complex with regex should be done
        //with function pointers
        while(getline(cin,line,dl)){
		temp_column.buffer.push_back(process_line(line,split_index,split_char));
        }
        columns.emplace(columns.begin(), temp_column);
   }
}
