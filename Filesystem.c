#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

#define MAX_COMMAND_SIZE 60
#define NUM_BLOCKS 4226
#define BLOCK_SIZE 8096
#define NUM_FILES 128
#define NUM_INODES 128
#define NUM_BLOCKS_PER_FILE 32
#define FILENAME_LEN 32


/*#########################################
 * 
 * Author : Vaibhav Murkute
 * Project: Indexed Filesystem
 * Date   : 04/26/2018
 *
 *######################################### */


struct inode {
	char file_name[FILENAME_LEN+1];
	int valid;
	int file_size;
	int file_blocks[NUM_BLOCKS_PER_FILE];
	time_t mod_time;
};

unsigned char file_data[NUM_BLOCKS][BLOCK_SIZE];
int used_blocks[NUM_BLOCKS];
struct inode * inode_ptr[NUM_INODES];
static int file_offset = NUM_INODES + 1;


void init();
int df();
void put(char* file_name);
void get(char* file_name, char* dest_file);
void delete(char* file_name);
void undelete(char* file_name);
int getFreeBlock();
int getFreeInode();
char* StrDup (char *str);

int main()
{

    char input[MAX_COMMAND_SIZE];
    char* command[MAX_COMMAND_SIZE];
    memset(command,'\0',sizeof(command));
    memset(input,'\0',sizeof(input));

    //initialization
    init();

    while(1){
        int count=0;	
        memset(input,'\0',sizeof(input));   // clearing input for fresh start

        printf("mfs:>\t");   // prints out the msf prompt
        while( !fgets (input, MAX_COMMAND_SIZE, stdin) );     // scans all the input

        char* str = strtok(input," ");  // Tokenizing the first word

        while(str != NULL){
            str = StrDup(str);
            command[count] = str;       // tokenizing rest of the input string
            str = strtok(NULL," ");
            count += 1;		// keeps track of number of words in input
        }

        command[count] = NULL;      // NULL terminating input string
	
	if(strcmp(command[0],"df") == 0){
		printf("%d bytes free.\n\n\r",df());
		continue;

	}else if(strcmp(command[0],"list") == 0){
		int i;
		int empty = 0;
		char time[50];
		for(i=0; i < NUM_INODES; i++){
			if(inode_ptr[i]->valid == 1){
				empty = 1;
				snprintf(time,sizeof(time),"%s",ctime(&(inode_ptr[i]->mod_time)));
				// removing new line char from ctime
				time[strcspn(time,"\n")] = '\0';
				printf("%-7d %s %s\n\r", inode_ptr[i]->file_size, time, inode_ptr[i]->file_name);
			}
		}

		if(!empty){
			printf("No Files Found\n\r");
		}

		printf("\n\r");
		continue;

	}else if(strcmp(command[0],"put") == 0){
		if(count == 2){
			put(command[1]);	
		}else{
			printf("<!> Usage: put <filename>\n\r");	
		}
		
		printf("\n\r");
		continue;

	}else if(strcmp(command[0],"get") == 0){
		if(count == 3){
			get(command[1], command[2]);	
		}else if(count == 2){
			get(command[1], command[1]);	
		}else{
			printf("<!> Usage: get <filename> or get <filename> <newfilename>\n\r");
		}
		
		printf("\n\r");
		continue;

	}else if(strcmp(command[0],"del") == 0){
		if(count == 2){
			delete(command[1]);	
		}else{
			printf("<!> Usage: del <filename>\n\r");
		}
		
		printf("\n\r");
		continue;

	}else if(strcmp(command[0],"undelete") == 0){
		if(count == 2){
			undelete(command[1]);	
		}else{
			printf("<!> Usage: undelete <filename>\n\r");
		}
		
		printf("\n\r");
		continue;

	}else if(strcmp(command[0],"exit") == 0 || strcmp(command[0],"quit") == 0){
		
		exit(EXIT_SUCCESS);

	}else if(strcmp(command[0],"") == 0){
		
		continue;

	}else{
		printf("<!> Invalid Command.\n\n\r");
		continue;
	}

    }
    return 0;
}

void init(){

	int j;
	// allocating blocks 1-128 for inodes
	for(j=0; j < NUM_INODES; j++){
		inode_ptr[j] = (struct inode *) &file_data[j+1];
		inode_ptr[j]->valid = 0;
		memset(inode_ptr[j]->file_name,'\0',FILENAME_LEN+1);
	}

	// free blocks
	for(j=file_offset; j < NUM_BLOCKS; j++){
		used_blocks[j] = 0;
	}

}

int df(){
	int count = 0;
	int i = file_offset;
	for(; i < NUM_BLOCKS; i++){
		if(used_blocks[i]==0)
			count++;
	}

	return count*BLOCK_SIZE;
}

