#ifndef CLIENT_H
#define CLIENT_H

#include "data.h"

/**
 * Perform a query from the client to the server.
 *
 * @param q [in] The string to query on the server.
 * @return A Person strunct recording the result of query. On failure, NULL is
 *         returned.
 */
Person *client_query(const char *q);

#endif
