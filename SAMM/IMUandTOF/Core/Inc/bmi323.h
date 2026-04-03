#ifndef BMI323_H
#define BMI323_H

//includes 


#include "main.h"
#include "spi.h"
#include "stm32g474xx.h"

//BMI323 register defines

#define BMI323_CHIP_ID 0x00
#define BMI323_ERR_REG 0x01
#define BMI323_STATUS 0x02
#define BMI323_ACC_X 0x03
#define BMI323_ACC_Y 0x04
#define BMI323_ACC_Z 0x05
#define BMI323_GYR_X 0x06
#define BMI323_GYR_Y 0x07
#define BMI323_GYR_Z 0x08
#define BMI323_TEMP_DATA 0x09

#define BMI323_FEATURE_IO0 0x10
#define BMI323_FEATURE_I01 0x11
#define BMI323_FEATURE_IO2 0x12
#define BMI323_FEATURE_IO3 0x13
#define BMI323_FEATURE_IO_STATUS 0x14

#define BMI323_ACC_CONF 0x20
#define BMI323_GYR_CONF 0x21
#define BMI323_ALT_ACC_CONF 0x28
#define BMI323_ALT_GYR_CONF 0x29
#define BMI323_ALT_CONF 0x2A

#define BMI323_FIFO_CONF 0x36
#define BMI323_FIFO_CTRL 0x38

#define BMI323_FEATURE_CTRL 0x40

#define BMI323_ACC_DP_OFF_X 0x60
#define BMI323_ACC_DP_DGAIN_X 0x61
#define BMI323_ACC_DP_OFF_Y 0x62
#define BMI323_ACC_DP_DGAIN_Y 0x63
#define BMI323_ACC_DP_OFF_Z 0x64
#define BMI323_ACC_DP_DGAIN_Z 0x65
#define BMI323_GYR_DP_OFF_X 0x66
#define BMI323_GYR_DP_DGAIN_X 0x67
#define BMI323_GYR_DP_OFF_Y 0x68
#define BMI323_GYR_DP_DGAIN_Y 0x69
#define BMI323_GYR_DP_OFF_Z 0x6A
#define BMI323_GYR_DP_DGAIN_Z 0x6B

#define BMI323_CMD 0x7E
#define BMI323_CFG_RES 0x7F

//BMI323 potential spi address
#define BMI323_I2C_ADDR 0x68
#define BMI323_I2C_ADDR_ALT 0x69

//BMI323 sub-defines for registers
#define BMI323_CHIP_ID_RESET_VAL 0x0043
#define BMI323_ERR_REG_RESET_VAL 0x0000
#define BMI323_STATUS_RESET_VAL 0x0001
#define BMI323_ACC_X_RESET_VAL 0x8000
#define BMI323_ACC_Y_RESET_VAL 0x8000
#define BMI323_ACC_Z_RESET_VAL 0x8000
#define BMI323_GYR_X_RESET_VAL 0x8000
#define BMI323_GYR_Y_RESET_VAL 0x8000
#define BMI323_GYR_Z_RESET_VAL 0x8000
#define BMI323_TEMP_DATA_RESET_VAL 0x8000

#define BMI323_FEATURE_IO0_RESET_VAL 0x0000
#define BMI323_FEATURE_I01_RESET_VAL 0x0000

#define BMI323_ACC_CONF_RESET_VAL 0x0028
#define BMI323_GYR_CONF_RESET_VAL 0x0048

#define BMI323_FIFO_CONF_RESET_VAL 0x0000
#define BMI323_FIFO_CTRL_RESET_VAL 0x0000

#define BMI323_ACC_DP_OFF_X_RESET_VAL 0x0000
#define BMI323_ACC_DP_DGAIN_X_RESET_VAL 0x0000
#define BMI323_ACC_DP_OFF_Y_RESET_VAL 0x0000
#define BMI323_ACC_DP_DGAIN_Y_RESET_VAL 0x0000
#define BMI323_ACC_DP_OFF_Z_RESET_VAL 0x0000
#define BMI323_ACC_DP_DGAIN_Z_RESET_VAL 0x0000
#define BMI323_GYR_DP_OFF_X_RESET_VAL 0x0000
#define BMI323_GYR_DP_DGAIN_X_RESET_VAL 0x0000
#define BMI323_GYR_DP_OFF_Y_RESET_VAL 0x0000
#define BMI323_GYR_DP_DGAIN_Y_RESET_VAL 0x0000
#define BMI323_GYR_DP_OFF_Z_RESET_VAL 0x0000
#define BMI323_GYR_DP_DGAIN_Z_RESET_VAL 0x0000

#define BMI323_CMD_RESET_VAL 0x0000
#define BMI323_CFG_RES_RESET_VAL 0x0000

//defines for commands
#define BMI323_CMD_SOFT_RESET 0xDEAF
#define BMI323_CMD_CALIB 0x0101
#define BMI323_CMD_CALIB_ABORT 0x0200

