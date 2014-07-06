data = $(wildcard *.dat)

all : bigtest-find.tex epsilon.tex 

data : 
	rm -f *.dat
	(cd ../cpp ; g++ -O4 -o ../experiments/main main.cpp)
	./test-epsilons.sh
	./test-all.sh
	./test-static.sh
	./test-tradeoff.sh
	python ./normalize.py

bigtest-find.tex : bigtest.gp $(data)
	gnuplot bigtest.gp

epsilon.tex : epsilon.gp $(data)
	gnuplot epsilon.gp

