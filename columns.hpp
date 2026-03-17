#ifndef COLUMNS_HPP
#define COLUMNS_HPP

#include <vector>
#include <string>

struct entry_line{
    std::string t="";//Terminal buffer string formatted for display
    std::string o="";//output string
    entry_line(std::string term,std::string output);
    static bool translit;
};

struct column{
    bool is_pipe=false;
    int scroll_pos=0;
    int selection=0;
    std::vector<entry_line> buffer;//Used for output
    std::vector<entry_line> display_buffer;//The visable output in the terminal
    std::string command="";
    int output_string_pos;
    int total_count;
    int cursor_position=0;
    column();
    void move_up();
    void scroll_up(int y);
    void move_down(int y);
    void scroll_down(int y);
    void reset_pos();
    std::string get_output();
    void populate_display_buffer(int y);
    void resize(int y);
};

#endif
