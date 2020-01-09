/*
This code is my own work, it was written without consulting a tutor or 
code written by other students - Gene Lee
imitate the native ar function

t and A completed
q and xo completed for single files only
d and tv not attempted
*/

#include <ar.h>
#include<time.h>
#include <stdio.h>   
#include <stdlib.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <utime.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>

struct meta {
	char name[16];
	int mode;
	int size;
	int uid;
	int gid;
	time_t mtime; //type long
};


int fill_ar_hdr(char *filename, struct ar_hdr *hdr){
	struct stat *statFile = malloc(sizeof(struct stat));
	int a = stat(filename, statFile);
	if(a<0) return -1;

	//filename = strcat(filename, "/");

	sprintf(hdr->ar_name, "%s/", filename);
	
	sprintf(hdr->ar_name, "%-16s/", hdr->ar_name);
	sprintf(hdr->ar_date, "%-12ld", statFile->st_mtime); //save just the mtime, will be used to restore m and atime

	sprintf(hdr->ar_uid, "%-6u",statFile->st_uid);
	sprintf(hdr->ar_gid, "%-6u", statFile->st_gid);
	sprintf(hdr->ar_mode, "%-8o", statFile->st_mode);
	sprintf(hdr->ar_size, "%-10ld",(long)statFile->st_size);
	sprintf(hdr->ar_fmag, "%-1s", ARFMAG);
//printf("%s\n", hdr->ar_gid);

	return statFile->st_blksize;
	//return 0;
}

int fill_meta(struct ar_hdr hdr, struct meta *meta){

	strcpy(meta->name,  hdr.ar_name);
	sscanf(hdr.ar_mode, "%o", &meta->mode);
	sscanf(hdr.ar_size, "%d", &meta->size);
	sscanf(hdr.ar_uid, "%d", &meta->uid);
	sscanf(hdr.ar_gid, "%d", &meta->gid);
	sscanf(hdr.ar_date, "%ld", &meta->mtime);

	return 1;
}

void q(int fd, char *filename){
	int fd2 = open(filename, O_RDWR | O_CREAT, 0666);
	struct ar_hdr *hdr;
	hdr = malloc(sizeof(struct ar_hdr));
	
	int f_blocksize = fill_ar_hdr(filename, hdr); //gets blocksize from stat call in fill_ar_hdr
	//int f_blocksize = (int)hdr->ar_size;

	//int f_blocksize = 1;	
	//lseek to the end of the archive 
	//if on an even byte, adjust to start new header on an even byte
	if ((lseek(fd, 0, SEEK_END) % 2)) write(fd, "\n", 1);


	lseek(fd, 0, SEEK_END);
	write(fd, (char*) hdr, sizeof(struct ar_hdr)); //writes in header info

	char f_content[f_blocksize];

	//write the body of the file into the archive
	int i = read(fd2,f_content,f_blocksize);
	
	while(i>0){
		if(f_blocksize > i) f_blocksize = i;
		write(fd,f_content,f_blocksize);
		i = read(fd2,f_content,f_blocksize);
	}

	// read(fd2, f_content, (int)hdr->ar_size);
	// write(fd, f_content, (int)hdr->ar_size);
	
	close(fd2);	
}
void find_file(struct ar_hdr *hdr)
{
	//printf("%d\n", (int)'/'); 47
	//printf("%d\n", (int)'\0'); 0
    for(int i = 0; i < 15; i++){
    	if((int)hdr->ar_name[i] == 32 || (int)hdr->ar_name[i] == 47){
			hdr->ar_name[i] = '\0';
			break;
		}
    }
}

