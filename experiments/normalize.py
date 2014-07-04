#!/usr/bin/python

from __future__ import division

structs={"todolist-0.2", "todolist-0.35", "skiplist", "redblack", "treap",  
         "scapegoat", "bst", "sortedarray"}

ns = range(25000, 2000001, 25000)
print 'Reading bst-find.dat'
data = [s.split() for s in open('bst-find.dat').read().splitlines()]
denominators = dict([ (x[2], x[3]) for x in data])
for s in structs:
    infile = '{}-find.dat'.format(s)
    outfile = '{}-find-norm.dat'.format(s)
    data = [s.split() for s in open(infile).read().splitlines()]
    print "Normalizing {} saving to {}".format(infile, outfile)
    of = open(outfile, 'w')
    for d in data:
        denom = denominators[d[2]]
        of.write("{} {}\n".format(d[2], float(d[3])/float(denom)))
        
structs.remove('bst')
structs.remove('sortedarray')
data = [s.split() for s in open('redblack-add.dat').read().splitlines()]
denominators = dict([ (x[2], x[3]) for x in data])
for s in structs:
    infile = '{}-add.dat'.format(s)
    outfile = '{}-add-norm.dat'.format(s)
    data = [s.split() for s in open(infile).read().splitlines()]
    print "Normalizing {} saving to {}".format(infile, outfile)
    of = open(outfile, 'w')
    for d in data:
        denom = denominators[d[2]]
        of.write("{} {}\n".format(d[2], float(d[3])/float(denom)))
 
