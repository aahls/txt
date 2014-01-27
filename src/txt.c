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

#include "txt.h"
#include "db.h"

int main(int argc, char **argv){
    int remove=0;
    int longout=0;

    int tag_add=0;
    int tag_del=0;
    char *tag_filter=NULL;

    int importance=5;

    enum db which_db=DB_STD;

    int invert_order=0;
    enum sort_policy sort=ID;

    char opt;

    note_db_t db;

    while((opt=getopt(argc, argv, "vlri:Is:ARt:GD")) != -1){
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
            case 't':
                if(optarg[0]=='#'){
                    tag_filter=optarg+1;
                }else{
                    printf("Invalid tag %s (must be specified with a # sign).", optarg);
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
            case 'A':
                tag_add=1;
                break;
            case 'R':
                tag_del=1;
                break;
            case 'G':
                which_db=DB_GLOBAL;
                break;
            case 'D':
                which_db=DB_DIR;
                break;
        }
    }
    if(remove+tag_add+tag_del>1){
        puts("Options -r, -A and/or -R can not be used simultaneously.");
        exit(9);
    }

    db=get_db(which_db);

    if(remove){
        mode_remove(argc, argv, &db);
        save_db(&db, NOTESFILE);
    }else if(tag_add || tag_del){
        mode_tag_edit(argc, argv, &db, tag_del);
        save_db(&db, NOTESFILE);
    }else if(argc==optind){
        mode_list(&db, longout, sort, invert_order, tag_filter);
    }else{
        mode_add(argc, argv, &db, importance);
        save_db(&db, NOTESFILE);
    }

    free_db(&db);

    return 0;
}

note_db_t get_db(enum db which_db){
    if((which_db==DB_STD && access(NOTESFILE, F_OK) == -1) || which_db==DB_GLOBAL)
        chdir(getenv("HOME"));
    if(access(NOTESFILE, F_OK) != -1)
        return load_db(NOTESFILE);
    else
        return empty_db();
}

void mode_remove(int argc, char **argv, note_db_t *db){
    int i;
    for(i=optind;i<argc;i++){
        int id=atoi_altfail(argv[i]);
        if(id==-1 || id<0){
            printf("Bad ID '%s' specified. Ignoring\n", argv[i]);
            continue;
        }
        if(del_note_id(db, id)==-1){
            printf("No note with ID %d found. Ignoring.\n", id);
        }
    }
}

void mode_list(note_db_t *db, int longout, enum sort_policy sort, int invert_order, char *tag){
    int i;
    note_t note;
    sort_notes(db, sort);
    for(i=0;i<db->len;i++){
        int tag_i;
        if(invert_order) note=*get_note(db, db->len-1-i);
        else note=*get_note(db, i);

        if(tag!=NULL && !has_tag(&note, tag)) continue;

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
}

void mode_add(int argc, char **argv, note_db_t *db, int importance){
    int i, has_text=0;
    note_t note=make_note("");
    for(i=optind;i<argc;i++){
        if(argv[i][0]=='#'){
            //Increment pointer to skip past hash
            add_tag(&note, ++argv[i]);
        }else{
            append_note_text(&note, argv[i]);
            append_note_text(&note, " ");
            has_text=1;
        }
    }
    if(!has_text){
        puts("Can't create note without text.");
        exit(9);
    }
    note.importance=importance;
    add_note(db, note);
}

void mode_tag_edit(int argc, char **argv, note_db_t *db, int remove){
    int i, tag_i=0, id_i=0;
    char **tags=(char **) malloc(sizeof(char *)*(argc-optind));
    int *ids=(int *) malloc(sizeof(int *)*(argc-optind));
    for(i=optind;i<argc;i++){
        if(argv[i][0]=='#'){
            //Point to characters just after the hash
            tags[tag_i]=argv[i]+1;
            tag_i++;
        }else if((ids[id_i]=atoi_altfail(argv[i]))!=-1){
            id_i++;
        }else{
            printf("%s is neither a tag (specified with a # sign) or an ID. Ignoring.\n", argv[i]);
        }
    }

    for(i=0;i<id_i;i++){
        int j;
        note_t *note=get_note_id(db, ids[i]);
        for(j=0;j<tag_i;j++){
            if(remove) del_tag(note, tags[j]);
            else add_tag(note, tags[j]);
        }
    }

    free(tags);
    free(ids);
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
