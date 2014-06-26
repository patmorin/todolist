data = $(wildcard *.dat)

all : bigtest-find.tex epsilon.tex

bigtest-find.tex : bigtest.gp $(data)
	gnuplot bigtest.gp

epsilon.tex : epsilon.gp $(data)
	gnuplot epsilon.gp