void put(char* file_name){
	struct stat buffer;
	int status = stat(file_name, &buffer);

	if(status == -1){
		printf("<!> File Not Found.\n\r");
		return;
	}

	if(buffer.st_size > df()){
		printf("<!> Not enough free space in the file system.\n\r");
		return;
	}
	
	int i;
	for(i=0; i < NUM_INODES; i++){
		if(inode_ptr[i]->valid == 1 && strcmp(inode_ptr[i]->file_name, file_name) == 0){
			printf("<!> %s already exists on the file system.\n\r",file_name);
			return;
		}
	}
	
	// allocate an inode
	int inode_index = getFreeInode();

	if(inode_index == -1){
		printf("<!> No free inodes in the file system.\n\r");
		return;
	}

	if(strlen(file_name) > FILENAME_LEN){
		printf("<!> File name too long. Keep it %d character long.\n\r", FILENAME_LEN);
		return;
	}

	// copying metadata in inode
	strncpy(inode_ptr[inode_index]->file_name, file_name, strlen(file_name)+1);
	// marking inode as used
	inode_ptr[inode_index]->valid = 1;
	inode_ptr[inode_index]->file_size = buffer.st_size;
	inode_ptr[inode_index]->mod_time = time(NULL);
	
	FILE* fp = fopen(file_name,"r");
	int copy_size = buffer.st_size;
	int temp_idx = 0;	// used to store file_blocks in inode
	while(copy_size > 0){
		int num_bytes;
		if(copy_size < BLOCK_SIZE){
			num_bytes = copy_size;
		}else{
			num_bytes = BLOCK_SIZE;
		}

		int block_index = getFreeBlock();
		if(block_index == -1){
			printf("<!> No Free Block available in the file System.\n\r");
			return;
		}

		used_blocks[block_index] = 1;
		
		// clearing the previous block data
		memset(&(file_data[block_index]),0,BLOCK_SIZE);
		
		inode_ptr[inode_index]->file_blocks[temp_idx] = block_index;
		fread(&file_data[block_index], num_bytes, 1, fp);

		temp_idx++;

		copy_size -= num_bytes;

	}

	fclose(fp);
	printf("\n\r");

}

void get(char* file_name, char* dest_file){
	int found = 0;
	int index;
	int i;
	for(i=0; i < NUM_INODES; i++){
		if(inode_ptr[i]->valid == 1 && strcmp(inode_ptr[i]->file_name, file_name) == 0){
			found = 1;
			index = i;
			break;
		}
	}

	if(!found){
		printf("<!> File Not Found.\n\r");
		return;
	}

	FILE* fp = fopen(dest_file, "w");
	
	int copy_size = inode_ptr[index]->file_size;
	int temp_index = 0;

	while(copy_size > 0){
		int num_bytes;
		if(copy_size < BLOCK_SIZE){
			num_bytes = copy_size;
		}else{
			num_bytes = BLOCK_SIZE;
		}

		int block_index = inode_ptr[index]->file_blocks[temp_index];

		fwrite(&(file_data[block_index]), num_bytes, 1, fp);

		temp_index++;
		copy_size -= num_bytes;
	}

	fclose(fp);

	printf("\n\r");

}

void delete(char* file_name){
	int found = 0;
	int index;
	int i;
	for(i=0; i < NUM_INODES; i++){
		if(inode_ptr[i]->valid == 1 && strcmp(inode_ptr[i]->file_name, file_name) == 0){
			found = 1;
			index = i;
			break;
		}
	}

	if(!found){
		printf("<!> File Not Found.\n\r");
		return;
	}
	
	// freeing inode
	inode_ptr[index]->valid = 0;

	int file_size = inode_ptr[index]->file_size;
	int temp_index = 0;

	while(file_size > 0){
		int num_bytes;
		if(file_size < BLOCK_SIZE){
			num_bytes = file_size;
		}else{
			num_bytes = BLOCK_SIZE;
		}

		int block_index = inode_ptr[index]->file_blocks[temp_index];
		// freeing file blocks
		used_blocks[block_index] = 0;
		
		temp_index++;
		file_size -= num_bytes;
	}

}

void undelete(char* file_name){
	int found = 0;
	int index;
	int i;
	for(i=0; i < NUM_INODES; i++){
		if(inode_ptr[i]->valid == 0 && strcmp(inode_ptr[i]->file_name, file_name) == 0){
			found = 1;
			index = i;
			break;
		}
	}

	if(!found){
		// block is already overwritten with some other file data
		printf("<!> This File cannot be recovered.\n\r");
		return;
	}
	
	// reinstate inode
	inode_ptr[index]->valid = 1;

	int file_size = inode_ptr[index]->file_size;
	int temp_index = 0;

	while(file_size > 0){
		int num_bytes;
		if(file_size < BLOCK_SIZE){
			num_bytes = file_size;
		}else{
			num_bytes = BLOCK_SIZE;
		}

		int block_index = inode_ptr[index]->file_blocks[temp_index];
		// reinstate file blocks
		used_blocks[block_index] = 1;
		
		temp_index++;
		file_size -= num_bytes;
	}
	
}

int getFreeBlock(){
	int retVal = -1;
	int i= file_offset;
	for(; i < NUM_BLOCKS; i++){
		if(used_blocks[i] == 0)
			return i;
	}

	return retVal;
}

int getFreeInode(){
	int retVal = -1;
	int i=0;
	for(; i < NUM_INODES; i++){
		if(inode_ptr[i]->valid == 0)
			return i;
	}

	return retVal;
}

char* StrDup (char *str) {
    char *result = (char *) malloc (strlen (str) + 1);
    if (result == NULL)
        return NULL;
    str[strcspn(str,"\n")]='\0';   // to remove trailing new line character put by fgets()
    strcpy (result, str);
    return result;
}


