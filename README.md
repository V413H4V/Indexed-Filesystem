# Indexed Filesystem

#### It is a dropbox-like "in-memory" filesystem with ~33 MB of virtual drive space. Users will have the ability to list the files currently in this file system, add new files, delete files and even recover the deleted files. Since this file system resides in the RAM, files will not persist when the program exits.
=================================================================================

  - #### Compilation : <p><code> gcc -Wall filesystem.c -o Filesystem.out </code></p>
  - #### Execute with : <p><code> ./Filesystem.out </code></p>
  
###  
### Features:

  - #### Supports 4226 file-blocks on the file system with the block size of 8KB each.
  #####
  - #### Supports up to 128 files on the file system.
 #####
- #### Provides ~33MB space for storing files.
######
- #### This File system uses Indexed file allocation scheme and assigns blocks 1-128 for inodes.
######
- #### Currently support a single level hierarchy with no subdirectories.
######
- #### Supports file name up to 32 characters.
######
- #### These specs values can be easily modified in the code to support higher values.
###
###
###
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
- ###### First block (Block 0) of this file system is intentionally left unused so that it can be used later to store all directory information or anything else.
- ###### <p><code> read `<filename>` `<offset position>` `<no. of bytes to read>` </code><p>: Reads the number of bytes from the given file, from the given position (in bytes) and prints it out in the terminal.
- ###### Support for multi-level hierarchy with subdirectories.
######

