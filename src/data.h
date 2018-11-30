#ifndef DATA_H
#define DATA_H

typedef struct {
    char **name;
    char **mobile;
    char **fax;
    char **note;
} Person;

/**
 * Get information of a person
 *
 * @param query [in] Keywords to get person information
 * @param size  [in] Array to store size of each category of Person
 * @return Structure Person of first entity found in database
 */
Person *get_person(char *query, int size[4]);

/**
 * Insert new person into database
 *
 * @param person [in] Structure Person of person you want to insert
 * @return Status code, 1->Success, 0->Failed
 */
int insert_info(Person *person);

/**
 * Check password of user
 *
 * @param username
 * @param password
 * @return Status code of authentication, 1->Success, 0->Failed
 */
int check_pass(char *username, char *password);

/**
 * Update information for person
 *
 * @param person [in] All the information of person to update
 * @param name   [in] One of person's name to locate him/her
 * @return Status code of update, 1->Success, 0->Failed
 */
int update_info(Person *person, char *name);

#endif
