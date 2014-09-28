for CC in gcc clang ; do
	for COPT in -O1 -O2 -O3 ; do
		make clean
		make all CC=$CC COPT=$COPT
		make test_file
		make bench | tee bench/"$CC"_"COPT".out
	done
done
