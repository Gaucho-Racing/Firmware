#include "battery.h"
#include "state.h"

extern uint8_t cycle;
extern bcc_status_t bcc_error;
extern char print_buffer[1000];
extern uint8_t bcc_cooked_count;
extern volatile uint8_t TPL_RxBuffer[256]; // Array to store received SPI data => Replace with struct holding CAN RxBuffer
extern volatile uint8_t TPL_RxBufferLevel; // Number of bytes to be read
extern volatile uint8_t TPL_RxBufferBottom; // Index of oldest data

extern void set_state(uint8_t value);
extern void print_lpuart(char* arr);
extern void write_LED(bool state);
extern void print_bcc_status(bcc_status_t stat);

// Thermistor mapping array
const uint8_t thermistor_map[28] = {
    0,  1,  2,  3, 
    8,  9,  10, 11, 12, 13, 14, 15, 
    16, 17, 18, 19, 20, 21, 22, 23, 
    24, 25, 26, 27, 28, 29, 30, 31
};

/// @brief for bcc spi read
/// @param buffer 
/// @param length 
/// @return 0 for success, 1 for FAILURE 🤦‍♂️
uint8_t bcc_read_string(uint8_t *buffer, uint16_t length){
    for (uint16_t i = 0; i < length; i++) {
        uint32_t counter = 0;
        while (TPL_RxBufferLevel == 0) {
            BCC_MCU_WaitUs(1);
            if(counter++ > SPI_LOOP_TIMEOUT) {
                return 1;
            }
        }
        __disable_irq();
        buffer[i] = TPL_RxBuffer[TPL_RxBufferBottom];
        TPL_RxBufferBottom++;
        TPL_RxBufferLevel--;
        __enable_irq();
    }
    return 0;
  }
  
  /// @brief for bcc spi send
  /// @param data 
  /// @param length 
  /// @return 0 for success, 1 for FAILURE 🤦‍♂️
uint8_t bcc_send_string(const uint8_t *data, uint16_t length) {
    uint32_t counter = 0;
    BCC_MCU_WriteCsbPin(0, 0); // CS LOW
    BCC_MCU_WaitUs(2); // delay required by MC33664
    while (!LL_SPI_IsActiveFlag_TXE(SPI1)) {
        if(counter++ > SPI_LOOP_TIMEOUT) return 1;
        BCC_MCU_WaitUs(1);
    }
    for (uint16_t i = 0; i < length; i++) {
        while(!LL_SPI_IsActiveFlag_TXE(SPI1));
        LL_SPI_TransmitData8(SPI1, data[i]);
    }
    
    while (LL_SPI_IsActiveFlag_BSY(SPI1));
    BCC_MCU_WaitUs(1); // delay required by MC33664
    BCC_MCU_WriteCsbPin(0, 1); // CS HIGH
    return 0;
}

/// @brief do we need this?
/// @param bty 
void get_faults(Battery * bty){
    // bcc_error = BCC_Fault_GetStatus(&(bty->drvConfig), cid, fault_status);
}

void clear_faults(bcc_drv_config_t * drvConfig)
{
    for (uint8_t ic = 1; ic <= NUM_CELL_IC; ic++)
    {
        bcc_cid_t cid = (bcc_cid_t)ic;
        bcc_error = BCC_Fault_ClearStatus(drvConfig, cid, BCC_FS_CELL_OV);
        bcc_error = BCC_Fault_ClearStatus(drvConfig, cid, BCC_FS_CELL_UV);
        bcc_error = BCC_Fault_ClearStatus(drvConfig, cid, BCC_FS_CB_OPEN);
        bcc_error = BCC_Fault_ClearStatus(drvConfig, cid, BCC_FS_CB_SHORT);
        bcc_error = BCC_Fault_ClearStatus(drvConfig, cid, BCC_FS_GPIO_STATUS);
        bcc_error = BCC_Fault_ClearStatus(drvConfig, cid, BCC_FS_AN_OT_UT);
        bcc_error = BCC_Fault_ClearStatus(drvConfig, cid, BCC_FS_GPIO_SHORT);
        bcc_error = BCC_Fault_ClearStatus(drvConfig, cid, BCC_FS_COMM);
        bcc_error = BCC_Fault_ClearStatus(drvConfig, cid, BCC_FS_FAULT1);
        bcc_error = BCC_Fault_ClearStatus(drvConfig, cid, BCC_FS_FAULT2);
        bcc_error = BCC_Fault_ClearStatus(drvConfig, cid, BCC_FS_FAULT3);
        if(bcc_error != BCC_STATUS_SUCCESS) return;
    }
}

