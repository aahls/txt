/*
Copyright (c) 2014 Anders Ahlstr�m

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

/*
  Save a note database as a CSV file, implements related prototypes in db.h
  Actually uses tabs, not commas

  Does no error checking. Input is assumed to be correct and file permissions
  are assumed to be okay. If this is not true, things *will* go wrong!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db.h"

//TODO: Make sure bad input files fail gracefully
note_db_t load_db(const char *filename){
    FILE* fp;
    note_db_t db=empty_db();
    char c;
    int i=0;
    int buffsize=1024;
    char *buffer=(char *)malloc(sizeof(char)*buffsize);
    //Store the address of the buffer seperately as it will be modified
    char *bufferaddr=buffer;
    fp=fopen(filename, "r");
    while((c=fgetc(fp))!=EOF){
        buffer[i]=c;
        i++;
        if(i==buffsize-1){
            buffsize*=2;
            buffer=(char *)realloc(buffer, sizeof(char)*buffsize);
        }
    }
    buffer[i]='\0';
    while(1){
        note_t note;
        char *text=strsep(&buffer, "\t");
        if(buffer==NULL)
            break;
        note=make_note("");
        set_note_text(&note, text);
        note.importance=atoi(strsep(&buffer, "\t"));
        note.created=atoi(strsep(&buffer, "\n"));
        //Will automatically generate an ID
        //IDs are not preseved between runs since this is more practical
        note.id=-1;
        add_note(&db, note);
    }
    free(bufferaddr);
    fclose(fp);
    return db;
}

int save_db(note_db_t *db, const char *filename){
    int i;
    FILE *fp;
    fp=fopen(filename, "w");
    for(i=0;i<db->len;i++){
        fprintf(fp, "%s\t%d\t%d\n", db->notes[i].text, db->notes[i].importance, (int)db->notes[i].created);
    }
    fclose(fp);
    return 0;
}
