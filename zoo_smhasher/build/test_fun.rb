for c1 in [3,5,7,9]
  for c3 in [3,5,7,9]
    for c2 in [3,5,7,9]
      txt = File.read('../funny_hash.h')
      txt.sub!(/^#define C1 .*$/, "#define C1 #{c1}")
      txt.sub!(/^#define C2 .*$/, "#define C2 #{c2}")
      txt.sub!(/^#define C3 .*$/, "#define C3 #{c3}")
      File.open('../funny_hash.h', 'w') do |f| f.write(txt) end
      system('make')
      IO.popen(%w'taskset -c 1 stdbuf -o0 ./SMHasher --test=Avalanche,Cyclic,TwoBytes,Sparse,Permutation,Zeroes,Seed Funny32'+[{err: [:child, :out]}]) do |f|
        print "m#{c1} r#{c2} m#{c3}\n"
        File.open("funny32_#{c1}_#{c2}_#{c3}.out", 'w') do |o|
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
