for CC in gcc clang ; do
	for COPT in -O2 -O3 ; do
		make clean
		make all CC=$CC COPT=$COPT
		make bench | tee bench/"$CC""$COPT".json
	done
done
