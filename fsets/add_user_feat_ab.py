'''add user features to features for pair of users
	for an ab pair, adds a features then b features to the features
	
	usage: python ass_user_feat_ab.py <base_file> <user_feat_file>
	
	note: pairs missing user features will have zeros for those features
'''

import sys

def main(argv):
    user_feat_file = argv[1]
    base_file = argv[0]
    
    user_feat = {}
    for line in open(user_feat_file):
        user = line[:line.find(' ')]
        feat = line[line.find(' '):].rstrip()
        user_feat[user] = feat
    
    zero_feat = ' 0'*len(user_feat[user_feat.keys()[0]].split())
    
    for line in open(base_file):
        columns = line.split(' ')
        user1 = columns[0]
        user2 = columns[1]
        if user1 in user_feat:
            feat1 = user_feat[user1]
        else:
            feat1 = zero_feat
        if user2 in user_feat:
            feat2 = user_feat[user2]
        else:
            feat2 = zero_feat
        print line.strip()+feat1+feat2
        

if __name__ == '__main__':
    main(sys.argv[1:])