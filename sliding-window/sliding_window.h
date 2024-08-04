#ifndef SLIDING_WINDOW_H
#define SLIDING_WINDOW_H

#define WINDOW_SIZE 5
#define FRAME_COUNT 20

typedef struct {
    int frame_no;
    int ack;
} Frame;

void sender();
void receiver();

#endif // SLIDING_WINDOW_H
