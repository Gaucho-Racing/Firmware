# Nodes

<!--
- ECU P / D
- CCU C
- ACU P / C / D
- TCM P / D
- Suspension node front 1 D / rear 2 D
- Central node P
- Inboard node fl 1 D / fr 2 D / rl 3 D / rr 4 D
- Outboard node fl 1 D / fr 2 D / rl 3 D / rr 4 D
- Dashboard P
- Steering P
- GLVPD P
- Fan controller 1 P / 2 P / 3 P / 4 P / 5 P
- DTI inverter P
- GR inverter P
- IMD C
- Energy meter C
-->

<!--
0xXAABBBYY
AA -> GR node ID
BBB -> MSG ID
YY -> GR node ID

OR

0xABCDEFGH
Custom node id from vendor
-->

| Node | Primary Bus | Charging Bus | Data Bus | GR ID | Vendor ID |
| --- | --- | --- | --- | --- | --- |
| Debugger | &check; | &check; | &check; | `0x01` | &cross; |
| ECU | &check; | &cross; | &check; | `0x02` | &cross; |
| ACU | &check; | &check; | &check; | `0x03` | &check; |
| TCM | &check; | &cross; | &check; | `0x04` | &cross; |
| Dash Panel | &check; | &cross; | &cross; | `0x05` | &cross; |
| GR Inv | &check; | &cross; | &cross; | `0x08` | &cross; |
| Charging SDC | &cross; | &check; | &cross; | `0x0C` | &cross; |
| Fan Ctrl 1 | &check; | &cross; | &cross; | `0x0D` | &cross; |
| Fan Ctrl 2 | &check; | &cross; | &cross; | `0x0E` | &cross; |
| Fan Ctrl 3 | &check; | &cross; | &cross; | `0x0F` | &cross; |
| Suspension FL | &cross; | &cross; | &check; | `0x14` | &cross; |
| Suspension FR | &cross; | &cross; | &check; | `0x15` | &cross; |
| Suspension RL | &cross; | &cross; | &check; | `0x16` | &cross; |
| Suspension RR | &cross; | &cross; | &check; | `0x17` | &cross; |
| InboardFloor FL | &cross; | &cross; | &check; | `0x18` | &cross; |
| InboardFloor FR | &cross; | &cross; | &check; | `0x19` | &cross; |
| InboardFloor RL | &cross; | &cross; | &check; | `0x1A` | &cross; |
| InboardFloor RR | &cross; | &cross; | &check; | `0x1B` | &cross; |
| Central node | &check; | &cross; | &cross; | `0x20` | &cross; |
| Steering Wheel | &cross; | &cross; | &check; | `0x22` | &cross; |
| HV DC-DC | &check; | &cross; | &cross; | `0x2A` | &cross; |
| CCU | &cross; | &check; | &cross; | `0x02` | &cross; |
| Charger | &cross; | &check; | &cross; | N/A | &check; |
| DTI Inv | &check; | &cross; | &cross; | N/A | &check; |
| EM | &cross; | &check; | &cross; | N/A | &check; |
| IMD | &cross; | &check; | &cross; | N/A | &check; |
