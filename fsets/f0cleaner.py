import sys

def main(argv):
	abfeat = open(argv[0])
	x = int(argv[1])
	y = int(argv[2])
	for line in abfeat:
		columns = line.split()
		if int(columns[25])>=x and int(columns[35])>=x and int(columns[16])>=y:
			print line,
	abfeat.close()

if __name__ == '__main__':
	main(sys.argv[1:])