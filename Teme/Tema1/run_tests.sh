#! /bin/bash

SRC=src
OUT=out
REFS=refs
SINGLE_TESTS=tests/single
MULTIPLE_TESTS=tests/multiple

SERVER=rpc_db_server
CLIENT=rpc_db_client

DOTS="................................................................................"


if [ ! -d $OUT ]; then
	mkdir $OUT
else
	rm -rf $OUT/*
fi

make -C $SRC clean
make -C $SRC

if [ $? != "0" ]; then
	echo "There were build errors. Stopping now:"
	exit 1
fi

echo -e "\nServer and client built successfully. Running single client tests:"
for tst in $SINGLE_TESTS/*; do
	test_name=$(basename $tst)

	./$SRC/$SERVER &> $OUT/server_$test_name &
	sleep 1
	./$SRC/$CLIENT < $tst &> $OUT/client_$test_name &
	sleep 1
	pkill $SERVER

	diff_client=$(diff $OUT/client_$test_name $REFS/client_$test_name)
	diff_server=$(diff $OUT/server_$test_name $REFS/server_$test_name)

	if [ "$diff_client" == "" ] && [ "$diff_server" == "" ]; then
		last_part="PASSED"
	else
		last_part="FAILED"
	fi

	printf "%s%s%s\n" "$test_name" "${DOTS:$((${#last_part} + ${#test_name}))}" "$last_part";

	if [ $last_part == "FAILED" ]; then
		echo "Server diff:"
		echo -e "$diff_server"

		echo -e "\nClient diff:"
		echo -e "$diff_client"
	fi

	pkill $SERVER
done

echo -e "\nFinished single client tests. Running multiple clients test:"

test_name="multiple_clients.txt"
./$SRC/$SERVER &> $OUT/server_$test_name &
sleep 1
./$SRC/$CLIENT < $MULTIPLE_TESTS/client1.txt &> $OUT/client1_$test_name &
./$SRC/$CLIENT < $MULTIPLE_TESTS/client2.txt &> $OUT/client2_$test_name &
sleep 4
pkill $SERVER

diff_client1=$(diff $OUT/client1_$test_name $REFS/client1_$test_name)
diff_client2=$(diff $OUT/client2_$test_name $REFS/client2_$test_name)
diff_server=$(diff $OUT/server_$test_name $REFS/server_$test_name)

if [ "$diff_client1" == "" ] && [ "$diff_client2" == "" ] && [ "$diff_server" == "" ]; then
	last_part="PASSED"
else
	last_part="FAILED"
fi

printf "%s%s%s\n" "$test_name" "${DOTS:$((${#last_part} + ${#test_name}))}" "$last_part";

if [ $last_part == "FAILED" ]; then
	echo "Server diff:"
	echo -e "$diff_server"

	echo -e "\nClient 1 diff:"
	echo -e "$diff_client1"
	echo -e "\nClient 2 diff:"
	echo -e "$diff_client2"
fi

echo -e "\nAll tests run. Cleaning up:"
make -C $SRC clean
rm -f *.rpcdb