bool init_registers(Battery * bty)
{
    uint8_t cid, i;
    for (cid = 1; cid <= bty->drvConfig.devicesCnt; cid++)
    {
        for (i = 0; i < INIT_REG_CNT; i++)
        {
            if (init_regs[i].value != init_regs[i].defaultVal)
            {
                bcc_error = BCC_Reg_Write(&(bty->drvConfig), (bcc_cid_t)cid,
                        init_regs[i].address, init_regs[i].value);
                    if(bcc_error != BCC_STATUS_SUCCESS) return false;
            }
        }
    }
    return true;
}

void print_volt(const float * voltages, const uint8_t cid, uint8_t index){
    bzero(print_buffer, sizeof(print_buffer));
    sprintf(print_buffer, "Row %d, Cell %d: %.3f\n", (int)cid, (int)index, voltages[index]);
    print_lpuart(print_buffer);
}

/// @brief turn off/on cell_balancing for all cells
/// @param bty battery
/// @param on True => turn on, False => turn off
void reset_discharge(Battery * bty, bool on){
    for(uint8_t i = 0; i < NUM_TOTAL_IC; i++)
    {
        for(uint8_t j = 0; j < NUM_CELL_IC; j++){
            if ((bcc_error = set_cell_balance(bty, (bcc_cid_t)(i), j, 0, on))!= BCC_STATUS_SUCCESS){ // resets after default = 30 seconds
                print_bcc_status(bcc_error);
                set_state(SHITDOWN);
                return;
            }
        }
    }
}

