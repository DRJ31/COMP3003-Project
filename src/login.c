#include <glib.h>
#include <stdbool.h>

#define BUFSIZE 128

bool _is_logged_in = false;
char _username[BUFSIZE] = {0};
char _password[BUFSIZE] = {0};

bool is_logged_in(void)
{
  return _is_logged_in;
}

const char *get_username(void)
{
  if (_is_logged_in)
    return _username;
  else
    return NULL;
}

const char *get_password(void)
{
  if (_is_logged_in)
    return _password;
  else
    return NULL;
}

void set_username(const char *username)
{
  if (username)
    g_strlcpy(_username, username, BUFSIZE);
  else
    _username[0] = '\0';
}

void set_password(const char *password)
{
  if (password)
    g_strlcpy(_password, password, BUFSIZE);
  else
    _password[0] = '\0';
}

void pb_login(const char *username, const char *password)
{
  if (username)
    set_username(username);
  if (password)
    set_password(password);

  _is_logged_in = true;

  g_message("%s: logged in", __func__);
}

void pb_logout(void)
{
  _is_logged_in = false;

  g_message("%s: logged out", __func__);
}
