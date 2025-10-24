/**
 * \file models/shadow/stdc/isxdigit.c
 *
 * \brief Shadow version of standard library isxdigit.
 *
 * \copyright 2025 Justin Handville.  Please see license.txt in this
 * distribution for the license terms under which this software is distributed.
 */

int isxdigit(int c)
{
    if (c >= '0' && c <= '9')
        return 1;

    if (c >= 'A' && c <= 'F')
        return 1;

    if (c >= 'a' && c <= 'f')
        return 1;

    return 0;
}
