#include <string.h>

#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lvgl_port_display.h"
#include "msgIDs.h"
#include "CANdler.h"
#include "grIDs.h"
#include "steering.h"
#include "main.h"
#include "fdcan.h"
#include "stm32u5xx_hal.h"
#include "encoders.h"
#include "tim.h"
#include "gui.h"

volatile OutgoingData outgoingData = {0};
volatile IncomingData incomingData = {0};
volatile int isDataChanged = 0;

void updatedDataRecieved(void)
{
    outgoingData.steeringStatusMsg.CMEandTME = (globalEncoderPercentages.current << 4) | globalEncoderPercentages.torque;
    outgoingData.steeringStatusMsg.RMEandButtonMap = (globalEncoderPercentages.regen << 4) | ((outgoingData.steeringStatusMsg.RMEandButtonMap << 4) >> 4);
    incomingData.cellData[40].cellVoltage = 50; // FIXME For real car

    isDataChanged = 1;
    
    writeMessage(MSG_STEERING_STATUS, GR_ECU, (uint8_t*)&outgoingData.steeringStatusMsg, 2);

    // TODO: You have new data, update screen with stuff
}
