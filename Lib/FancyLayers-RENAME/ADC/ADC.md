# ADC Usage
This library is used to set up ADC for STM32 G4 boards. To initialize each individual ADC, use the `ADC_Init` function.

## Example
```cppset up ADC

```

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
