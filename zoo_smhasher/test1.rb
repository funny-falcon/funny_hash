files = Dir['funny32_*.out'] & Dir['funny32_*.out_'].map{|ff|ff[0...-1]}
puts files
makemtx = Mutex.new
queue = Queue.new
files.each{|f| queue << f}
threads = 3.times.map do
  queue << nil
  Thread.new do
    while f = queue.pop
      next unless f =~ /funny32_(\d+)_(\d+)_(\d+).out/
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
      IO.popen("./SMHasher --test=Sanity,Cyclic,TwoBytes,Sparse,Permutation,Window,Text,Seed funny32") do |io|
        io.each_line do |line|
          if locked
            makemtx.unlock
            locked = false
          end
          out << line
          if line.include?('!!!') || line =~ /- 0\.[89]/ || line.include?("FAIL")
            good = false
            Process.kill("KILL", io.pid)
            io.close
            break
          end
        end
      end
      if good
        print "Good #{rota} #{rotb} #{rotd}\n"
        File.write("funny32_#{rota}_#{rotb}_#{rotd}.out2", out)
      else
        print "Bad #{rota} #{rotb} #{rotd}\n"
      end
    end
  end
end
threads.each(&:join)
