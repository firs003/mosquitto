#define THREAD_SUCCESS (void *)0
#define THREAD_FAILURE (void *)-1

typedef struct client_args
{
    int argc;
    char **argv;
} CLIENT_ARGS, *PCLIENT_ARGS;
#define CLIENT_ARGS_LEN sizeof(CLIENT_ARGS)

extern void *client1_body(void *arg);
extern void *client2_body(void *arg);
