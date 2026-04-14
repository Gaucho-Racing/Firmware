# Changelog

## 2026-04-13
- Added 8 new sensor nodes to CAN2 (Data bus) in `Web/can_topology.txt`: SAMM_Mag_1, SAMM_Mag_2, SAMM_ToF_1, SAMM_ToF_2, TireTemp_1, TireTemp_2, TireTemp_3, TireTemp_4.
- Created `Web/README.md` documenting the GRCAN Web Viewer/Editor: file inventory, script load order, test info, and topology file format.
- Added Ping routing in `GRCAN.CANdo` for all 8 new sensor nodes on CAN2: ECU sends Ping to each, each sends Ping to ECU.
- Added GR IDs for new sensors (0x10–0x17): SAMM_Mag_1/2, SAMM_ToF_1/2, TireTemp_1/2/3/4.
