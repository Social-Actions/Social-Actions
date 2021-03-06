'''combine undirected features for a pair of users
	
	usage: python combine_feat_ab_excl.py <base_file> <added_features_file>
	
	note: pairs without added features will be excluded from output
	pairs ab and ba are added the same features
'''

import sys

def main(argv):
    base_file = argv[0]
    added_features_file = argv[1]
    
    fpairs= {}
    for line in open(added_features_file):
        columns = line.split(' ')
        pair = columns[0]+','+columns[1]
        fspace = line[line.find(' ')+1:]
        feat = fspace[fspace.find(' '):]
        fpairs[pair] = feat
    
    for line in open(base_file):
        columns = line.split(' ')
        pair = columns[0]+','+columns[1]
        rpair = columns[1]+','+columns[0]
        if pair in fpairs:
            print line.strip() + fpairs[pair].rstrip()
        elif rpair in fpairs:
            print line.strip() + fpairs[rpair].rstrip()        

if __name__ == '__main__':
    main(sys.argv[1:])