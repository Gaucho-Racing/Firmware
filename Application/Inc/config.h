#ifndef CONFIG_H
#define CONFIG_H

#define DEBUGG 0
#define SPAMPRINT 0

#define NUM_TOTAL_IC 2U
#define REAL_NUM_TOTAL_IC 10U // only used in defining struct so no mem leaks LOL
#define NUM_CELL_IC 14U

#define INIT_REG_CNT 45
#define GPIO_CFG1 0.0f
#define GPIO_CFG2 0.0f
#define CBX_SET 1.0f

#define SPI_LOOP_TIMEOUT 500

#define CELL_INT_RESISTANCE 0.006
#define CELL_EMPTY_VOLTAGE 3
#define CELL_FULL_VOLTAGE 4.18

#define CELL_MAX_VOLT 4.2f
#define CELL_MIN_VOLT 1.0f

#define CELL_MIN_TEMP -300.0f
#define CELL_MAX_TEMP 60.0f

//acu limits
#define MAX_HV_CURRENT 135
#define MIN_GLV_VOLT 4.0f
#define MAX_GLV_VOLT 14.4f
#define ERRMG_ISNS_VREF 0.2f
#define STACK_VOLT_DIFF 1.0f
#define MAX_READ_COUNT 8
#define TRIES 5 // defines how many times we can retry an action

#define GET_INDEX(x, y) ((x * NUM_CELL_IC) + y) // get the index of the cell in the array

// error masks
#define ACU_ERR_OVER_TEMP   0b1000000000000000
#define ACU_ERR_OVER_VOLT   0b0100000000000000
#define ACU_ERR_UNDER_VOLT  0b0010000000000000
#define ACU_ERR_OVER_CURR   0b0001000000000000
#define ACU_PRECHARGE       0b0000000100000000
#define ACU_CLEAR_ERRR      0b1111000100000000

#define ACU_ERR_UV_20_V     0b0000100000000000 // warning
#define ACU_ERR_UV_12_V     0b0000010000000000 // warning
#define ACU_ERR_UV_SDC      0b0000001000000000 // warning
#define ACU_CLEAR_WARN      0b0000111000000000

// battery fault masks
#define BCC_FS_MAX 11
#define BATTERY_FAULT_CELL_OV 0b10000000
#define BATTERY_FAULT_CELL_UV 0b01000000
#define BATTERY_FAULT_CB_OPEN 0b00100000
#define BATTERY_FAULT_CB_SHRT 0b00010000
#define BATTERY_FAULT_CELL_OT 0b00001000
#define BATTERY_FAULT_GPIO_ST 0b00000100
#define BATTERY_FAULT_GPIO_SH 0b00000010
#define BATTERY_FAULT_COMM_FL 0b00000001


// charger error masks
#define CHARGER_HW_FAIL     0b00001U
#define CHARGER_OV_TEMP     0b00010U
#define CHARGER_IN_VOLT     0b00100U
#define CHARGER_CONNECT     0b01000U
#define CHARGER_COOMMMM     0b10000U

// energy measurement status
#define EM_VIOLATION        0b1000
#define EM_LOGGING          0b0100

// precharge cmd
#define PLS_CHARGE 0U
#define NO_CHARGE  1U

// relay state muxes
#define AIR_PLUS   0b1000 // IR- : 0: Open, 1: Closed
#define AIR_MINUS  0b0100 // IR+ : 0: Open, 1: Closed
#define RELAY_PRE  0b0010 // Precharge 0: Open, 1: Closed
#define ACU_LATCH  0b0001 // ACU Latch / Software Latch

// IMD error masks
#define IMD_ERROR_ACTIVE    0b10000000000U
#define HV_POS_CONN_FAIL    0b01000000000U
#define HV_NEG_CONN_FAIL    0b00100000000U
#define EARTH_CONNN_FAIL    0b00010000000U
#define ISO_ALARM_ERRROR    0b00001000000U
#define ISO_WARN_ERRRROR    0b00000100000U
#define ISO_OUTDATED_ERR    0b00000010000U
#define UN_BALANCE_ALARM    0b00000001000U
#define UNDERVOLTG_ALARM    0b00000000100U
#define UNSAFE_TOO_START    0b00000000010U
#define EARTH_LIFT_OPENN    0b00000000001U

