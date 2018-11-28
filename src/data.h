#ifndef DATA_H
#define DATA_H

typedef struct {
    char **name;
    char **mobile;
    char **fax;
    char **note;
} Person;

// *query: The query sentence
// size[4]: The array to store number of each information
Person *get_person(char *query, int size[4]);

int insert_info(Person *person);

int check_pass(char *username, char *password);

int update_info(Person *person, char *name);

#endif
