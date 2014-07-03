data = $(wildcard *.dat)

all : bigtest-find.tex epsilon.tex 

data : 
	rm -f *.dat
	./test-epsilons.sh
	./test-all.sh
	./test-static.sh
	./test-tradeoff.sh

bigtest-find.tex : bigtest.gp $(data)
	gnuplot bigtest.gp

epsilon.tex : epsilon.gp $(data)
	gnuplot epsilon.gp