/// @brief TODO: each cycle measure ith temp
/// @param bty 
/// @param read_volt True if we're reading voltages
/// @param read_temp True if we're reading temps
/// @return SUCCESS if read was successful, else failed
float max_volt = -1000.0f, max_temp = -1000.0f, min_volt = 1000.0f, min_temp = 1000.0f;
bcc_status_t read_device_measurements(Battery * bty, uint8_t read_volt, uint8_t read_temp) 
{
    float temp_stack_volt[NUM_TOTAL_IC] = {0};
    float battery_total_volt_temp = 0;

    uint32_t measurements[NUM_CELL_IC];
    int16_t temp_measures;

    for(uint8_t i = 0; i < NUM_TOTAL_IC; i++){
        bcc_error = BCC_CB_Pause(&(bty->drvConfig), (bcc_cid_t)(i+1), true); // pause b4 read
        if(bcc_error != BCC_STATUS_SUCCESS) {
            print_bcc_status(bcc_error);
            bcc_error = BCC_CB_Pause(&(bty->drvConfig), (bcc_cid_t)(i+1), true);
        }
        if(read_volt){ // CELL VOLTAGES
            bcc_error = BCC_Meas_StartAndWait(&(bty->drvConfig), (bcc_cid_t)(i+1), BCC_AVG_1);
            if(bcc_error != BCC_STATUS_SUCCESS) {
                print_bcc_status(bcc_error);
            }
            bcc_error = BCC_Meas_GetCellVoltages(&(bty->drvConfig), (bcc_cid_t)(i+1), measurements);
            
            if(bcc_error != BCC_STATUS_SUCCESS) {
                bcc_cooked_count++;
                if(bcc_cooked_count == 0){
                    set_state(INIT);
                    print_lpuart("\nerror in BCC_Meas_GetCellVoltages: ");
                    print_bcc_status(bcc_error);
                    #if DEBUGG == 0
                    return bcc_error;
                    #endif
                }
            }
            for (uint8_t j = 0; j < NUM_CELL_IC; j++){
                bty->cell_volt[GET_INDEX(i, j)] = measurements[j] * 1e-6f;
                battery_total_volt_temp += (measurements[j] * 1e-6f);
                temp_stack_volt[i] += measurements[j] * 1e-6f;

                // measurements
                max_volt = fmaxf(max_volt, measurements[j] * 1e-6f);
                min_volt = fminf(min_volt, measurements[j] * 1e-6f);
            }

            bzero(measurements, sizeof(measurements));
            bcc_error = BCC_Meas_GetStackVoltage(&(bty->drvConfig), (bcc_cid_t)(i+1), measurements);
            bty->stack_voltage[i] = *measurements * 1e-6f; // theres ~ .3 difference from manual calcs
            bty->calculated_stack_voltage[i] = temp_stack_volt[i];
            
            if(bcc_error != BCC_STATUS_SUCCESS) {
                bcc_cooked_count++;
                if(bcc_cooked_count == 0){
                    set_state(INIT);
                    print_lpuart("\nerror in BCC_Meas_GetStackVoltage: ");
                    print_bcc_status(bcc_error);
                    #if DEBUGG == 0
                    return bcc_error;
                    #endif
                }
            }
        }

        if(read_temp && cycle == i){
            temp_measures = 0;
            bcc_error = BCC_Meas_GetIcTemperature(&(bty->drvConfig), (bcc_cid_t)(i+1), BCC_TEMP_CELSIUS, &temp_measures);
            bty->icTemp[i] = (float)temp_measures; // TRIAGE: check this in charging

            if(bcc_error != BCC_STATUS_SUCCESS) {
                bcc_cooked_count++;
                if(bcc_cooked_count == 0){
                    set_state(INIT);
                    print_lpuart("error in BCC_Meas_GetIcTemperature: ");
                    print_bcc_status(bcc_error);
                    #if DEBUGG == 0
                    return bcc_error;
                    #endif
                }
            }
        }

        // CELL TEMPS
        if(read_temp && cycle == i){
            for(uint8_t j = 0; j < 28; j++) {
                uint8_t readByte = 0;
                bcc_error = BCC_EEPROM_Read(&(bty->drvConfig), (bcc_cid_t)(i+1), thermistor_map[j]+1, &readByte);
                if (bcc_error == BCC_STATUS_SUCCESS) {
                    bty->cell_temp[GET_INDEX(i*2, j)] = V2T(readByte * 0.01953125f, 4250.0f);
                    max_temp = fmaxf(max_temp, bty->cell_temp[GET_INDEX(i, j)]);
                    min_temp = fminf(min_temp, bty->cell_temp[GET_INDEX(i, j)]);
                }
                // uncomment this when temp is normal
                if(bcc_error != BCC_STATUS_SUCCESS) {
                    print_lpuart("error in BCC_EEPROM_Read: ");
                    print_bcc_status(bcc_error);
                    bcc_cooked_count++;
                    if(bcc_cooked_count == 0){
                        set_state(INIT);
                        #if DEBUGG == 0
                        print_lpuart("error in BCC_EEPROM_Read: ");
                        return bcc_error;
                        #endif
                    }
                }
            }
        }

        bcc_error = BCC_CB_Pause(&(bty->drvConfig), (bcc_cid_t)(i+1), false); // resume after read
        if (bcc_error != BCC_STATUS_SUCCESS) {
            print_bcc_status(bcc_error);
            #if DEBUGG == 0
            return bcc_error;
            #endif
        }
    }
    
    if(read_volt){
        bty->min_cell_volt = min_volt;
        bty->max_cell_volt = max_volt;
        bty->battery_total_voltage = battery_total_volt_temp;
        min_volt = 1000.0f; max_volt = -1000.0f;
    }
    if(read_temp && cycle == NUM_TOTAL_IC - 1){
        bty->max_cell_temp = max_temp;
        bty->min_cell_temp = min_temp;
        min_temp = 1000.0f; max_temp = -1000.0f; 
    }
    cycle = (cycle + 1) % NUM_TOTAL_IC; 
    return BCC_STATUS_SUCCESS;
}

