# Tree
Recursive directory listing program with depth-indented listing of files in C

## Usage
`./tree [-rifsF] [PATH ...]`
- if no path is specified, default path is used, which is path to the current directory

### Options
- `-r` -> Recursively walk down the directory file tree
- `-i` -> Makes tree not print the indentation lines
- `-f` -> Prints the full path prefix for each file
- `-s` -> Print the size of each file in bytes along with the name
- `-A` -> Print ASCII pipes instead of the UNICODE ones
- `-F` -> Append a '/' for directories, a '=' for socket files and a '|' for FIFO's, same as `ls -F`
