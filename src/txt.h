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

#pragma once

#include "db.h"

#define VERSION     "1.0.0" 
#define NOTESFILE   ".txtdb.csv"

enum db { DB_STD, DB_GLOBAL, DB_DIR };

int atoi_altfail(const char *str);
int hash_str(const char *str, int variation);
note_db_t get_db(enum db which_db);

void mode_remove(int argc, char **argv, note_db_t *db);
void mode_list(note_db_t *db, int longout, enum sort_policy sort, int invert_order, char *tag);
void mode_add(int argc, char **argv, note_db_t *db, int importance);
void mode_tag_edit(int argc, char **argv, note_db_t *db, int remove);
