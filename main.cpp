#include <ncurses.h>
#include <stdio.h>
#include <string.h>

#include <vector>
#include <string>
#include <iostream>

#include "columns.hpp"
#include "display.hpp"
#include "flag.hpp"

using namespace std;

/*Prints the final output of the program*/
void print_output(vector<column> &col,int selected_column, flag f){
    endwin();//End curses mode
    if(f.direct_input){
	    cout << col[0].command << endl;
    }
    if(f.single_output)
        cout << col[selected_column].get_output();
    else
        for(size_t i=0+(size_t)f.direct_input;i<col.size();i++)
            cout << col[i].get_output() << endl;
    return;
}

/*Returns a string that is autocompleted to the nearest matching character of the array
 or a ""
*/
string get_tab_auto(vector<entry_line> &buffer, string &command){
   int command_length=command.length();
   string temp_command=command;
   if(command_length<=0)
       return "";
   //Filler value in case of infinite loop
   for(size_t string_pos=command_length;;string_pos++){
        char c='\0';
        for(size_t i=0;i<buffer.size();i++)
            if(buffer[i].t.rfind(command,0)!=string::npos && string_pos<buffer[i].t.length()){
                c=buffer[i].t[string_pos];
                break;
            }
        //Exits if the buffer doesn't have any matches
        if(c=='\0')
            return temp_command;

        for(size_t i=0;i<buffer.size();i++){
            //Added incase of null string
            if(buffer[i].t.rfind(command,0)!=string::npos && string_pos>=buffer[i].t.length())
                return temp_command;
            if(buffer[i].t.rfind(command,0)!=string::npos && buffer[i].t[string_pos]!=c)
                return temp_command;
        }
        temp_command.push_back(c);
    }
    return "";
}


