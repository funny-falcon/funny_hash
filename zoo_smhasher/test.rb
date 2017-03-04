
1.step(31,2) do |rota|
  2.step(30,2) do |rotb|
    1.step(31,2) do |rotd|
      print "Try #{rota} #{rotb} #{rotd} "
      f = File.read('../funny_hash.h')
      f.sub!(/#define rota \d+/, "#define rota #{rota}")
      f.sub!(/#define rotb \d+/, "#define rotb #{rotb}")
      f.sub!(/#define rotd \d+/, "#define rotd #{rotd}")
      File.write('../funny_hash.h', f)
      `make`
      out = ''
      good = true
      IO.popen("./SMHasher --test=Avalanche funny32") do |io|
        io.each_line do |line|
          out << line
          if line.include?('!!!') || line =~ /0\.[89]/
            good = false
            Process.kill("KILL", io.pid)
            io.close
            break
          end
        end
      end
      if good
        puts "Good"
        File.write("funny32_#{rota}_#{rotb}_#{rotd}.out_", out)
      else
        puts "Bad"
      end
    end
  end
end
