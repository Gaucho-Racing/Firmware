#include <stdint.h>

#ifndef ENCODERS_H
    #define ENCODERS_H

    typedef enum {
        ENC_CURRENT,
        ENC_TORQUE,
        ENC_REGEN,
    } Encoder;

    typedef struct {
        uint8_t current;
        uint8_t torque;
        uint8_t regen;
    } EncoderPercentages;

    extern volatile EncoderPercentages globalEncoderPercentages;

    void pollEncoderStatus(Encoder encoder);
    void pollButtonStatus(void);
#endif
