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

- head <file_name> # read first 10 lines by default
- head -n 20 <filen_1> <file_2> # read first 20 lines from each file
- head -c 10 < <file_1>  # read first 10 bytes from stdin

#### tail

### Example Usage

- tail <file_name> # read last 10 lines by default
- tail -n 20 <filen_1> <file_2> # read last 20 lines from each file
- tail -c 10 < <file_name> # read last 10 bytes from stdin
- tail -n 5 -f <file_name> # read last 5 lines and follow file
