reset
set encoding utf8

set terminal png size 1485,1050 enhanced font 'Verdana,20'
set output 'graph.png'

set title "График зависимости числа объектов от времени (числа проведённых итераций)"

set xlabel "Количество проведённых итераций" offset 1,0,0
set ylabel "Число объектов" 				 offset 1,0,0

#https://www2.uni-hamburg.de/Wiss/FB/15/Sustainability/schneider/gnuplot/colors.htm
#food
	set style line 10 lw 4.5 lc rgb "#FFD700" pt 6 ps 1.1 #gold
	set style line 11 lw 4.5 lc rgb "#FFD700" pt 6 ps 1.1 #goldenrod
#poison
	set style line 20 lw 4.5 lc rgb "#CD5C5C" pt 5 ps 1.2 #indianred
	set style line 21 lw 4.5 lc rgb "#8B0000" pt 5 ps 1.2 #darkred
#cell
	set style line 30 lw 4.5 lc rgb "#6A5ACD" pt 5 ps 1.2 #slateblue
	set style line 31 lw 4.5 lc rgb "#191970" pt 5 ps 1.2 #midnightblue


set termoption enhanced

#f1(x) = k1*x + z1
#fit f1(x) "data1.txt" using 1:4 via k1, z1
#set label sprintf("d{/=25 {/Symbol \163}}/dT = %.3f [{/=15 Н/(м{/Symbol \327}C^{\260}){/Symbol \327}10^{/=12 -5}}]", k1 * 100) at 41, 20 textcolor ls 1

set yrange [0:] 
#set xrange [10:70]


plot "statistics_log.txt" using 1:6 			  notitle 		     ls 21,\
 	 "statistics_log.txt" using 1:7 			  notitle 			 ls 31,\
	 "statistics_log.txt" using 1:3  			  notitle    	     ls 11,\
	 "statistics_log.txt" using 1:6 smooth bezier title "Бактерии 1" ls 20,\
 	 "statistics_log.txt" using 1:7 smooth bezier title "Бактерии 2" ls 30,\
	 "statistics_log.txt" using 1:3 smooth bezier title "Пища" 	     ls 10
