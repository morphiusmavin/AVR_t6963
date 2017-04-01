function find_src_files_of_name {
  find . -type f -name "$1" -print0 
}

find_src_files_of_name "*.c"
find_src_files_of_name "*.h"
