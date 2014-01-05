/*
Copyright (c) 2014 Anders Ahlström

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

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
                char date_string[20];
                struct tm *time=localtime(&note.created);
                strftime(date_string, 20, "%F %H:%M:%S",time);
                printf("%2d %d %s %s\n",
                        note.id, note.importance,
                        date_string, note.text);
            }else{
                printf("%2d %s\n", note.id, note.text);
            }
            //Disable all ANSI
            printf("\x1b[0m");
        }
    }else{
        int i;
        note=make_note("");
        for(i=optind;i<argc;i++){
            append_note_text(&note, argv[i]);
            append_note_text(&note, " ");
        }
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
