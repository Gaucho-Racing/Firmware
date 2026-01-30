# ADC Usage
This library is used to set up ADC for STM32 G4 boards. To initialize each individual ADC, use the `ADC_Init` function. Using the `ADC_Init` function requires the clock and all initialization parameters to already be set. `ADC_Init` takes in the address to a struct of type `ADC_Init_Values`.

## Example
```cppset
	ADC_Init_Values init_vals = {0};

	init_vals.ADC = ADC1;
	init_vals.PS_Value = PS_8;
	init_vals.res = RESOLUTION_12;
	init_vals.Num_Pin_Port_Objs = 1;
	Pin_Ports p = {LL_GPIO_PIN_0, GPIOA};
	init_vals.Pins = &p;
	init_vals.Num_Channels = 1;
	Channel c = ADC_CHANNEL_1;
	init_vals.Channels = &c;
	SamplingTime s = SAMPLINGTIME_247CYCLES_5;
	init_vals.SamplingTimes = &s;

    ADC_Init(&init_vals);
```
ADC is
## Parameters of ADC_Init_Values
- ADC: The ADC to be initialized (from 1 to 5)
- PS_Value: Number of clock cycles in one tick, determines conversion speed
- Res: Number of bits used to store the converted data, defines range/resolution of data
- Num_Pin_Port_Objs: Number of Pin_Ports Objects. Each pin port object defines all the pins for a specific port.
-

## Functions
ADC: ADC1/2/3 ...

ADC_Group_Init(ADC1, PS_Val)

### `ADC_Init(ADC_Init_Values *Init_Values)`
```cpp
//ADC_Init_Values is the struct used to initialize each ADC
```
### Resolution: RESOLUTION:_6/8/10/12 bytes, Alignment: LEFT/RIGHT
### ADC_Regular_Group_Init(ADC, sequence_length)


## Backend

## Smoothing
We are currently using a basic sliding window average with roughly 5 past data points, but we will rewrite it to use an exponential sliding function.

## DMA
