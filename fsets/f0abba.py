'''
Adds BA pairs for every AB pair in the f0 set

usage: python f0abba.py <f0>

'''
import sys

def main(argv):
	f0 = open(argv[0])

	for line in f0:
		cols = line.strip().split()
		a = cols[0]
		b = cols[1]
		print line,
		print b, a, cols[2], cols[4], cols[3],
		for ii in range(5,12):
			print cols[ii],
		print cols[13], cols[12], cols[15], cols[14], cols[17], cols[16], cols[19], cols[18],
		print cols[21], cols[20], cols[22],
		for ii in range(33,43):
			print cols[ii],
		for ii in range(23,33):
			print cols[ii],
		for ii in range(92,141):
			print cols[ii],
		for ii in range(43,92):
			print cols[ii],
		print


if __name__ == '__main__':
	main(sys.argv[1:])