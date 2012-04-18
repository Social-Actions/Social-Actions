'''combine directed features for a pair of users
	
	usage: python combine_feat_ab-ba.py <base_file> <added_features_file>
	
	note: pairs without added features will be excluded from output
	adds ab and ba features to ab pairs
	'''

import sys

def main(argv):
    base_file = argv[0]
    added_features_file = argv[1]
    
    pairs = []
    for line in open(base_file):
        columns = line.split(' ')
        pair = columns[0]+','+columns[1]
        pairs.append(pair)
    
    pairset = set(pairs)
    fpairs= {}
    for line in open(added_features_file):
        columns = line.split(' ')
        pair = columns[0]+','+columns[1]
        rpair = columns[1]+','+columns[0]
        if pair in pairset or rpair in pairset:
            fspace = line[line.find(' ')+1:]
            feat = fspace[fspace.find(' '):]
            fpairs[pair] = feat.rstrip()
    
    for line in open(base_file):
        columns = line.split(' ')
        pair = columns[0]+','+columns[1]
        rpair = columns[1]+','+columns[0]
        if pair in fpairs and rpair in fpairs:
            print line.strip() + fpairs[pair] + fpairs[rpair]
        

if __name__ == '__main__':
    main(sys.argv[1:])