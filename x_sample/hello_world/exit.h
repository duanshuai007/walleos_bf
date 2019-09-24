#ifndef _X_SAMPLE_HELLO_WORLD_EXIT_H_
#define _X_SAMPLE_HELLO_WORLD_EXIT_H_

#ifdef __cplusplus
extern "C" {
#endif

enum {
    EXIT_SUCCESS    = 0,
    EXIT_FAILURE    = 1,
};

void abort(void);
void exit(int status);

#ifdef __cplusplus
}
#endif

#endif /* __EXIT_H__ */
