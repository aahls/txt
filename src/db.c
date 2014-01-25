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
#include <time.h>

#include "db.h"

static void free_note(note_t *note);

note_t make_note(const char *text){
    note_t note;
    note.text=malloc(1);
    set_note_text(&note, text);
    note.importance=5;
    note.id=-1; //This is later set to a valid value as it is added to a db
    note.created=time(0);
    note.ntags=0;
    note.tags=malloc(sizeof(char *));
    return note;
}

void set_note_text(note_t *note, const char *text){
    note->text=realloc(note->text, sizeof(char)*(strlen(text)+1));
    strcpy(note->text, text);
}

void append_note_text(note_t *note, const char *text){
    note->text=realloc(note->text, sizeof(char)*(strlen(text)+strlen(note->text)+2));
    strcat(note->text, text);

}

note_db_t empty_db(){
    note_db_t db;
    db.notes=(note_t *)malloc(sizeof(note_t)*16);
    db.len=0;
    db.allocated=16;
    return db;
}

void free_db(note_db_t *db){
    int i;
    for(i=0;i<db->len;i++){
        free_note(&(db->notes[i]));
    }
    db->len=0;
    db->allocated=0;
    free(db->notes);
}

void free_note(note_t *note){
    int i;
    free(note->text);
    for(i=0;i<note->ntags;i++){
        free(note->tags[i]);
    }
    free(note->tags);
}

int add_note(note_db_t *db, note_t note){
    int id=0;
    if(db->len==db->allocated){
        db->notes=(note_t *)realloc(db->notes, sizeof(note_t)*db->allocated*2);
        db->allocated*=2;
    }

    if(note.id==-1){
        while(get_note_id(db, id)!=NULL) id++;
        note.id=id;
    }
    
    db->notes[db->len]=note;
    db->len++;

    return 0;
}

int del_note(note_db_t *db, int i){
    if(i<0 || i>=db->len) return 1;

    free_note(&(db->notes[i]));

    memmove(&((db->notes)[i]), &((db->notes)[i+1]),
                sizeof(note_t)*(db->len-(i+1)));
    db->len--;

    return 0;
}

int del_note_id(note_db_t *db, int id){
    int i;
    for(i=0;i<db->len;i++){
        if(db->notes[i].id==id){
            del_note(db, i);
            return 0;
        }
    }

    return -1;
}

note_t* get_note(note_db_t *db, int i){
    if(i<db->len && i>=0)
        return &(db->notes[i]);
    else
        return NULL;
}

note_t* get_note_id(note_db_t *db, int id){
    int i;
    for(i=0;i<db->len;i++)
        if(db->notes[i].id==id) return &(db->notes[i]);

    return NULL;
}

void swap_notes(note_db_t *db, int a, int b){
    note_t temp=db->notes[a];
    db->notes[a]=db->notes[b];
    db->notes[b]=temp;
}

int sort_notes(note_db_t *db, enum sort_policy policy){
    int i, j;
    if(db->len<2) return 0;
    //For now, just use insertion sort
    for(i=1;i<db->len;i++){
        j=i;
        while(j>0){
            if(policy==IMPORTANCE && !(db->notes[j].importance<db->notes[j-1].importance)) break;
            else if(policy==CREATED && !(db->notes[j].created<db->notes[j-1].created)) break;
            else if(policy==ID && !(db->notes[j].id<db->notes[j-1].id)) break;
            swap_notes(db, j, j-1);
            j--;
        }
    }
    return 0;
}

int add_tag(note_t *note, char *tag){
    if(has_tag(note, tag)) return 1;
    note->tags=realloc(note->tags, sizeof(char *)*(note->ntags+1));
    note->tags[note->ntags]=(char *) malloc((strlen(tag)+1)*sizeof(char));
    strcpy(note->tags[note->ntags], tag);
    note->ntags++;
    return 0;
}

int del_tag(note_t *note, char *tag){
    int i;
    for(i=0;i<note->ntags;i++){
        if(!strcmp(note->tags[i], tag)){
            free(note->tags[i]);
            memmove(&(note->tags[i]), &(note->tags[i+1]),
                sizeof(note->tags[0])*(note->ntags-(i+1)));
            note->ntags--;
            return 1;
        }
    }
    return 0;
}

int has_tag(note_t *note, char *tag){
    int i;
    if(note->ntags==0) return 0;
    for(i=0;i<note->ntags;i++)
        if(!strcmp(note->tags[i], tag)) return 1;
    return 0;
}
