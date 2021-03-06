#!/usr/bin/gnuplot -persist
#
#    
#    	G N U P L O T
#    	Version 4.6 patchlevel 4    last modified 2013-10-02 
#    	Build System: Linux x86_64
#    
#    	Copyright (C) 1986-1993, 1998, 2004, 2007-2013
#    	Thomas Williams, Colin Kelley and many others
#    
#    	gnuplot home:     http://www.gnuplot.info
#    	faq, bugs, etc:   type "help FAQ"
#    	immediate help:   type "help"  (plot window: hit 'h')
set term tikz color size 5in,3in
set output 'bigtest.tex'
unset clip points
set clip one
unset clip two
set bar 1.000000 front
set border 31 front linetype -1 linewidth 1.000
set timefmt z "%d/%m/%y,%H:%M"
set zdata 
set timefmt y "%d/%m/%y,%H:%M"
set ydata 
set timefmt x "%d/%m/%y,%H:%M"
set xdata 
set timefmt cb "%d/%m/%y,%H:%M"
set timefmt y2 "%d/%m/%y,%H:%M"
set y2data 
set timefmt x2 "%d/%m/%y,%H:%M"
set x2data 
set boxwidth
set style fill  empty border
set style rectangle back fc lt -3 fillstyle   solid 1.00 border lt -1
set style circle radius graph 0.02, first 0, 0 
set style ellipse size graph 0.05, 0.03, first 0 angle 0 units xy
set dummy x,y
set format x "%.0f"
set format y "% g"
set format x2 "% g"
set format y2 "% g"
set format z "% g"
set format cb "% g"
set format r "% g"
set angles radians
unset grid
set raxis
set key title ""
#set key lmargin top vertical Right noreverse enhanced autotitles nobox
set key top left
set key noinvert samplen 4 spacing 2 width 0 height 0 
set key maxcolumns 0 maxrows 0
set key noopaque
unset label
unset arrow
set style increment default
unset style line
set style line 2  linetype 2 linewidth 1.000 pointtype 2 pointsize default pointinterval 0
unset style arrow
set style histogram clustered gap 2 title  offset character 0, 0, 0
unset logscale
set offsets 0, 0, 0, 0
set pointsize 1
set pointintervalbox 1
set encoding default
unset polar
unset parametric
unset decimalsign
set view 60, 30, 1, 1
set samples 100, 100
set isosamples 10, 10
set surface
unset contour
set clabel '%8.3g'
set mapping cartesian
set datafile separator whitespace
unset hidden3d
set cntrparam order 4
set cntrparam linear
set cntrparam levels auto 5
set cntrparam points 5
set size ratio 0 1,1
set origin 0,0
set style data points
set style function lines
set xzeroaxis linetype -2 linewidth 1.000
set yzeroaxis linetype -2 linewidth 1.000
set zzeroaxis linetype -2 linewidth 1.000
set x2zeroaxis linetype -2 linewidth 1.000
set y2zeroaxis linetype -2 linewidth 1.000
set ticslevel 0.5
set mxtics default
set mytics default
set mztics default
set mx2tics default
set my2tics default
set mcbtics default
set xtics border in scale 1,0.5 mirror norotate  offset character 0, 0, 0 autojustify
set xtics 500000 norangelimit
set ytics border in scale 1,0.5 mirror norotate  offset character 0, 0, 0 autojustify
set ytics autofreq  norangelimit
set ztics border in scale 1,0.5 nomirror norotate  offset character 0, 0, 0 autojustify
set ztics autofreq  norangelimit
set nox2tics
set noy2tics
set cbtics border in scale 1,0.5 mirror norotate  offset character 0, 0, 0 autojustify
set cbtics autofreq  norangelimit
set rtics axis in scale 1,0.5 nomirror norotate  offset character 0, 0, 0 autojustify
set rtics autofreq  norangelimit
set title "" 
set title  offset character 0, 0, 0 font "" norotate
set timestamp bottom 
set timestamp "" 
set timestamp  offset character 0, 0, 0 font "" norotate
set rrange [ * : * ] noreverse nowriteback
set trange [ * : * ] noreverse nowriteback
set urange [ * : * ] noreverse nowriteback
set vrange [ * : * ] noreverse nowriteback
set xlabel "$n$ (number of insertions)" 
set xlabel  offset character 0, 0, 0 font "" textcolor lt -1 norotate
set x2label "" 
set x2label  offset character 0, 0, 0 font "" textcolor lt -1 norotate
set xrange [ * : * ] noreverse nowriteback
set x2range [ * : * ] noreverse nowriteback
set ylabel  offset character 0, 0, 0 font "" textcolor lt -1 rotate by -270
set y2label "" 
set y2label  offset character 0, 0, 0 font "" textcolor lt -1 rotate by -270
set yrange [ * : * ] noreverse nowriteback
set y2range [ * : * ] noreverse nowriteback
set zlabel "" 
set zlabel  offset character 0, 0, 0 font "" textcolor lt -1 norotate
set zrange [ * : * ] noreverse nowriteback
set cblabel "" 
set cblabel  offset character 0, 0, 0 font "" textcolor lt -1 rotate by -270
set cbrange [ * : * ] noreverse nowriteback
set zero 1e-08
set lmargin  -1
set bmargin  -1
set rmargin  -1
set tmargin  -1
set locale "en_CA.UTF-8"
set pm3d explicit at s
set pm3d scansautomatic
set pm3d interpolate 1,1 flush begin noftriangles nohidden3d corners2color mean
set palette positive nops_allcF maxcolors 0 gamma 1.5 color model RGB 
set palette rgbformulae 7, 5, 15
set colorbox default
set colorbox vertical origin screen 0.9, 0.2, 0 size screen 0.05, 0.6, 0 front bdefault
set style boxplot candles range  1.50 outliers pt 7 separation 1 labels auto unsorted
set loadpath 
set fontpath 
set psdir
set fit noerrorvariables
GNUTERM = "wxt"
set ylabel "time for $5n$ searches (s)" 
set key at 80000,9.5
set output 'bigtest-find.tex'
plot 'skiplist-find.dat' using 3:4 title 'skiplist' with lines lt 1 lw 2 lc rgb 'red', 'treap-find.dat' using 3:4 title 'treap' with lines lt 1 lw 2 lc rgb 'brown', 'scapegoat-find.dat' using 3:4 title 'scapegoat tree' with lines lt 1 lw 2 lc rgb 'blue', 'stlset-find.dat' using 3:4 title 'STL set' with lines lt 1 lw 2 lc rgb '#8A2BE2', 'redblack-find.dat' using 3:4 title 'red-black tree' with lines lt 1 lw 2 lc rgb 'sea-green', 'todolist-0.35-find.dat' using 3:4 title 'todolist ($\varepsilon=0.35$)' with lines lt 1 lw 2 lc rgb 'orange', 'todolist-0.2-find.dat' using 3:4 title 'todolist ($\varepsilon=0.2$)' with lines lt 1 lw 2 lc rgb '#B8860B', 'bst-find.dat' using 3:4 title 'optimal search tree' with lines lt 1 lw 2 lc rgb 'black', 'sortedarray-find.dat' using 3:4 title 'sorted array' with lines lt 1 lw 2 lc rgb '#9400D3',

