data = $(wildcard *.dat)

all : bigtest-find.tex epsilon.tex 

data : 
	rm -f *.dat
	(cd ../cpp ; make) 
	cp ../cpp/main .
	./test-epsilons.sh
	./test-all.sh
	./test-static.sh
	./test-tradeoff.sh
	python ./normalize.py

bigtest-find.tex : bigtest.gp $(data)
	gnuplot bigtest.gp

epsilon.tex : epsilon.gp $(data)
	gnuplot epsilon.gp

