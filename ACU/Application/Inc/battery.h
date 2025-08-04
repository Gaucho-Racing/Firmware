
#ifndef BATTERY_H
#define BATTERY_H

#include "config.h"
#include "stm32g474xx.h"
#include "bcc_communication.h"
#include "string.h"
#include "debug.h"

typedef struct
{
    const uint8_t address;
    const uint16_t defaultVal;
    const float value;
} bcc_init_reg_t;

typedef struct
{
    float cell_temp[NUM_CELL_IC*REAL_NUM_TOTAL_IC*2];
    uint8_t cell_temp_err_cnt[NUM_CELL_IC*REAL_NUM_TOTAL_IC*2];
    float cell_volt[NUM_CELL_IC*REAL_NUM_TOTAL_IC];
    uint8_t cell_volt_err_cnt[NUM_CELL_IC*REAL_NUM_TOTAL_IC];
    uint8_t cell_balancing[NUM_CELL_IC*REAL_NUM_TOTAL_IC]; // 0 = off, 1 = on, 2 = disabled

    // Data we calculated from BCC measurements
    float min_cell_volt; // the cell with the min volt
    float max_cell_volt; // the cell with the max volt
    float min_cell_temp; // the cell with the min temp
    float max_cell_temp; // the cell with the max temp
    
    // stacks
    float battery_total_voltage;
    float stack_voltage[NUM_TOTAL_IC];
    float calculated_stack_voltage[NUM_TOTAL_IC];
    
    float icTemp[NUM_TOTAL_IC];

    float max_chg_current; // idk what the purpose of this one is, doesn't really get used

    // ACU Config Operational Parameters via Rx
    uint16_t min_volt_thresh;
    uint16_t max_temp_thresh;

    // Internal Data
    uint16_t max_volt_thresh;
    uint16_t min_temp_thresh;
    uint8_t cell_temp_errors;
    uint8_t cell_volt_errors;
    uint16_t faults[BCC_FS_MAX*NUM_TOTAL_IC];
    uint8_t battery_check_faults;


    // config
    bcc_drv_config_t drvConfig;

} Battery;

