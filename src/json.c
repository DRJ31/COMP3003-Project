#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parson.h"

#define BUFSIZE 1024
#define TRUE    1
#define FALSE   0

typedef struct {
    char **name;
    char **mobile;
    char **fax;
    char **note;
} Person;

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

char **alloc_mem(int size)
{
    char **pt;
    pt = (char **)malloc(sizeof(char *) * size);
    for (int i = 0; i <size; i++) {
        pt[i] = (char *)malloc(sizeof(char) * BUFSIZE);
        memset(pt[i], 0, BUFSIZE * sizeof(char));
    }
    return pt;
}

void field_to_json_array(JSON_Object *root_object, char* field_name, char ** struct_item){
    int i = 0;
    while (struct_item[i] != NULL){
        if (i == 0){
            json_object_dotset_value(root_object, field_name, json_parse_string("[\"\"]"));
        }
        JSON_Array *field;
        field = json_object_get_array(root_object, field_name);
        if (i == 0){
            json_array_remove(field, 0);
        }
        json_array_append_string(field, struct_item[i]);
        i++;
    }
}

char* person_to_json_string (JSON_Value *root_value, Person *person) {
    JSON_Object *root_object = json_value_get_object(root_value);
    field_to_json_array(root_object, "name", person->name);
    field_to_json_array(root_object, "mobile", person->mobile);
    field_to_json_array(root_object, "fax", person->fax);
    field_to_json_array(root_object, "note", person->note);
    return json_serialize_to_string_pretty(root_value);
}

// int main(){
//     char *name_1= "Ricky Hou";
//     char *mobile_1 = "+8613000000000";
//     char *mobile_2= "+8613666666666";
//     char *fax_1= "12345678";
//     char *fax_2= "87654321";
//     char *note_1= "Hahah, this is a note~";


//     Person *person = (Person *)malloc(sizeof(Person));
//     char **name_str_arr;
//     name_str_arr = alloc_mem(1 + 1);
//     name_str_arr[0] = name_1;
//     name_str_arr[1] = NULL;
//     person->name = name_str_arr;

//     char **mobile_str_arr;
//     mobile_str_arr = alloc_mem(2 + 1);
//     mobile_str_arr[0] = mobile_1;
//     mobile_str_arr[1] = mobile_2;
//     mobile_str_arr[2] = NULL;
//     person->mobile = mobile_str_arr;

//     char **fax_str_arr;
//     fax_str_arr = alloc_mem(2 + 1);
//     fax_str_arr[0] = fax_1;
//     fax_str_arr[1] = fax_2;
//     fax_str_arr[2] = NULL;
//     person->fax = fax_str_arr;

//     char **note_str_arr;
//     note_str_arr = alloc_mem(1 + 1);
//     note_str_arr[0] = note_1;
//     note_str_arr[1] = NULL;
//     person->note = note_str_arr;


//     JSON_Value *root_value = json_value_init_object();

//     printf("%s\n", person_to_json_string(root_value, person));
//     json_value_free(root_value);
//     return 0;
// }
