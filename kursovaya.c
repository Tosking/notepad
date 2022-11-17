#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <mcheck.h>
//Адаптация очистки терминала под Windows и Unix системы
#if defined(unix) || defined(__unix__) || defined(__unix)
	#define clear_win() system("clear") //system("clear")
#elif defined(_WIN32)
	#define clear_win() system("cls")
#endif

/*
list - сам лист
lines - количество строк
llen - длина каждой строки
categ - массив с координатами каждой категории
catnum - количество категорий
*/
typedef struct {
    char **list;
    int lines;
	int *llen;
    int **categ;
    int catnum;
} Note;

//ожидание нажатия Enter от пользователя
void enter_press(){
	printf("Press [Enter] key to continue.\n");
	fflush(stdin);
   	while(getchar()!='\n');
	getchar();
}

//получение размера файла
int get_fsize(FILE *f){
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	rewind(f);
    return size;
}

//получение кол-ва строк
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
//Парсинг сохраненных файлов
Note *get_strs(FILE *f){
	char ch;
	int strs = get_strnum(f);
	int size = strs * sizeof(int);				
	int *len = (int*) malloc(size);
    len[0] = 1;
	int snum = 0;
	//подсчет длин строк для веделения памяти
	while((ch = fgetc(f)) != EOF){
		if(strs == snum + 1){
			break;
		}
		if(ch == '\n'){
			snum++;
            len[snum] = 1;
		}
		else {
			len[snum]++;
		}
	}
    rewind(f);
	//выделение памяти под строки
    char **strarr = (char**) malloc(sizeof(char*) * (strs + 1));
    for(int i = 0; i < strs; i++){
        strarr[i] = (char*) malloc(sizeof(char) * len[i] + 10);
        memset(strarr[i], 0, len[i] - 1);
    }
	Note *note = (Note*) malloc(sizeof(Note));
	//запись строк в структуру
    char buff[2];
    int line = 0;
	rewind(f);
	buff[1] = '\0';
    while((ch = fgetc(f)) != EOF){
        if(ch == '\n'){
			buff[0] = ch;
			strcat(strarr[line], buff);
			line++;
			if(line == strs)
				break;
			continue;
        }
		buff[0] = ch;
		strcat(strarr[line], buff);
    }
	//запись сохраненных координат категорий в структуру
	FILE *conf = fopen("./.noteconf", "r");
	if(conf != NULL){
		fscanf(conf, "%d", &note->catnum);
		note->categ = (int**) malloc(sizeof(int*));
		note->categ[0] = (int*) malloc(sizeof(int) * 2);
		for(int i = 0; i < note->catnum; i++){
			fscanf(conf, "%d %d", &note->categ[i][0], &note->categ[i][1]);
			int **temp = (int**) realloc(note->categ, sizeof(int*) * (i + 2));
			if(temp != NULL){
				note->categ = temp;
				note->categ[i+1] = (int*) malloc(sizeof(int) * 2);
			}
			else{
				fclose(conf);
				return NULL;
			}
		}
		fclose(conf);
	}
    note->lines = strs;
	note->list = strarr;
	note->llen = (int*) malloc(size);
	memcpy(note->llen, len, size);
    free(len);
	return note;
}


//вывод листа на экран
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
			\n4.Create a category \
			\n5.Delete category \
			\n");
}

