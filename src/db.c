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

note_t make_note(const char *text){
    note_t note;
    note.text=malloc(1);
    set_note_text(&note, text);
    note.importance=5;
    note.id=-1; //This is later set to a valid value as it is added to a db
    note.created=time(0);
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
        free(db->notes[i].text);
    }
    db->len=0;
    db->allocated=0;
    free(db->notes);
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

    free(db->notes[i].text);

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
