This is a ncurses menu program it is controlled by alt tab to switch columns and scales to window
size. Page up and Page down work as expected. Tab is auto complete. I wrote this read me as quick
as possible. Later it will be updated.<br>
Also mouse control double click selects enter completes.
<br>
<br>
Example:<br>
seq 1 2000 | ./nmenu -p -c a b c -c 1 2 3<br>

-t: changes the title<br>
-p: enables pipe input<br>
-preserve: preserves the pipe column while tabbing around<br>
-direct_input: allows the user to enter incomplete strings<br>
-no_translit: disables anyaskii transliteration<br>
-0: Changes deliminator to null terminator<br>
-dl: Sets a custom deliminator<br>
-m: enables multicolumn output I think its seperated by enters<br>
-split: seperates columns based on a deliminator and displays the index based on the number<br>
-c: is used to seperate columns 1 2 3<br>
./nmenu -c 1 2 3<br>
1<br>
2<br>
3<br>
./nmenu -c 1 2 3 -c a b c<br>
1 | a<br>
2 | b<br>
3 | c<br>
-c1 I think this is to disable the formatting<br>

![nmenu_plus](nmenu_plus.png)
