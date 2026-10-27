#ifndef TIRE_EMISSIVITY_H
#define TIRE_EMISSIVITY_H

// Car tires range from 0.85 to 0.95, but are commonly stated as 0.95 in automotive industry
#define GR_TIRE_EMISSIVITY 0.95f

// This is the negative temperature difference (Celsius) from ambient to reflected
#define TIRETEMP_TA_SHIFT 8 // 8C when sensor is in open air

#endif // TIRE_EMISSIVITY_H
