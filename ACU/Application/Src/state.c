#include "acu.h"
#include "state.h"

extern ACU acu;
extern Battery battery;
extern State state;

extern bool first_init;
extern bool check_ts_active;

extern uint8_t cycle;
extern uint16_t adc_data[6];
extern char print_buffer[1000];
extern uint8_t bcc_cooked_count;
extern bcc_status_t bcc_error; 
extern FDCAN_RxHeaderTypeDef RxHeader_Charger;

extern void print_lpuart(char* arr);
extern void print_bcc_status(bcc_status_t stat);

/// @brief constrains float vlaues
/// @param value 
/// @param lowerBound 
/// @param upperBound 
/// @return another float
float constrain(float value, float lowerBound, float upperBound) {
    if (value < lowerBound) {
        return lowerBound;
    } else if (value > upperBound) {
        return upperBound;
    } else {
        return value;
    }
}

/// @brief returns the current state
/// @return uint8_t of state
uint8_t get_state(){
    return (uint8_t)(state);
}

/// @brief sets the state in functions that can't access the state
void set_state(uint8_t value){
    state = value;
}

/// @brief: just prints the state
void print_state(){
    switch(state){
        case INIT:
            print_lpuart("State: INIT\n");
            break;
        case STANDBY:
            print_lpuart("State: STANDBY\n");
            break;
        case PRECHARGE:
            print_lpuart("State: PRECHARGE\n");
            break;
        case CHARGE:
            print_lpuart("State: CHARGE\n");
            break;
        case NORMAL:
            print_lpuart("State: NORMAL\n");
            break;
        case SHITDOWN:
            print_lpuart("State: SHITDOWN\n");
            break;
        default:
            print_lpuart("State: Error\n");
            break;
    }
}

/// @brief self explanatory
void shitdown(){
    // Open all
    acu.chg_ctrl = NO_CHARGE;
    write_prechg(false);
    write_IRneg(false);
    write_IRpos(false);
    enqueue(ACU_Charger_Control, FDCAN3);

    //indicates to battery to stop charging
    reset_discharge(&battery, false); // TODO: double check this
    // disable_cell_balancing
    
    acu.acu_err_warns &= ~(ACU_CLEAR_WARN);
    uint16_t precharge_error = acu.acu_err_warns & ACU_PRECHARGE;

    acu.acu_err_warns &= ~(ACU_CLEAR_ERRR);
    if (precharge_error) acu.acu_err_warns |= ACU_PRECHARGE;

    uint8_t pass = state_system_check(true, false);
    
    if (acu.ts_voltage < SAFE_V_TO_TURN_OFF && pass) { // safe to turn off if TS voltage < 60V
        // print_lpuart("Shutdown (Safe) => Standby");
        state = STANDBY;
    }
}

/// @brief either mega-cooked (BMS comms fail so we rerun init) or in init
/// potentially caused by wires being plugged in incorrectly
void init(){
    write_prechg(false);
    write_IRneg(false);
    write_IRpos(false);
    acu_init(&acu);

    if(first_init == true){
        acu.bty = &battery;
    }

    BCC_MCU_WaitMs(300);
    bcc_error = BCC_Init(&(battery.drvConfig));
    
    // start the cooked counter
    uint8_t counter = TRIES;
    while (bcc_error != BCC_STATUS_SUCCESS /*&& counter > 0*/){ 
        BCC_MCU_WaitMs(10);
        print_lpuart("(¬_¬\") Trying BCC_Init again\n");
        bcc_error = BCC_Init(&(battery.drvConfig)); 
        print_bcc_status(bcc_error);
        counter--;
    }
    if (counter == 0){
        state = SHITDOWN;
        print_lpuart("(¬_¬\") [BCC_Init = MEGA-COOKED]\n");
        return;
    }
    
    print_lpuart("PASSED from BCC_Init\n");
    battery.min_temp_thresh = (uint16_t)CELL_MIN_TEMP;
    battery.max_temp_thresh = (uint16_t)CELL_MAX_TEMP;
    battery.min_volt_thresh = (uint16_t)CELL_MIN_VOLT;
    battery.max_volt_thresh = (uint16_t)CELL_MAX_VOLT;

    // initialize registers
    bool succ = init_registers(&battery);
    if (!succ) {
        print_lpuart("(*_*) [Failed init_registers...]\n");
        return;
    }

    clear_faults(&(battery.drvConfig)); 
    print_lpuart("PASSED clear_faults\n"); 

    // cb & first battery check
    reset_discharge(&battery, false);
    state = init_cell_balancing(&battery) == 1 ? STANDBY : SHITDOWN;

    if (state == SHITDOWN) return;
    print_lpuart("CALL state_system_check(true, true)\n");
    if(!state_system_check(true, true)){
        print_lpuart("Failed 1st state_system_check. SHIT\n");
        #if DEBUGG == 0
        state = SHITDOWN;
        #endif
    }
    else {
        state = STANDBY;
    }
    print_lpuart("DONE state_system_check(true, true)\n");
    if(first_init == true){
        first_init = false;
    }
    bcc_cooked_count = 0;
    check_ts_active = false;
}

