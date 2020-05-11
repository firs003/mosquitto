/*
Copyright (c) 2009-2019 Roger Light <roger@atchoo.org>

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License v1.0
and Eclipse Distribution License v1.0 which accompany this distribution.

The Eclipse Public License is available at
http://www.eclipse.org/legal/epl-v10.html
and the Eclipse Distribution License is available at
http://www.eclipse.org/org/documents/edl-v10.php.

Contributors:
Roger Light - initial implementation and documentation.
*/


#include <stdio.h>
#include <pthread.h>
#include <signal.h>

#include "client_thread.h"


int main(int argc, char *argv[])
{
    int rc;

#ifndef WIN32
    struct sigaction sigact;
#endif

#if 1
    int i;
    CLIENT_ARGS cargs[2] = {0};
    const char *cargv[][6] = {
        {argv[0], "-h", "192.168.0.200", "-t", "application/+/+/+/rx", "-v"},
        {argv[0], "-h", "192.168.0.200", "-t", "application/+/+/+/join", "-v"},
    };
    pthread_t tid[2] = {-1, -1};
    void *thread_ret = THREAD_FAILURE;

    // mosquitto_lib_init();

    cargs[0].argc = 6;
    cargs[0].argv = cargv[0];
    if (pthread_create(&tid[0], NULL, client1_body, (void *)&cargs[0]) == -1)
    {
        printf("pthread_create for tid[0] error\n");
    }

    cargs[1].argc = 6;
    cargs[1].argv = cargv[1];
    if (pthread_create(&tid[1], NULL, client2_body, (void *)&cargs[1]) == -1)
    {
        printf("pthread_create for tid[1] error\n");
    }

#ifndef WIN32
    // sigact.sa_handler = my_signal_handler;
    // sigemptyset(&sigact.sa_mask);
    // sigact.sa_flags = 0;

    // if(sigaction(SIGALRM, &sigact, NULL) == -1){
    //     perror("sigaction");
    //     goto cleanup;
    // }

    // if(cfg.timeout){
    //     alarm(cfg.timeout);
    // }
#endif

    for (i = 0; i < 2; i++)
    {
        if (tid[i] > 0)
        {
            pthread_join(tid[i], &thread_ret);
            printf("thread[%d](%lu) return %s\n", i, tid[i], thread_ret? "failure": "success");
        }

    }

    return 0;
#else
    mosquitto_lib_init();

    rc = client_config_load(&cfg, CLIENT_SUB, argc, argv);
    if(rc){
        if(rc == 2){
            /* --help */
            print_usage();
        }else{
            fprintf(stderr, "\nUse 'mosquitto_sub --help' to see usage.\n");
        }
        goto cleanup;
    }

    if(cfg.no_retain && cfg.retained_only){
        fprintf(stderr, "\nError: Combining '-R' and '--retained-only' makes no sense.\n");
        goto cleanup;
    }

    if(client_id_generate(&cfg)){
        goto cleanup;
    }

    mosq = mosquitto_new(cfg.id, cfg.clean_session, &cfg);
    if(!mosq){
        switch(errno){
            case ENOMEM:
                err_printf(&cfg, "Error: Out of memory.\n");
                break;
            case EINVAL:
                err_printf(&cfg, "Error: Invalid id and/or clean_session.\n");
                break;
        }
        goto cleanup;
    }
    if(client_opts_set(mosq, &cfg)){
        goto cleanup;
    }
    if(cfg.debug){
        mosquitto_log_callback_set(mosq, my_log_callback);
        mosquitto_subscribe_callback_set(mosq, my_subscribe_callback);
    }
    mosquitto_connect_v5_callback_set(mosq, my_connect_callback);
    mosquitto_message_v5_callback_set(mosq, my_message_callback);

    rc = client_connect(mosq, &cfg);
    if(rc){
        goto cleanup;
    }

#ifndef WIN32
    sigact.sa_handler = my_signal_handler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;

    if(sigaction(SIGALRM, &sigact, NULL) == -1){
        perror("sigaction");
        goto cleanup;
    }

    if(cfg.timeout){
        alarm(cfg.timeout);
    }
#endif

    rc = mosquitto_loop_forever(mosq, -1, 1);

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    if(cfg.msg_count>0 && rc == MOSQ_ERR_NO_CONN){
        rc = 0;
    }
    client_config_cleanup(&cfg);
    if(rc){
        err_printf(&cfg, "Error: %s\n", mosquitto_strerror(rc));
    }
    return rc;

cleanup:
    mosquitto_lib_cleanup();
    client_config_cleanup(&cfg);
    return 1;
#endif
}

