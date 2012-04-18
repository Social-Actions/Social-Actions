'''combine features for a user
	
	usage: python combine_feat_a.py <base_file> <added_features_file>
	
	note: users without added features will have zeros for missing features
'''

import sys

def main(argv):
    base_file = argv[0]
    added_features_file = argv[1]

    feat = {}
    for line in open(added_features_file):
        feat[line[:line.find(' ')]] = line[line.find(' '):].rstrip()
    
    zero_field = ' 0'*len(feat[feat.keys()[0]].split())
    
    for line in open(base_file):
        line = line.rstrip()
        user = line[:line.find(' ')]
        if user in feat: print line+feat[user]
        else: print line+zero_field

if __name__ == '__main__':
    main(sys.argv[1:])