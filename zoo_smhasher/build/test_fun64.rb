for c1 in [5,9]
  for c3 in [5,9]
    for c2 in [7,9,11,13,15,17,19]
      txt = File.read('../funny_hash.h')
      txt.sub!(/^#define CC1 .*$/, "#define CC1 #{c1}")
      txt.sub!(/^#define CC2 .*$/, "#define CC2 #{c2}")
      txt.sub!(/^#define CC3 .*$/, "#define CC3 #{c3}")
      File.open('../funny_hash.h', 'w') do |f| f.write(txt) end
      system('make')
      IO.popen(%w'taskset -c 1 stdbuf -o0 ./SMHasher --test=Avalanche,Cyclic,TwoBytes,Sparse,Permutation,Zeroes,Seed Funny64'+[{err: [:child, :out]}]) do |f|
        print "m#{c1} r#{c2} m#{c3}\n"
        File.open("funny64_#{c1}_#{c2}_#{c3}.out", 'w') do |o|
          while s = f.gets
            $stdout.write s
            $stdout.flush
            o.write s
            if s =~ /bias.*!!!!/
              f.close
              break
            end
          end
        end
      end
    end
  end
end
