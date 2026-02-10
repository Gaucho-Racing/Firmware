#ifndef STM32G4xx_LL_GPIO_H
#define STM32G4xx_LL_GPIO_H

#include <stdint.h>

typedef enum { SOFTWARE_OK_CONTROL_GPIO_Port, TSSI_R_CONTROL_GPIO_Port, TSSI_G_CONTROL_GPIO_Port, RTD_CONTROL_GPIO_Port } GPIO_TypeDef_e;

#define TSSI_R_CONTROL_Pin 0
#define TSSI_G_CONTROL_Pin 0
#define RTD_CONTROL_Pin 0
#define SOFTWARE_OK_CONTROL_Pin 0

void LL_GPIO_SetOutputPin(GPIO_TypeDef_e GPIOx, uint32_t PinMask);
void LL_GPIO_ResetOutputPin(GPIO_TypeDef_e GPIOx, uint32_t PinMask);

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