int main(int argc, char* argv[]){
    //A char list of allowed characters for the command
    string allowed_characters = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

    //Stores the column objects for multiple rows
    vector<column> columns;
    //Handles the flags and setup for columns
    flag opt(argc, argv, columns);
    //Exits if any column has a size of 0 or no columns
    if(columns.size()==0){
	    cerr << "No valid columns" << endl;
	    exit(0);
    }
    for(size_t i=0;i<columns.size();i++)
	    if(columns[i].buffer.size()==0){
		    cerr << "Column " << i << " is empty";
            exit(1);
	    }
    //Setup a title string, flag.title is used for the preserve pipe option
    string title=opt.title;

    /* Start curses mode*/
    //Slight edit for partial unicode support
    //setlocale(LC_ALL, "");
    FILE *f = fopen("/dev/tty", "r+");
    SCREEN *screen = newterm(NULL, f, f);
    set_term(screen);
    noecho();
    start_color();
    keypad(stdscr, TRUE);
    curs_set(0);

    /* Define colors */
    //-1 is the terminal default instead of COLOR_BLACK
    use_default_colors();
    init_pair(2, COLOR_CYAN, -1); 
    init_pair(3, COLOR_GREEN, -1); 
    init_pair(1, COLOR_MAGENTA, -1);

    //Mouse input stuff
    mousemask(BUTTON1_CLICKED, NULL);
    MEVENT m;

    //Creates char for input
    int c='\0';

    //Creates smaller vectors that fits within the boundaries of the display and adjust
    //them for autocomplete and scroll
    int columns_size=columns.size();
    int selected_column=0;
    int old_mouse_x=-1;
    int old_mouse_y=-1;
    int x,y;
    for(int i=0;i<columns_size;i++)
        columns[i].reset_pos();
    do{
        clear();
        getmaxyx(stdscr,y,x);
        //Decreases the column height
        if(y>4 && x>columns_size)
            y=y-4;
        else if(y>0){
            napms(10);
            continue;
        }

        //Adjust select based off window resizing
        for(int i=0;i<columns_size;i++){
            if(columns[i].selection>=y || columns[i].scroll_pos+y>columns[i].total_count){
                columns[i].resize(y);
            }
        }


        //Handles keys
        switch(c){
            case 27:
                halfdelay(1);
                c=getch();
                if(c=='\t')
                    if(selected_column<columns_size-1)
                        selected_column++;
                    else
                        selected_column=0;
                else if(c==ERR){
                    endwin();
                    return 0;
                }
                cbreak();
                break;
            case KEY_MOUSE:
                if(getmouse(&m)==OK && x > columns_size){
                    int mouse_y=m.y-2;
                    selected_column=min((m.x*columns_size)/(x-columns_size),columns_size-1);
                    if(selected_column>=columns_size)
                        selected_column=columns_size-1;
                    if(mouse_y>=static_cast<int>(columns[selected_column].display_buffer.size()))
                        columns[selected_column].scroll_down(y);
                    else if(mouse_y<0)
                        columns[selected_column].scroll_up(y);
                    else{
                        //Double click returns a selection
                        if(old_mouse_x==selected_column && old_mouse_y==mouse_y){
                            print_output(columns,selected_column,opt);
                            return 0;
                        }
                        columns[selected_column].selection=mouse_y;
                        old_mouse_x=selected_column;
                        old_mouse_y=mouse_y;
                    }
                }
                break;
            case '\n':
                if(opt.single_output || selected_column==columns_size-1){
                    print_output(columns,selected_column,opt);
                    return 0;
                }
                else
                    selected_column++;
                break;
            case KEY_DOWN:
                columns[selected_column].move_down(y);
                break;
            case KEY_NPAGE:
                columns[selected_column].scroll_down(y);
                break;
            case KEY_UP:
                columns[selected_column].move_up();
                break;
            case KEY_PPAGE:
                columns[selected_column].scroll_up(y);
                break;
            case KEY_LEFT:
                if(columns[selected_column].cursor_position>0)
                    columns[selected_column].cursor_position--;
                break;
            case KEY_RIGHT:
                if(columns[selected_column].cursor_position<columns[selected_column].command.length())
                    columns[selected_column].cursor_position++;
                break;
            case KEY_BACKSPACE:
                if(columns[selected_column].cursor_position>0){
                    columns[selected_column].command.erase(columns[selected_column].cursor_position-1,1);
                    columns[selected_column].cursor_position--;
                    columns[selected_column].reset_pos();
                }
                break;
            case '\t'://Added this last condition
                if(columns[selected_column].command.length()>0 && columns[selected_column].display_buffer.size()!=0){
                    columns[selected_column].command=get_tab_auto(columns[selected_column].buffer,columns[selected_column].command);
                    columns[selected_column].cursor_position=columns[selected_column].command.length();
                }
                break;
            default:
		    //Odd hack for preserve_pipe
                if(allowed_characters.find(c)!=string::npos && (!opt.preserve_pipe || selected_column==0)){
                    columns[selected_column].command.push_back(c);
                    columns[selected_column].cursor_position++;
                    columns[selected_column].reset_pos();
                }
                break;
        }


	//Hack to get preserve_pipe working
	if(opt.preserve_pipe){
		if(selected_column==0)
			title=opt.title;
		else if(columns[0].command.length())
			title=columns[0].command;
	}

        //Update display buffer
        for(int i=0;i<columns_size;i++)
            columns[i].populate_display_buffer(y);

        attron(COLOR_PAIR(2));
        printw("%s",title.c_str());
        for(int i=0;i<columns[selected_column].command.length() && i<x-1-title.length();i++)
            if(i==columns[selected_column].cursor_position){
                attron(A_REVERSE);
                addch(columns[selected_column].command[i]);
                attroff(A_REVERSE);
            }
            else
                addch(columns[selected_column].command[i]);
        attroff(COLOR_PAIR(2));
        addch('\n');
        print_columns(columns,x,selected_column);
        refresh();                       
        }while(c=getch());
        endwin();//End curses mode
        return 0;
}
