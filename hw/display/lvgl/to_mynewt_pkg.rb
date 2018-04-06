
# Only run once

require 'fileutils'
require 'pathname'

include FileUtils


{ "src"     => [ "lv*/**/*.c" ],
  "include" => [ "lv*/**/*.h", "lv*.h" ],
}.each {|dir, globs|
    Dir[*globs].each {|f| f = Pathname(f)
        dst = "#{dir}/#{f.dirname}"
        mkdir_p dst
        mv f, dst
    }
}

Dir["**/*.[ch]"].each {|f|
    dir = File.dirname(f).to_s.split('/')[1..-1].join('/')
    dir = Pathname(dir)
    
    lines = File.readlines(f).map {|line|
        line.gsub(/(?<=^#include ")[^"]+(?=")/) {|inc|
            (dir + Pathname(inc)).sub(/^\.\.\//, '')
        }

    }

    File.write(f, lines.join)
}

Dir["lv_*"].each {|dir|
    rm_rf dir
}
