#include"columns.hpp"

#include"unicode_to_ascii.hpp"

using namespace std;

bool entry_line::translit = true;
entry_line::entry_line(string term, string output){
	if(translit)
		t=utf8_to_ascii(term);
	else
		t=ascii_only_filter(term);
	o=output;
}

/*Counts how much stuff in the buffer matches the command*/
int get_total_count(const vector<entry_line>& buffer,const string command){
    int total_count=0;
    for(size_t i=0;i<buffer.size();i++)
        if(buffer[i].t.rfind(command,0)!=string::npos)
            total_count++;
    return total_count;
}

column::column(){
	total_count=get_total_count(buffer,"");
        output_string_pos=0;
}

void column::move_up(){
        if(selection>0)
            selection--;
        else if(scroll_pos>0)
            scroll_pos--;
    }

void column::scroll_up(int y){
        for(int i=0;i<y-1;i++)
            move_up();
    }

void column::move_down(int y){
        if(selection<y-1 && selection<total_count-1)
            selection++;
        else if(scroll_pos+y<total_count)
            scroll_pos++;
    }

void column::scroll_down(int y){
        for(int i=0;i<y-1;i++)
            move_down(y);
    }

void column::reset_pos(){
        selection=0;
        scroll_pos=0;
        total_count=get_total_count(buffer,command);
    }

string column::get_output(){
	if(display_buffer.empty())
		return command;
	else{
        	return display_buffer[selection].o;
	}
    }

void column::populate_display_buffer(int y){
        //This stores the string position for resizing
        output_string_pos=scroll_pos+selection;

        //Maybe replace with vector<string>.swap(display_buffer)
        display_buffer.clear();

        //Makes sure scroll doesn`t go past the display
        int length=buffer.size();
        int i=0;
        //Check if scroll position is less then screen length
        //Check if y is at its highest value
        //Check if the number of entrys is larger then y
        //Temp scroll is used to find the amount of lines to skip that are valid with the tab skip
        for(int temp_scroll=scroll_pos;i<length && temp_scroll>0;i++)
            if(buffer[i].t.rfind(command,0)!=string::npos){
                temp_scroll--;
        }
        for(;i<length && display_buffer.size()<y;i++)
            if(buffer[i].t.rfind(command,0)!=string::npos){
                display_buffer.push_back(buffer[i]);
                //Update the output string
                //output_string needs to be updated somewhere else it is probably scroll_pos+selection this works, but will need to be changed due to display_buffer not being updated.
                //output_string=display_buffer[selection];
            }
    }

void column::resize(int y){
        reset_pos();
        //Increments y when the window is resized,
        for(int i=0;output_string_pos!=scroll_pos+selection && i<total_count;i++){
            move_down(y);
        }
    }
