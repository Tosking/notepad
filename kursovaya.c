#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define clear_win() system("clear")

typedef struct {
    char **list;
    int lines;
	int *llen;
    int *categ[2];
    int catnum;
} Note;

void enter_press(){
	printf("Press [Enter] key to continue.\n");
	fflush(stdin);
   	while(getchar()!='\n');
	getchar();
}

int get_fsize(FILE *f){
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	rewind(f);
    return size;
}

int get_strnum(FILE *f){
	char ch;
	int strnum = 0;
	rewind(f);
	while((ch = fgetc(f)) != EOF){
		if(ch == '\n'){
			strnum++;
		}
	}
	rewind(f);
	return strnum;
}

Note *get_strs(FILE *f){
	char ch;
	int size = get_strnum(f) * sizeof(int);
	int *len = malloc(size);
    len[0] = 0;
	int snum = 0;
	while((ch = fgetc(f)) != EOF){
		if(ch == '\n'){
			snum++;
            len[snum] = 0;
		}
		else {
			len[snum]++;
		}
	}
    rewind(f);
    char **strarr = (char**)malloc(sizeof(char*) * snum);
    for(int i = 0; i < snum; i++){
        strarr[i] = (char*)malloc(sizeof(char) * len[i]);
        memset(strarr[i], 0, len[i]);
    }
    snum = 0;
	Note *note = (Note*)malloc(sizeof(Note));
    char buff[2];
    char categ[3];
    int level = 0;
    int catnum = 0;
    int line = 0;
    while((ch = fgetc(f)) != EOF){
        if(ch == '\n'){
        	line++;
            fgets(categ, 3, f);
            if(categ == ":::"){
                catnum++;
				level++;
                realloc(note->categ, catnum * 2 * sizeof(int));
                note->categ[level][0] = line;
            }
			if(categ == "---"){
				note->categ[level][1] = line;
				level--;
			}
            strcat(strarr[snum], categ);
            snum++;
        }
        else{
            buff[0] = ch;
            strcat(strarr[snum], buff);
        }
    }
    note->lines = snum;
	note->catnum = catnum;
	note->list = (char**) malloc(snum * sizeof(char*));
    free(len);
	return note;
}



void show_list(FILE *f, Note *note){
	clear_win();
	char ch;
    for(int i = 0; i < note->lines; i++){
        printf("%d. %s", i, note->list[i]);
    }
	enter_press();
}

void show_menu(){
	clear_win();
	printf("1.Show list \
			\n2.Add item to a list \
			\n3.Delete one item from list \
			\n4.Create a category");
}

void add_item(FILE *f, Note *note){
	clear_win();
	printf("Type item:");
	char buffer[100];
	scanf("%s", &buffer);
	if(strlen(buffer) == 0){
		printf("Item is empty!\n");
		enter_press();
		return;
	}
	time_t curtime;
	time(&curtime);
	strcat(buffer, " (");
	strcat(buffer, ctime(&curtime));
	memmove(&buffer[strlen(buffer)-1], &buffer[strlen(buffer)], strlen(buffer) - 2);
	strcat(buffer, ")\n");
	int size = 0;
	for(int i = 0; i < note->lines; i++){
		size += note->llen[i];
	}
	note->lines++;
	realloc(note->list, size + strlen(buffer) * sizeof(char));
	memcpy(note->list[note->lines - 1], buffer, strlen(buffer));
}

void delete_item(FILE *f, int num, const char *SAVE){
	char ch;
	int line = 1;
	int size = get_fsize(f);
	char *list = (char*)malloc(size);
	char buf[2];
	while((ch = fgetc(f)) != EOF){
		if(ch == '\n'){
			line++;
			continue;
		}
		if(line == num)	continue;		
		buf[0] = ch;
		strcat(list, buf);
	}
	fclose(f);
	f = fopen(SAVE, "w");
	fputs(list, f);
	free(list);
}

void create_category(FILE *f, int start_num, int end_num, char *name, const char *SAVE, Note *note){
	if(start_num >= end_num){
		printf("Start number and end number is equal or start number is larger\n");
		enter_press();
		return;
	}
	char ch;
	int lines = get_strnum(f) + 2;
	bool steps = 0;
	realloc(note->list, note->lines * sizeof(char));
    for(int i = 0; i < lines; i++){
        if(1);
    }
	fclose(f);
	f = fopen(SAVE, "w");
}

void delete_category(FILE* f, char* name, const char *SAVE){
	bool is_nl = 0;
	bool is_in_cat = 0;
	char ch;
	int name_size = strlen(name);
	char chcat[2];
	char *buff = malloc(name_size);
	memset(buff, 0, name_size);
	char *list = malloc(get_fsize(f));
	memset(list, 0, get_fsize(f));
	

	fclose(f);
	f = fopen(SAVE, "w");
	fputs(list, f);
	free(buff);
	free(list);
}

int main(int argc, char **argv){
    if(argc == 0){
        printf("No file input");
        return 1;
    }
    FILE *f;
	const char *SAVE = "list";
	f = fopen(SAVE, "ab+");
    Note *note = get_strs(f);
	fclose(f);
	bool run = 1;
	while(run){
		int var;
		f = fopen(SAVE, "ab+");
		if(f == NULL){
			continue;
		}
		show_menu();
		scanf("%d", &var);
		switch (var){
			case 1:
				show_list(f, note);
				break;
			case 2:
				add_item(f, note);
				break;
			case 3:
				clear_win();
				printf("Enter a number of item, that you want to delete:");
				int number;
				scanf("%d", &number);
				delete_item(f, number, SAVE);
				break;
			case 4:
				clear_win();
				printf("Enter a numbers where category starts and ends, and a name of the category:");
				int start_num, end_num;
				char name[100]; 
				scanf("%d%d%s", &start_num, &end_num, &name);
				create_category(f, start_num, end_num, name, SAVE, note);
				break;
			case 5:
				clear_win();
				printf("Enter the name of a category than you want do delete");
				memset(name, 0, 100);
				scanf("%s", &name);
				delete_category(f, name, SAVE);
				break;
		}
		fclose(f);
	}
}
