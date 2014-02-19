#!/bin/bash

BASE_DIR=`pwd`;
SETTINGS_FILE="/home/ubuntu/NXT/brofist/my_settings.h"
OIL_FILE="/home/ubuntu/NXT/brofist/spam/BRO_spam_client.oil"

echo -e "Welcome!\n"

echo -e "NOTES:\n"
echo -e "- Sometimes the bluetooth connection may hang or be lost during the test, especially in noisy WIFI environments (like UniTN): do it at home!\n"
echo -e "- If you connect the brick but it's not detected for several times, reboot the virtual machine.\n"

echo -e "\nBefore starting, you'll be asked to edit the preference file to select\nyour own experiment configuration.\n(press Enter)\n";
read;
nano ${SETTINGS_FILE}
echo -e "\nIf you want to change the sampling interval, then modify the CYCLETIME field\nfor the BRO_Comm_alarm task. You can set [2, 5, 10] ms, 5 is reasonable.\n(press Enter)\n";
read;
nano ${OIL_FILE}
echo -e "The code will now be recompiled\n(press Enter)\n";
read;

# Recompile!
cd /home/ubuntu/NXT/brofist/spam ;
make clean; make all;

cd /home/ubuntu/NXT/brofist/src ;
make clean; make all;

cd ${BASE_DIR} ;

if [[ ! -d /home/ubuntu/TESTS ]]; then
	echo -e "\n# The test outputs will be saved in </home/ubuntu/TESTS> directory.\n"
	mkdir /home/ubuntu/TESTS ;
fi

echo -e "\n# If the code has been compiled with new settings, \nremember to delete the BRO_client code on the brick\n(press Enter when you're ready)\n"
read;

echo -e "\n# Type the name of the file in which data will be saved: "
read file_name;
echo -e "\n Data will be saved in: /home/ubuntu/TESTS/"${file_name}".mat\n"

for i in {1..1} ;
do
	echo -e "\n# Ready to start the server, connect the brick (turn it on)\n(press Enter)\n";
	read;
	echo -e "\n# After you will shut down the brick, type CTRL+C to terminate the server.\n"
	start_bt_server 2> /home/ubuntu/TESTS/${file_name}.mat
	echo -e "\n> Remember to wait 5-10s before turning on again the brick, \notherwise the bluetooth connection may fail to collect data next time.\n"
done

# Update the settings values accordingly
#sed -i 's/TARGET_POWER .*$/TARGET_POWER '${target_power}'/' ${SETTINGS_FILE}
#sed -i 's/INPUT_TYPE .*$/INPUT_TYPE '${type_fun}'/' ${SETTINGS_FILE}
#sed -i 's/NUM_TESTS .*$/NUM_TESTS '${num_tests}'/' ${SETTINGS_FILE}
#sed -i 's/LENGTH_WORKING_CYCLE .*$/LENGTH_WORKING_CYCLE '${length_working_cycle}'/' ${SETTINGS_FILE}
#sed -i 's/LENGTH_EMPTY_CYCLE .*$/LENGTH_EMPTY_CYCLE '${length_empty_cycle}'/' ${SETTINGS_FILE}
#sed -i 's/MILLIS_UPDATE .*$/MILLIS_UPDATE '${millis_update}'/' ${SETTINGS_FILE}
#sed -i 's/BUFFER_SIZE .*$/BUFFER_SIZE '${buffer_size}'/' ${SETTINGS_FILE}
