#include <stdio.h>

#ifndef JSON_JSON_H
#define JSON_JSON_H
typedef struct {
    char **name;
    char **mobile;
    char **fax;
    char **note;
} Person;

char **size_t_alloc_mem(size_t size);
void field_to_json_array(JSON_Object *root_object, char* field_name, char ** struct_item);
char* person_to_json_string (JSON_Value *root_value, Person *person);
Person* json_string_to_person (char* json_string);

#endif //JSON_JSON_H