/// @brief do nothing, in initial state wait for VDM to send start command, maybe poll CAN
void standby(){
    write_prechg(false);
    write_IRneg(false);
    write_IRpos(false);
    if(state_system_check(false, false) == false){
        print_lpuart("𝓕𝓾𝓬𝓴\n");
        #if DEBUGG == 0
        state = state == INIT ? INIT : SHITDOWN;
        #endif
    }

    // check ts_active status
    if(acu.ts_active && check_ts_active){
    print_lpuart("(¬_¬\") [STANDBY => PRECHARGE]\n");
        state = PRECHARGE;
        check_ts_active = false;
    }
    else if(!acu.ts_active && check_ts_active)
    {
    print_lpuart("(¬_¬\") [STANDBY => SHITDOWN]\n");
        state = SHITDOWN;
        check_ts_active = false;
    }
    else {}
}

/// @brief State: PRECHARGE
void precharge(){
    print_lpuart("PRECHARGE\n");
    acu.acu_err_warns &= ~(ACU_CLEAR_WARN);
    acu.acu_err_warns &= ~(ACU_PRECHARGE);

    // all to zero
    write_prechg(false);
    write_IRneg(false);
    write_IRpos(false);

    // reset SDC latch and wait for voltage to stablize
    sdc_reset();
    LL_mDelay(10);

    if ((fabsf(acu.sdc_volt_w - acu.sdc_volt_v) < GLV_SDC_LOW) && acu.sdc_volt_w < SDC_HIGH) {
        print_lpuart("¯\\_(ツ)_/¯ Latch not closed, skill issue\n");
        #if DEBUGG == 0
        state = STANDBY;
        return;
        #endif
    }
    
    // system check
    if (!state_system_check(true, false)) {
        print_lpuart("𝓕𝓾𝓬𝓴\n");
        print_lpuart("¯\\_(ツ)_/¯ precharge: bad state_sys_check\n");
        #if DEBUGG == 0
        state = state == INIT ? INIT : SHITDOWN;
        return;
        #endif
    }

    uint32_t start_time = HAL_GetTick();
    // close AIR-
    write_IRneg(true); // starting precharge
    // Close precharge relay
    write_prechg(true);

    // keep looping until ts_voltage reaches 0.95 of total cell voltage
    while (acu.ts_voltage < get_total_voltage(&acu) * PRECHARGE_THRESHOLD) {
        read_device_measurements(&battery, true, true);
        sprintf(print_buffer, "TS voltage: %.3f / %.3f\n", acu.ts_voltage, acu.bty->battery_total_voltage);
        print_lpuart(print_buffer);
        // LL_mDelay(100);
        if (!state_system_check(true, false)) {
            print_lpuart("¯\\_(ツ)_/¯ PreCharge state_system_check() => Shutdown\n");
            #if DEBUGG == 0
            state = state == INIT ? INIT : SHITDOWN;
            return;
            #endif
        }

        if(fabsf(acu.voltage_12v - acu.sdc_volt_w) > GLV_SDC_LOW){
            print_lpuart("¯\\_(ツ)_/¯ PreCharge (250) SDC volt dropped\n");
            acu.acu_err_warns |= ACU_PRECHARGE;
            // enqueue(ACU_Status_2, FDCAN1);
            #if DEBUGG == 0
            state = SHITDOWN;
            return;
            #endif
        }
        if (HAL_GetTick() - start_time > 5000) { // timeout, throw error
            print_lpuart("¯\\_(ツ)_/¯ (259) Precharge timeout\n");
            acu.acu_err_warns |= ACU_PRECHARGE;
            // enqueue(ACU_Status_2,FDCAN1);
            #if DEBUGG == 0
            state = SHITDOWN;
            return;
            #endif
        }

        if(state != PRECHARGE){
            print_lpuart("🦍💨 NOT IN PRECHARGE??????\n");
        }
    }

    start_time = HAL_GetTick();
    uint8_t goToCharge = 0; // change this to false on final build

    // 1 second to check if we go to charge
    while (HAL_GetTick() - start_time < 2000) {
        read_device_measurements(&battery, true, true);
        sprintf(print_buffer, "TS voltage: %.3f / %.3f\n", acu.ts_voltage, acu.bty->battery_total_voltage);
        print_lpuart(print_buffer);
        LL_mDelay(100);
        if(!acu_check(&acu, false)){
            print_lpuart("PRECHARGE (274) => SHITDOWN");
            #if DEBUGG == 0
            state = SHITDOWN;
            return;
            #endif
        }

        // Charger detected, go to charge!
        if(HAL_GetTick() - acu.lastChrgRecieveTime < 1000){
            print_lpuart("Charger_Data received!\n");
            goToCharge = 1;
        }
        
        // actively check total_voltage vs ts_voltage just in case
        if(acu.ts_voltage < get_total_voltage(&acu) * PRECHARGE_THRESHOLD){
            print_lpuart("( ˶°ㅁ°) !! TS Voltage went down in Precharge\n");
            acu.acu_err_warns |= ACU_PRECHARGE;
            #if DEBUGG == 0
            state = SHITDOWN;
            return;
            #endif
        }
        // LL_mDelay(100);
    }
    // close AIR+
    write_IRpos(true); // precharge complete
    write_prechg(false); // open precharge relay
    print_lpuart("Precharge complete, closing AIR+\n");

    if(goToCharge){
        acu.chg_ctrl = PLS_CHARGE;
        state = CHARGE;
    }
    else{
        acu.chg_ctrl = NO_CHARGE;
        state = NORMAL;
    }
}

