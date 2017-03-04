files = Dir['funny64_*.out'].sort
puts files
makemtx = Mutex.new
queue = Queue.new
files.each{|f| queue << f}
threads = 2.times.map do
  queue << nil
  Thread.new do
    while f = queue.pop
      next unless f =~ /funny64_(\d+)_(\d+)_(\d+).out.?/
      rota, rotb, rotd = $1.to_i, $2.to_i, $3.to_i
      print "Try #{rota} #{rotb} #{rotd}\n"
      makemtx.lock
      locked = true
      f = File.read('../funny_hash.h')
      f.sub!(/#define rot64a \d+/, "#define rot64a #{rota}")
      f.sub!(/#define rot64b \d+/, "#define rot64b #{rotb}")
      f.sub!(/#define rot64d \d+/, "#define rot64d #{rotd}")
      File.write('../funny_hash.h', f)
      `make`
      out = ''
      good = true
      IO.popen("./SMHasher --test=Diff,DiffDist,BIC funny64") do |io|
      #IO.popen("./SMHasher --test=Sanity,Cyclic,TwoBytes,Sparse,Permutation,Window,Text,Seed funny64") do |io|
      #IO.popen("./SMHasher --test=Avalanche funny32") do |io|
        io.each_line do |line|
          if locked
            makemtx.unlock
            locked = false
          end
          out << line
          if line.include?('!!!') || line =~ /(-|is) 0\.[89]/ || line.include?("FAIL")
            good = false
            Process.kill("KILL", io.pid)
            io.close
            break
          end
        end
      end
      if good
        print "Good #{rota} #{rotb} #{rotd}\n"
        #File.write("funny32_#{rota}_#{rotb}_#{rotd}.out4", out)
      else
        print "Bad #{rota} #{rotb} #{rotd}\n"
        unlink = Dir["*_#{rota}_#{rotb}_#{rotd}.*"]
        print "unlink #{unlink.join(" ")}\n"
        #unlink.each{|ff| File.unlink(ff)}
      end
    end
  end
end
threads.each(&:join)
