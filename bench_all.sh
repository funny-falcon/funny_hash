make bench/test_file
for CC in gcc clang ; do
	for COPT in -O1 -O2 -O3 ; do
		make clean_bench_exe
		make all CC=$CC COPT=$COPT
		make bench | tee bench/"$CC""$COPT".json
	done
done
