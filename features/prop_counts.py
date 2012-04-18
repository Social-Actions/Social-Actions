'''
usage: python prop_counts.py <propagation_features>    

1. A
2. # of users A propagates
3. # of users that propagate A

'''

import sys
	
	
def main(argv):
    filename = argv[0]
    counts = {}
    for line in open(filename):
        columns = line.split()
        a = columns[0]
        b = columns[1]
        if not a in counts: counts[a] = 0,0
        if not b in counts: counts[b] = 0,0
        a1, a2 = counts[a]
        b1, b2 = counts[b]
        counts[a] = a1, a2+1
        counts[b] = b1+1, b2
    for user in counts:
        x, y = counts[user]
        print user, x, y

if __name__ == '__main__':
    main(sys.argv[1:])
