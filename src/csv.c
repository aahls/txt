//Save a note database as a CSV file, implements related prototypes in db.h
//Actually uses tabs, not commas

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db.h"

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
        strncpy(note.text, text, TEXT_MAXLEN);
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
