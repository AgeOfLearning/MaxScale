#include <parser.h>

struct parser_token_t* alloc_token();
struct parser_token_t* create_token(int type, char* val);
void free_token(struct parser_token_t*);
void free_all_tokens(struct parser_token_t*);
void link_tokens(struct parser_token_t** a, struct parser_token_t* b);
void fprint_all_tokens(FILE* dest,struct parser_token_t* head,int depth);

void begin_substring(struct parser_token_t** tok)
{
    struct parser_token_t* sub = alloc_token();
    sub->type = PARSER_SUBSTRING;
    sub->value.substring = alloc_token();
    sub->value.substring->parent = sub;
    if(tok && *tok)
    {
        sub->parent = (*tok)->parent;
        sub->head = (*tok)->head;
        sub->value.substring->parent->head = sub->head;
        (*tok)->next = sub;
    }
    else
    {
        sub->head = sub;
        sub->value.substring->parent->head = sub;
    }
    *tok = sub->value.substring;
}

void end_substring(struct parser_token_t** tok)
{
    struct parser_token_t* sub = *tok;
    if(sub && sub->parent)
        *tok = sub->parent;
}

struct parser_token_t* create_token(int type, char* val)
{
    struct parser_token_t* tok = alloc_token();
    char *ptr,*saved;

    if(tok)
    {
        tok->type = type;
        switch(type)
        {
        case PARSER_STRING:
        case PARSER_QUOTED_STRING:
        case PARSER_ABS_PATH:
            tok->value.stringval = strdup(val);
            break;
        case PARSER_INT:
            tok->value.intval = atoi(val);
            break;
        case PARSER_FLOAT:
            tok->value.floatval = atof(val);
            break;
        case PARSER_PAIR:
            ptr = strtok_r(val,"=",&saved);
            tok->value.pairval.key = strdup(ptr);
            ptr = strtok_r(NULL,"=",&saved);
            tok->value.pairval.value = strdup(ptr);
            break;
        case PARSER_SUBSTRING:
            {
                struct parser_token_t* subtok;
                char* substr = strchr(val,'(');
                char* endptr = strrchr(substr,')');
                substr++;
                if(endptr == NULL)
                    endptr = strrchr(substr,'(');
                if(endptr)
                    *endptr = '\0';
                tokenize_string(&subtok,substr,strlen(substr));
                tok->value.substring = subtok;
                
            }
            break;
        default:
            break;
        }
    }
    return tok;   
}

struct parser_token_t* alloc_token()
{
    struct parser_token_t* tok;
    if((tok = malloc(sizeof(struct parser_token_t))) != NULL)
    {
        tok->next = NULL;
        tok->parent = NULL;
        tok->head = NULL;
        tok->type = PARSER_UNDEFINED;
    }
    return tok;
}

void free_token(struct parser_token_t* tok)
{
    if(tok)
    {
        switch(tok->type)
        {
        case PARSER_STRING:
        case PARSER_QUOTED_STRING:
        case PARSER_ABS_PATH:
            free(tok->value.stringval);
            break;
        case PARSER_PAIR:
            free(tok->value.pairval.key);
            free(tok->value.pairval.value);
            break;
        case PARSER_SUBSTRING:
            free_all_tokens(tok->value.substring);
            break;
        }
        free(tok);
    }
}

void free_all_tokens(struct parser_token_t* tok)
{
    struct parser_token_t *head = tok;
    struct parser_token_t *node;
    while(head)
    {
        node = head;
        head = head->next;
        free_token(node);
    }
}

void link_tokens(struct parser_token_t** a, struct parser_token_t* b)
{
    if(*a == NULL)
    {
        b->head = b;
        *a = b;
        
    }
    else
    {
        b->head = (*a)->head;
        b->parent = (*a)->parent;
        if((*a)->type == PARSER_UNDEFINED)
        {
            /** This is an uninitialized substring head, copy the new values to it
             * and discard the new one*/
            (*a)->type = b->type;
            (*a)->value = b->value;
            free(b);
        }
        else
        {
            (*a)->next = b;
            *a = b;
        }
    }
}

/**
* Find a key-value pair from the parsed token string
*
* Search through the parsed chain of tokens and find the key-value pair that
* matches the given parameter. This function ignores the case in the key values.
* @param head The head of the parsed token string
* @param key The ke whose value should be returned
* @return the value associated with the key or NULL if the key wasn't found
*/
char*
parser_get_keyvalue(struct parser_token_t* head, char* key)
{
    struct parser_token_t* ptr;

    ptr = head;

    while(ptr)
    {
        if(ptr->type == PARSER_PAIR)
        {
            if(strcasecmp(ptr->value.pairval.key,key) == 0)
                return ptr->value.pairval.value;
        }
        ptr = ptr->next;
    }
    return NULL;
}

bool parser_has_string(struct parser_token_t* head, char* token)
{
        struct parser_token_t* ptr;

    ptr = head;

    while(ptr)
    {
        switch(ptr->type)
        {
        case PARSER_STRING:
            if(strcasecmp(ptr->value.stringval,token) == 0)
                return true;
            break;
        case PARSER_SUBSTRING:
            if(parser_has_string(ptr->value.substring,token))
                return true;
            break;
        }
        ptr = ptr->next;
    }
    return false;
}



void fprint_token(FILE* dest,struct parser_token_t* head, int depth)
{
    struct parser_token_t* ptr;
    int i;
    switch(head->type)
    {
    case PARSER_STRING:
    case PARSER_QUOTED_STRING:
    case PARSER_ABS_PATH:
        fprintf(dest,"%s ",head->value.stringval);
        break;
    case PARSER_INT:
        fprintf(dest,"%d ",head->value.intval);
        break;
    case PARSER_FLOAT:
        fprintf(dest,"%f ",head->value.floatval);
        break;
    case PARSER_PAIR:
        fprintf(dest,"%s=",head->value.pairval.key);
        fprintf(dest,"%s ",head->value.pairval.value);
        break;
    case PARSER_SUBSTRING:
        fprintf(dest,"(\n");
        for(i = 0;i<depth;i++)
            fprintf(dest,"    ");
        fprint_all_tokens(dest,head->value.substring,depth);
        fprintf(dest,"\n");
        for(i = 0;i<depth-1;i++)
            fprintf(dest,"    ");
        fprintf(dest,") ");
        break;
    default:
        fprintf(dest,"ERROR");        
        break;
    }
}


void fprint_all_tokens(FILE* dest,struct parser_token_t* head,int depth)
{
    struct parser_token_t* ptr = head;
    while(ptr)
    {
        fprint_token(dest,ptr,depth + 1);
        ptr = ptr->next;
    }
}


void print_all_tokens(FILE* dest,struct parser_token_t* head)
{
    fprint_all_tokens(dest,head,0);
}
