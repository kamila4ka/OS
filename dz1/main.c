#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>


int main(){

	DIR* dir;
	struct dirent* currentEntry;
	struct stat* fileStatus;
	dir = opendir(".");
	if(dir == NULL){
		perror("Couldn't open directory: ");
	}
	else{
		currentEntry = readdir(dir);
		while(currentEntry){

			uid_t uid = getuid();
			struct passwd* usr = getpwuid(uid);
			struct group* grp = getgrgid(uid);

			stat(currentEntry->d_name, fileStatus);

			char timeOfLastMod[30];
			strcpy(timeOfLastMod, ctime(&fileStatus->st_mtime));
			
			char is_dir = (S_ISDIR(fileStatus->st_mode)) ? 'd' : '-';
    			char usr_r = (S_IRUSR & fileStatus->st_mode) ? 'r' : '-';
        		char usr_w = (S_IWUSR & fileStatus->st_mode) ? 'w' : '-';
	    		char usr_x = (S_IXUSR & fileStatus->st_mode) ? 'x' : '-';
	        	char grp_r = (S_IRGRP & fileStatus->st_mode) ? 'r' : '-';
		        char grp_w = (S_IWGRP & fileStatus->st_mode) ? 'w' : '-';
		        char grp_x = (S_IXGRP & fileStatus->st_mode) ? 'x' : '-';
			char oth_r = (S_IROTH & fileStatus->st_mode) ? 'r' : '-';
			char oth_w = (S_IWOTH & fileStatus->st_mode) ? 'w' : '-';
			char oth_x = (S_IXOTH & fileStatus->st_mode) ? 'x' : '-';
			

			printf("%c%c%c%c%c%c%c%c%c%c ", is_dir, usr_r, usr_w, 
				usr_x, grp_r, grp_w, grp_x, oth_r, oth_w, oth_x);			
			printf("%ld %s %s %ju %s %s", fileStatus->st_nlink,
				usr->pw_name, grp->gr_name, fileStatus->st_size,
				timeOfLastMod, currentEntry->d_name);	

		}
	}
	closedir(dir);

	return 0;
}

