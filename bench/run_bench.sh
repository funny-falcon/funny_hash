for fun in funny32 funny64 murmur32 murmur128 sip24 sip13 lookup3 ; do
  for ch in piece whole ; do
    taskset -c 1 $1 $fun $ch test_file
  done
done