bool search(int ar_fd, struct ar_hdr *f_header, char *f_name, int f_size, off_t offset)
{

	while (read(ar_fd, f_header, sizeof(struct ar_hdr)) == sizeof(struct ar_hdr)){
		
		find_file(f_header);
		f_size = (int)atoi(f_header->ar_size);
		
		if ((strcmp(f_header->ar_name, f_name) == 0))
		{
			//has been found, end
			offset = lseek(ar_fd, 0, SEEK_CUR);
			printf("Found '%s' file in archive!\n", f_header->ar_name);
			return 1;
	
		} 
		else
		{
			//keep searching
			if (f_size%2) f_size++;
			lseek(ar_fd, f_size, SEEK_CUR);
			printf("Current file found: '%s''\n", f_header->ar_name);
		}
	}

	return 0;

}
void xo(int ar_fd, char *f_name){
	//fill_meta();
	lseek(ar_fd,8,SEEK_SET);

	int new_fd, f_blocks, f_size;

	struct ar_hdr *f_header = malloc(sizeof(struct ar_hdr));
	struct utimbuf *f_utime = malloc(sizeof(struct utimbuf)); 

	int num;
	off_t offset;
	
	if(search(ar_fd, f_header, f_name, f_size, offset)==1)
	{

		new_fd = open(f_header->ar_name,(O_WRONLY | O_CREAT), 0666);

		if(new_fd < 0) printf("Error: could not create new file. \n");

		if (offset < 0) printf("Error: could not find file. \n");

		//fill meta structure with binary
		struct meta *f_meta = malloc(sizeof(struct meta));
		fill_meta(*f_header, f_meta);

		char writ[f_meta->size];

		//writing into the new fd the amount of the file size
		if ((num = read(ar_fd, writ, f_meta->size)) > 0) {
			write(new_fd, writ, num);
		}

		//set old time, mtime stored in ar_date and now both a and m time restored to it
		mode_t ftime = (time_t) atoi(f_header->ar_date);
		f_utime->modtime = ftime;
		f_utime->actime = ftime;

		utime(f_header->ar_name,f_utime);

		
		close(new_fd);
	}	
	else printf("Sorry, cannot find the file you are looking for. Please try again!\n");
}

void t(int fd){
	//variable declarations
    struct ar_hdr *header = malloc(sizeof(struct ar_hdr));
    int size_content;
    
    while (read(fd, header, sizeof(struct ar_hdr)) == sizeof(struct ar_hdr))
    {
        find_file(header);
        size_content = (int) atoi(header->ar_size);
        
        printf("%s\n",header->ar_name);
        
        if(size_content%2==1) size_content = size_content + 1;
        
        lseek(fd, size_content, SEEK_CUR);
    }
    
    close(fd);
}

void tv(){
	printf("This is an unimplemented, optional feature.\n");
}

void d(){
	printf("This is an unimplemented, optional feature.\n");
}

void A(int ar_fd, char *archive){

	DIR *dir;
	struct dirent *files;

	//populate list of ordinary files
	dir = opendir("./");

	if (dir){
    	while (((files = readdir(dir)) != NULL))
    	{
    		//DT_REG checks if it is a normal file
    		if((files->d_type == DT_REG) && (strcmp(files->d_name,archive) != 0) && (strcmp(files->d_name,"myar") != 0) && (strcmp(files->d_name,"myar.c") != 0)) //normal file, not an archive
            { 
				q(ar_fd, files->d_name);
            }
      		
    	}
    	printf("All ordinary files (excluding myar, myar.c, and the given archive) have been appended!\n");
    	closedir(dir);
  	}
  	else{
  		printf("Sorry, directory could not be opened!\n");
  	}

}

int main(int argc, char *argv[]){

	//need minimum, program + key + archive, so minimum acceptable argc is 3
	if(argc < 3) {
		printf("Needs more arguments, sorry!\n");
		exit(-1);
	}

	char *key = argv[1], *archive = argv[2], *filename = argv[3];

	//if key is q or xo, also NEED file, min argc is 4
	if((*key=='q' || key[0]=='x') && argc != 4){
		printf("You either didn't give a file or gave more than one file. Program can't support that!\n");
		exit(-1);
	}

	int fd;

	if(*key == 'q'){
			fd = open(archive, O_RDWR | O_CREAT, 0666);
			printf("Archive file named %s has been created and/or opened.\n", archive);
			write(fd, ARMAG, 8);
	} else{
		fd = open(archive, O_RDWR);
		if(fd<0){
			printf("The archive %s does not exist, please try again.\n", archive);
			exit(-1);
		}
		char ch[8];
		read(fd, ch, 8);
	}


	if(*key=='q') q(fd, filename);
	else if(key[0]=='x') xo(fd, filename);
	else if(key[1]=='v') tv(); //optional
	else if(key[0]=='t') t(fd);
	else if(*key=='d') d(); //optional
	else if(*key=='A') A(fd, archive);
	else printf("This key is not valid in my program!\n");	

}