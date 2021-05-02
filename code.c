#include "art.h"
#include "linklist.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef struct _infix_iter_arg{
    linked_list_t *rst;
    char *infix;
} infix_iter_arg_t;

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

int contains(const char *str, const char *tok){
    return strstr(str, tok) != NULL;
}

int prefix_callback(void *data, const unsigned char *key, uint32_t key_len, void *value){
    linked_list_t *rst = (linked_list_t *)data;
    return list_push_value(rst, value);
}

int infix_callback(void *data, const unsigned char *key, uint32_t key_len, void *value) {
    infix_iter_arg_t *args = (infix_iter_arg_t *)data;
    if (contains((const char *)key, args->infix)) {
        return list_push_value(args->rst, value);
    }
    return 0;
}

linked_list_t *affix_search(art_tree *t, char *affix, int flag)
{
    linked_list_t *rst = NULL;
    //if art is empty return null
    if (t == NULL) {
        printf("ART is empty\n");
        return rst;
    }

    //return list of elements that match
    rst = list_create();

    infix_iter_arg_t data = {rst, affix};
    
    if(flag == 0)
    {
        //printf("%s\n", affix);
        art_iter_prefix(t, (const unsigned char *)affix, strlen(affix), prefix_callback, rst);
    }
    else
        art_iter(t, infix_callback, &data);

    return rst;
}

int main(int argc, char **argv)
{
    art_tree* t;

    int elements_in_tree = 0;

    int res = art_tree_init(&t);
    if(res != 0)
    {
        printf("Tree init failed");
        return 0;
    }

    clock_t begin_insert = clock();
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
        elements_in_tree++;

        //insert all possible suffixes of string
        /*
        for(int c = 1; c < strlen(buf); c++)
        {
            char *suffix;
            suffix = &buf[c];
            art_insert(&t, (unsigned char*)suffix, strlen(suffix), (void*)line);
            elements_in_tree++;
        }
        */

        //insert reversed
        char *reversed = strrev(buf);
        art_insert(&t, (unsigned char*)reversed, len, (void*)line);
        elements_in_tree++;

        line++;
    }

    fclose(f);

    clock_t end_insert= clock();
    double time_spent_insert = (double)(end_insert - begin_insert) / CLOCKS_PER_SEC;
    printf("The time it took to insert was %f\n", time_spent_insert);

    //get user input to search for different affixes
    printf("Format is prefix*/*infix*/*suffix\n");
    printf("There are %d elemenets in the tree\n", elements_in_tree);
    size_t mem_cons = get_mem_usage_by_all_arts();
    printf("The memory consumption is: %zu\n", mem_cons);
    /*
    for(;;)
    {
        char input[512];
        char affix[512];
        printf("Enter an affix to search for (0 to quit): ");
        scanf("%s", input);

        if(strcmp("0", input) == 0)
            break;

        if(input[0] == '*') //case of infix and suffix
        {
            strcpy(affix,&input[1]); //remove star at beginning

            if(affix[strlen(affix)-1] == '*') //case of infix
            {
                affix[strlen(affix)-1] = '\0'; //remove star at end
            }
            
            else //case of suffix
            {
                char *temp = strrev(affix); //reverse the input for affix
                strcpy(affix, temp);
            }
        }
        else if(input[strlen(input)-1] == '*') //case of prefix
        {
            strcpy(affix, input);
            affix[strlen(affix)-1] = '\0'; //remove star at end
        }
        else
        {
            printf("Incorrect format, try again\n");
        }

        if(affix[0] != '\0')
        {
            clock_t begin_query = clock();
            linked_list_t *search_results = affix_search(&t, affix, (void*)line);
            clock_t end_query = clock();
            double time_spent_query = (double)(end_query - begin_query) / CLOCKS_PER_SEC;
            size_t num = list_count(search_results);
            printf("The number of elements for %s is: %zu\n", input, num);
            printf("The time it took for that query was %f\n", time_spent_query);
        }
    }
    */

    char input[512];
    char affix[512];
    FILE *q = fopen("query.txt", "r");
    
    line = 1;
    int count = 0; 
    double prefix_sum = 0;
    double infix_sum = 0;
    double suffix_sum = 0;
    double total_sum = 0;

    while(fgets(input, sizeof input, q))
    {
        len = strlen(input);
        input[len-2] = '\0'; //-2 instead of -1 due to \n char
        int infix_flag = 0;
        
        if(strcmp("0", input) == 0)
            break;

        if(input[0] == '*') //case of infix and suffix
        {
            strcpy(affix,&input[1]); //remove star at beginning

            if(affix[strlen(affix)-1] == '*') //case of infix
            {
                affix[strlen(affix)-1] = '\0'; //remove star at end
                infix_flag = 1;
            }
            
            else //case of suffix
            {
                char *temp = strrev(affix); //reverse the input for affix
                strcpy(affix, temp);
            }
        }
        else if(input[strlen(input)-1] == '*') //case of prefix
        {
            strcpy(affix, input);
            affix[strlen(affix)-1] = '\0'; //remove star at end
        }
        else
        {
            printf("Incorrect format: %d. Try again\n", count);
        }

        if(affix[0] != '\0')
        {
            clock_t begin_query = clock();
            linked_list_t *search_results = affix_search(&t, affix, infix_flag);
            clock_t end_query = clock();
            double time_spent_query = (double)(end_query - begin_query) / CLOCKS_PER_SEC;
            size_t num = list_count(search_results);
            //printf("The number of elements for %s is: %zu\n", input, num);
            //printf("The time it took for that query was %f\n", time_spent_query);

            if(count < 333)
                prefix_sum = prefix_sum + time_spent_query;
            else if (count < 666)
                infix_sum = infix_sum + time_spent_query;
            else
                suffix_sum = suffix_sum + time_spent_query;
        }

        count++;
        line++;
    }

    fclose(q);

    printf("The prefix average query was: %0.8f\n", prefix_sum/333);
    printf("The infix average query was: %0.8f\n", infix_sum/333);
    printf("The suffix average query was: %0.8f\n", suffix_sum/333);
    printf("The total average query was: %0.8f\n", (prefix_sum+infix_sum+suffix_sum)/999);

    return 0;
}
