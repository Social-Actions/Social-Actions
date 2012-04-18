'''
extract fields from json files

usage: python extract_from_json.py <fields_file> <input1.json> <input2.json> ...

fields file should have just one line specifying fields to be extracted
eg. 'id user:id text'
- only records that contain specified fields will be extracted
- if records are nested use a ':' to specify sub record field, eg. user:id
'''

import sys
import twittercollect as tc
fields = []

def fit_criterion(criterion,record):
	if not type(record) == type({'a':5}): return False
	col=criterion.find(':')
	if col!=-1:
		if not criterion[:col] in record: return False
		return fit_criterion(criterion[col+1:],record[criterion[:col]])
	eq=criterion.find('=')
	if eq==-1:
		if criterion in record: return True
	elif criterion[:eq] in record and record[criterion[:eq]]==criterion[eq+1:] : return True
	return False


def get_val(field,record):
	col=field.find(':')
	if col!=-1:
		return get_val(field[col+1:],record[field[:col]])
	eq=field.find('=')
	if eq!=-1:
		return repr(record[field[:eq]])
	return repr(record[field])
		
		
def tweet_printer(tweet):
	global fields
	for field in fields:
		if not fit_criterion(field,tweet): return
	str=''
	for field in fields:
		if str: str+=' '+get_val(field,tweet)
		else: str = get_val(field,tweet)
	print str
	return


def main(argv):
	fieldsFile = open(argv[0]).readlines()
	fieldsString = fieldsFile[0].split(' ')
	for word in fieldsString:
		if word.strip():
			fields.append(word.strip())
	for filename in argv:
		if filename.find('.json')!=-1:
			tc.parse(filename, tweet_printer)


if __name__ == '__main__':
    main(sys.argv[1:])
