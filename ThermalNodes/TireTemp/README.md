# Tire Temp Sensor

Copied from [Sensor-Nodes](https://github.com/Gaucho-Racing/Sensor-Nodes/tree/main/Software/STM32G4/G431-MLX90640) from GR25

Driver library for MLX90640 that we are using/modified:
https://github.com/melexis/mlx90640-library

Important things to note for future development:

-Ta is ambient (sensor) temperature, To is object (what we care about) temperature, and Tr is reflected (background radiation) temperature
    -Tr can be approximated by ~Ta with some shifting. The datasheet recommends 8C shift in open air but setting Tr=Ta is good for an enclosed box.
-Emissivity is the blackbody radiation ratio of an objects temperature. Emissivity and reflectivity add up to 1.0 by conservation of energy. If an object's emissivity is 0.2 (such as steel), then it emits little of its own energy and reflects a lot of the background temperature.
-Once the MLX90640 sensor is powered on, it will continuously record temperatures. There may be other versions of this sensor which have a trigger measurement function and do not operate continuously (this is not the case for the sensors we are using as of 6/2/2026)
-The MLX90640 sensor requires two, separate frame requests to provide all of its pixel data (this can be even and odd rows, or checkerboard)
    1) Get first frame (50% of the total image)
    2) Get the ambient temperature
    3) Calculate To
    4) Correct bad pixels (these are known faults at the time of manufacturing)
    5) Using the SAME frame, To, and MLX90640 handler structs/arrays, get the second frame
        -By using the same arrays, the functions merely populate the remaining 50% of the values which were not altered in the first frame request
    6) Optionally requery the ambient temperature (you can prob do this infrequently)
    7) Calculate the To (only performs calcs on the new frame even though its in the same array as before)
    8) Correct bad pixels
-FDCAN has a max of 64 bytes per message
-We have 32x24 pixel camera
-We use uint16_t (2 bytes) to report the normalized temperature per pixel
    -Therefore, each can message sends 32 uint16_t's (an entire row). So, we send a total of 24 rows across 24 CAN messages
    -Be wary that pixel sizes may vary if we switch from MLX90640 to something else
