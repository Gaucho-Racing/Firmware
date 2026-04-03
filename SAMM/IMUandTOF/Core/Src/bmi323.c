#include "bmi323.h"
#include <stdio.h>
#include "stm32g4xx_hal_spi.h"
#include "stm32g474xx.h"

//init spi port before calling this function
uint8_t bmi323_init(bmi323 *bmi323_dev, SPI_HandleTypeDef *spi_port, GPIO_TypeDef *port, uint16_t pin){
  uint8_t tx_word[4];
  uint8_t rx_word[4] = {0};
  uint8_t status = 0;
  bmi323_dev->spi_port = spi_port;
  bmi323_dev->port = port;
  bmi323_dev->pin = pin;
  tx_word[1] = (BMI323_CHIP_ID << 8);
  tx_word[1] |= 0x80;
  tx_word[0] = 0x69;
  /*
  Okay so for one of these transmits we need to follow the following operation:bmi323_dev
  1. to read the register we want to:
    transmit first 8 bytes, then transmit a fake 8 bytes
    after we want to read 16 bytes. This should complete a single read 
  */
  //first we read do the dummy read to switch to spi mode
  HAL_GPIO_WritePin(bmi323_dev->port, bmi323_dev->pin, GPIO_PIN_RESET);
  status = HAL_SPI_TransmitReceive(bmi323_dev->spi_port, tx_word, rx_word, 2, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(bmi323_dev->port, bmi323_dev->pin, GPIO_PIN_SET);
  // rx_word = 0;
  HAL_GPIO_WritePin(bmi323_dev->port, bmi323_dev->pin, GPIO_PIN_RESET); 
  status = HAL_SPI_TransmitReceive(bmi323_dev->spi_port, tx_word, rx_word, 2, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(bmi323_dev->port, bmi323_dev->pin, GPIO_PIN_SET); 
  if(rx_word[3] == 0x43){
    return HAL_OK;
  }
  return HAL_ERROR;
}



/*
TODO: VIN DO THESE FUNCYIONS
*/
uint16_t bmi323_read(bmi323 *bmi323_dev, uint8_t reg){
    uint16_t data;
    // i2c_read_single(BMI323_I2C_ADDR, reg, &data, bmi323_dev->i2c_port);
    return data;
}

uint8_t bmi323_write(bmi323 *bmi323_dev, uint8_t reg, uint16_t data){
    // i2c_write(BMI323_I2C_ADDR, reg, data, bmi323_dev->i2c_port);
    return 1;
}

uint8_t bmi323_soft_reset(bmi323 *bmi323_dev){
    bmi323_write(bmi323_dev, BMI323_CMD, BMI323_CMD_SOFT_RESET);
    return 1;
}


/*
CALIBRATION PROCEDURE. VERY DANGEROUS. ONLY USE IF YOU KNOW WHAT YOU ARE DOING
THIS FUNCTION IS BLOCKING, CAN TAKE UP TO 3 SECONDS TO TIMEOUT
IF EVERYTHING IS DONE PROPERLY, THE FUNCTION WILL BLOCK FOR UP TO 500ms
THE FUNCTION WILL RETURN 1 IF CALIBRATION WAS SUCCESSFUL, 0 IF NOT
STEPS:
1. Disable all sensors
2. Enable the feature engine
3. Check if a calibration is already in progress
4. Check if the accelerometer is in high performance mode and the ODR is between 25 and 200 hertz
5. Start the calibration
6. Check if the calibration was successful
7. If not, reset the accelerometer and try again
8. If successful, reset the accelerometer and gyro to their original configuration
9. Done
*/
uint8_t bmi323_calib(bmi323 *bmi323_dev){
    //first we have to fucking start the feature engine
    //to do that we have to disable all sensors
    uint16_t acc_conf = bmi323_read(bmi323_dev, BMI323_ACC_CONF);
    uint16_t gyro_conf = bmi323_read(bmi323_dev, BMI323_GYR_CONF);
    uint32_t timeout_ref = 0;
    if((acc_conf & 0x7000) != 0b000){
        bmi323_write(bmi323_dev, BMI323_ACC_CONF, BMI323_ACC_CONF_RESET_VAL);
    }
    if((gyro_conf & 0x7000) != 0b000){
        bmi323_write(bmi323_dev, BMI323_GYR_CONF, BMI323_GYR_CONF_RESET_VAL);
    }
    //now we need to check if the feature engine has been enabled prior
    if((bmi323_read(bmi323_dev, BMI323_FEATURE_CTRL) & 0x0001) == 0b0){
        //then we write 0x012C to Feature Io 2
        bmi323_write(bmi323_dev, BMI323_FEATURE_IO2, BMI323_FEATURE_IO2_EN);
        //then we write 0x0001 to Feature Io status
        bmi323_write(bmi323_dev, BMI323_FEATURE_IO_STATUS, BMI323_FEATURE_IO_STS);
        //now we set the feature ctrol engine enable to 1
        bmi323_write(bmi323_dev, BMI323_FEATURE_CTRL, BMI323_FEATURE_CTRL_ENGINE_EN);
        //now we poll the feature engine
        timeout_ref = millis();
        while((bmi323_read(bmi323_dev, BMI323_FEATURE_I01) & 0x000F) != 0b001){
            if(millis() - timeout_ref > BMI323_TIMEOUT){
                D_println("Feature engine enable timeout");
                return 0;
            }
            continue;
        }
        D_println("enabled feature engine Hooray!");
    }
    //no need for an else statement as the feature engine is already enabled and we can continue
    //now we need to check if a calibration is already in progress
    //polls the bmi323 FEATURE I01 state untill the state is 0b00, this means calibration can start. 
    timeout_ref = millis();
    while(((bmi323_read(bmi323_dev, BMI323_FEATURE_I01) & (0x1800)) >> 11) != 0b00){
        if(millis() - timeout_ref > BMI323_TIMEOUT){
            D_println("Calibration state timeout");
            return 0;
        }
        continue;
    }
    //check the current configuration of the accelerometer and make sure it is in high performance adn 
    //ODR is between 25 and 200 hertz
    calibrate:
    //read the acc conf register
    uint16_t data = bmi323_read(bmi323_dev, BMI323_ACC_CONF) & 0x700F;
    //checks if we are in the correct configuration. if not skips if statement
    if(((data >> 12) == HIGH_PERF) && (((data & 0x000F) >= ODR_25) && ((data & 0x000F) <= ODR_200))){
        D_println("Starting calibration");
        //if it is, then we can start the calibration
        //first we check if the alernalte configuration acc mode is set to 0:
        if(((bmi323_read(bmi323_dev, BMI323_ALT_ACC_CONF) & 0x7000)>>11) != 0b000){
            //if it is, we set it to 0
            bmi323_write(bmi323_dev, BMI323_ALT_ACC_CONF, 0x0206);
        }
        //next we check if the alternate configuration gyro mode is set to 0:
        if(((bmi323_read(bmi323_dev, BMI323_ALT_GYR_CONF) & 0x7000)>>11) != 0b000){
            //if it is, we set it to 0
            bmi323_write(bmi323_dev, BMI323_ALT_GYR_CONF, 0x0206);
        }
        D_println("Alternate configurations set");
        //next we can actually send the command for calibration
        //reset all of the gyro calibration values
        bmi323_write(bmi323_dev, BMI323_GYR_DP_DGAIN_X, BMI323_ACC_DP_DGAIN_X_RESET_VAL);
        bmi323_write(bmi323_dev, BMI323_GYR_DP_DGAIN_Y, BMI323_ACC_DP_DGAIN_Y_RESET_VAL);
        bmi323_write(bmi323_dev, BMI323_GYR_DP_DGAIN_Z, BMI323_ACC_DP_DGAIN_Z_RESET_VAL);
        bmi323_write(bmi323_dev, BMI323_GYR_DP_OFF_X, BMI323_ACC_DP_OFF_X_RESET_VAL);
        bmi323_write(bmi323_dev, BMI323_GYR_DP_OFF_Y, BMI323_ACC_DP_OFF_Y_RESET_VAL);
        bmi323_write(bmi323_dev, BMI323_GYR_DP_OFF_Z, BMI323_ACC_DP_OFF_Z_RESET_VAL);
        //now we poll the state of the calibration untill we get 0b1
        D_println("Starting calibration");
        bmi323_write(bmi323_dev, BMI323_CMD, BMI323_CMD_CALIB);

        D_println("Polling calibration state");
        //check if the feature engine is enabled
        timeout_ref = millis();
        while(((bmi323_read(bmi323_dev, BMI323_FEATURE_I01) & 0x0010) >> 4 ) != 0b1){
            if(millis() - timeout_ref > BMI323_TIMEOUT){
                D_println("Feature engine enable timeout");
                return 0;
            }
            continue;
        }
        D_println("Calibration complete");
        if(((bmi323_read(bmi323_dev, BMI323_FEATURE_I01) & 0x0020) >> 5) == 0b1){
            // D_println("Calibration successful");
            // D_println("reseting values to original configuration");
            //cycle the acc
            bmi323_write(bmi323_dev, BMI323_ACC_CONF, BMI323_ACC_CONF_RESET_VAL);
            bmi323_write(bmi323_dev, BMI323_ACC_CONF, acc_conf);
            //cycle the gyro
            bmi323_write(bmi323_dev, BMI323_GYR_CONF, BMI323_GYR_CONF_RESET_VAL);
            // D_println(gyro_conf);
            bmi323_write(bmi323_dev, BMI323_GYR_CONF, gyro_conf);
            //display the calibration values
            // D_println(bmi323_read(bmi323_dev, BMI323_ACC_DP_DGAIN_X), HEX);
            // D_println(bmi323_read(bmi323_dev, BMI323_ACC_DP_DGAIN_Y), HEX);
            // D_println(bmi323_read(bmi323_dev, BMI323_ACC_DP_DGAIN_Z), HEX);
            // D_println(bmi323_read(bmi323_dev, BMI323_ACC_DP_OFF_X), HEX);
            // D_println(bmi323_read(bmi323_dev, BMI323_ACC_DP_OFF_Y), HEX);
            // D_println(bmi323_read(bmi323_dev, BMI323_ACC_DP_OFF_Z), HEX);
            return 1;
        }
        else{
            // D_println("Calibration failed");
            return 0;
        }

    }
    // D_println("Calibration failed");
    // D_println("reseting values trying again");
    //turns off the acc
    bmi323_write(bmi323_dev, BMI323_ACC_CONF, BMI323_ACC_CONF_RESET_VAL);
    //turn on the acc
    bmi323_write(bmi323_dev, BMI323_ACC_CONF, BMI323_ACC_CONF_RESET_VAL | 0x7000);
    //jumps to the calibration sequence
    goto calibrate;
    return 1;
}

uint8_t bmi323_calib_abort(bmi323 *bmi323_dev){
    bmi323_write(bmi323_dev, BMI323_CMD, BMI323_CMD_CALIB_ABORT);
    return 1;
}

/*
TODO:
Check the status of the acc, gyro and temp before returning the values
    if they are not ready return 0
    if they are ready return 1
    need to pass by reference the values to be returned

*/

uint16_t bmi323_read_acc_x(bmi323 *bmi323_dev){
    return bmi323_read(bmi323_dev, BMI323_ACC_X);
}

uint16_t bmi323_read_acc_y(bmi323 *bmi323_dev){
    return bmi323_read(bmi323_dev, BMI323_ACC_Y);
}

uint16_t bmi323_read_acc_z(bmi323 *bmi323_dev){
    return bmi323_read(bmi323_dev, BMI323_ACC_Z);
}

uint16_t bmi323_read_gyr_x(bmi323 *bmi323_dev){
    return bmi323_read(bmi323_dev, BMI323_GYR_X);
}

uint16_t bmi323_read_gyr_y(bmi323 *bmi323_dev){
    return bmi323_read(bmi323_dev, BMI323_GYR_Y);
}

uint16_t bmi323_read_gyr_z(bmi323 *bmi323_dev){
    return bmi323_read(bmi323_dev, BMI323_GYR_Z);
}

uint16_t bmi323_read_temp_data(bmi323 *bmi323_dev){
    return bmi323_read(bmi323_dev, BMI323_TEMP_DATA);
}

uint16_t bmi323_read_status(bmi323 *bmi323_dev){
    return bmi323_read(bmi323_dev, BMI323_STATUS);
}

uint16_t bmi323_read_err_reg(bmi323 *bmi323_dev){
    return bmi323_read(bmi323_dev, BMI323_ERR_REG);
}

uint16_t bmi323_read_chip_id(bmi323 *bmi323_dev){
    return bmi323_read(bmi323_dev, BMI323_CHIP_ID);
}

uint16_t bmi323_read_acc_conf(bmi323 *bmi323_dev){
    return bmi323_read(bmi323_dev, BMI323_ACC_CONF);
}

uint16_t bmi323_read_gyr_conf(bmi323 *bmi323_dev){
    return bmi323_read(bmi323_dev, BMI323_GYR_CONF);
}

uint8_t bmi323_enable_acc(bmi323 *bmi323_dev, uint8_t acc_mode, uint8_t acc_avg_num, uint8_t acc_bw, uint8_t acc_range, uint8_t acc_odr){
    //uint16_t acc_conf = bmi323_read_acc_conf(bmi323_dev);
    uint16_t new_conf = 0;
    new_conf |= acc_mode << 12;
    new_conf |= acc_avg_num << 8;
    new_conf |= acc_bw << 7;
    new_conf |= acc_range << 4;
    new_conf |= acc_odr;
    bmi323_write(bmi323_dev, BMI323_ACC_CONF, new_conf);
    return 1;
}

uint8_t bmi323_enable_gyro(bmi323 *bmi323_dev, uint8_t gyr_mode, uint8_t gyr_avg_num, uint8_t gyr_bw, uint8_t gyr_range, uint8_t gyr_odr){
    //uint16_t acc_conf = bmi323_read_acc_conf(bmi323_dev);
    uint16_t new_conf = 0;
    new_conf |= gyr_mode << 12;
    new_conf |= gyr_avg_num << 8;
    new_conf |= gyr_bw << 7;
    new_conf |= gyr_range << 4;
    new_conf |= gyr_odr;
    bmi323_write(bmi323_dev, BMI323_GYR_CONF, new_conf);
    return 1;
}



