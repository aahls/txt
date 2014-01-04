#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>

#include "db.h"

#define VERSION     "1.0.0" 

int atoi_altfail(const char *str);
void print_usage(void);

int main(int argc, char **argv){
    int remove=0;
    int longout=0;

    int importance=5;

    char opt;

    note_db_t db;
    note_t note;

    if(access("notes.csv", F_OK) != -1)
        db=load_db("notes.csv");
    else
        db=empty_db();

    while((opt=getopt(argc, argv, "vlri:")) != -1){
        switch(opt){
            case 'v':
                printf("txt v%s\n", VERSION);
                exit(0);
            case 'l':
                longout=1;
                break;
            case 'r':
                remove=1;
                break;
            case 'i':
                importance=atoi(optarg);
                if(importance<1 || importance>10){
                    puts("Bad importance specified, must be integer between" \
                         "1 and 10 (inclusive)");
                    exit(9);
                }
                break;
        }
    }

    if(remove){
        int i;
        for(i=optind;i<argc;i++){
            int id=atoi_altfail(argv[i]);
            if(id==-1){
                printf("Bad ID '%s' specified, ignoring", argv[i]);
                continue;
            }
            del_note_id(&db, id);
        }
    }else if(argc==optind){
        int i;
        for(i=0;i<db.len;i++){
            note=*get_note(&db, i);
            if(note.importance>7)
                //ANSI red and bright
                printf("\x1b[31m\x1b[1m");
            if(longout){
                printf("%2d\t%d\t%d\t%s\n",
                        note.id, note.importance,
                        note.created, note.text);
            }else{
                printf("%2d %s\n", note.id, note.text);
            }
            //Disable all ANSI
            printf("\x1b[0m");
        }
    }else{
        char text[TEXT_MAXLEN]="";
        int i;
        for(i=optind;i<argc;i++){
            strncat(text, argv[i], TEXT_MAXLEN-strlen(text));
            strncat(text, " ", TEXT_MAXLEN-strlen(text));
        }
        note=make_note(text);
        note.importance=importance;
        add_note(&db, note);
    }

    save_db(&db, "notes.csv");
    free_db(&db);

    return 0;
}

//Returns -1 instead of 0 on failure, which is easier to check for
int atoi_altfail(const char *str){
    int val;
    val=atoi(str);
    if(val==0){
        if(str[0]=='0' && str[1]=='\0'){
            return 0;
        }else{
            return -1;
        }
    }
    return val;
}
