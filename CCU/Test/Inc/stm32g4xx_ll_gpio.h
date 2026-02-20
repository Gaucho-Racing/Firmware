
#ifndef STM32G4xx_LL_GPIO_H
#define STM32G4xx_LL_GPIO_H

#include <stdint.h>

typedef enum {
	SOFTWARE_OK_CONTROL_GPIO_Port,
	TSSI_R_CONTROL_GPIO_Port,
	TSSI_G_CONTROL_GPIO_Port,
	RTD_CONTROL_GPIO_Port
} GPIO_TypeDef;

typedef struct {
	uint32_t Pin; /*!< Specifies the GPIO pins to be configured.
			  This parameter can be any value of @ref GPIO_pins */

	uint32_t Mode; /*!< Specifies the operating mode for the selected pins.
			   This parameter can be a value of @ref GPIO_mode */

	uint32_t Pull; /*!< Specifies the Pull-up or Pull-Down activation for
			  the selected pins. This parameter can be a value of
			  @ref GPIO_pull */

	uint32_t Speed; /*!< Specifies the speed for the selected pins.
			    This parameter can be a value of @ref GPIO_speed */

	uint32_t Alternate; /*!< Peripheral to be connected to the selected pins
				 This parameter can be a value of @ref
			       GPIOEx_Alternate_function_selection */
} GPIO_InitTypeDef;

#define TSSI_R_CONTROL_Pin 0
#define TSSI_G_CONTROL_Pin 0
#define RTD_CONTROL_Pin 0
#define SOFTWARE_OK_CONTROL_Pin 0

void LL_GPIO_SetOutputPin(GPIO_TypeDef GPIOx, uint32_t PinMask);
void LL_GPIO_ResetOutputPin(GPIO_TypeDef GPIOx, uint32_t PinMask);


// void LL_GPIO_SetOutputPin(GPIO_TypeDef *GPIOx, uint32_t PinMask) {
//     (void)GPIOx;
//     (void)PinMask;
// }
//
// void LL_GPIO_ResetOutputPin(GPIO_TypeDef *GPIOx, uint32_t PinMask) {
//     (void)GPIOx;
//     (void)PinMask;
// }
//
// uint32_t HAL_GetTick(void) {
//     return 0;
// }
//
// uint32_t HAL_GetTickFreq(void){
//     return 1;
// }

#endif
