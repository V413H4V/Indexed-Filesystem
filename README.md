# Indexed Filesystem

#### It is a dropbox-like "in-memory" filesystem with ~33 MB of virtual drive space. Users will have the ability to list the files currently in this file system, add new files, delete files and even recover the deleted files. Since this file system resides in the RAM, files will not persist when the program exits.
=================================================================================

  - #### Compilation : <p><code> gcc -Wall filesystem.c -o Filesystem.out </code></p>
  - #### Execute with : <p><code> ./Filesystem.out </code></p>

### Supported commands:

  - #### <p><code> put `<filename>` </code></p> : This command will copy the file from current directory to the file system.
  #####
  - #### <p><code> list </code></p> : This command will list the files currently present in the file system.
 #####
- #### <p><code> df </code></p> : This command will display the amount of disk space (in bytes) left in the file system.
######
- #### <p><code> get `<filename>` </code></p> : This command will retrieve the file from this file system and store it in your current working directory (in local hard-drive).
######
- #### <p><code> get `<filename>` `<newfilename>` </code></p> : This command will retrieve the file from this file system and store it in your current working directory (in local hard-drive), with new file name.
######
- #### <p><code> del `<filename>` </code></p> : It will delete the file from this file system.
######
- #### <p><code> undelete `<filename>`  </code></p> : It will recover the deleted file and re-instate it in the file system. Note that if the deleted file blocks get used by some other files in the file system, then this file cannot be recovered. In short, undelete will work when it is executed before "put"ting a new file onto the file system.
###
###
###
###
================================================================================
- ###### Future upgrades:
- ###### <p><code> read `<filename>` `<offset position>` `<no. of bytes to read>` </code><p>: Reads the number of bytes from the given file, from the given position (in bytes) and prints it out in the terminal.
######

