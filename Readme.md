### Build executables

- make

### Remove executables

- make clean

### Update PATH to point to your custom shell counterpart

- export PATH=$(pwd):$PATH

### Utilities supported

#### cat

### Example Usage

- cat zen.txt
- cat -o <output_file> <file_name>
- cat -a <output_file> <file_name>
- cat -o <output_file> <file_1> <file_2>
- cat -n <file_name>

P.S: The order of the flags for the cat utility matters since that's how getopts parses. In later versions,
     adding a customized parsing logic could help resolve it.

#### find

### Example Usage

- find /path
- find /path -name <file_name>
- find /path -type f -iname <file_name>
- find /path -user <user_name> -group <group_name>  -perm <r,x,rw,rx>
- find /path -size +100M -mtime +5

#### head

### Example Usage

- head <file_name> # Read 10 lines by default
- head -n 20 <filen_1> <file_2> # Read 20 lines from each file
- head -c 10 <file_1> <file_2> # Read 10 bytes from each file
- head < <file_name> # Read from stdin
