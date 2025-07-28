#ifndef ACU_H
#define ACU_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "battery.h"
#include "fdcan.h"
#include "adc.h"
#include "math.h"
#include "stm32g4xx_hal_fdcan.h"

/*******************************************************************
* acu.h
BIG BOY. ACU typedef, functions that handle can queuing, and helper
functions to parse out different messages over CAN. 
********************************************************************/

/*******************************************************************
* Declarations and Type Definitions
********************************************************************/
typedef struct {
    uint8_t data[64];
    FDCAN_GlobalTypeDef* instance; // Options: FDCAN1, FDCAN2, FDCAN3
    uint32_t identifier; // CANID
    uint32_t length; // Bytes
} CAN_RX_message;

typedef struct {
    float em_current;
    float em_voltage;
    float energy;

    uint8_t min_temp;
    uint8_t max_temp;
    uint8_t num_sensors;

    uint8_t status; /* Map: 0: violation, 1: logging*/
    float temps[32]; // degree: celcius, all temps sent as unsigned bytes
    int32_t team_data[4]; // team data messages, probably not needed

} EM;

typedef struct {
    uint8_t id;
    float hv_system_voltage;

    uint8_t r_iso_status;
    uint8_t r_iso_meas_count; 
    uint8_t isolation_quality;
    uint8_t status_device_activity; // 0 - initialization, 1 - Normal Operation, 2 - Self Test 
    uint16_t r_iso_corrected;
    uint16_t status_warnings_alarms; // 11 bits long

    uint16_t r_iso_negative;
    uint16_t r_iso_positive;
    uint16_t r_iso_original;
    uint8_t iso_meas_count;
    
} IMD;

typedef struct {

    // Charger Data (ACU reads from RX)
    uint8_t chgr_status; /* Map:    Bit 0: Hardware Failure: 0-Normal, 1-Error
                                    Bit 1: OverTemp: 0-Normal, 1-Error
                                    Bit 2: Input Voltage: 0-Normal, 1-Wrong input voltage
                                    Bit 3: Starting State: 0-Correct, 1-Wrong polarity or NC
                                    Bit 4: Communication State: 0-Normal, 1-Timeout
                        */
    int16_t charger_output_voltage; // data from charger (via Rx)
    int16_t charger_output_current; // data from charger (via Rx)
} Charger;

typedef struct {
    Battery * bty;
    Charger * chgr;
    IMD * imd;
    EM * em;

    // Config Charge Parameters (ACU to sends via TX to Charger)
    float target_chg_voltage; 
    float target_chg_current;

    uint8_t chg_ctrl; // 0 : Start Charging, 1: Stop Charging => to be sent from ACU to others via Tx
    float acu_current; // current output of ACU

    // ADC Stuff
    // 0:ts_current, 1:ts_voltage, 2:sdc_volt_w, 3:sdc_volt_v, 4:voltage_12v,5:water_sense
    volatile float ts_voltage;  // ts_v
    volatile float ts_current;  // uc_ts_i
    volatile float sdc_volt_w;  // sdcw_w => AFTER ACU latch
    volatile float sdc_volt_v;  // sdcv_v => BEFORE ACU latch
    volatile float voltage_12v; // glv voltage
    volatile float water_sense; // uc_water

    // SOC
    float bat_voltage, bat_soc;
    
    // ACU-Status 3 ... not needed anymore
    // from GR24 => not sure if we still need to use
    uint32_t cur_LastHighTime;
    uint32_t lastChrgRecieveTime;

    // 0: AIR+ | 1: AIR- | 2: Precharge | 3: ACU_latch (1: Closed | 0: Open)
    uint8_t relay_state; 

    // ACU errors/warnings
    uint8_t acuErrCount; // cummulative error counts
    uint16_t acu_err_warns; // [ 0:OT, OV, UV, OC, UC, UV_20v, UV_GLV, 7:UV_SDC, 8:Precharge, 0, 0, 0, 0, 0, 0, 0]        

    // ACU PRECHARGE via TX
    uint8_t ts_active; // 0: shutdown, 1: go TS Active/Precharge
    
} ACU;

/// @brief good for parsing CAN messages
 typedef union {
    uint16_t u16; 
    int32_t i32; 
    int16_t i16;
    float flt;
    uint8_t byts[4];
} data_union;


/*******************************************************************
* Function Declarations
********************************************************************/

// Basic blocks
void acu_init(ACU * acu);
bool acu_check(ACU * acu, bool startup);

// Send CAN Messages
void dequeue(ACU* acu);
void enqueue(uint32_t id, FDCAN_GlobalTypeDef * which_can);

// Receive CAN Messages
void can_read_handler(ACU* acu);
void can_read(ACU * acu, FDCAN_GlobalTypeDef * which_can, uint32_t id, uint8_t * data);

// other data conversions
float magical_union_flt(uint8_t data[], uint8_t size, bool big_endian);
int32_t magical_union_i32(uint8_t data[], bool big_endian);
uint16_t magical_union_u16(uint8_t data[]);

// charger conversions
int16_t magical_union_chgr_rcv(uint8_t data[], bool weird);
void magical_union_chgr_send(uint8_t * buffer, float data);

// Print functions
void print_targets(ACU * acu);
void print_imd_data(ACU* acu);
void print_adc_data(ACU *acu);
void update_adc_data(ACU* acu);
void print_imd_err_warn(ACU* acu);
void print_charger_data(ACU* acu);
void print_errors_warning(ACU * acu);
float get_total_voltage(ACU* acu);
void print_relay_status(uint8_t relay);

// helpers
uint8_t fconstrain(float value);
void prepare_can_send(uint8_t offset, float source, uint8_t type);
float map(float x, float in_min, float in_max, float out_min, float out_max);

// SOC calculation functions
float calculate_acu_soc(ACU* acu);
float calculate_glv_soc(ACU* acu);

// GPIO functions
void sdc_reset();
void write_bms_ok(bool state);
void write_IRneg(bool state);
void write_IRpos(bool state);
void write_prechg(bool state);
void write_acu_latch(bool state);
void turn_of_hv_pump(bool state);
void update_relay_state(ACU* acu);


// Debug functions
void write_LED(bool state);
/*******************************************************************/

#endif