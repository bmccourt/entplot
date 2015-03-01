entplot - a tool for generating entropy plots using gnuplot.

bmccourt, March 2015

Based heavily on the stackoverflow discussion at:
http://stackoverflow.com/questions/990477/how-to-calculate-the-entropy-of-a-file 

This tool simply spits out gnuplot data and plt files. Gnuplot is used
to actually create the image.

Build instructions - gcc entplot.c -o entplot -lm -Wall

usage: entplot <target file> <output.png>  ; this generates entplot.plt
       gnuplot entplot.plt                 ; this generates <output.png>

Feel free to use this code however you like. 