static const bcc_init_reg_t init_regs[INIT_REG_CNT] = {
    {MC33771C_GPIO_CFG1_OFFSET, MC33771C_GPIO_CFG1_POR_VAL, GPIO_CFG1},
    {MC33771C_GPIO_CFG2_OFFSET, MC33771C_GPIO_CFG2_POR_VAL, GPIO_CFG2},
    {MC33771C_TH_ALL_CT_OFFSET, MC33771C_TH_ALL_CT_POR_VAL, CELL_MAX_VOLT},
    {MC33771C_TH_CT14_OFFSET, MC33771C_TH_CT14_POR_VAL, CELL_MAX_VOLT},
    {MC33771C_TH_CT13_OFFSET, MC33771C_TH_CT13_POR_VAL, CELL_MAX_VOLT},
    {MC33771C_TH_CT12_OFFSET, MC33771C_TH_CT12_POR_VAL, CELL_MAX_VOLT},
    {MC33771C_TH_CT11_OFFSET, MC33771C_TH_CT11_POR_VAL, CELL_MAX_VOLT},
    {MC33771C_TH_CT10_OFFSET, MC33771C_TH_CT10_POR_VAL, CELL_MAX_VOLT},
    {MC33771C_TH_CT9_OFFSET, MC33771C_TH_CT9_POR_VAL, CELL_MAX_VOLT},
    {MC33771C_TH_CT8_OFFSET, MC33771C_TH_CT8_POR_VAL, CELL_MAX_VOLT},
    {MC33771C_TH_CT7_OFFSET, MC33771C_TH_CT7_POR_VAL, CELL_MAX_VOLT},
    {MC33771C_TH_CT6_OFFSET, MC33771C_TH_CT6_POR_VAL, CELL_MAX_VOLT},
    {MC33771C_TH_CT5_OFFSET, MC33771C_TH_CT5_POR_VAL, CELL_MAX_VOLT},
    {MC33771C_TH_CT4_OFFSET, MC33771C_TH_CT4_POR_VAL, CELL_MAX_VOLT},
    {MC33771C_TH_CT3_OFFSET, MC33771C_TH_CT3_POR_VAL, CELL_MAX_VOLT},
    {MC33771C_TH_CT2_OFFSET, MC33771C_TH_CT2_POR_VAL, CELL_MAX_VOLT},
    {MC33771C_TH_CT1_OFFSET, MC33771C_TH_CT1_POR_VAL, CELL_MAX_VOLT},
    {MC33771C_TH_AN6_OT_OFFSET, MC33771C_TH_AN6_OT_POR_VAL, CELL_MAX_TEMP},
    {MC33771C_TH_AN5_OT_OFFSET, MC33771C_TH_AN5_OT_POR_VAL, CELL_MAX_TEMP},
    {MC33771C_TH_AN4_OT_OFFSET, MC33771C_TH_AN4_OT_POR_VAL, CELL_MAX_TEMP},
    {MC33771C_TH_AN3_OT_OFFSET, MC33771C_TH_AN3_OT_POR_VAL, CELL_MAX_TEMP},
    {MC33771C_TH_AN2_OT_OFFSET, MC33771C_TH_AN2_OT_POR_VAL, CELL_MAX_TEMP},
    {MC33771C_TH_AN1_OT_OFFSET, MC33771C_TH_AN1_OT_POR_VAL, CELL_MAX_TEMP},
    {MC33771C_TH_AN0_OT_OFFSET, MC33771C_TH_AN0_OT_POR_VAL, CELL_MAX_TEMP},
    {MC33771C_TH_AN6_UT_OFFSET, MC33771C_TH_AN6_UT_POR_VAL, CELL_MAX_TEMP},
    {MC33771C_TH_AN5_UT_OFFSET, MC33771C_TH_AN5_UT_POR_VAL, CELL_MAX_TEMP},
    {MC33771C_TH_AN4_UT_OFFSET, MC33771C_TH_AN4_UT_POR_VAL, CELL_MAX_TEMP},
    {MC33771C_TH_AN3_UT_OFFSET, MC33771C_TH_AN3_UT_POR_VAL, CELL_MAX_TEMP},
    {MC33771C_TH_AN2_UT_OFFSET, MC33771C_TH_AN2_UT_POR_VAL, CELL_MAX_TEMP},
    {MC33771C_TH_AN1_UT_OFFSET, MC33771C_TH_AN1_UT_POR_VAL, CELL_MAX_TEMP},
    {MC33771C_TH_AN0_UT_OFFSET, MC33771C_TH_AN0_UT_POR_VAL, CELL_MAX_TEMP},
    {MC33771C_CB1_CFG_OFFSET, MC33771C_CB1_CFG_POR_VAL, CBX_SET},
    {MC33771C_CB2_CFG_OFFSET, MC33771C_CB2_CFG_POR_VAL, CBX_SET},
    {MC33771C_CB3_CFG_OFFSET, MC33771C_CB3_CFG_POR_VAL, CBX_SET},
    {MC33771C_CB4_CFG_OFFSET, MC33771C_CB4_CFG_POR_VAL, CBX_SET},
    {MC33771C_CB5_CFG_OFFSET, MC33771C_CB5_CFG_POR_VAL, CBX_SET},
    {MC33771C_CB6_CFG_OFFSET, MC33771C_CB6_CFG_POR_VAL, CBX_SET},
    {MC33771C_CB7_CFG_OFFSET, MC33771C_CB7_CFG_POR_VAL, CBX_SET},
    {MC33771C_CB8_CFG_OFFSET, MC33771C_CB8_CFG_POR_VAL, CBX_SET},
    {MC33771C_CB9_CFG_OFFSET, MC33771C_CB9_CFG_POR_VAL, CBX_SET},
    {MC33771C_CB10_CFG_OFFSET, MC33771C_CB10_CFG_POR_VAL, CBX_SET},
    {MC33771C_CB11_CFG_OFFSET, MC33771C_CB11_CFG_POR_VAL, CBX_SET},
    {MC33771C_CB12_CFG_OFFSET, MC33771C_CB12_CFG_POR_VAL, CBX_SET},
    {MC33771C_CB13_CFG_OFFSET, MC33771C_CB13_CFG_POR_VAL, CBX_SET},
    {MC33771C_CB14_CFG_OFFSET, MC33771C_CB14_CFG_POR_VAL, CBX_SET},
};

bcc_status_t read_device_measurements(Battery * bty, uint8_t read_volt, uint8_t read_temp);
bcc_status_t set_cell_balance(Battery * bty, bcc_cid_t cid, uint8_t cellIndex, bool all, bool enable);

bool disable_cell_balancing(Battery * bty);
bool init_cell_balancing(Battery * bty);
bool init_registers(Battery * bty);
float V2T(float voltage, float B);
void reset_discharge(Battery * bty, bool on);

bool check_volt(Battery *bty);
bool check_temp(Battery *bty);
bool do_cell_balancing(Battery * bty);
bool battery_check(Battery *bty, bool full_check);

void get_faults(Battery * bty);
void clear_faults(bcc_drv_config_t * drvConfig);

// print individuals
void print_volt(const float * voltages, const uint8_t cid, uint8_t index);

// print group
void print_voltage(Battery *bty);
void print_temperature(Battery * bty);

// bcc stuff with spi
uint8_t bcc_read_string(uint8_t *buffer, uint16_t length);
uint8_t bcc_send_string(const uint8_t *data, uint16_t length);

#endif