void add_item(Note *note){
	printf("Type item:");
	char *buffer = (char*) malloc(sizeof(char) * 100);
	scanf("%s", buffer);
	if(strlen(buffer) == 0){
		printf("Item is empty!\n");
		enter_press();
		return;
	}
	//добавление времени в конец строки
	time_t curtime;
	time(&curtime);
	strcat(buffer, " (");
	strcat(buffer, ctime(&curtime));
	buffer[strlen(buffer)-1] = '\0';
	strcat(buffer, ")\n");

	note->lines++;
	char **temp = (char**) realloc(note->list, note->lines * sizeof(char*));
	if(temp != NULL){
		note->list = temp;
	}
	else{
		printf("realloc failed\n");
		free(buffer);
		return;
	}

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
	//проверки на корректность ввода
	if(start_num >= end_num){
		printf("Start number and end number is equal or start number is larger\n");
		enter_press();
		return;
	}
	if (start_num < 0 || end_num > note->lines){
		printf("Start number lower than 0 or end number larger than lines in list");
		enter_press();
		return;
	}
	for(int i = 0; i < note->catnum; i++){
		if((note->categ[i][0] >= start_num && note->categ[i][1] <= start_num) || (note->categ[i][0] >= end_num && note->categ[i][1] <= end_num)){
			printf("New category cannot cross other categories\n");
			enter_press();
			return;
		}
	}

	char ch;
	note->lines += 2;

	char **temp = (char**) realloc(note->list, (note->lines + 1) * sizeof(char*) );
	if(temp != NULL){
		note->list = temp;
	}
	else{
		printf("realloc failed\n");
		return;
	}
	//Смещение координат всех других категорий
	if(note->catnum != 0){
		for(int i = 0; i < note->catnum; i++){
			if(note->categ[i][0] >= start_num){
				note->categ[i][0] += 2;
				note->categ[i][1] += 2;
			}
		}
	}
	//занесение категории в лист
    for(int i = 0; i < note->lines; i++){
        if(start_num == i){
			for(int k = note->lines; k >= i; k--) {
				note->list[k + 1] = note->list[k];
			}
			note->list[i] = (char*)malloc(sizeof(char) * (strlen(name) + 5));
			memset(note->list[i], 0, sizeof(char) * (strlen(name) + 3));
			strcat(note->list[i], ":::");
			strcat(note->list[i], name);
			strcat(note->list[i], "\n");
		}
		else if(end_num == i){
			for(int k = note->lines; k >= i; k--) 
				note->list[k + 1] = note->list[k];
			note->list[i] = (char *) malloc(sizeof(char) * 5);
			memset(note->list[i], 0, sizeof(char) * 4);
			strcat(note->list[i], "---");
			strcat(note->list[i], "\n");
		}
    }
	//сохранение координат категории
	note->catnum++;
	if(note->catnum != 1){
		note->categ = (int**) realloc(note->categ, note->catnum * sizeof(int*));
		note->categ[note->catnum - 1] = (int *) malloc(sizeof(int) * 2);
		note->categ[note->catnum - 1][0] = start_num;
		note->categ[note->catnum - 1][1] = end_num;
	}
	else{
		note->categ = (int**) malloc(sizeof(int*));
		note->categ[0] = (int *) malloc(sizeof(int) * 2);
		note->categ[0][0] = start_num;
		note->categ[0][1] = end_num;
	}
}

void delete_category(FILE* f, char* name, Note *note){
	int *coords = (int*) malloc(sizeof(int) * 2);
	int found;
	//поиск координат по имени
	for(int i = 0; i < note->catnum; i++){
		found = 1;
		for(int k = 0; k < strlen(name); k++){
			if(note->list[note->categ[i][0]][k + 3] != name[k]){
				found = 0;
				break;
			}
		}
		if(found){
			memcpy(coords, &note->categ[i], sizeof(int) * 2);
			break;
		}
		else{
			coords = NULL;
		}
	}
	if(coords == NULL){
		printf("category with that name is not found\n");
		return;
	}

	//удаление категории
	free(note->list[coords[0]]);
	free(note->list[coords[1]]);

	for(int i = 0; i < note->lines; i++){
		if(i > coords[0] && i != coords[1]){
			if(i > coords[1]){
				note->list[i - 2] = note->list[i];
			}
			else{
				note->list[i - 1] = note->list[i];
			}
		}
	}

	note->lines -= 2;
	free(coords);

}

void save_list(FILE *f, Note *note, const char *SAVE){
	//сохранение листа
	fclose(f);
	f = fopen(SAVE, "w");
	for(int i = 0; i < note->lines; i++){
		fprintf(f, "%s", note->list[i]);
	}
	//сохрание файла с категориями
	FILE *conf = fopen("./.noteconf", "w");
	char *temp = (char*) malloc(note->catnum * sizeof(char));
	sprintf(temp, "%d", note->catnum);
	fputs(temp, conf);
	for(int i = 0; i < note->catnum; i++){
		fprintf(conf, " %d %d", note->categ[i][0], note->categ[i][1]);
	}
	fclose(conf);
	free(temp);
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
	if(note == NULL){
		
	}
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
		clear_win();
		switch (var){
			case 1:
				show_list(f, note);
				break;
			case 2:
				add_item(note);
				break;
			case 3:
				printf("Enter a number of item, that you want to delete:");
				int number;
				scanf("%d", &number);
				delete_item(number, note);
				break;
			case 4:
				printf("Enter a numbers where category starts and ends, and a name of the category:");
				int start_num, end_num;
				char name[100]; 
				scanf("%d%d%s", &start_num, &end_num, &name);
				create_category(f, start_num, end_num, name, SAVE, note);
				break;
			case 5:
				printf("Enter the name of a category than you want do delete:");
				memset(name, 0, 100);
				scanf("%s", &name);
				delete_category(f, name, note);
				break;
		}
		save_list(f, note, SAVE);
		fclose(f);
	}
}
