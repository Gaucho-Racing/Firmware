# ADC Usage
This library is used to set up ADC for STM32 G4 boards. To initialize each individual ADC, use the `ADC_Init` function. This requires the clock and all initialization parameters to already be set. `ADC_Init` takes in the address to a struct of type `ADC_Init_Values`.

## Example Initialization of ADC_Init_Values
```cpp
ADC_Init_Values init_vals = {0};

init_vals.ADC = ADC1;
init_vals.PS_Value = PS_8;
init_vals.Res = RESOLUTION_12;
init_vals.Num_Pin_Port_Objs = 1;
Pin_Ports p = {LL_GPIO_PIN_0, GPIOA};  // add more pin outs if necessary
init_vals.Pins = &p;
init_vals.Num_Channels = 1;
Channel c = ADC_CHANNEL_1;
init_vals.Channels = &c;
SamplingTime s = SAMPLINGTIME_247CYCLES_5;
init_vals.SamplingTimes = &s;

ADC_Init(&init_vals);
```

## Parameters of ADC_Init_Values
- **ADC**: The ADC to be initialized
- **PS_Value**: Number of clock cycles in per tick, determines conversion frequency
- **Res**: Number of bits used to store the converted data, defines range/resolution of data
- **Num_Pin_Port_Objs**: Number of Pin_Ports Objects. Each pin port object defines all the pins for a specific port.
- **Pins**: Array of pin port objects to be initialized - allows each ADC to use multiple ports
- **Num_Channels**: Number of channels to be initialized
- **Channels**: Array of Channel objects to be initialized - output will be returned in this order
- **SamplingTime**: Array of oversampling rates - allows each channel to have its own oversampling rate (should match order of Channels)
```cpp
typedef struct {
	ADC_TypeDef *ADC;
	Pre_Scaler_Values PS_Value;
	Resolution Res;
	uint32_t Num_Pin_Port_Objs;
	Pin_Ports *Pins;
	uint32_t Num_Channels;
	Channel *Channels;
	SamplingTime *SamplingTimes;
} ADC_Init_Values;
```

# DMA Usage
This library also allows the use of DMA with ADC allowing continuous conversion and transfer of data between peripheral and memory without the use of interrupts or CPU cycles.To initialize each individual DMA, use the `DMA_Init` function. This requires the ADC is already initialized and all parameters to already be set. `DMA_Init` takes in the address to a struct of type `DMA_Init_Values`.

`ADC_Enable_And_Calibrate` must be called for each ADC after DMA initialization to start conversion. It takes in one parameter of type `ADC_TypeDef`, the ADC to be initialized (ex: ADC1).

## Example Initialization of DMA_Init_Values
```cpp
// Initialize DMA
DMA_Init_Values DMA_Init_Vals = {0};
DMA_Init_Vals.DMA = DMA1;
DMA_Init_Vals.ADC = ADC1;
DMA_Init_Vals.Channel = DMA_CHANNEL_1;
DMA_Init_Vals.Src_Address = LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA);
DMA_Init_Vals.Dest_Address = &buffers;
DMA_Init_Vals.Data_Size = Word;
DMA_Init_Vals.Priority = LOW;
DMA_Init(&DMA_Init_Vals);
LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
```

## Parameters of DMA_Init_Values
- **DMA**: The DMA to be initialized
- **ADC**: The ADC instance this DMA channel handles
- **Channel**: DMA channel to initialize - each channel handles a single instance of ADC
- **Src_Address**: Source address, use LL_ADC_DMA_GetRegAddr() to get the DMA register address of the ADCs
- **Dest_Address**: Pointers to destination buffer
- **Data_Size**: Size of data to be transfered, can be options of DMA_Data_Size
- **Priority**: Sets priority of each DMA channel
```cpp
typedef struct {
	DMA_TypeDef *DMA;
	ADC_TypeDef *ADC;
	DMA_Channel Channel;
	uint32_t Src_Address;
	const volatile void *Dest_Address;
	DMA_Data_Size Data_Size;
	DMA_Priority Priority;
} DMA_Init_Values;
```

## Smoothing
We implemented a smoothing function using an Exponential Moving Average. Call `ADC_UpdateAnalogValues_EMA` to update the old values with new analog values.

## Example Usage of ADC_UpdateAnalogValues_EMA
```cpp
#define NUM_SIGNALS 1
volatile uint16_t buffers[NUM_SIGNALS] = {0}; // Contains new values
uint16_t outputs[NUM_SIGNALS] = {0};	      // Updated weighted outputs
double alpha = 0.3;  // [0.0-1.0]: weight of the new value relative to old average

// call this every update to get the updated
ADC_UpdateAnalogValues_EMA(buffers, NUM_SIGNALS, alpha, outputs);
```

## Parameters of ADC_UpdateAnalogValues_EMA
- **new_values** An array of the most recent values
- **num_signals** Number of signals to be updated
- **alpha** The weight of the newest values (0.0 - 1.0)
- **weighted_output** Takes in the current output, overwrites with the new output
