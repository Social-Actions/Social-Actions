'''
From this random set, extract the features used in the original experiment::

   ./features input.json

This will output a table with columns:

 0. Message ID.
 1. Sender ID
 2. Recipient ID (None if no recipient)
 3. # of URLS in the tweet.
 4. # of hashtags in the tweet.
 5. # of usernames mentioned in the tweet.
 6. # of users following the author of the tweet.
 7. # of users the author of the tweet follows.
 8. # of days the author has had an account.
 9. # of tweets created by the user.
 10. # of tweets the user has marked as "favorite".
 11. # of chars in message
 12. if the message is a retweet or not (0 if not, 1 if it is)
 13. # of seconds till now since the message was created
'''


import datetime
import sys
import urlparse
import twittercollect
import re

#Rob Escriva's regular expression to determine if a tweet is a retweet using the message text
RETWEET_REGEXS = [re.compile(r'rt(?::|\s)+@', re.IGNORECASE)
        ,re.compile(r'retweet(?:ing)?(?::|\s)+@', re.IGNORECASE)
        ,re.compile(r'\(via\s*@', re.IGNORECASE)
        ,re.compile(r'RT\s*\(via\s*@', re.IGNORECASE)
        ,re.compile(r'thx(?::|\s)+@', re.IGNORECASE)
        ,re.compile(r'ht(?::|\s)+@', re.IGNORECASE)
        ,re.compile(r'r(?::|\s)+@', re.IGNORECASE)
        ]


def is_retweet(text):
    return True in [r.search(text) is not None for r in RETWEET_REGEXS]
	
	
def feature_urls(text):
    urls = 0
    words = text.split()
    for word in words:
        try:
            if urlparse.urlparse(word).netloc is not '': urls += 1
        except Exception:
            pass
    return urls


def feature_hashes(text):
    words = text.split(' ')
    return len([word for word in words
                if word.startswith('#')])


def feature_mentions(text):
    words = text.split(' ')
    return len([word for word in words
                if word.startswith('@')])


def user_created_at(tweet):
    dt = datetime.datetime.strptime(tweet['user']['created_at'],
            '%a %b %d %H:%M:%S +0000 %Y')
    return (datetime.datetime.utcnow() - dt).days


def tweet_created_at(tweet):
    dt = datetime.datetime.strptime(tweet['created_at'],'%a %b %d %H:%M:%S +0000 %Y')
    return (datetime.datetime.utcnow() - dt).seconds+(datetime.datetime.utcnow() - dt).days*24*60*60


def print_features(tweet):
    if not type(tweet)==type({'a':5}):
        print >> sys.stderr, 'non-tweet'
        return
    if 'id' in tweet and 'user' in tweet and 'followers_count' in tweet['user']:
        id = tweet['id']
        sender = tweet['user']['id']
        recipient = 'None'
        if 'in_reply_to_user_id' in tweet and tweet['in_reply_to_user_id'] is not None:
		      recipient = tweet['in_reply_to_user_id']
        urls = feature_urls(tweet['text'])
        hashes = feature_hashes(tweet['text'])
        mentions = feature_mentions(tweet['text'])
        followers = tweet['user']['followers_count']
        following = tweet['user']['friends_count']
        created = user_created_at(tweet)
        status = tweet['user']['statuses_count']
        favourites = tweet['user']['favourites_count']
        message_length = len(tweet['text'])
	is_a_retweet = 0
        if is_retweet(tweet['text']):
			is_a_retweet = 1
	timestamp = tweet_created_at(tweet)

        print id, sender, recipient, urls, hashes, mentions, followers, following, created, \
                status, favourites, message_length, is_a_retweet, timestamp


def main(argv):
	for file in argv:
		twittercollect.parse(file,print_features)

if __name__ == '__main__':
    main(sys.argv[1:])
