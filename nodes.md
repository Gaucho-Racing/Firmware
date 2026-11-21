# Nodes

| Node             | Primary Bus | Charging Bus | Data Bus | GR ID  |
| ---------------- | ----------- | ------------ | -------- | ------ |
| Debugger         | &check;     | &check;      | &check;  | `0x01` |
| ECU              | &check;     | &cross;      | &check;  | `0x02` |
| CCU              | &cross;     | &check;      | &cross;  | `0x02` |
| ACU              | &check;     | &check;      | &check;  | `0x03` |
| Dash Panel       | &check;     | &cross;      | &cross;  | `0x04` |
| Steering Wheel   | &check;     | &cross;      | &cross;  | `0x05` |
| GR Inverter      | &check;     | &cross;      | &cross;  | `0x06` |
| GLVPD 0          | &check;     | &cross;      | &cross;  | `0x10` |
| GLVPD 1          | &check;     | &cross;      | &cross;  | `0x11` |
| GLVPD 2          | &check;     | &cross;      | &cross;  | `0x12` |
| Fan Controller 0 | &check;     | &cross;      | &cross;  | `0x1D` |
| Fan Controller 1 | &check;     | &cross;      | &cross;  | `0x1E` |
| Fan Controller 2 | &check;     | &cross;      | &cross;  | `0x1F` |
| Suspension 0 F   | &cross;     | &cross;      | &check;  | `0x20` |
| Suspension 1 R   | &cross;     | &cross;      | &check;  | `0x21` |
| Inboard 0 FL     | &cross;     | &cross;      | &check;  | `0x26` |
| Inboard 1 FR     | &cross;     | &cross;      | &check;  | `0x27` |
| Inboard 2 RL     | &cross;     | &cross;      | &check;  | `0x28` |
| Inboard 3 RR     | &cross;     | &cross;      | &check;  | `0x29` |
| Outboard 0 FL    | &cross;     | &cross;      | &check;  | `0x2C` |
| Outboard 1 FR    | &cross;     | &cross;      | &check;  | `0x2D` |
| Outboard 2 RL    | &cross;     | &cross;      | &check;  | `0x2E` |
| Outboard 3 RR    | &cross;     | &cross;      | &check;  | `0x2F` |
| TCM              | &check;     | &cross;      | &check;  | `0x30` |
| ALL              | &check;     | &check;      | &check;  | `0x3F` |
| DTI Inverter     | &check;     | &cross;      | &cross;  | N/A    |
| Charger          | &cross;     | &check;      | &cross;  | N/A    |
| Energy Meter     | &cross;     | &check;      | &cross;  | N/A    |
| IMD              | &cross;     | &check;      | &cross;  | N/A    |
| Central          | &check;     | &cross;      | &cross;  | N/A    |
