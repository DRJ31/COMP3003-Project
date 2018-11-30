#ifndef JSON_JSON_H
#define JSON_JSON_H

#include "data.h"
#include "parson.h"
/**
 * Convert Person struct into json string
 *
 * @param root_value [in] Empty json object to store person information
 * @param person     [in] Person information in
 * @return Json string of person information
 */
char* person_to_json_string (JSON_Value *root_value, Person *person);

/**
 * Convert json string into person struct
 *
 * @param json_string [in] Json pattern of person information
 * @return A structure Person which contain all the information in json
 */
Person* json_string_to_person (char* json_string);

#endif //JSON_JSON_H