// error margins
#define GLV_SDC_LOW 2.0f
#define SDC_HIGH 9.0f
#define ERRMG_ACU_ERR 10
#define ERRMG_BMS_ERR 5

// datatype
#define UNSIGNED_16 0
#define SIGNED_16   1

// ADC Warning Thresholds
#define UNDER_VOLTAGE_20V 15
#define UNDER_VOLTAGE_GLV 10
#define UNDER_VOLTAGE_SDCV 9

#define PRECHARGE_THRESHOLD 0.95f // fraction of total cell voltage
#define SAFE_V_TO_TURN_OFF 60

/* Send ***********************************************************/
// CAN1
#define ACU_Debug_2_Debug       0x300001    // FLAG = 1 (CAN1)
#define ACU_Ping_Debug          0x300201    // FLAG = 2 (CAN1)
#define ACU_Ping_ECU            0x300202    // FLAG = 3 (CAN1)

#define ACU_Status_1            0x300702    // FLAG = 5 (CAN1)
#define ACU_Status_2            0x300802    // FLAG = 6 (CAN1)
#define ACU_Status_3            0x300902    // FLAG = 7 (CAN1)
#define ACU_DC_DC_Status        0x301202    // DEPRECATED

// CAN2
// ACU_Ping_Debug               0x300201    // FLAG = 1 (CAN2)
// ACU_Ping_ECU                 0x300202    // FLAG = 2 (CAN2)
#define ACU_Debug_FD            0x300101    // FLAG = 3 (CAN2)
#define ACU_Cell_Data_1         0x300DFF    // FLAG = 4 (CAN2)
#define ACU_Cell_Data_2         0x300EFF    // FLAG = 5 (CAN2)
#define ACU_Cell_Data_3         0x300FFF    // FLAG = 6 (CAN2)
#define ACU_Cell_Data_4         0x3010FF    // FLAG = 7 (CAN2)
#define ACU_Cell_Data_5         0x3011FF    // FLAG = 8 (CAN2)

// CAN3
#define ACU_Charger_Control     0x1806E5F4  // FLAG = 1 (CAN3)

/* Receive ********************************************************/
// CAN1
#define Debug_2_ACU             0x100003    // Debugger sends Debug 2.0 to ACU (CAN1)
#define Debug_Ping_ACU          0x100203    // Debugger sends PING to ACU (CAN1/CAN2)
#define Precharge_ACU           0x200A03    // ECU sends PRECHARGE to ACU (CAN1)
#define Config_Charge_ACU       0x200B03    // ECU sends Charger Config to ACU (CAN1)
#define Config_Ops_ACU          0x200C03    // ECU sends Operation Config to ACU (CAN1)

// CAN2
#define Debug_FD_ACU            0x100103    // Debugger sends Debug FD to ACU (CAN2)
#define ECU_Ping_ALL            0x2002FF    // ECU sends PING to ALL (CAN1/CAN2)

// CAN3
#define Charger_Data_ACU        0x18FF50E5  // Charger sends Charger Data to ACU (CAN3)

#define EM_Measurements_ACU     0x10D       // EM sends EM Measurements to ACU (CAN3)
#define EM_Data_1_ACU           0x30D       // EM sends EM Team Data 1 to ACU (CAN3)
#define EM_Data_2_ACU           0x30E       // EM sends EM Team Data 2 to ACU (CAN3)
#define EM_Status_ACU           0x40D       // EM sends EM Status to ACU (CAN3)
#define EM_Temperature_ACU      0x60D       // EM sends EM Temperature to ACU (CAN3)

#define IMD_Response_ACU        0x23        // IMD sends IMD response to ACU (CAN3)
#define IMD_Isolation_ACU       0x18FF02F4  // IMD sends IMD iso info to ACU (CAN3) (prev 0x18EFF4FE)
#define IMD_Voltage_ACU         0x18FF03F4  // IMD sends IMD voltage to ACU (CAN3) (prev 0x18EFF4FE)
#define IMD_IT_System_ACU       0x18FF04F4  // IMD sends IMD it-system to ACU (CAN3) (prev 0x18EFF4FE)
#define IMD_Request_ACU         0x18EFF4FE  // IMD sends IMD request to ACU (CAN3) (Should be 1CEF00F4)
#define IMD_General_ACU         0x18FF01F4  // IMD sends IMD general to ACU (CAN3)

#endif