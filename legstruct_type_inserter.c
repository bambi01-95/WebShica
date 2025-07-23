#include "stdlib.h"
#include "stdio.h"
#include "string.h"

/*
TARGET STRUCT 
 - _yycontext
 - yythunk

TARGET POSITION 
    typedef struct _yythunk { 'here' int begin, end;  yyaction  action;  struct _yythunk *next; } yythunk;
    struct _yycontext {
    'here'
    char     *__buf;
    ...
STEP
 typedef struct _yythunk {
 struct _yycontext {
*/

char yycontextStr[] = "struct _yycontext {";
char yythunkStr[]   = "struct _yythunk {";
char typeStr[]      = "  type_t _type; /*not original element*/"; // インデントを含める

// ファイルを開いて構造体宣言位置を検出し、type_t _type; を挿入
void insert_type(const char *fileName){
    FILE *file = fopen(fileName, "r");
    if (!file) {
        printf("cannot open %s\n", fileName);
        return;
    }

    FILE *tmp = fopen("tmp.c", "w");
    if (!tmp) {
        printf("cannot create tmp.c\n");
        fclose(file);
        return;
    }

    char line[1024];
    char* headPosAddr = 0;
    while (fgets(line, sizeof(line), file)) {

        // _yycontext の行か検出
        if (strstr(line, yycontextStr)) {
            fputs(line, tmp); // まず行を書き出す
            fputs(typeStr, tmp); // 挿入
            fputc('\n',tmp);
        }
        // _yythunk の行か検出
        else if ((headPosAddr = strstr(line, yythunkStr))) {
            int i;
            // yythunkStr が見つかる位置まで書き出し
            for(i=0;(&line[i])<headPosAddr;i++){
                fputc(line[i], tmp);
            }
        
            // yythunkStr 本体を書き出し
            fputs(yythunkStr, tmp);
            i += strlen(yythunkStr);
        
            // 挿入
            fputs(typeStr, tmp);
        
            // 残りの行を書き出し
            while(line[i]!='\n' && line[i]!='\0'){
                fputc(line[i++], tmp);
            }
        
            // 行末が \n の場合は改行を書き出す
            if(line[i]=='\n'){
                fputc('\n', tmp);
            }
        } else {
            // 見つからない場合はそのまま書き出す
            fputs(line, tmp);
        }        
    }

    fclose(file);
    fclose(tmp);

    // 元ファイルに上書き
    remove(fileName);
    rename("tmp.c", fileName);
}

int main(int argc, char* argv[]){
    char *fileName;
    if(argc == 1){ // 指示がないとき
        fileName = strdup("vm.c");
    } else { 
        printf("read %s\n",argv[1]);
        fileName = strdup(argv[1]);
    }

    insert_type(fileName);
    free(fileName);

    return 0;
}
