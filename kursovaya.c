#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define clear_win() system("clear")

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

void show_list(FILE *f){
	clear_win();
	char ch;
	int num = 1;
	bool is_nl = 0;
	if(fgetc(f) == EOF){
		printf("List is empty");
		enter_press();
		return;
	}
	else{
		rewind(f);
	}
	printf("%d. ", num);
	while((ch = fgetc(f)) != EOF){
		if(ch == '\n'){
			is_nl = 1;
			printf("\n");
			continue;
		}
		if(is_nl == 1 && ch != '-' && ch != ':'){
			num++;
			printf("%d. ", num);
		}
		printf("%c", ch);
		is_nl = 0;
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

void add_item(FILE *f){
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
	fputs(buffer, f);
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

void create_category(FILE *f, int start_num, int end_num, char *name, const char *SAVE){
	if(start_num >= end_num){
		printf("Start number and end number is equal or start number is larger\n");
		enter_press();
		return;
	}
	char ch;
	int line = 1;
	bool is_nl = 0;
	bool placed = 0;
	int size = get_fsize(f) + strlen(name) + 5;
	char *list = malloc(size);
	memset(list,0,size);
	char buf[2];
	while((ch = fgetc(f)) != EOF){
		if(ch == '\n'){
			line++;
			is_nl = 1;
			placed = 0;
			strcat(list, "\n");
			continue;
		}
		if(is_nl && line >= start_num && line <= end_num && (ch == '-' || ch == ':')){
			printf("Cannot create a category that crossing another category\n");
			enter_press();
			return;
		}
		is_nl = 0;
		if(line == start_num && placed == 0){
			placed = 1;
			strcat(list, ":::");
			strcat(name, "\n");
			strcat(list, name);
			continue;
		}	
		else if(line == end_num && placed == 0){
			placed = 1;
			strcat(list, "---\n");
		}
		buf[0] = ch;
		strcat(list, buf);
	}
	fclose(f);
	f = fopen(SAVE, "w");
	fputs(list, f);
	free(list);
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
	while((ch = fgetc(f)) != EOF){
		if(ch == '\n'){
			strcat(list, "\n");
			is_nl = 1;
			continue;
		}
		if(is_nl && ch == ':') {
			is_in_cat = 1;
			continue;
		}
		else if(is_in_cat){
			is_nl = 0;
			for(int i = 0; i < name_size; i++){
				buff[i] = ch;
				ch = fgetc(f);
			}
			if(strcmp(name, buff) == 0){
				while((ch = fgetc(f)) != EOF){
					if(ch = '\n'){
						if(!is_in_cat) break;
						is_nl = 1;
						continue;
					}
					if(is_nl && ch == '-'){
						is_in_cat = 0;
					}
				}
			}
			else {
				strcat(list, ":::");
				strcat(list, buff);
				strcat(list, "\n");
			}
		}
		else {
			chcat[0] = ch;
			strcat(list, chcat);
		}

	}
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
				show_list(f);
				break;
			case 2:
				add_item(f);
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
				create_category(f, start_num, end_num, name, SAVE);
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
