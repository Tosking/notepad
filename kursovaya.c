#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#if defined(unix) || defined(__unix__) || defined(__unix)
	#define clear_win() system("clear")
#elif defined(_WIN32)
	#define clear_win() system("cls")
#endif

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
    char *categ = malloc(sizeof(char) * 3);
    int level = 0;
    int catnum = 0;
    int line = 0;

	buff[1] = '\0';
    while((ch = fgetc(f)) != EOF){
        if(ch == '\n'){
        	line++;
			strcat(strarr[snum], "\n");
			for(int i = 0; i < 3; i++){
				if((ch = fgetc(f)) != EOF) categ[i] = ch;
				else {
					realloc(categ, sizeof(char));
					categ[0] = '\0';
					break;
				}
			}
            if(categ == ":::"){
                catnum++;
				level++;
                realloc(note->categ, catnum * 2 * sizeof(int));
                note->categ[level][0] = line;
            }
			else if(categ == "---"){
				note->categ[level][1] = line;
				level--;
			}
			snum++;
			if(categ[0] != '\0'){
				strcat(strarr[snum], categ);
			}
        }
        else{
            buff[0] = ch;
			strcat(strarr[snum], buff);
        }
    }
    note->lines = snum;
	note->catnum = catnum;
	note->list = (char**) malloc(snum * sizeof(char*));
	note->llen = (int*) malloc(size);
	memcpy(note->llen, len, size);
	memcpy(note->list, strarr, snum * sizeof(char*));
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

void add_item(Note *note){
	clear_win();
	printf("Type item:");
	char *buffer = (char*) malloc(sizeof(char) * 100);
	scanf("%s", buffer);
	if(strlen(buffer) == 0){
		printf("Item is empty!\n");
		enter_press();
		return;
	}
	time_t curtime;
	time(&curtime);
	strcat(buffer, " (");
	strcat(buffer, ctime(&curtime));
	buffer[strlen(buffer)-1] = '\0';
	strcat(buffer, ")\n");
	int size = 0;
	for(int i = 0; i < note->lines; i++){
		size += note->llen[i];
	}
	note->lines++;
	note->list[note->lines - 1] = buffer;
}

void delete_item(int num, Note *note){
	note->lines--;
	free(note->list[num]);
	for(int i = num; i < note->lines; i++){
		note->list[i] = note->list[i+1];
	}
}

void create_category(FILE *f, int start_num, int end_num, char *name, const char *SAVE, Note *note){
	if(start_num >= end_num){
		printf("Start number and end number is equal or start number is larger\n");
		enter_press();
		return;
	}
	char ch;
	note->lines += 2;
	bool steps = 0;
	
    for(int i = 0; i < note->lines; i++){
        if(start_num == i){
			for(int k = note->lines; k >= i; k--) {
				note->list[k + 1] = note->list[k];
			}
			note->list[i] = (char*)malloc(sizeof(char) * (strlen(name) + 3));
			memset(note->list[i], 0, sizeof(char) * (strlen(name) + 3));
			strcat(note->list[i], ":::");
			strcat(note->list[i], name);
			strcat(note->list[i], "\n");
		}
		else if(end_num == i){
			for(int k = note->lines; k >= i; k--) note->list[k + 1] = note->list[k];
			note->list[i] = malloc(sizeof(char) * 4);
			memset(note->list[i], 0, sizeof(char) * 4);
			strcat(note->list[i], "---");
			strcat(note->list[i], "\n");
		}
    }
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

void save_list(FILE *f, Note *note, const char *SAVE){
	fclose(f);
	f = fopen(SAVE, "w");
	for(int i = 0; i < note->lines; i++){
		fprintf(f, "%s", note->list[i]);
	}
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
				add_item(note);
				break;
			case 3:
				clear_win();
				printf("Enter a number of item, that you want to delete:");
				int number;
				scanf("%d", &number);
				delete_item(number, note);
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
		save_list(f, note, SAVE);
		fclose(f);
	}
}
