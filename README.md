# Tree
Recursive directory listing program with depth-indented listing of files in C

## Usage
`./tree [-adfirsAF] [PATH ...]`
- if no path is specified, default path is used, which is path to the current directory

### Options
- `-a` -> Print hidden files (files starting with a dot)
- `-d` -> Print directories only
- `-f` -> Prints the full path prefix for each file
- `-i` -> Makes tree not print the indentation lines
- `-r` -> Recursively walk down the directory file tree
- `-s` -> Print the size of each file in bytes along with the name
- `-A` -> Print ASCII pipes instead of the UNICODE ones
- `-F` -> Append a '/' for directories, a '=' for socket files and a '|' for FIFO's, same as `ls -F`
