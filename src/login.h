#ifndef LOGIN_H
#define LOGIN_H

const char *get_username(void);
const char *get_password(void);
void set_username(const char *username);
void set_password(const char *password);

void pb_login(const char *username, const char *password);
void pb_logout(void);

#endif