/// @brief checks which cells need to be discharged
/// @param bty hopefully not a cooked battery
/// @return True if success, False otherwise
// TODO: review this
bool do_cell_balancing(Battery * bty){

    float threshold = (bty->min_cell_volt + bty->max_cell_volt) * 0.5f;
    bcc_error = BCC_STATUS_SUCCESS;

    // cell balancing ~
    for(uint8_t i = 0; i < NUM_TOTAL_IC; i++)
    {
        uint8_t to_discharge = 0;
        for(uint8_t j = 0; j < NUM_CELL_IC; j++){
            if((bty->cell_volt[i*NUM_CELL_IC+j] > threshold || bty->cell_volt[i*NUM_CELL_IC + j] - bty->min_cell_volt > 0.02f)){
                to_discharge = 1;
            }
        }
        if(to_discharge == 1){
            for(uint8_t j = 0; j < NUM_CELL_IC; j++){
                // resets after default = 30 seconds
                if ((bcc_error = set_cell_balance(bty, (bcc_cid_t)(i), j, 0, 1))!= BCC_STATUS_SUCCESS){ 
                    print_bcc_status(bcc_error);
                    return false;
                }
            }
        }
        to_discharge = 0;
        
    }
    return true;
}

/// @brief set cell_balancing for specific cell
/// @param bty hopefully not a cooked battery
/// @param cid IC
/// @param cellIndex cell from IC
/// @param all configure all?
/// @param enable turn on or off?
/// @return status from configuration
// TODO: review this
bcc_status_t set_cell_balance(Battery * bty, bcc_cid_t cid, uint8_t cellIndex, bool all, bool enable)
{
    bcc_error = BCC_STATUS_SUCCESS;
    // set all
    if(all){
        for(uint8_t i = 0; i < NUM_TOTAL_IC; i++)
        {
            for(uint8_t j = 0; j < NUM_CELL_IC; j++){

                bcc_status_t errors;
                if((errors = BCC_CB_SetIndividual(&(bty->drvConfig), (bcc_cid_t)(i+1), j, true, 0)) != BCC_STATUS_SUCCESS){
                    bzero(print_buffer, sizeof(print_buffer));
                    sprintf(print_buffer, "Error IC %d, Cell %d: ", (int)(i+1), (int)(j));
                    print_lpuart(print_buffer);
                    print_bcc_status(errors);
                    bcc_error = errors;
                }
                bty->cell_balancing[GET_INDEX(i, j)] = enable == true ? 1 : 0;
            }
        }
        return bcc_error;
    }
    else{
        // sanity check
        if(cellIndex >= NUM_CELL_IC){
            print_lpuart("Invalid cell index");
            return bcc_error;
        }

        // set individuals
        if((bcc_error = BCC_CB_SetIndividual(&(bty->drvConfig), (bcc_cid_t)(cid+1), cellIndex, enable, 0)) != BCC_STATUS_SUCCESS) {
            bzero(print_buffer, sizeof(print_buffer));
            sprintf(print_buffer, "Error IC %d, Cell %d: ", (int)(cid+1), (int)(cellIndex));
            print_lpuart(print_buffer);
            return bcc_error;
        }
        bty->cell_balancing[(uint8_t)cid*NUM_TOTAL_IC+cellIndex] = enable == true ? 1 : 0;
    }
    return bcc_error;
}

/// @brief volt to temp
float V2T(float voltage, float B){
  float R = voltage / ((5.0 - voltage) / 47e3) / 100e3;
  float T = 1.0 / ((log(R) / B) + (1.0 / 298.15));
  return T - 273.15;
}

