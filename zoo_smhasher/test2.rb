files = Dir['funny32_*.out4'].sort
puts files
makemtx = Mutex.new
queue = Queue.new
files.each{|f| queue << f}
threads = 3.times.map do
  queue << nil
  Thread.new do
    while f = queue.pop
      next unless f =~ /funny32_(\d+)_(\d+)_(\d+).out.?/
      rota, rotb, rotd = $1.to_i, $2.to_i, $3.to_i
      print "Try #{rota} #{rotb} #{rotd}\n"
      makemtx.lock
      locked = true
      f = File.read('../funny_hash.h')
      f.sub!(/#define rota \d+/, "#define rota #{rota}")
      f.sub!(/#define rotb \d+/, "#define rotb #{rotb}")
      f.sub!(/#define rotd \d+/, "#define rotd #{rotd}")
      File.write('../funny_hash.h', f)
      `make`
      out = ''
      good = true
      #IO.popen("./SMHasher --test=Diff,DiffDist,BIC funny32") do |io|
      IO.popen("./SMHasher --test=Avalanche,Sanity,Cyclic,TwoBytes,Sparse,Permutation,Window,Text,Seed funny32") do |io|
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