//defines for features
#define BMI323_FEATURE_IO2_EN 0x012C
#define BMI323_FEATURE_IO_STS 0x0001
#define BMI323_FEATURE_CTRL_ENGINE_EN 0x0001
#define BMI323_FEATURE_IO1_STATUS 0x0001


//defines for acc and gyro conf
#define SUSPEND 0b000
#define LOW_POWER 0b011
#define HIGH_PERF 0b111
#define NORMAL 0b100

//these are the values that are used for the polling frequency
#define ODR_0_78 0x1
#define ODR_1_56 0x2
#define ODR_3_12 0x3
#define ODR_6_25 0x4
#define ODR_12_5 0x5
#define ODR_25 0x6
#define ODR_50 0x7
#define ODR_100 0x8
#define ODR_200 0x9
#define ODR_400 0xA
#define ODR_800 0xB
#define ODR_1600 0xC
#define ODR_3200 0xD
#define ODR_6400 0xE

//these are the values that are used for the -3dB bandwidth 
#define ODR_DIV_2 0x0
#define ODR_DIV_4 0x1

//these are defines for the sample averaging
#define AVG_0 0x0
#define AVG_2 0x1
#define AVG_4 0x2
#define AVG_8 0x3
#define AVG_16 0x4
#define AVG_32 0x5
#define AVG_64 0x6

//acc specific defines for range
#define ACC_RANGE_2G 0x0
#define ACC_RANGE_4G 0x1
#define ACC_RANGE_8G 0x2
#define ACC_RANGE_16G 0x3

//gyro specific defines for range
//WRONG VALUES
#define GYR_RANGE_2000 0x0
#define GYR_RANGE_1000 0x1
#define GYR_RANGE_500 0x2
#define GYR_RANGE_250 0x3

#define BMI323_TIMEOUT 1000

typedef struct
{
    SPI_HandleTypeDef *spi_port;
    uint16_t chip_id;
    GPIO_TypeDef  *port;
    uint16_t pin;
    /* data */
} bmi323 ;

uint8_t bmi323_init(bmi323 *bmi323_dev, SPI_HandleTypeDef *spi_port, GPIO_TypeDef *port, uint16_t pin);
uint16_t bmi323_read(bmi323 *bmi323_dev, uint8_t reg);
uint8_t bmi323_write(bmi323 *bmi323_dev, uint8_t reg, uint16_t data);
uint8_t bmi323_soft_reset(bmi323 *bmi323_dev);
uint8_t bmi323_calib(bmi323 *bmi323_dev);
uint8_t bmi323_calib_abort(bmi323 *bmi323_dev);
uint16_t bmi323_read_acc_x(bmi323 *bmi323_dev);
uint16_t bmi323_read_acc_y(bmi323 *bmi323_dev);
uint16_t bmi323_read_acc_z(bmi323 *bmi323_dev);
uint16_t bmi323_read_gyr_x(bmi323 *bmi323_dev);
uint16_t bmi323_read_gyr_y(bmi323 *bmi323_dev);
uint16_t bmi323_read_gyr_z(bmi323 *bmi323_dev);
uint16_t bmi323_read_temp_data(bmi323 *bmi323_dev);
uint16_t bmi323_read_status(bmi323 *bmi323_dev);
uint16_t bmi323_read_err_reg(bmi323 *bmi323_dev);
uint16_t bmi323_read_chip_id(bmi323 *bmi323_dev);
uint16_t bmi323_read_acc_conf(bmi323 *bmi323_dev);
uint16_t bmi323_read_gyr_conf(bmi323 *bmi323_dev);
uint16_t bmi323_read_acc_alt_conf(bmi323 *bmi323_dev);
uint16_t bmi323_read_gyr_alt_conf(bmi323 *bmi323_dev);
// uint8_t bmi323_enable_acc(bmi323 *bmi323_dev);
// uint8_t bmi323_enable_gyr(bmi323 *bmi323_dev);
uint8_t bmi323_disable_acc(bmi323 *bmi323_dev);
uint8_t bmi323_enable_gyro(bmi323 *bmi323_dev, uint8_t gyr_mode, uint8_t gyr_avg_num, uint8_t gyr_bw, uint8_t gyr_range, uint8_t gyr_odr);
uint8_t bmi323_enable_acc(bmi323 *bmi323_dev, uint8_t acc_mode, uint8_t acc_avg_num, uint8_t acc_bw, uint8_t acc_range, uint8_t acc_odr);
uint8_t bmi323_read_acc(bmi323 *bmi323_dev, int16_t *acc_data);
uint8_t bmi323_read_gyr(bmi323 *bmi323_dev, int16_t *gyr_data);
uint8_t bmi323_read_all(bmi323 *bmi323_dev, int16_t *temp_data);



#endif // BMI323_H