/// @brief initialize cell balancing
/// @param bty battery
/// @return 0 if failure, 1 if success
bool init_cell_balancing(Battery * bty){
    for(uint8_t i = 0; i < NUM_TOTAL_IC; i++)
    {
        bcc_error = BCC_CB_Enable(&(bty->drvConfig), (bcc_cid_t)(i+1),  true);
        if(bcc_error != BCC_STATUS_SUCCESS) {
            for(uint8_t j = 0; j < NUM_CELL_IC; j++){
                bty->cell_balancing[GET_INDEX(i, j)] = 2;
            };
            print_bcc_status(bcc_error);
            return 0;
        }
        for(uint8_t j = 0; j < NUM_CELL_IC; j++){
            bty->cell_balancing[GET_INDEX(i, j)] = 0;
        }
    }
    return 1;
}

bool disable_cell_balancing(Battery * bty){
    for(uint8_t i = 0; i < NUM_TOTAL_IC; i++)
    {
        bcc_error = BCC_CB_Enable(&(bty->drvConfig), (bcc_cid_t)(i+1),  false);
        if(bcc_error != BCC_STATUS_SUCCESS) {
            for(uint8_t j = 0; j < NUM_CELL_IC; j++){
                bty->cell_balancing[GET_INDEX(i, j)] = 2;
            }
            print_bcc_status(bcc_error);
            return 0;
        }
        for(uint8_t j = 0; j < NUM_CELL_IC; j++){
            bty->cell_balancing[GET_INDEX(i, j)] = 2;
        }
    }
    return 1;
}

/// @brief Check cell temps with bounds, updates errs
/// @param bty the hopefully not cooked battery
/// @return 1 if it passes, 0 otherwise
bool check_temp(Battery *bty){
    bool success = 1;
    bty->cell_temp_errors = 0;

    for(uint8_t i = 0; i < NUM_TOTAL_IC; i++){
        for(uint8_t j = 0; j < (NUM_CELL_IC); j++){
            // overtemp check
            if(bty->cell_temp[i*NUM_CELL_IC + j] > bty->max_temp_thresh){
                bty->cell_temp_errors++;
                bty->cell_temp_err_cnt[i*NUM_CELL_IC + j]++;
                if(bty->cell_temp_err_cnt[i*NUM_CELL_IC + j] > ERRMG_BMS_ERR){
                    bty->cell_temp_err_cnt[i*NUM_CELL_IC + j] = ERRMG_BMS_ERR;
                    bty->battery_check_faults |= BATTERY_FAULT_CELL_OT; // probably not the correct one
                    success = 0;
                }
            }
            else{
                bty->cell_temp_err_cnt[i*NUM_CELL_IC + j] = 0;
            }
        }
    }
    return success;
}

/// @brief Check cell volts with bounds, updates errs
/// @param bty the hopefully not cooked battery
/// @return 1 if it passes, 0 otherwise
bool check_volt(Battery *bty) {
    uint8_t success = 1;
    bty->cell_volt_errors = 0;

    for(uint8_t i = 0; i < NUM_TOTAL_IC; i++){
        for(uint8_t j = 0; j < NUM_CELL_IC; j++){

            // max_volt check
            if(bty->cell_volt[i*NUM_CELL_IC + j] > bty->max_volt_thresh){
                bty->cell_volt_errors++;
                bty->cell_volt_err_cnt[i*NUM_CELL_IC + j]++;
                if(bty->cell_volt_err_cnt[i*NUM_CELL_IC + j] > ERRMG_BMS_ERR){
                    bty->cell_volt_err_cnt[i*NUM_CELL_IC + j] = ERRMG_BMS_ERR;
                    bty->battery_check_faults |= BATTERY_FAULT_CELL_OV; // probably not the correct one
                    success = 0;
                }
            }

            // min_volt check
            if(bty->cell_volt[i*NUM_CELL_IC + j] < bty->min_volt_thresh){
                bty->cell_volt_errors++;
                bty->cell_volt_err_cnt[i*NUM_CELL_IC + j]++;
                if(bty->cell_volt_err_cnt[i*NUM_CELL_IC + j] > ERRMG_BMS_ERR){
                    bty->cell_volt_err_cnt[i*NUM_CELL_IC + j] = ERRMG_BMS_ERR;
                    bty->battery_check_faults |= BATTERY_FAULT_CELL_UV; // probably not the correct one
                    success = 0;
                }
            }
        }
        
        // check stack voltage vs real sum aren't too different
        if(fabsf(bty->stack_voltage[i] - bty->calculated_stack_voltage[i]) > STACK_VOLT_DIFF){
            bty->cell_volt_errors++;
            print_lpuart("stack voltage vs manual sum of cell volts is > 0.5!\n");
            success = 0;
        }
    }
    return success;
}

