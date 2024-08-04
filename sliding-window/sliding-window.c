#include <stdio.h>
#include "sliding_window.h"

Frame window[WINDOW_SIZE];
Frame frames[FRAME_COUNT];

void sender() {
    int i, j;
    int sent = 0;
    int ack_received = 0;

    while (ack_received < FRAME_COUNT) {
        // Send frames in the window
        for (i = 0; i < WINDOW_SIZE && sent < FRAME_COUNT; i++) {
            window[i] = frames[sent];
            printf("Sender: Frame %d sent.\n", window[i].frame_no);
            sent++;
        }

        // Simulate acknowledgment reception
        for (j = 0; j < i; j++) {
            if (window[j].frame_no < FRAME_COUNT) {
                window[j].ack = 1; // Simulate ACK
                printf("Receiver: Acknowledgment for Frame %d received.\n", window[j].frame_no);
                ack_received++;
            }
        }

        // Slide window
        for (j = 0; j < WINDOW_SIZE - 1; j++) {
            window[j] = window[j + 1];
        }

        // Clear the last frame of the window
        window[WINDOW_SIZE - 1].frame_no = -1;
        window[WINDOW_SIZE - 1].ack = 0;
    }
}

void receiver() {
    // This function is simplified as acknowledgments are handled in sender()
}

int main() {
    // Initialize frames
    for (int i = 0; i < FRAME_COUNT; i++) {
        frames[i].frame_no = i;
        frames[i].ack = 0;
    }

    // Initialize sliding window
    for (int i = 0; i < WINDOW_SIZE; i++) {
        window[i].frame_no = -1;
        window[i].ack = 0;
    }

    // Start sender
    sender();

    return 0;
}
