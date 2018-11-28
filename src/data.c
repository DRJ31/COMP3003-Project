#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "data.h"

#define BUFSIZE 1024
#define TRUE    1
#define FALSE   0

char query_fmt[BUFSIZE] = "SELECT * FROM contact WHERE "
                           "name LIKE '%%%s%%' OR "
                           "mobile LIKE '%%%s%%' OR "
                           "fax LIKE '%%%s%%' OR "
                           "note LIKE '%%%s%%'";

char insert_fmt[BUFSIZE] = "INSERT INTO contact "
                           "(name, mobile, fax, note) VALUES "
                           "('%s', '%s', '%s', '%s')";

char pass_fmt[BUFSIZE] = "SELECT * FROM users WHERE "
                         "username='%s' AND password=PASSWORD('%s')";

char update_fmt[BUFSIZE] = "UPDATE contact SET name='%s',mobile='%s',"
                           "fax='%s',note='%s' WHERE name LIKE '%%%s%%'";
                         
// Initialize memory string array
char **alloc_mem(int size)
{
    char **pt;
    pt = (char **)malloc(sizeof(char *) * size);
    for (int i = 0; i < size; i++) {
        pt[i] = (char *)malloc(sizeof(char) * BUFSIZE);
        memset(pt[i], 0, BUFSIZE * sizeof(char));
    }
    return pt;
}

// Initialize memory of attributes for person
Person *init_person(int size[4])
{
    Person *person = (Person *)malloc(sizeof(Person));
    person->name = alloc_mem(size[0] + 1);
    person->mobile = alloc_mem(size[1] + 1);
    person->fax = alloc_mem(size[2] + 1);
    person->note = alloc_mem(size[3] + 1);
    return person;
}

// Get the length of string array
int get_arr_size(char *str) {
    int size = 1, str_len = strlen(str);
    for (int i = 0; i < str_len; i++) {
        if (str[i] == ',') {
            size++;
        }
    }
    return size;
}

int connect_mysql(MYSQL *mysql)
{
    if (!mysql_init(mysql)) {
        fprintf(stderr, "Mysql init error!\n");
        return FALSE;
    }

    if (!mysql_real_connect(mysql, "localhost", "demonist", "APTX4869", "phonebook", 0, NULL, 0)) {
        fprintf(stderr, "Mysql connection error!\n");
        return FALSE;
    }
    puts("Connection established");
    return TRUE;
}

// Decompose string into string array
char **str_decomposer(char *str, int *arr_len) 
{
    int index = 0, count = 0, start = 0;
    int str_len = strlen(str);
    char **str_arr;
    *arr_len = get_arr_size(str);
    str_arr = alloc_mem(*arr_len + 1);

    for (int i = 0; i < str_len; i++) {
        if (str[i] == ',') {
            strncpy(str_arr[index], str + start, count);
            str_arr[index][i] = '\0';
            index++;
            start = i + 1;
            count = 0;
            continue;
        }
        count++;
    }
    strncpy(str_arr[index], str + start, count);
    str_arr[++index] = NULL;
    return str_arr;
}

// Compose string array into single string
char *str_composer(char **arr)
{
    char *str = (char *)malloc(sizeof(char) * BUFSIZE);
    int i = 0;
    while (arr[i] != NULL) {
        strcat(str, arr[i]);
        if (arr[i + 1] != NULL) {
            strcat(str, ",");
        }
        i++;
    }
    return str;
}

// Get information of the person
Person *get_person(char *query, int size[4])
{
    Person *person = (Person *)malloc(sizeof(Person));
    int status;
    MYSQL *mysql = (MYSQL *)malloc(sizeof(MYSQL));
    MYSQL_RES *res;
    MYSQL_ROW row;
    char find_query[BUFSIZE];
    sprintf(find_query, query_fmt, query, query, query, query);

    if (!connect_mysql(mysql)) {
        return NULL;
    }

    status = mysql_real_query(mysql, find_query, strlen(find_query));
    if (status) {
        fprintf(stderr, "Failed to query: %s\n", mysql_error(mysql));
        return NULL;
    }
    puts("Query success");

    res = mysql_store_result(mysql);
    if (mysql_num_rows(res) == 0) {
        return NULL;
    }
    row = mysql_fetch_row(res);
    person->name = str_decomposer(row[1], &size[0]);
    person->mobile = str_decomposer(row[2], &size[1]);
    person->fax = str_decomposer(row[3], &size[2]);
    person->note = str_decomposer(row[4], &size[3]);
    mysql_close(mysql);
    return person;
}

// Insert information into database
int insert_info(Person *person)
{
    char *name = str_composer(person->name);
    char *mobile = str_composer(person->mobile);
    char *fax = str_composer(person->fax);
    char *note = str_composer(person->note);
    MYSQL *mysql = (MYSQL *)malloc(sizeof(MYSQL));
    char insert_query[BUFSIZE];
    int status;
    sprintf(insert_query, insert_fmt, name, mobile, fax, note);

    if (!connect_mysql(mysql)) {
        return FALSE;
    }

    status = mysql_real_query(mysql, insert_query, strlen(insert_query));
    if (status) {
        fprintf(stderr, "Failed to insert: %s\n", mysql_error(mysql));
        return FALSE;
    }
    puts("Insert sucess");
    return TRUE;
}

int check_pass(char *username, char *password)
{
    MYSQL *mysql = (MYSQL *)malloc(sizeof(MYSQL));
    MYSQL_RES *res;
    MYSQL_ROW row;
    char check_query[BUFSIZE];
    int status;
    sprintf(check_query, pass_fmt, username, password);

    if (!connect_mysql(mysql)) {
        return FALSE;
    }

    status = mysql_real_query(mysql, check_query, strlen(check_query));
    if (status) {
        fprintf(stderr, "Failed to query: %s\n", mysql_error(mysql));
        return FALSE;
    }
    res = mysql_store_result(mysql);
    if (mysql_num_rows(res) == 0) {
        return FALSE;
    }
    return TRUE;
}

int update_info(Person *person, char *name)
{
    char *fullname = str_composer(person->name);
    char *mobile = str_composer(person->mobile);
    char *fax = str_composer(person->fax);
    char *note = str_composer(person->note);
    MYSQL *mysql = (MYSQL *)malloc(sizeof(MYSQL));
    char update_query[BUFSIZE];
    int status;
    sprintf(update_query, update_fmt, fullname, mobile, fax, note, name);

    if (!connect_mysql(mysql)) {
        return FALSE;
    }

    status = mysql_real_query(mysql, update_query, strlen(update_query));
    if (status) {
        fprintf(stderr, "Failed to update: %s\n", mysql_error(mysql));
        return FALSE;
    }
    puts("Update sucess");
    return TRUE;
}

int main(void)
{
    int size[4] = {2, 1, 1, 1};
    Person *person = init_person(size);
    person->name[0] = "zh";
    person->name[1] = "zzzh";
    person->name[2] = NULL;
    person->mobile[0] = "12309132";
    person->fax[0] = "2139123123";
    person->note[0] = "Gay";
    person->mobile[1] = NULL;
    person->fax[1] = NULL;
    person->note[1] = NULL;
    int status = update_info(person, "zzz");
    printf("%d", status);
    return 0;
}