/// @brief reads cell volts & cell temps, updates neccessary struct data
/// @param bty 
/// @param fullcheck 
/// @return 1 if passes, 0 otherwise
bool battery_check(Battery *bty, bool fullcheck){
    // note, reading temp may or may not be different bc Vamsi changed thermisters, now data is 12 bit resolution instead of 10
    bcc_error = BCC_STATUS_SUCCESS;
    
    // clear all faults before reading again
    clear_faults(&(bty->drvConfig)); 
    bool success = true;
    if(fullcheck){
        bcc_error = read_device_measurements(bty, true, true);
        if(bcc_error != BCC_STATUS_SUCCESS) print_bcc_status(bcc_error);
    }

    // check temp
    success = check_temp(bty);
    if(!success) {
        bzero(print_buffer, sizeof(print_buffer));
        sprintf(print_buffer, "%d errors from check_temp()\n", bty->cell_temp_errors);
        print_lpuart(print_buffer);
    }

    // check volts
    success = check_volt(bty);
    if(!success) {
        bzero(print_buffer, sizeof(print_buffer));
        sprintf(print_buffer, "%d errors from check_volt()\n", bty->cell_volt_errors);
        print_lpuart(print_buffer);
    }
    return success;
}

// dump temp measurements
void print_temperature(Battery * bty){
    print_lpuart("Cell Temp: ------------------------------\n");
    for(size_t i = 0; i < NUM_TOTAL_IC; i++){
        bzero(print_buffer, sizeof(print_buffer));
        sprintf(print_buffer, "IC %u:\n", i);
        print_lpuart(print_buffer);
        for(size_t j = 0; j < NUM_CELL_IC; j++){
            if(j == 7){ print_lpuart("\n"); }
            const float curr_temp = fmaxf(bty->cell_temp[(i*2*NUM_CELL_IC) + j*2], bty->cell_temp[(i*2*NUM_CELL_IC) + j*2+1]);
            if (curr_temp < -40.0f) {
                sprintf(print_buffer, "C%u: NC |", j);
                print_lpuart(print_buffer);
            }
            else{
                sprintf(print_buffer, "C%u: %.3f |", j, curr_temp);
                print_lpuart(print_buffer);
            }
        }
        print_lpuart("\n");
    }
    sprintf(print_buffer, "Min Temp: %.3f | Max Temp: %.3f\n", bty->min_cell_temp, bty->max_cell_temp);
    print_lpuart(print_buffer);
    // print_lpuart("-----------------------------------------\n");
}

// dump voltage measurements
void print_voltage(Battery *bty){
    print_lpuart("Cell Voltage: ------------------------------\n");
    for(uint8_t i = 0; i < NUM_TOTAL_IC; i++){ // change this later
        bzero(print_buffer, sizeof(print_buffer));
        sprintf(print_buffer, "IC %u:\n", i);
        print_lpuart(print_buffer);
        for (uint8_t j = 0; j < NUM_CELL_IC; j++){
            if(j == 7){print_lpuart("\n");}
            sprintf(print_buffer, "C%u: %.3f | ", j, bty->cell_volt[GET_INDEX(i, j)]);
            print_lpuart(print_buffer);
        }
        bzero(print_buffer, 1000U);
        bzero(print_buffer, sizeof(print_buffer));
        sprintf(print_buffer, "\nStack Voltage: %.3f | Calculated Stack: %.3f\n", bty->stack_voltage[i], bty->calculated_stack_voltage[i]);
        print_lpuart(print_buffer);
    }
    sprintf(print_buffer, "Min: %.3f | Max: %.3f\n", bty->min_cell_volt, bty->max_cell_volt);
    print_lpuart(print_buffer);
    // print_lpuart("-----------------------------------------\n");
}