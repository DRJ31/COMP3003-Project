#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "data.h"

#define BUFSIZE 1024

char query_info[BUFSIZE] = "SELECT * FROM contact WHERE"
                           "name LIKE %%%s%%"
                           "mobile LIKE %%%s%%"
                           "fax LIKE %%%s%%"
                           "note LIKE %%%s%%";

// Initialize memory string array
char **alloc_mem(const int *size)
{
    char **pt;
    pt = (char **)malloc(sizeof(char *) * size[0]);
    for (int i = 0; i < size[0]; i++) {
        pt[i] = (char *)malloc(sizeof(char) * size[1]);
        memset(pt[i], 0, size[1] * sizeof(char));
    }
    return pt;
}

// Initialize memory of attributes for person
Person *init_person(int size[4][2])
{
    Person *person = (Person *)malloc(sizeof(Person));
    person->name = alloc_mem(size[0]);
    person->mobile = alloc_mem(size[1]);
    person->fax = alloc_mem(size[2]);
    person->note = alloc_mem(size[3]);
    return person;
}

int connect_mysql(MYSQL *mysql)
{
    if (!mysql_init(mysql)) {
        fprintf(stderr, "Mysql init error!\n");
        return -1;
    }

    if (!mysql_real_connect(mysql, "localhost", "demonist", "APTX4869", "phonebook", 0, NULL, 0)) {
        fprintf(stderr, "Mysql connection error!\n");
        return -1;
    }
    puts("Connection established");
    return 0;
}

Person *get_person(char *query)
{
    Person *person;
    MYSQL *mysql = (MYSQL *)malloc(sizeof(MYSQL));
    int size[4][2];

    if (connect_mysql(mysql) < 0) {
        return NULL;
    }


    for (int i = 0; i < 4; i++) {
        size[i][0] = 2;
        size[i][1] = 20;
    }
    person = init_person(size);
    strcpy(person->name[0], query);
    return person;
}

int main(void) {
    Person *person = get_person("Ricky");
    puts(person->name[0]);
    return 0;
}