uint32_t last_charge_time = 0;
uint32_t last_discharge_time = 0;
uint32_t last_send_time = 0;
uint32_t last_call_time = 0;

/// charge state
void charge(){
    acu.acu_err_warns &= ~(ACU_CLEAR_WARN);

    // monitor accumulator
    if(!acu_check(&acu, false)){

        // if fault detected, open charging shutdown circuits
        // all current flow to accumulator MUST stop
        // voltage in tractive system must be low voltage in 5 seconds or less
        // turn off charger, disabled until manually reset
        print_lpuart("CHARGE (328) => SHITDOWN");
        #if DEBUGG == 0
        state = SHITDOWN;
        return;
        #endif
    }
    
    if(HAL_GetTick() - last_charge_time >= 2000){
        last_charge_time = HAL_GetTick();
        reset_discharge(&battery, false); 
        if(!state_system_check(true, false)){
            print_lpuart("( ˶°ㅁ°) !! Failed system check inside of charge\n");
            #if DEBUGG == 0
            state = state == INIT ? INIT : SHITDOWN;
            return;
            #endif
        }
        // do cell balancing
        do_cell_balancing(&battery); 
    }

    if(HAL_GetTick() - last_send_time > 990){
        last_send_time = HAL_GetTick();
        if(battery.max_cell_volt > battery.max_volt_thresh){
            battery.max_chg_current = map(battery.max_cell_volt, CELL_MAX_VOLT-0.06, CELL_MAX_VOLT-0.01, acu.target_chg_current, 0);
            battery.max_chg_current = constrain(battery.max_chg_current, 0.0f, acu.target_chg_voltage);
        } else {
            battery.max_chg_current = acu.target_chg_current;
        }
        // TODO: figure this line out
        battery.max_chg_current = acu.target_chg_current; 
        //every 0.99 seconds send charger "ping"
        enqueue(ACU_Charger_Control, FDCAN3);
    }

    //if no CAN data for 5 seconds, shut down
    if(HAL_GetTick() - acu.lastChrgRecieveTime > 5000U){
        print_lpuart("( ˶°ㅁ°) !! CHARGE: Charger CAN timeout, shutting down\n");
        #if DEBUGG == 0
        state = SHITDOWN;
        return;
        #endif
    }

    // re-measure current sensor ref every 5 minutes
    // if (HAL_GetTick() - last_call_time > 300000U) {

    //     last_call_time = HAL_GetTick();
    //     state = NORMAL; // turn charger off

    //     acu.chg_ctrl = NO_CHARGE;
    //     enqueue(ACU_Charger_Control,FDCAN3);

    //     LL_mDelay(1000);

    //     state = CHARGE; // turn charger back on
    //     acu.chg_ctrl = PLS_CHARGE;
    //     enqueue(ACU_Charger_Control,FDCAN3);
    // }
    
    return;
}

