'''
Extract all the followers of each entity::

   ./findFollowers input.json

This will output the sender ID and the number of users following the sender.
'''

import datetime
import sys
import twittercollect

def feature_created_at(tweet):
    dt = datetime.datetime.strptime(tweet['user']['created_at'],
            '%a %b %d %H:%M:%S +0000 %Y')
    return (datetime.datetime.now() - dt).days

users = {}

def print_features(tweet):
    global users
    if not type(tweet)==type({'a':5}):
        print >> sys.stderr, 'non-tweet'
        return
    if 'id' in tweet and 'user' in tweet and 'followers_count' in tweet['user']:
        sender = tweet['user']['id']
        created = feature_created_at(tweet)
        followers = tweet['user']['followers_count']
        if sender in users:
	  if users[sender][0] <= created:
	    users[sender][0] = created
	    users[sender][1] = followers
	else:
	  users[sender]=[created,followers]

def main(argv):
	out = sys.stdout
	global users
	for file in argv: twittercollect.parse(file,print_features)
	for k in users.keys():
		sender = k
		followers = users[k][1]
		print >> out, sender, followers

if __name__ == '__main__':
    main(sys.argv[1:])
