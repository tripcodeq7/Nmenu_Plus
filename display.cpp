#include <ncurses.h>

#include "display.hpp"

using namespace std;

void print_columns(vector<column> &col,int x, int selected_column){
    int max_rows=0;
    int col_size=col.size();
    //width for the each column added conditional for edge case of col_size=1
    int column_width=x/col_size-(col_size==1 ? 2 : 1);

    //Add offset for the other printed details y is adjusted for formatting thoughout the program
    int current_x,current_y;
    getyx(stdscr, current_y,current_x);

    for(int i=0;i<col_size;i++){
        int db_size=col[i].display_buffer.size();
        if(max_rows<db_size)
            max_rows=db_size;
    }
    //Draw top line
    addch(ACS_ULCORNER);
    for(int i=1;i<column_width*col_size;i++)
        if(i%column_width==0)
            addch(ACS_TTEE);
        else
            addch(ACS_HLINE);
    addch(ACS_URCORNER);

    int i=0;
    for(;i<col_size;i++){
        int db_size=col[i].display_buffer.size();
        for(int j=0;j<max_rows;j++){
            if(column_width>0)
                move(j+current_y+1,column_width*i);
            int k=0;
            addch(ACS_VLINE);
            if(j<db_size){
                int line_length=col[i].display_buffer[j].t.length();
                if(i==selected_column){
                    attron(COLOR_PAIR(1));
                }
                else
                    attron(COLOR_PAIR(3));
                if(j==col[i].selection)
                    attron(A_REVERSE);
                if(j==col[i].selection && i==selected_column)
                    attron(COLOR_PAIR(2));
                for(;k<column_width-1 && k<line_length;k++)
                    addch(col[i].display_buffer[j].t[k]);
                attroff(COLOR_PAIR(1));
                attroff(COLOR_PAIR(2));
                attroff(COLOR_PAIR(3));
                attroff(A_REVERSE);
            }
            for(;k<column_width-1;k++)
                addch(' ');
            //Add last vertical box line
            if(i==col_size-1)
                addch(ACS_VLINE);
        }
    }
   
    //Draw bottom line
    addch('\n');
    addch(ACS_LLCORNER);
    for(int i=1;i<column_width*col_size;i++)
        if(i%column_width==0)
            addch(ACS_BTEE);
        else
            addch(ACS_HLINE);
    addch(ACS_LRCORNER);
}

