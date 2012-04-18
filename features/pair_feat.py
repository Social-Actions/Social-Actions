'''
Generates features for pairs of users in timeline
Timeline needs to be sorted by time
	Sorting can be done by running "sort -kn1,1 timeline.txt" in the terminal
    
    usage: python pair_feat.py <directed_timeline> > pair_feat.txt
    
    output:
    0. user_A
    1. user_B
    2. statuses from A to B
    3. statuses from B to A
    4. statuses A received
    5. statuses B received
    6. response time A to B in seconds
    7. response time B to A in seconds
    8. entropy for messages between A and B
'''

import sys
import math

def ave(list):
    if len(list) == 0: return 0
    s = 0
    for num in list:
        s += num
    return 1.0*s/len(list)
	
        
def main(argv):
    
    timeline_file = argv[0]
    
    aTob = {}
    toB = {}
    aTobTime = {}
    aTobDur = {}
       
    n = 0
    
    for line in open(timeline_file):
        columns = line.split(' ')
        if len(columns)<4: continue
        a = int(columns[1])
        b = int(columns[2])
        time = int(columns[3])
        ab = (a,b)
        ba = (b,a)
        if not ab in aTob: aTob[ab] = 0
        if not b in toB: toB[b] = 0
        if not ab in aTobDur: aTobDur[ab] = []
        aTobTime[ab] = time
        aTob[ab] = aTob[ab]+1
        toB[b] = toB[b]+1
        dur = aTobDur[ab]
        if ba in aTobTime:
            dur.append(time-aTobTime[ba])
            del(aTobTime[ba])
        
    print >> sys.stderr, "Parsed timeline"
        
    for ab in aTob:
        a = ab[0]
        b = ab[1]
        ba = (b,a)
        aTobs = 0
        bToas = 0
        tas = 0
        bs = 0
        if ab in aTob: aTobs = aTob[ab]
        if ba in aTob: bToas = aTob[ba]
        if a in toB: tas = toB[a]
        if b in toB: bs = toB[b]
        if not ba in aTobDur: aTobDur[ba] = []
        if aTobs == 0 and bToas == 0: p = 0
        else: p = aTobs/(aTobs+bToas)
        entropy = 0
        if p > 0 and not p == 1:
            entropy = -(p*(math.log10(p)))-((1-p)*(math.log10(1-p)))
        print a,b,aTobs,bToas,tas,bs,ave(aTobDur[ab]),ave(aTobDur[ba]),entropy

if __name__ == '__main__':
    main(sys.argv[1:])
