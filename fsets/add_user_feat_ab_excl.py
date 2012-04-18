'''add user features to features for pair of users
	for an ab pair, adds a features then b features to the features
	
	usage: python ass_user_feat_ab_excl.py <base_file> <user_feat_file>

	note: pairs that are missing user features will be excluded from output
'''

import sys

def main(argv):
    base_file = argv[0]
    user_feat_file = argv[1]
    
    user_feat = {}
    for line in open(user_feat_file):
        user = line[:line.find(' ')]
        feat = line[line.find(' '):].rstrip()
        user_feat[user] = feat
    
    for line in open(base_file):
        columns = line.split(' ')
        user1 = columns[0]
        user2 = columns[1]
        if user1 in user_feat and user2 in user_feat:
            print line.strip()+user_feat[user1]+user_feat[user2]
        

if __name__ == '__main__':
    main(sys.argv[1:])