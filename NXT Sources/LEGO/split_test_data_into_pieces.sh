#!/bin/bash

FILE_NAME="DATA.mat";
NUM_TESTS=$((1000)); # FIXME: manually set the value on the right


cd STEP_FUNCTION; # Subdirectory containing the file FILE_NAME

# Allows to split the Unique File Containing all The Tests into several pieces:
csplit ${FILE_NAME} -b "%05d" /-----$/ {*}

counter=$((0));
for file in `ls .`
do
	# Do Operation ONLY for files created by csplit!
	if [[ -n `echo ${file} | grep "xx"` ]]; then
		# Filter to the tests done by the Brick (hard coded into BRO_spam_client.c through do_test.sh launch script)
		if [[ ${counter} -lt ${NUM_TESTS} ]]; then
			echo "Found Good Test: "${file};
			if [[ ${counter} -gt 0 ]]; then # Its contents must be cleaned from the line separating the tests
				num_lines=$((`wc -l ${file}|cut -d\  -f1`));
				tail ${file} -n $((${num_lines} - 1)) > test_$((${counter} + 1)).mat # FIXME: remember to personalize this name of file
			else # Doesn't hold for file n. 0
				mv ${file} test_$((${counter} + 1)).mat # FIXME: file name personalization
			fi;
		fi
		counter=$((counter + 1));
	fi;
done

rm xx*;
