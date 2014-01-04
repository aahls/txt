#pragma once

#include <stdio.h>
#include <time.h>

#define TEXT_MAXLEN 1024

enum sort_policy { IMPORTANCE, CREATED };

struct note{
    int id;
    char text[TEXT_MAXLEN];
    int importance;
    time_t created;
};
typedef struct note     note_t;

struct note_db{
    note_t *notes;
    int len;
    int allocated;
};
typedef struct note_db  note_db_t;

note_t make_note(const char *text);

note_t* get_note(note_db_t *db, int i);
note_t* get_note_id(note_db_t *db, int id);

note_db_t empty_db(void);
note_db_t load_db(const char *filename);
int save_db(note_db_t *db, const char *filename);
//Most likely won't be needed since a single db will likely last the whole run
void free_db(note_db_t *db);

int add_note(note_db_t *db, note_t note);
int del_note(note_db_t *db, int i);
int del_note_id(note_db_t *db, int id);

int sort_notes(note_db_t db, enum sort_policy policy, char inverted);
