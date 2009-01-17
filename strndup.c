#include <stdio.h>
#include <sys/types.h>

#undef __strndup
#undef strndup

#ifndef weak_alias
# define __strndup strndup
#endif

size_t
strnlen (const char *string, size_t maxlen)
{
  const char *end = (char*)memchr (string, '\0', maxlen);
  return end ? end - string : maxlen;
}

char *
__strndup (const char *s, size_t n)
{
  size_t len = strnlen (s, n);
  char *new = malloc (len + 1);

  if (new == NULL)
    return NULL;

  new[len] = '\0';
  return (char *) memcpy (new, s, len);
}
#ifdef weak_alias
weak_alias (__strndup, strndup)
#endif

