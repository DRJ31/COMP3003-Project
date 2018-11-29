#ifndef JSON_JSON_H
#define JSON_JSON_H

#include "data.h"
#include "parson.h"

char* person_to_json_string (JSON_Value *root_value, Person *person);

Person* json_string_to_person (char* json_string);

#endif //JSON_JSON_H