set ylabel "relative time for $5n$ searches" 
set key off
#set key at 20000,4
set output 'bigtest-find-norm.tex'
plot 'skiplist-find-norm.dat' title 'skiplist' with lines lt 1 lw 2 lc rgb 'red', 'treap-find-norm.dat' title 'treap' with lines lt 1 lw 2 lc rgb 'brown', 'scapegoat-find-norm.dat' title 'scapegoat tree' with lines lt 1 lw 2 lc rgb 'blue', 'stlset-find-norm.dat' title 'STL set' with lines lt 1 lw 2 lc rgb '#8A2BE2', 'redblack-find-norm.dat' title 'red-black tree' with lines lt 1 lw 2 lc rgb 'sea-green', 'todolist-0.35-find-norm.dat' title 'todolist ($\varepsilon=0.35$)' with lines lt 1 lw 2 lc rgb 'orange', 'todolist-0.2-find-norm.dat' title 'todolist ($\varepsilon=0.2$)' with lines lt 1 lw 2 lc rgb '#B8860B', 'bst-find-norm.dat' title 'optimal search tree' with lines lt 1 lw 2 lc rgb 'black', 'sortedarray-find-norm.dat' title 'sorted array' with lines lt 1 lw 2 lc rgb '#9400D3',

set ylabel "time for $n$ insertions (s)" 
set key on
set key at 200000,5.5
set output 'bigtest-add.tex'
plot 'todolist-0.2-add.dat' using 3:4 title 'todolist ($\varepsilon=0.2$)' with lines lt 1 lw 2 lc rgb '#B8860B', 'todolist-0.35-add.dat' using 3:4 title 'todolist ($\varepsilon=0.35$)' with lines lt 1 lw 2 lc rgb 'orange', 'skiplist-add.dat' using 3:4 title 'skiplist' with lines lt 1 lw 2 lc rgb 'red', 'treap-add.dat' using 3:4 title 'treap' with lines lt 1 lw 2 lc rgb 'brown', 'scapegoat-add.dat' using 3:4 title 'scapegoat tree' with lines lt 1 lw 2 lc rgb 'blue', 'stlset-add.dat' using 3:4 title 'STL set' with lines lt 1 lw 2 lc rgb '#8A2BE2', 'redblack-add.dat' using 3:4 title 'red-black tree' with lines lt 1 lw 2 lc rgb 'sea-green', 

set ylabel "relative time for $n$ insertions" 
set key off
set output 'bigtest-add-norm.tex'
plot 'todolist-0.2-add-norm.dat' title 'todolist ($\varepsilon=0.2$)' with lines lt 1 lw 2 lc rgb '#B8860B', 'todolist-0.35-add-norm.dat' title 'todolist ($\varepsilon=0.35$)' with lines lt 1 lw 2 lc rgb 'orange', 'skiplist-add-norm.dat' title 'skiplist' with lines lt 1 lw 2 lc rgb 'red', 'treap-add-norm.dat' title 'treap' with lines lt 1 lw 2 lc rgb 'brown', 'scapegoat-add-norm.dat' title 'scapegoat tree' with lines lt 1 lw 2 lc rgb 'blue', 'stlset-add-norm.dat' title 'STL set' with lines lt 1 lw 2 lc rgb '#8A2BE2', 'redblack-add-norm.dat' title 'red-black tree' with lines lt 1 lw 2 lc rgb 'sea-green', 
 
 #    EOF
