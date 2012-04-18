'''
Select a timeline of all messages for the status files provided.

   ./timeline [<statuses.json> ...]

The output will be in the format::

    <id> SP <from> SP <to> SP <unix timestamp>
'''


import datetime
import sys

import twittercollect


def is_at_message(tweet):
    return 'id' in tweet and 'created_at' in tweet and 'text' in tweet \
            and 'in_reply_to_user_id' in tweet \
            and tweet['in_reply_to_user_id'] is None


def main(argv, stdout=None, stderr=None):
    stdout = stdout or sys.stdout
    stderr = stderr or sys.stderr

    def tweet_printer(tweet):
        if is_at_message(tweet):
            id = tweet['id']
            fr = tweet['user']['id']
            dt = datetime.datetime.strptime(tweet['created_at'],
                '%a %b %d %H:%M:%S +0000 %Y').strftime('%s')
            print >> stdout, id, fr, dt

    for filename in argv:
        twittercollect.parse(filename, tweet_printer)


if __name__ == '__main__':
    main(sys.argv[1:])
