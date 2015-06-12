require 'json'

gcc = JSON.load(File.read("gcc-O2.json"))["res"]
clang = JSON.load(File.read("clang-O2.json"))["res"]

x32gccw = gcc.select{|h| h["exe"] =~ /32/ && h["chunk"] == 'whole'}.map{|h| [h["fun"], h["time"]]}
x64gccw = gcc.select{|h| h["exe"] =~ /64/ && h["chunk"] == 'whole'}.map{|h| [h["fun"], h["time"]]}
x32gccp = gcc.select{|h| h["exe"] =~ /32/ && h["chunk"] == 'piece'}.map{|h| [h["fun"], h["time"]]}
x64gccp = gcc.select{|h| h["exe"] =~ /64/ && h["chunk"] == 'piece'}.map{|h| [h["fun"], h["time"]]}
x32clangw = clang.select{|h| h["exe"] =~ /32/ && h["chunk"] == 'whole'}.map{|h| [h["fun"], h["time"]]}
x64clangw = clang.select{|h| h["exe"] =~ /64/ && h["chunk"] == 'whole'}.map{|h| [h["fun"], h["time"]]}
x32clangp = clang.select{|h| h["exe"] =~ /32/ && h["chunk"] == 'piece'}.map{|h| [h["fun"], h["time"]]}
x64clangp = clang.select{|h| h["exe"] =~ /64/ && h["chunk"] == 'piece'}.map{|h| [h["fun"], h["time"]]}

x32w = {}
x32gccw.each{|f,t| x32w[f] = [t, nil]}
x32clangw.each{|f,t| x32w[f][1] = t}
x32p = {}
x32gccp.each{|f,t| x32p[f] = [t, nil]}
x32clangp.each{|f,t| x32p[f][1] = t}
x64w = {}
x64gccw.each{|f,t| x64w[f] = [t, nil]}
x64clangw.each{|f,t| x64w[f][1] = t}
x64p = {}
x64gccp.each{|f,t| x64p[f] = [t, nil]}
x64clangp.each{|f,t| x64p[f][1] = t}

def format(tab)
  puts <<HD
function  | gcc -O2 | clang -O2
----------|---------|----------
HD
  tab.each do |fun, t|
    t1, t2 = t
    puts("%-10s| % 6.2f  | % 6.2f" % [fun, t1, t2])
  end
end

puts <<HD
x86_64
------

By 1-14byte substrings twice

HD
format(x64p)
puts <<HD

10 times 300M at once

HD
format(x64w)
puts <<HD

x86 (by -m32)
-------------

By 1-14byte substrings twice

HD
format(x32p)
puts <<HD

10 times 300M at once

HD
format(x32w)
