makemtx = Mutex.new
queue = Queue.new
#1.upto(63) do |rota|
#  2.upto(63) do |rotb|
#    1.upto(63) do |rotd|
#      next if ([rota, rotb, rotd]<=>[18,17,1])<0
#      queue << [rota, rotb, rotd]
#    end
#  end
#end
Dir['funny64*.out'].sort.each do |f|
  if f =~ /_(\d+)_(\d+)_(\d+)\./
    queue << [$1.to_i, $2.to_i, $3.to_i]
  end
end
threads = 4.times.map do
  queue << nil
  Thread.new do
    while r = queue.pop
      rota, rotb, rotd = r
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
      IO.popen({"rot64a"=>rota.to_s, "rot64b"=>rotb.to_s, "rot64d"=>rotd.to_s}, "./SMHasher --test=Avalanche funny64") do |io|
        io.each_line do |line|
          out << line
          if locked
            makemtx.unlock
            locked = false
          end
          if line.include?('!!!') || line =~ /0\.9/
            good = false
            Process.kill("KILL", io.pid)
            io.close
            break
          end
        end
      end
      if good
        print "Good #{rota} #{rotb} #{rotd}\n"
        File.write("funny64_#{rota}_#{rotb}_#{rotd}.out_", out)
      else
        print "Bad #{rota} #{rotb} #{rotd}\n"
      end
    end
  end
end
threads.each(&:join)