uint8_t tsVoltErrCount = 0;
void normal(){
    write_prechg(false);
    write_IRneg(true);
    write_IRpos(true);
    if(!state_system_check(false, false)){
        print_lpuart("( ˶°ㅁ°) !! SystemCheck failed in NORMAL state\n");
        #if DEBUGG == 0
        state = state == INIT ? INIT : SHITDOWN;
        return;
        #endif
    }

    float totalV = get_total_voltage(&acu);
    if (fabsf(acu.ts_voltage - totalV) > 80U) {
        print_lpuart("∘ ∘ ∘ ( °ヮ° ) ? TS voltage mismatch");
        tsVoltErrCount++;
        if (tsVoltErrCount >= ERRMG_ACU_ERR) {
            tsVoltErrCount = ERRMG_ACU_ERR;
            print_lpuart("NORMAL (409) => SHITDOWN");
            #if DEBUGG == 0
            state = SHITDOWN;
            return;
            #endif
            if (acu.ts_voltage < totalV) {
                acu.acu_err_warns |= ACU_ERR_UNDER_VOLT;
            }
            else {
                acu.acu_err_warns |= ACU_ERR_OVER_VOLT;
            }
        }
    }
    else {
        tsVoltErrCount = 0;
    }

    // check ts_active status
    if((!acu.ts_active) && check_ts_active){
    print_lpuart("(¬_¬\") [NORMAL => SHITDOWN]\n");
        state = SHITDOWN;
        check_ts_active = false;
    }

    if (fabsf(acu.ts_current) > 0.5) acu.cur_LastHighTime = HAL_GetTick();

}

/// @brief system check on acu, battery, errors/warnings
/// @param full_check check everything
/// @param startup true if this is the 1st system check
/// @return returns True if passes, False otherwise
bool state_system_check(bool full_check, bool startup){

    if (startup == true){
        read_device_measurements(&battery, true, true);
    }

    calculate_acu_soc(&acu);

    bool a_check = acu_check(&acu, startup);
    if(a_check == false){
        print_lpuart("(¬_¬\") Failed acu_check\n");
    }
    
    bool b_check = battery_check(&battery, full_check);
    if(b_check == false){
        print_lpuart("(¬_¬\") Failed battery_check\n");
    }

    // voltage values vs allowable range EV.7.4.2
    if(battery.battery_check_faults & BATTERY_FAULT_CELL_OV){
        acu.acu_err_warns |= ACU_ERR_OVER_VOLT;
    }
    if(battery.battery_check_faults & BATTERY_FAULT_CELL_UV){
        acu.acu_err_warns |= ACU_ERR_UNDER_VOLT;
    }
    // (3) temp values outside the allowable range EV.7.5.2
    if(battery.battery_check_faults & BATTERY_FAULT_CELL_OT){
        acu.acu_err_warns |= ACU_ERR_OVER_TEMP;
    }

    // print_errors_warning(&acu);
    if (b_check){
        write_bms_ok(state);
        // print_lpuart("BMS ok <3\n");
    }
    else {
        write_bms_ok(false);
        // print_lpuart("BMS not ok :(\n");
    }
    return a_check && b_check;
}