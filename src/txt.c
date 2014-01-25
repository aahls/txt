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
#define NOTESFILE   ".txtdb.csv"

int atoi_altfail(const char *str);
int hash_str(const char *str, int variation);

int main(int argc, char **argv){
    int remove=0;
    int longout=0;

    int importance=5;

    int invert_order=0;
    enum sort_policy sort=ID;

    char opt;

    note_db_t db;
    note_t note;

    chdir(getenv("HOME"));
    if(access(NOTESFILE, F_OK) != -1)
        db=load_db(NOTESFILE);
    else
        db=empty_db();

    while((opt=getopt(argc, argv, "vlri:Is:")) != -1){
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
            case 's':
                switch(optarg[0]){
                    case 'c':
                        sort=CREATED;
                        break;
                    case 'i':
                        sort=IMPORTANCE;
                        break;
                    default:
                        puts("Unknown sorting policy. Ignoring.");
                        break;
                }
                break;
            case 'I':
                invert_order=1;
                break;
        }
    }

    if(remove){
        int i;
        for(i=optind;i<argc;i++){
            int id=atoi_altfail(argv[i]);
            if(id==-1 || id<0){
                printf("Bad ID '%s' specified. Ignoring\n", argv[i]);
                continue;
            }
            if(del_note_id(&db, id)==-1){
                printf("No note with ID %d found. Ignoring.\n", id);
            }
        }
        save_db(&db, NOTESFILE);
    }else if(argc==optind){
        int i;
        sort_notes(&db, sort);
        for(i=0;i<db.len;i++){
            int tag_i;
            if(invert_order) note=*get_note(&db, db.len-1-i);
            else note=*get_note(&db, i);

            if(note.importance>7)
                //ANSI red and bright
                printf("\x1b[31m\x1b[1m");
            if(longout){
                char date_string[20];
                struct tm *time=localtime(&note.created);
                strftime(date_string, 20, "%F %H:%M:%S",time);
                printf("%2d %d %s %s",
                        note.id, note.importance,
                        date_string, note.text);
            }else{
                printf("%2d %s", note.id, note.text);
            }
            //Disable all ANSI
            printf("\x1b[0m");
            for(tag_i=0;tag_i<note.ntags;tag_i++){
                int fg, bg;
                putchar(' ');

                fg=(hash_str(note.tags[tag_i], 22))%8;
                bg=(hash_str(note.tags[tag_i], 19))%8;
                if(fg==bg) bg=(bg==7 ? bg+1 : bg-1);

                //ANSI: set foreground and background colors
                printf("\x1b[%dm\x1b[%dm", 30+fg, 40+bg);
                printf("#%s", note.tags[tag_i]);
                printf("\x1b[0m"); //Disable all ANSI
            }
            putchar('\n');
        }
    }else{
        int i;
        note=make_note("");
        for(i=optind;i<argc;i++){
            if(argv[i][0]=='#'){
                //Increment pointer to skip past hash
                add_tag(&note, ++argv[i]);
            }else{
                append_note_text(&note, argv[i]);
                append_note_text(&note, " ");
            }
        }
        note.importance=importance;
        add_note(&db, note);
        save_db(&db, NOTESFILE);
    }

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

int hash_str(const char *str, int variation){
    int i, seed=256;
    seed*=variation;
    for(i=0;i<strlen(str);i++){
        seed*=str[i];
        seed+=str[i];
    }
    srand(seed);
    return rand()%256;
}
