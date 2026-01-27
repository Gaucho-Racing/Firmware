# ADC Usage
First of all read main.c and figure it out.

## Example
```cpp

```

## Functions
ADC: ADC1/2/3 ...

ADC_Group_Init(ADC1, PS_Val)

### ADC_Init(ADC, Resolution, Alignment)
### Resolution: RESOLUTION:_6/8/10/12 bytes, Alignment: LEFT/RIGHT
### ADC_Regular_Group_Init(ADC, sequence_length)


## Backend

## Smoothing
We are currently using a basic sliding window average with roughly 5 past data points, but we will rewrite it to use an exponential sliding function.

## DMA
