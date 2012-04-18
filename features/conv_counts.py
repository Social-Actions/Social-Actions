'''
usage: python conv_counts.py <conversation_features>    

1. A
2. # of users A has conversations with


'''

import sys
	
	
def main(argv):
    filename = argv[0]
    counts = {}
    for line in open(filename):
        columns = line.split()
        a = columns[0]
        b = columns[1]
        if not a in counts: counts[a] = 0
        if not b in counts: counts[b] = 0
        counts[a] = counts[a]+1
        counts[b] = counts[b]+1
    for user in counts:
        print user, counts[user]

if __name__ == '__main__':
    main(sys.argv[1:])
