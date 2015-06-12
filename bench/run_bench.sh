for fun in funny32 funny64 fnv1a murmur32 murmur128 sip24 sip13 lookup3 spooky ; do
  for ch in piece whole ; do
    taskset -c 1 $1 $fun $ch 300000000
  done
done
