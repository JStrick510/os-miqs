#include "art.h"
#include "linklist.h"
#include <stdio.h>
#include <string.h>

char *strrev(char *str)
{
      char *p1, *p2;

      if (! str || ! *str)
            return str;
      for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
      {
            *p1 ^= *p2;
            *p2 ^= *p1;
            *p1 ^= *p2;
      }
      return str;
}

int prefix_callback(void *data, const unsigned char *key, uint32_t key_len, void *value){
    linked_list_t *rst = (linked_list_t *)data;
    return list_push_value(rst, value);
}

linked_list_t *affix_search(art_tree *t, char *affix, void *data)
{
    linked_list_t *rst = NULL;
    //if art is empty return null
    if (t == NULL) {
        printf("ART is empty\n");
        return rst;
    }

    //return list of elements that match
    rst = list_create();
    art_iter_prefix(t, (const unsigned char *)affix, strlen(affix), prefix_callback, rst);
    return rst;
}

int main(int argc, char **argv)
{
    art_tree* t;

    int res = art_tree_init(&t);
    if(res != 0)
    {
        printf("Tree init failed");
        return 0;
    }

    int len;
    char buf[512];
    FILE *f = fopen("words.txt", "r");
    
    uintptr_t line = 1;
    while(fgets(buf, sizeof buf, f))
    {
        len = strlen(buf);
        buf[len-1] = '\0';
        
        //input normally
        art_insert(&t, (unsigned char*)buf, len, (void*)line);

        //insert all possible suffixes of string
        for(int c = 1; c < strlen(buf); c++)
        {
            char *suffix;
            suffix = &buf[c];
            art_insert(&t, (unsigned char*)suffix, strlen(suffix), (void*)line);
        }

        //insert reversed
        char *reversed = strrev(buf);
        art_insert(&t, (unsigned char*)reversed, len, (void*)line);

        line++;
    }

    char *affix = "ni";

    linked_list_t *test = affix_search(&t, affix, (void*)line);
    size_t num = list_count(test);
    printf("The number of elements for %s is: %zu\n", affix, num);

    //search through the tree and print the results
    /*
    // Seek back to the start
    fseek(f, 0, SEEK_SET);

    // Search for each line
    line = 1;
    while (fgets(buf, sizeof buf, f)) {
        len = strlen(buf);
        buf[len-1] = '\0';

        //search for normal inputs
        uintptr_t val = (uintptr_t)art_search(&t, (unsigned char*)buf, len);
        printf("Line: %d Val: %lu Str: %s\n", line, val, buf);

        //search for suffixes
        for(int d = 1; d < strlen(buf); d++)
        {
            char *suffix;
            suffix = &buf[d];
            uintptr_t val = (uintptr_t)art_search(&t, (unsigned char*)suffix, strlen(suffix));
            printf("Line: %d Val: %lu Str: %s\n", line, val, suffix);
        }

        //search for reversed
        char *reversed = strrev(buf);
        val = (uintptr_t)art_search(&t, (unsigned char*)reversed, len);
        printf("Line: %d Val: %lu Str: %s\n", line, val, reversed);

        line++;
    }
    */


    fclose(f);

    return 0;
}