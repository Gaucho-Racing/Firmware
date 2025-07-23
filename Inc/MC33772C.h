/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 * @file MC33772C.h
 *
 * Register map for MC33772C device.
 */

#ifndef MC33772C_MCU_IF_REG_H
#define MC33772C_MCU_IF_REG_H


#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>


/* Overall register macros */
#define MC33772C_MCU_NR_OF_REGISTERS (88U)
#define MC33772C_MCU_MAX_OFFSET (0x70U)

/* --------------------------------------------------------------------------
 * INIT (read-write): Initialization register
 * -------------------------------------------------------------------------- */
#define MC33772C_INIT_OFFSET                                 (0x1U)
#define MC33772C_INIT_POR_VAL                                (0x0U)

/* Field CID: Cluster Identifier, can be overridden by any combination different from all zeros. Not accessible with global write. */
#define MC33772C_INIT_CID_SHIFT                              (0x0U)
#define MC33772C_INIT_CID_MASK                               (0x3FU)
#define MC33772C_INIT_CID(x)                                 ((uint16_t)((uint16_t)(x) << MC33772C_INIT_CID_SHIFT) & MC33772C_INIT_CID_MASK)

/* Enumerated value DEFAULT: Default CID. */
#define MC33772C_INIT_CID_DEFAULT_ENUM_VAL                   (0x0U)

/* Field TPL2_TX_TERM: TPL transmitter termination for upper port (RDTX_OUT). */
#define MC33772C_INIT_TPL2_TX_TERM_SHIFT                     (0x6U)
#define MC33772C_INIT_TPL2_TX_TERM_MASK                      (0x40U)
#define MC33772C_INIT_TPL2_TX_TERM(x)                        ((uint16_t)((uint16_t)(x) << MC33772C_INIT_TPL2_TX_TERM_SHIFT) & MC33772C_INIT_TPL2_TX_TERM_MASK)

/* Enumerated value DISABLED: DISABLED */
#define MC33772C_INIT_TPL2_TX_TERM_DISABLED_ENUM_VAL         (0x0U)

/* Enumerated value ENABLED: ENABLED */
#define MC33772C_INIT_TPL2_TX_TERM_ENABLED_ENUM_VAL          (0x1U)

/* Field TPL1_TX_TERM: TPL transmitter termination for lower port (RDTX_IN). */
#define MC33772C_INIT_TPL1_TX_TERM_SHIFT                     (0x7U)
#define MC33772C_INIT_TPL1_TX_TERM_MASK                      (0x80U)
#define MC33772C_INIT_TPL1_TX_TERM(x)                        ((uint16_t)((uint16_t)(x) << MC33772C_INIT_TPL1_TX_TERM_SHIFT) & MC33772C_INIT_TPL1_TX_TERM_MASK)

/* Enumerated value DISABLED: DISABLED */
#define MC33772C_INIT_TPL1_TX_TERM_DISABLED_ENUM_VAL         (0x0U)

/* Enumerated value ENABLED: ENABLED */
#define MC33772C_INIT_TPL1_TX_TERM_ENABLED_ENUM_VAL          (0x1U)

/* Field BUS_FW: Bus forwarding, from one port to the opposite port and single side response to a READ request. */
#define MC33772C_INIT_BUS_FW_SHIFT                           (0x8U)
#define MC33772C_INIT_BUS_FW_MASK                            (0x100U)
#define MC33772C_INIT_BUS_FW(x)                              ((uint16_t)((uint16_t)(x) << MC33772C_INIT_BUS_FW_SHIFT) & MC33772C_INIT_BUS_FW_MASK)

/* Enumerated value ENABLED: Forwarding to neighboring node and response on both sides (only if CID != 0). */
#define MC33772C_INIT_BUS_FW_ENABLED_ENUM_VAL                (0x0U)

/* Enumerated value DISABLED: Stop forwarding and response on single side (side at which the request was received). */
#define MC33772C_INIT_BUS_FW_DISABLED_ENUM_VAL               (0x1U)

/* --------------------------------------------------------------------------
 * SYS_CFG_GLOBAL (write-only): System configuration global register
 * -------------------------------------------------------------------------- */
#define MC33772C_SYS_CFG_GLOBAL_OFFSET                       (0x2U)
#define MC33772C_SYS_CFG_GLOBAL_POR_VAL                      (0x0U)

/* Field GO2SLEEP (write-only): Go to sleep command */
#define MC33772C_SYS_CFG_GLOBAL_GO2SLEEP_SHIFT               (0x0U)
#define MC33772C_SYS_CFG_GLOBAL_GO2SLEEP_MASK                (0x1U)
#define MC33772C_SYS_CFG_GLOBAL_GO2SLEEP(x)                  ((uint16_t)((uint16_t)(x) << MC33772C_SYS_CFG_GLOBAL_GO2SLEEP_SHIFT) & MC33772C_SYS_CFG_GLOBAL_GO2SLEEP_MASK)

/* Enumerated value DISABLED: No reaction. */
#define MC33772C_SYS_CFG_GLOBAL_GO2SLEEP_DISABLED_ENUM_VAL   (0x0U)

/* Enumerated value ENABLED: Device goes to sleep mode after all conversions in progress are completed. */
#define MC33772C_SYS_CFG_GLOBAL_GO2SLEEP_ENABLED_ENUM_VAL    (0x1U)

/* --------------------------------------------------------------------------
 * SYS_CFG1 (read-write): System configuration register 1
 * -------------------------------------------------------------------------- */
#define MC33772C_SYS_CFG1_OFFSET                             (0x3U)
#define MC33772C_SYS_CFG1_POR_VAL                            (0x1001U)

/* Field WAVE_DC_BITx: Controls the off time of the heartbeat pulse. */
#define MC33772C_SYS_CFG1_WAVE_DC_BITX_SHIFT                 (0x1U)
#define MC33772C_SYS_CFG1_WAVE_DC_BITX_MASK                  (0x6U)
#define MC33772C_SYS_CFG1_WAVE_DC_BITX(x)                    ((uint16_t)((uint16_t)(x) << MC33772C_SYS_CFG1_WAVE_DC_BITX_SHIFT) & MC33772C_SYS_CFG1_WAVE_DC_BITX_MASK)

/* Enumerated value 500US: 500 us */
#define MC33772C_SYS_CFG1_WAVE_DC_BITX_500US_ENUM_VAL        (0x0U)

/* Enumerated value 1MS: 1 ms */
#define MC33772C_SYS_CFG1_WAVE_DC_BITX_1MS_ENUM_VAL          (0x1U)

/* Enumerated value 10MS: 10 ms */
#define MC33772C_SYS_CFG1_WAVE_DC_BITX_10MS_ENUM_VAL         (0x2U)

/* Enumerated value 100MS: 100 ms */
#define MC33772C_SYS_CFG1_WAVE_DC_BITX_100MS_ENUM_VAL        (0x3U)

/* Field FAULT_WAVE: FAULT pin wave form control bit. */
#define MC33772C_SYS_CFG1_FAULT_WAVE_SHIFT                   (0x3U)
#define MC33772C_SYS_CFG1_FAULT_WAVE_MASK                    (0x8U)
#define MC33772C_SYS_CFG1_FAULT_WAVE(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_SYS_CFG1_FAULT_WAVE_SHIFT) & MC33772C_SYS_CFG1_FAULT_WAVE_MASK)

/* Enumerated value DISABLED: FAULT pin has high or low level behavior. FAULT pin high, fault is present. FAULT pin low indicates no fault present. */
#define MC33772C_SYS_CFG1_FAULT_WAVE_DISABLED_ENUM_VAL       (0x0U)

/* Enumerated value ENABLED: FAULT pin has heartbeat wave when no fault is present. Pulse high time is fixed at 500 us. */
#define MC33772C_SYS_CFG1_FAULT_WAVE_ENABLED_ENUM_VAL        (0x1U)

/* Field SOFT_RST (write-only): Software reset. */
#define MC33772C_SYS_CFG1_SOFT_RST_SHIFT                     (0x4U)
#define MC33772C_SYS_CFG1_SOFT_RST_MASK                      (0x10U)
#define MC33772C_SYS_CFG1_SOFT_RST(x)                        ((uint16_t)((uint16_t)(x) << MC33772C_SYS_CFG1_SOFT_RST_SHIFT) & MC33772C_SYS_CFG1_SOFT_RST_MASK)

/* Enumerated value DISABLED: DISABLED */
#define MC33772C_SYS_CFG1_SOFT_RST_DISABLED_ENUM_VAL         (0x0U)

/* Enumerated value ACTIVE: Active software reset. */
#define MC33772C_SYS_CFG1_SOFT_RST_ACTIVE_ENUM_VAL           (0x1U)

/* Field CB_MANUAL_PAUSE: Cell balancing manual pause. */
#define MC33772C_SYS_CFG1_CB_MANUAL_PAUSE_SHIFT              (0x5U)
#define MC33772C_SYS_CFG1_CB_MANUAL_PAUSE_MASK               (0x20U)
#define MC33772C_SYS_CFG1_CB_MANUAL_PAUSE(x)                 ((uint16_t)((uint16_t)(x) << MC33772C_SYS_CFG1_CB_MANUAL_PAUSE_SHIFT) & MC33772C_SYS_CFG1_CB_MANUAL_PAUSE_MASK)

/* Enumerated value DISABLED: Disabled CB switches can be normally commanded on/off by the dedicated logic functions. */
#define MC33772C_SYS_CFG1_CB_MANUAL_PAUSE_DISABLED_ENUM_VAL  (0x0U)

/* Enumerated value ENABLED: CB switches are forced off, CB counters are not frozen. */
#define MC33772C_SYS_CFG1_CB_MANUAL_PAUSE_ENABLED_ENUM_VAL   (0x1U)

/* Field GO2DIAG (write-only): Commands the device to diag mode. Rewriting the GO2DIAG bit restarts the DIAG_TIMEOUT. */
#define MC33772C_SYS_CFG1_GO2DIAG_SHIFT                      (0x6U)
#define MC33772C_SYS_CFG1_GO2DIAG_MASK                       (0x40U)
#define MC33772C_SYS_CFG1_GO2DIAG(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_SYS_CFG1_GO2DIAG_SHIFT) & MC33772C_SYS_CFG1_GO2DIAG_MASK)

/* Enumerated value EXIT: Exit diag mode. */
#define MC33772C_SYS_CFG1_GO2DIAG_EXIT_ENUM_VAL              (0x0U)

/* Enumerated value ENTER: Enter diag mode (starts timer). */
#define MC33772C_SYS_CFG1_GO2DIAG_ENTER_ENUM_VAL             (0x1U)

/* Field DIAG_ST (read-only): Identifies when the device is in diag mode. */
#define MC33772C_SYS_CFG1_DIAG_ST_SHIFT                      (0x6U)
#define MC33772C_SYS_CFG1_DIAG_ST_MASK                       (0x40U)
#define MC33772C_SYS_CFG1_DIAG_ST(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_SYS_CFG1_DIAG_ST_SHIFT) & MC33772C_SYS_CFG1_DIAG_ST_MASK)

/* Enumerated value NOT_IN_DIAG: System is not in diag mode. */
#define MC33772C_SYS_CFG1_DIAG_ST_NOT_IN_DIAG_ENUM_VAL       (0x0U)

/* Enumerated value IN_DIAG: System is in diag mode. */
#define MC33772C_SYS_CFG1_DIAG_ST_IN_DIAG_ENUM_VAL           (0x1U)

/* Field CB_DRVEN: General enable or disable for all cell balance drivers. */
#define MC33772C_SYS_CFG1_CB_DRVEN_SHIFT                     (0x7U)
#define MC33772C_SYS_CFG1_CB_DRVEN_MASK                      (0x80U)
#define MC33772C_SYS_CFG1_CB_DRVEN(x)                        ((uint16_t)((uint16_t)(x) << MC33772C_SYS_CFG1_CB_DRVEN_SHIFT) & MC33772C_SYS_CFG1_CB_DRVEN_MASK)

/* Enumerated value DISABLED: DISABLED */
#define MC33772C_SYS_CFG1_CB_DRVEN_DISABLED_ENUM_VAL         (0x0U)

/* Enumerated value ENABLED: Enabled, each cell balance driver can be individually switched on and off by CB_xx_CFG register. */
#define MC33772C_SYS_CFG1_CB_DRVEN_ENABLED_ENUM_VAL          (0x1U)

/* Field I_MEAS_EN: Enable for current measurement chain. */
#define MC33772C_SYS_CFG1_I_MEAS_EN_SHIFT                    (0x9U)
#define MC33772C_SYS_CFG1_I_MEAS_EN_MASK                     (0x200U)
#define MC33772C_SYS_CFG1_I_MEAS_EN(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_SYS_CFG1_I_MEAS_EN_SHIFT) & MC33772C_SYS_CFG1_I_MEAS_EN_MASK)

/* Enumerated value DISABLED: DISABLED */
#define MC33772C_SYS_CFG1_I_MEAS_EN_DISABLED_ENUM_VAL        (0x0U)

/* Enumerated value ENABLED: Current measurement chain is enabled. */
#define MC33772C_SYS_CFG1_I_MEAS_EN_ENABLED_ENUM_VAL         (0x1U)

/* Field DIAG_TIMEOUT: Timer to trigger cyclic measurements in normal mode or sleep mode */
#define MC33772C_SYS_CFG1_DIAG_TIMEOUT_SHIFT                 (0xAU)
#define MC33772C_SYS_CFG1_DIAG_TIMEOUT_MASK                  (0x1C00U)
#define MC33772C_SYS_CFG1_DIAG_TIMEOUT(x)                    ((uint16_t)((uint16_t)(x) << MC33772C_SYS_CFG1_DIAG_TIMEOUT_SHIFT) & MC33772C_SYS_CFG1_DIAG_TIMEOUT_MASK)

/* Enumerated value NO_TIMER: No timer, not allowed to enter diag mode. */
#define MC33772C_SYS_CFG1_DIAG_TIMEOUT_NO_TIMER_ENUM_VAL     (0x0U)

/* Enumerated value 0_05S: 0.05 s */
#define MC33772C_SYS_CFG1_DIAG_TIMEOUT_0_05S_ENUM_VAL        (0x1U)

/* Enumerated value 0_1S: 0.1 s */
#define MC33772C_SYS_CFG1_DIAG_TIMEOUT_0_1S_ENUM_VAL         (0x2U)

/* Enumerated value 0_2S: 0.2 s */
#define MC33772C_SYS_CFG1_DIAG_TIMEOUT_0_2S_ENUM_VAL         (0x3U)

/* Enumerated value 1S: 1.0 s */
#define MC33772C_SYS_CFG1_DIAG_TIMEOUT_1S_ENUM_VAL           (0x4U)

/* Enumerated value 2S: 2.0 s */
#define MC33772C_SYS_CFG1_DIAG_TIMEOUT_2S_ENUM_VAL           (0x5U)

/* Enumerated value 4S: 4.0 s */
#define MC33772C_SYS_CFG1_DIAG_TIMEOUT_4S_ENUM_VAL           (0x6U)

/* Enumerated value 8S: 8.0 s */
#define MC33772C_SYS_CFG1_DIAG_TIMEOUT_8S_ENUM_VAL           (0x7U)

/* Field CYCLIC_TIMER: Timer to trigger cyclic measurements in normal mode or sleep mode. */
#define MC33772C_SYS_CFG1_CYCLIC_TIMER_SHIFT                 (0xDU)
#define MC33772C_SYS_CFG1_CYCLIC_TIMER_MASK                  (0xE000U)
#define MC33772C_SYS_CFG1_CYCLIC_TIMER(x)                    ((uint16_t)((uint16_t)(x) << MC33772C_SYS_CFG1_CYCLIC_TIMER_SHIFT) & MC33772C_SYS_CFG1_CYCLIC_TIMER_MASK)

/* Enumerated value DISABLED: Cyclic measure is disabled, whatever the mode. */
#define MC33772C_SYS_CFG1_CYCLIC_TIMER_DISABLED_ENUM_VAL     (0x0U)

/* Enumerated value CONTINUOUS: Continuous measurements. */
#define MC33772C_SYS_CFG1_CYCLIC_TIMER_CONTINUOUS_ENUM_VAL   (0x1U)

/* Enumerated value 0_1S: 0.1 s */
#define MC33772C_SYS_CFG1_CYCLIC_TIMER_0_1S_ENUM_VAL         (0x2U)

/* Enumerated value 0_2S: 0.2 s */
#define MC33772C_SYS_CFG1_CYCLIC_TIMER_0_2S_ENUM_VAL         (0x3U)

/* Enumerated value 1S: 1.0 s */
#define MC33772C_SYS_CFG1_CYCLIC_TIMER_1S_ENUM_VAL           (0x4U)

/* Enumerated value 2S: 2.0 s */
#define MC33772C_SYS_CFG1_CYCLIC_TIMER_2S_ENUM_VAL           (0x5U)

/* Enumerated value 4S: 4.0 s */
#define MC33772C_SYS_CFG1_CYCLIC_TIMER_4S_ENUM_VAL           (0x6U)

/* Enumerated value 8S: 8.0 s */
#define MC33772C_SYS_CFG1_CYCLIC_TIMER_8S_ENUM_VAL           (0x7U)

/* --------------------------------------------------------------------------
 * SYS_CFG2 (read-write): System configuration register 2
 * -------------------------------------------------------------------------- */
#define MC33772C_SYS_CFG2_OFFSET                             (0x4U)
#define MC33772C_SYS_CFG2_POR_VAL                            (0x334U)

/* Field HAMM_ENCOD: Hamming encoders */
#define MC33772C_SYS_CFG2_HAMM_ENCOD_SHIFT                   (0x0U)
#define MC33772C_SYS_CFG2_HAMM_ENCOD_MASK                    (0x1U)
#define MC33772C_SYS_CFG2_HAMM_ENCOD(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_SYS_CFG2_HAMM_ENCOD_SHIFT) & MC33772C_SYS_CFG2_HAMM_ENCOD_MASK)

/* Enumerated value DECODE: Decode - the DED Hamming decoders fulfill their job. */
#define MC33772C_SYS_CFG2_HAMM_ENCOD_DECODE_ENUM_VAL         (0x0U)

/* Enumerated value ENCODE: Encode - the DED hamming decoders generate the redundancy bits */
#define MC33772C_SYS_CFG2_HAMM_ENCOD_ENCODE_ENUM_VAL         (0x1U)

/* Field NUMB_ODD: Odd number of cells in the cluster (useful for open load diagnosis) */
#define MC33772C_SYS_CFG2_NUMB_ODD_SHIFT                     (0x1U)
#define MC33772C_SYS_CFG2_NUMB_ODD_MASK                      (0x2U)
#define MC33772C_SYS_CFG2_NUMB_ODD(x)                        ((uint16_t)((uint16_t)(x) << MC33772C_SYS_CFG2_NUMB_ODD_SHIFT) & MC33772C_SYS_CFG2_NUMB_ODD_MASK)

/* Enumerated value EVEN: Even configuration. */
#define MC33772C_SYS_CFG2_NUMB_ODD_EVEN_ENUM_VAL             (0x0U)

/* Enumerated value ODD: Odd configuration. */
#define MC33772C_SYS_CFG2_NUMB_ODD_ODD_ENUM_VAL              (0x1U)

/* Field VPRE_UV: VPRE undervoltage detection */
#define MC33772C_SYS_CFG2_VPRE_UV_SHIFT                      (0x2U)
#define MC33772C_SYS_CFG2_VPRE_UV_MASK                       (0x4U)
#define MC33772C_SYS_CFG2_VPRE_UV(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_SYS_CFG2_VPRE_UV_SHIFT) & MC33772C_SYS_CFG2_VPRE_UV_MASK)

/* Enumerated value NOT_DETECTED: No undervoltage detected (reset value). */
#define MC33772C_SYS_CFG2_VPRE_UV_NOT_DETECTED_ENUM_VAL      (0x0U)

/* Enumerated value DETECTED: Undervoltage detected on VPRE. */
#define MC33772C_SYS_CFG2_VPRE_UV_DETECTED_ENUM_VAL          (0x1U)

/* Field TIMEOUT_COMM: No communication timeout - flag in FAULT1_STATUS[COM_LOSS] if no communication during... */
#define MC33772C_SYS_CFG2_TIMEOUT_COMM_SHIFT                 (0x4U)
#define MC33772C_SYS_CFG2_TIMEOUT_COMM_MASK                  (0x30U)
#define MC33772C_SYS_CFG2_TIMEOUT_COMM(x)                    ((uint16_t)((uint16_t)(x) << MC33772C_SYS_CFG2_TIMEOUT_COMM_SHIFT) & MC33772C_SYS_CFG2_TIMEOUT_COMM_MASK)

/* Enumerated value 32MS: 32 ms */
#define MC33772C_SYS_CFG2_TIMEOUT_COMM_32MS_ENUM_VAL         (0x0U)

/* Enumerated value 64MS: 64 ms */
#define MC33772C_SYS_CFG2_TIMEOUT_COMM_64MS_ENUM_VAL         (0x1U)

/* Enumerated value 128MS: 128 ms */
#define MC33772C_SYS_CFG2_TIMEOUT_COMM_128MS_ENUM_VAL        (0x2U)

/* Enumerated value 256MS: 256 ms */
#define MC33772C_SYS_CFG2_TIMEOUT_COMM_256MS_ENUM_VAL        (0x3U)

/* Field FLT_RST_CFG: Fault reset configuration. */
#define MC33772C_SYS_CFG2_FLT_RST_CFG_SHIFT                  (0x6U)
#define MC33772C_SYS_CFG2_FLT_RST_CFG_MASK                   (0x3C0U)
#define MC33772C_SYS_CFG2_FLT_RST_CFG(x)                     ((uint16_t)((uint16_t)(x) << MC33772C_SYS_CFG2_FLT_RST_CFG_SHIFT) & MC33772C_SYS_CFG2_FLT_RST_CFG_MASK)

/* Enumerated value RESET_DISABLED: Disabled COM timeout (1024 ms) reset and OSC fault monitoring and reset. */
#define MC33772C_SYS_CFG2_FLT_RST_CFG_RESET_DISABLED_ENUM_VAL (0x3U)

/* Enumerated value OSC_MON: Enabled OSC fault monitoring. */
#define MC33772C_SYS_CFG2_FLT_RST_CFG_OSC_MON_ENUM_VAL       (0x5U)

/* Enumerated value OSC_MON_RESET: Enabled OSC fault monitoring and reset. */
#define MC33772C_SYS_CFG2_FLT_RST_CFG_OSC_MON_RESET_ENUM_VAL (0x6U)

/* Enumerated value COM_RESET: Enabled COM timeout (1024 ms) reset. */
#define MC33772C_SYS_CFG2_FLT_RST_CFG_COM_RESET_ENUM_VAL     (0x9U)

/* Enumerated value COM_RESET_OSC_MON: Enabled COM timeout (1024 ms) reset and OSC fault monitoring. */
#define MC33772C_SYS_CFG2_FLT_RST_CFG_COM_RESET_OSC_MON_ENUM_VAL (0xAU)

/* Enumerated value COM_RESET_OSC_MON_RESET: Enabled COM timeout (1024 ms) reset and OSC fault monitoring and reset. */
#define MC33772C_SYS_CFG2_FLT_RST_CFG_COM_RESET_OSC_MON_RESET_ENUM_VAL (0xCU)

/* Field PREVIOUS_STATE (read-only): Information about the previous state of the device. */
#define MC33772C_SYS_CFG2_PREVIOUS_STATE_SHIFT               (0xAU)
#define MC33772C_SYS_CFG2_PREVIOUS_STATE_MASK                (0x1C00U)
#define MC33772C_SYS_CFG2_PREVIOUS_STATE(x)                  ((uint16_t)((uint16_t)(x) << MC33772C_SYS_CFG2_PREVIOUS_STATE_SHIFT) & MC33772C_SYS_CFG2_PREVIOUS_STATE_MASK)

/* Enumerated value INIT: The device is coming from INIT state. */
#define MC33772C_SYS_CFG2_PREVIOUS_STATE_INIT_ENUM_VAL       (0x0U)

/* Enumerated value IDLE: The device is coming from IDLE state. */
#define MC33772C_SYS_CFG2_PREVIOUS_STATE_IDLE_ENUM_VAL       (0x1U)

/* Enumerated value NORMAL: The device is coming from NORMAL state. */
#define MC33772C_SYS_CFG2_PREVIOUS_STATE_NORMAL_ENUM_VAL     (0x2U)

/* Enumerated value DIAG: The device is coming from DIAG state. */
#define MC33772C_SYS_CFG2_PREVIOUS_STATE_DIAG_ENUM_VAL       (0x3U)

/* Enumerated value CYCLIC_WUP: The device is coming from CYCLIC_WUP state. */
#define MC33772C_SYS_CFG2_PREVIOUS_STATE_CYCLIC_WUP_ENUM_VAL (0x6U)

/* Enumerated value SLEEP: The device is coming from SLEEP state. */
#define MC33772C_SYS_CFG2_PREVIOUS_STATE_SLEEP_ENUM_VAL      (0x7U)

/* --------------------------------------------------------------------------
 * SYS_DIAG (read-write): System diagnostics register
 * -------------------------------------------------------------------------- */
#define MC33772C_SYS_DIAG_OFFSET                             (0x5U)
#define MC33772C_SYS_DIAG_POR_VAL                            (0x0U)

/* Field CB_OL_EVEN: Control bit used to control the cell balance open load EVEB detection switches. */
#define MC33772C_SYS_DIAG_CB_OL_EVEN_SHIFT                   (0x0U)
#define MC33772C_SYS_DIAG_CB_OL_EVEN_MASK                    (0x1U)
#define MC33772C_SYS_DIAG_CB_OL_EVEN(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_SYS_DIAG_CB_OL_EVEN_SHIFT) & MC33772C_SYS_DIAG_CB_OL_EVEN_MASK)

/* Enumerated value OPEN: EVEN cell balance open load detection switches are open. */
#define MC33772C_SYS_DIAG_CB_OL_EVEN_OPEN_ENUM_VAL           (0x0U)

/* Enumerated value CLOSED: EVEN cell balance open load detection switches are closed. */
#define MC33772C_SYS_DIAG_CB_OL_EVEN_CLOSED_ENUM_VAL         (0x1U)

/* Field CB_OL_ODD: Control bit used to control the cell balance open load ODD detection switches. */
#define MC33772C_SYS_DIAG_CB_OL_ODD_SHIFT                    (0x1U)
#define MC33772C_SYS_DIAG_CB_OL_ODD_MASK                     (0x2U)
#define MC33772C_SYS_DIAG_CB_OL_ODD(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_SYS_DIAG_CB_OL_ODD_SHIFT) & MC33772C_SYS_DIAG_CB_OL_ODD_MASK)

/* Enumerated value OPEN: ODD cell balance open load detection switches are open. */
#define MC33772C_SYS_DIAG_CB_OL_ODD_OPEN_ENUM_VAL            (0x0U)

/* Enumerated value CLOSED: ODD cell balance open load detection switches are closed. */
#define MC33772C_SYS_DIAG_CB_OL_ODD_CLOSED_ENUM_VAL          (0x1U)

/* Field CT_OL_EVEN: Control bit used to control the even numbered cell terminal open detect switches. */
#define MC33772C_SYS_DIAG_CT_OL_EVEN_SHIFT                   (0x2U)
#define MC33772C_SYS_DIAG_CT_OL_EVEN_MASK                    (0x4U)
#define MC33772C_SYS_DIAG_CT_OL_EVEN(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_SYS_DIAG_CT_OL_EVEN_SHIFT) & MC33772C_SYS_DIAG_CT_OL_EVEN_MASK)

/* Enumerated value OPEN: Even switches are open. */
#define MC33772C_SYS_DIAG_CT_OL_EVEN_OPEN_ENUM_VAL           (0x0U)

/* Enumerated value CLOSED: Even switches are closed. */
#define MC33772C_SYS_DIAG_CT_OL_EVEN_CLOSED_ENUM_VAL         (0x1U)

/* Field CT_OL_ODD: Control bit used to control the odd numbered cell terminal open detect switches. */
#define MC33772C_SYS_DIAG_CT_OL_ODD_SHIFT                    (0x3U)
#define MC33772C_SYS_DIAG_CT_OL_ODD_MASK                     (0x8U)
#define MC33772C_SYS_DIAG_CT_OL_ODD(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_SYS_DIAG_CT_OL_ODD_SHIFT) & MC33772C_SYS_DIAG_CT_OL_ODD_MASK)

/* Enumerated value OPEN: Odd switches are open. */
#define MC33772C_SYS_DIAG_CT_OL_ODD_OPEN_ENUM_VAL            (0x0U)

/* Enumerated value CLOSED: Odd switches are closed. */
#define MC33772C_SYS_DIAG_CT_OL_ODD_CLOSED_ENUM_VAL          (0x1U)

/* Field CT_OV_UV: OV and UV diagnostic is enabled. This bit must be set to logic 0 when performing CT open load diagnostic. */
#define MC33772C_SYS_DIAG_CT_OV_UV_SHIFT                     (0x4U)
#define MC33772C_SYS_DIAG_CT_OV_UV_MASK                      (0x10U)
#define MC33772C_SYS_DIAG_CT_OV_UV(x)                        ((uint16_t)((uint16_t)(x) << MC33772C_SYS_DIAG_CT_OV_UV_SHIFT) & MC33772C_SYS_DIAG_CT_OV_UV_MASK)

/* Enumerated value DISABLED: OV and UV diagnostic disabled. */
#define MC33772C_SYS_DIAG_CT_OV_UV_DISABLED_ENUM_VAL         (0x0U)

/* Enumerated value ENABLED: OV and UV diagnostic enabled. */
#define MC33772C_SYS_DIAG_CT_OV_UV_ENABLED_ENUM_VAL          (0x1U)

/* Field CT_LEAK_DIAG: Control bit used in terminal leakage detection. Commands the MUX to route the CTx/CBx pin to ADC1-A,B converters. This bit must be exclusive vs. DA_DIAG. */
#define MC33772C_SYS_DIAG_CT_LEAK_DIAG_SHIFT                 (0x5U)
#define MC33772C_SYS_DIAG_CT_LEAK_DIAG_MASK                  (0x20U)
#define MC33772C_SYS_DIAG_CT_LEAK_DIAG(x)                    ((uint16_t)((uint16_t)(x) << MC33772C_SYS_DIAG_CT_LEAK_DIAG_SHIFT) & MC33772C_SYS_DIAG_CT_LEAK_DIAG_MASK)

/* Enumerated value NORMAL: Normal operation, CTx are MUXed to converter. */
#define MC33772C_SYS_DIAG_CT_LEAK_DIAG_NORMAL_ENUM_VAL       (0x0U)

/* Enumerated value DIFF: Difference between CT and CB pins are routed to the analog front end, to be converted. */
#define MC33772C_SYS_DIAG_CT_LEAK_DIAG_DIFF_ENUM_VAL         (0x1U)

/* Field POLARITY: Control bit used in terminal leakage detection. Controls the polarity between the level shifter and the ADC1-A and ADC1-B converters. */
#define MC33772C_SYS_DIAG_POLARITY_SHIFT                     (0x6U)
#define MC33772C_SYS_DIAG_POLARITY_MASK                      (0x40U)
#define MC33772C_SYS_DIAG_POLARITY(x)                        ((uint16_t)((uint16_t)(x) << MC33772C_SYS_DIAG_POLARITY_SHIFT) & MC33772C_SYS_DIAG_POLARITY_MASK)

/* Enumerated value NONINVERTED: Noninverted. */
#define MC33772C_SYS_DIAG_POLARITY_NONINVERTED_ENUM_VAL      (0x0U)

/* Enumerated value INVERTED: Inverted. */
#define MC33772C_SYS_DIAG_POLARITY_INVERTED_ENUM_VAL         (0x1U)

/* Field DA_DIAG: Cell voltage channel functional verification. Diagnostic mode function only. */
#define MC33772C_SYS_DIAG_DA_DIAG_SHIFT                      (0x7U)
#define MC33772C_SYS_DIAG_DA_DIAG_MASK                       (0x80U)
#define MC33772C_SYS_DIAG_DA_DIAG(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_SYS_DIAG_DA_DIAG_SHIFT) & MC33772C_SYS_DIAG_DA_DIAG_MASK)

/* Enumerated value DISABLED: No check. */
#define MC33772C_SYS_DIAG_DA_DIAG_DISABLED_ENUM_VAL          (0x0U)

/* Enumerated value ENABLED: Check is enabled (floating Zener conversion, ground Zener measurement added, comparison). */
#define MC33772C_SYS_DIAG_DA_DIAG_ENABLED_ENUM_VAL           (0x1U)

/* Field ANx_TEMP_DIAG: Control bit to activate the OT/UT diagnostic on GPIOx configured as ANx ratiometric or single ended ADC input. */
#define MC33772C_SYS_DIAG_ANX_TEMP_DIAG_SHIFT                (0x8U)
#define MC33772C_SYS_DIAG_ANX_TEMP_DIAG_MASK                 (0x100U)
#define MC33772C_SYS_DIAG_ANX_TEMP_DIAG(x)                   ((uint16_t)((uint16_t)(x) << MC33772C_SYS_DIAG_ANX_TEMP_DIAG_SHIFT) & MC33772C_SYS_DIAG_ANX_TEMP_DIAG_MASK)

/* Enumerated value DISABLED: Diagnostic inactive. */
#define MC33772C_SYS_DIAG_ANX_TEMP_DIAG_DISABLED_ENUM_VAL    (0x0U)

/* Enumerated value ENABLED: Diagnostic active. */
#define MC33772C_SYS_DIAG_ANX_TEMP_DIAG_ENABLED_ENUM_VAL     (0x1U)

/* Field ANx_OL_DIAG: ANx open load diagnostic control bit. Used to activate the pull down on GPIO input pins. */
#define MC33772C_SYS_DIAG_ANX_OL_DIAG_SHIFT                  (0x9U)
#define MC33772C_SYS_DIAG_ANX_OL_DIAG_MASK                   (0x200U)
#define MC33772C_SYS_DIAG_ANX_OL_DIAG(x)                     ((uint16_t)((uint16_t)(x) << MC33772C_SYS_DIAG_ANX_OL_DIAG_SHIFT) & MC33772C_SYS_DIAG_ANX_OL_DIAG_MASK)

/* Enumerated value DISABLED: Diagnostic disabled. */
#define MC33772C_SYS_DIAG_ANX_OL_DIAG_DISABLED_ENUM_VAL      (0x0U)

/* Enumerated value ENABLED: Diagnostic enabled. */
#define MC33772C_SYS_DIAG_ANX_OL_DIAG_ENABLED_ENUM_VAL       (0x1U)

/* Field ISENSE_OL_DIAG: ISENSE open load diagnostic control bit. Enables or disables internal pull-up resistors on the ISENSE input pins. */
#define MC33772C_SYS_DIAG_ISENSE_OL_DIAG_SHIFT               (0xAU)
#define MC33772C_SYS_DIAG_ISENSE_OL_DIAG_MASK                (0x400U)
#define MC33772C_SYS_DIAG_ISENSE_OL_DIAG(x)                  ((uint16_t)((uint16_t)(x) << MC33772C_SYS_DIAG_ISENSE_OL_DIAG_SHIFT) & MC33772C_SYS_DIAG_ISENSE_OL_DIAG_MASK)

/* Enumerated value DISABLED: Disabled. */
#define MC33772C_SYS_DIAG_ISENSE_OL_DIAG_DISABLED_ENUM_VAL   (0x0U)

/* Enumerated value ENABLED: Enabled. */
#define MC33772C_SYS_DIAG_ISENSE_OL_DIAG_ENABLED_ENUM_VAL    (0x1U)

/* Field I_MUX: Allows user to select between various inputs to PGA to be converted by ADC2. */
#define MC33772C_SYS_DIAG_I_MUX_SHIFT                        (0xBU)
#define MC33772C_SYS_DIAG_I_MUX_MASK                         (0x1800U)
#define MC33772C_SYS_DIAG_I_MUX(x)                           ((uint16_t)((uint16_t)(x) << MC33772C_SYS_DIAG_I_MUX_SHIFT) & MC33772C_SYS_DIAG_I_MUX_MASK)

/* Enumerated value ISENSE: (ISENSE+, ISENSE?) */
#define MC33772C_SYS_DIAG_I_MUX_ISENSE_ENUM_VAL              (0x0U)

/* Enumerated value GPIO_5_6: (GPIO5, GPIO6) */
#define MC33772C_SYS_DIAG_I_MUX_GPIO_5_6_ENUM_VAL            (0x1U)

/* Enumerated value VREF_DIAG: Calibrated internal reference (VREF_DIAG). */
#define MC33772C_SYS_DIAG_I_MUX_VREF_DIAG_ENUM_VAL           (0x2U)

/* Enumerated value PGA_ZERO: PGA zero (PGA differential inputs terminated to ground). */
#define MC33772C_SYS_DIAG_I_MUX_PGA_ZERO_ENUM_VAL            (0x3U)

/* Field FAULT_DIAG: FAULT pin driver command */
#define MC33772C_SYS_DIAG_FAULT_DIAG_SHIFT                   (0xFU)
#define MC33772C_SYS_DIAG_FAULT_DIAG_MASK                    (0x8000U)
#define MC33772C_SYS_DIAG_FAULT_DIAG(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_SYS_DIAG_FAULT_DIAG_SHIFT) & MC33772C_SYS_DIAG_FAULT_DIAG_MASK)

/* Enumerated value PACK_CTRL: No FAULT pin drive, FAULT pin is under command of the pack controller. */
#define MC33772C_SYS_DIAG_FAULT_DIAG_PACK_CTRL_ENUM_VAL      (0x0U)

/* Enumerated value FORCED_HIGH: FAULT pin is forced to high level. */
#define MC33772C_SYS_DIAG_FAULT_DIAG_FORCED_HIGH_ENUM_VAL    (0x1U)

/* --------------------------------------------------------------------------
 * ADC_CFG (read-write): ADC configuration register
 * -------------------------------------------------------------------------- */
#define MC33772C_ADC_CFG_OFFSET                              (0x6U)
#define MC33772C_ADC_CFG_POR_VAL                             (0x417U)

/* Field ADC2_DEF: ADC2 measurement resolution. */
#define MC33772C_ADC_CFG_ADC2_DEF_SHIFT                      (0x0U)
#define MC33772C_ADC_CFG_ADC2_DEF_MASK                       (0x3U)
#define MC33772C_ADC_CFG_ADC2_DEF(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_ADC_CFG_ADC2_DEF_SHIFT) & MC33772C_ADC_CFG_ADC2_DEF_MASK)

/* Enumerated value 13_BIT: 13 bit */
#define MC33772C_ADC_CFG_ADC2_DEF_13_BIT_ENUM_VAL            (0x0U)

/* Enumerated value 14_BIT: 14 bit */
#define MC33772C_ADC_CFG_ADC2_DEF_14_BIT_ENUM_VAL            (0x1U)

/* Enumerated value 15_BIT: 15 bit */
#define MC33772C_ADC_CFG_ADC2_DEF_15_BIT_ENUM_VAL            (0x2U)

/* Enumerated value 16_BIT: 16 bit */
#define MC33772C_ADC_CFG_ADC2_DEF_16_BIT_ENUM_VAL            (0x3U)

/* Field ADC1_B_DEF: ADC1_B measurement resolution. */
#define MC33772C_ADC_CFG_ADC1_B_DEF_SHIFT                    (0x2U)
#define MC33772C_ADC_CFG_ADC1_B_DEF_MASK                     (0xCU)
#define MC33772C_ADC_CFG_ADC1_B_DEF(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_ADC_CFG_ADC1_B_DEF_SHIFT) & MC33772C_ADC_CFG_ADC1_B_DEF_MASK)

/* Enumerated value 13_BIT: 13 bit */
#define MC33772C_ADC_CFG_ADC1_B_DEF_13_BIT_ENUM_VAL          (0x0U)

/* Enumerated value 14_BIT: 14 bit */
#define MC33772C_ADC_CFG_ADC1_B_DEF_14_BIT_ENUM_VAL          (0x1U)

/* Enumerated value 15_BIT: 15 bit */
#define MC33772C_ADC_CFG_ADC1_B_DEF_15_BIT_ENUM_VAL          (0x2U)

/* Enumerated value 16_BIT: 16 bit */
#define MC33772C_ADC_CFG_ADC1_B_DEF_16_BIT_ENUM_VAL          (0x3U)

/* Field ADC1_A_DEF: None */
#define MC33772C_ADC_CFG_ADC1_A_DEF_SHIFT                    (0x4U)
#define MC33772C_ADC_CFG_ADC1_A_DEF_MASK                     (0x30U)
#define MC33772C_ADC_CFG_ADC1_A_DEF(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_ADC_CFG_ADC1_A_DEF_SHIFT) & MC33772C_ADC_CFG_ADC1_A_DEF_MASK)

/* Enumerated value 13_BIT: 13 bit */
#define MC33772C_ADC_CFG_ADC1_A_DEF_13_BIT_ENUM_VAL          (0x0U)

/* Enumerated value 14_BIT: 14 bit */
#define MC33772C_ADC_CFG_ADC1_A_DEF_14_BIT_ENUM_VAL          (0x1U)

/* Enumerated value 15_BIT: 15 bit */
#define MC33772C_ADC_CFG_ADC1_A_DEF_15_BIT_ENUM_VAL          (0x2U)

/* Enumerated value 16_BIT: 16 bit */
#define MC33772C_ADC_CFG_ADC1_A_DEF_16_BIT_ENUM_VAL          (0x3U)

/* Field CC_RST (write-only): None */
#define MC33772C_ADC_CFG_CC_RST_SHIFT                        (0x7U)
#define MC33772C_ADC_CFG_CC_RST_MASK                         (0x80U)
#define MC33772C_ADC_CFG_CC_RST(x)                           ((uint16_t)((uint16_t)(x) << MC33772C_ADC_CFG_CC_RST_SHIFT) & MC33772C_ADC_CFG_CC_RST_MASK)

/* Enumerated value NO_ACTION: No action. */
#define MC33772C_ADC_CFG_CC_RST_NO_ACTION_ENUM_VAL           (0x0U)

/* Enumerated value RESET: Reset coulomb counter registers COULOMB_CNT1 and COULOMB_CNT2 and the CC_NB_SAMPLES registers. */
#define MC33772C_ADC_CFG_CC_RST_RESET_ENUM_VAL               (0x1U)

/* Field PGA_GAIN (write-only): Define the gain of the ADC2 programmable gain amplifier. */
#define MC33772C_ADC_CFG_PGA_GAIN_SHIFT                      (0x8U)
#define MC33772C_ADC_CFG_PGA_GAIN_MASK                       (0x700U)
#define MC33772C_ADC_CFG_PGA_GAIN(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_ADC_CFG_PGA_GAIN_SHIFT) & MC33772C_ADC_CFG_PGA_GAIN_MASK)

/* Enumerated value 4: 4 */
#define MC33772C_ADC_CFG_PGA_GAIN_4_ENUM_VAL                 (0x0U)

/* Enumerated value 16: 16 */
#define MC33772C_ADC_CFG_PGA_GAIN_16_ENUM_VAL                (0x1U)

/* Enumerated value 64: 64 */
#define MC33772C_ADC_CFG_PGA_GAIN_64_ENUM_VAL                (0x2U)

/* Enumerated value 256: 256 */
#define MC33772C_ADC_CFG_PGA_GAIN_256_ENUM_VAL               (0x3U)

/* Enumerated value AUTO: Automatic gain selection (internally adjusted). */
#define MC33772C_ADC_CFG_PGA_GAIN_AUTO_ENUM_VAL              (0x4U)

/* Field PGA_GAIN_AMP_S (read-only): Current gain of the ADC2 programmable gain amplifier (information available only if SYS_CFG1[I_MEAS_EN] = 1). */
#define MC33772C_ADC_CFG_PGA_GAIN_AMP_S_SHIFT                (0x8U)
#define MC33772C_ADC_CFG_PGA_GAIN_AMP_S_MASK                 (0x300U)
#define MC33772C_ADC_CFG_PGA_GAIN_AMP_S(x)                   ((uint16_t)((uint16_t)(x) << MC33772C_ADC_CFG_PGA_GAIN_AMP_S_SHIFT) & MC33772C_ADC_CFG_PGA_GAIN_AMP_S_MASK)

/* Enumerated value 4: 4 */
#define MC33772C_ADC_CFG_PGA_GAIN_AMP_S_4_ENUM_VAL           (0x0U)

/* Enumerated value 16: 16 */
#define MC33772C_ADC_CFG_PGA_GAIN_AMP_S_16_ENUM_VAL          (0x1U)

/* Enumerated value 64: 64 */
#define MC33772C_ADC_CFG_PGA_GAIN_AMP_S_64_ENUM_VAL          (0x2U)

/* Enumerated value 256: 256 */
#define MC33772C_ADC_CFG_PGA_GAIN_AMP_S_256_ENUM_VAL         (0x3U)

/* Field PGA_GAIN_S (read-only): Automatic gain mode status (information available only if SYS_CFG1[I_MEAS_EN] = 1). */
#define MC33772C_ADC_CFG_PGA_GAIN_S_SHIFT                    (0xAU)
#define MC33772C_ADC_CFG_PGA_GAIN_S_MASK                     (0x400U)
#define MC33772C_ADC_CFG_PGA_GAIN_S(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_ADC_CFG_PGA_GAIN_S_SHIFT) & MC33772C_ADC_CFG_PGA_GAIN_S_MASK)

/* Enumerated value FIXED: Fixed gain. */
#define MC33772C_ADC_CFG_PGA_GAIN_S_FIXED_ENUM_VAL           (0x0U)

/* Enumerated value AUTO: Automatic gain control. */
#define MC33772C_ADC_CFG_PGA_GAIN_S_AUTO_ENUM_VAL            (0x1U)

/* Field SOC (write-only): Control bit to command the MC33772C to initiate a conversion sequence. Writing SOC to 0 has no effect on an ongoing conversion sequence. */
#define MC33772C_ADC_CFG_SOC_SHIFT                           (0xBU)
#define MC33772C_ADC_CFG_SOC_MASK                            (0x800U)
#define MC33772C_ADC_CFG_SOC(x)                              ((uint16_t)((uint16_t)(x) << MC33772C_ADC_CFG_SOC_SHIFT) & MC33772C_ADC_CFG_SOC_MASK)

/* Enumerated value DISABLED: Writing SOC to 0 has no effect on an ongoing conversion sequence. */
#define MC33772C_ADC_CFG_SOC_DISABLED_ENUM_VAL               (0x0U)

/* Enumerated value ENABLED: Enabled. Initiate a conversion sequence. */
#define MC33772C_ADC_CFG_SOC_ENABLED_ENUM_VAL                (0x1U)

/* Field EOC_N (read-only): End of conversion flag. */
#define MC33772C_ADC_CFG_EOC_N_SHIFT                         (0xBU)
#define MC33772C_ADC_CFG_EOC_N_MASK                          (0x800U)
#define MC33772C_ADC_CFG_EOC_N(x)                            ((uint16_t)((uint16_t)(x) << MC33772C_ADC_CFG_EOC_N_SHIFT) & MC33772C_ADC_CFG_EOC_N_MASK)

/* Enumerated value COMPLETED: Device has completed the commanded conversion. */
#define MC33772C_ADC_CFG_EOC_N_COMPLETED_ENUM_VAL            (0x0U)

/* Enumerated value IN_PROGRESS: Device is performing the commanded conversion. */
#define MC33772C_ADC_CFG_EOC_N_IN_PROGRESS_ENUM_VAL          (0x1U)

/* Field AVG: With each conversion request, the number of samples to be averages can be configured. */
#define MC33772C_ADC_CFG_AVG_SHIFT                           (0xCU)
#define MC33772C_ADC_CFG_AVG_MASK                            (0xF000U)
#define MC33772C_ADC_CFG_AVG(x)                              ((uint16_t)((uint16_t)(x) << MC33772C_ADC_CFG_AVG_SHIFT) & MC33772C_ADC_CFG_AVG_MASK)

/* Enumerated value NO_AVERAGING: No averaging, the result is taken as is (compatibility mode). */
#define MC33772C_ADC_CFG_AVG_NO_AVERAGING_ENUM_VAL           (0x0U)

/* Enumerated value 2_SAMPLES: Averaging of 2 consecutive samples. */
#define MC33772C_ADC_CFG_AVG_2_SAMPLES_ENUM_VAL              (0x1U)

/* Enumerated value 4_SAMPLES: Averaging of 4 consecutive samples. */
#define MC33772C_ADC_CFG_AVG_4_SAMPLES_ENUM_VAL              (0x2U)

/* Enumerated value 8_SAMPLES: Averaging of 8 consecutive samples. */
#define MC33772C_ADC_CFG_AVG_8_SAMPLES_ENUM_VAL              (0x3U)

/* Enumerated value 16_SAMPLES: Averaging of 16 consecutive samples. */
#define MC33772C_ADC_CFG_AVG_16_SAMPLES_ENUM_VAL             (0x4U)

/* Enumerated value 32_SAMPLES: Averaging of 32 consecutive samples. */
#define MC33772C_ADC_CFG_AVG_32_SAMPLES_ENUM_VAL             (0x5U)

/* Enumerated value 64_SAMPLES: Averaging of 64 consecutive samples. */
#define MC33772C_ADC_CFG_AVG_64_SAMPLES_ENUM_VAL             (0x6U)

/* Enumerated value 128_SAMPLES: Averaging of 128 consecutive samples. */
#define MC33772C_ADC_CFG_AVG_128_SAMPLES_ENUM_VAL            (0x7U)

/* Enumerated value 256_SAMPLES: Averaging of 256 consecutive samples. */
#define MC33772C_ADC_CFG_AVG_256_SAMPLES_ENUM_VAL            (0x8U)

/* --------------------------------------------------------------------------
 * ADC2_OFFSET_COMP (read-write): Current measurement chain offset compensation.
 * -------------------------------------------------------------------------- */
#define MC33772C_ADC2_OFFSET_COMP_OFFSET                     (0x7U)
#define MC33772C_ADC2_OFFSET_COMP_POR_VAL                    (0x4000U)

/* Field ADC2_OFFSET_COMP: Offset value, signed (two's complement) with V2RES resolution. It can be used to compensate for a PCB offset. */
#define MC33772C_ADC2_OFFSET_COMP_ADC2_OFFSET_COMP_SHIFT     (0x0U)
#define MC33772C_ADC2_OFFSET_COMP_ADC2_OFFSET_COMP_MASK      (0xFFU)
#define MC33772C_ADC2_OFFSET_COMP_ADC2_OFFSET_COMP(x)        ((uint16_t)((uint16_t)(x) << MC33772C_ADC2_OFFSET_COMP_ADC2_OFFSET_COMP_SHIFT) & MC33772C_ADC2_OFFSET_COMP_ADC2_OFFSET_COMP_MASK)

/* Field ALLCBOFFONSHORT: All CB's turn off in case of at least one short. */
#define MC33772C_ADC2_OFFSET_COMP_ALLCBOFFONSHORT_SHIFT      (0x8U)
#define MC33772C_ADC2_OFFSET_COMP_ALLCBOFFONSHORT_MASK       (0x100U)
#define MC33772C_ADC2_OFFSET_COMP_ALLCBOFFONSHORT(x)         ((uint16_t)((uint16_t)(x) << MC33772C_ADC2_OFFSET_COMP_ALLCBOFFONSHORT_SHIFT) & MC33772C_ADC2_OFFSET_COMP_ALLCBOFFONSHORT_MASK)

/* Enumerated value SHORTED: Only shorted CB's are turned off. */
#define MC33772C_ADC2_OFFSET_COMP_ALLCBOFFONSHORT_SHORTED_ENUM_VAL (0x0U)

/* Enumerated value ALL: If at least one CB is shorted, all CB's are then turned off (CB_DRVEN is reset). */
#define MC33772C_ADC2_OFFSET_COMP_ALLCBOFFONSHORT_ALL_ENUM_VAL (0x1U)

/* Field CC_OVT: Overthreshold indicator on the COULOMB_CNT1,2[COULOMB_CNT]. */
#define MC33772C_ADC2_OFFSET_COMP_CC_OVT_SHIFT               (0xAU)
#define MC33772C_ADC2_OFFSET_COMP_CC_OVT_MASK                (0x400U)
#define MC33772C_ADC2_OFFSET_COMP_CC_OVT(x)                  ((uint16_t)((uint16_t)(x) << MC33772C_ADC2_OFFSET_COMP_CC_OVT_SHIFT) & MC33772C_ADC2_OFFSET_COMP_CC_OVT_MASK)

/* Enumerated value DISABLED: No over threshold. */
#define MC33772C_ADC2_OFFSET_COMP_CC_OVT_DISABLED_ENUM_VAL   (0x0U)

/* Enumerated value ENABLED: COULOMB_CNT1,2[COULOMB_CNT] went in over threshold (TH_COULOMB_CNT). */
#define MC33772C_ADC2_OFFSET_COMP_CC_OVT_ENABLED_ENUM_VAL    (0x1U)

/* Field SAMP_OVF: Overflow indicator on the CC_NB_SAMPLES. */
#define MC33772C_ADC2_OFFSET_COMP_SAMP_OVF_SHIFT             (0xBU)
#define MC33772C_ADC2_OFFSET_COMP_SAMP_OVF_MASK              (0x800U)
#define MC33772C_ADC2_OFFSET_COMP_SAMP_OVF(x)                ((uint16_t)((uint16_t)(x) << MC33772C_ADC2_OFFSET_COMP_SAMP_OVF_SHIFT) & MC33772C_ADC2_OFFSET_COMP_SAMP_OVF_MASK)

/* Enumerated value DISABLED: No overflow. */
#define MC33772C_ADC2_OFFSET_COMP_SAMP_OVF_DISABLED_ENUM_VAL (0x0U)

/* Enumerated value ENABLED: CC_NB_SAMPLES went in overflow. */
#define MC33772C_ADC2_OFFSET_COMP_SAMP_OVF_ENABLED_ENUM_VAL  (0x1U)

/* Field CC_N_OVF: Underflow indicator on the COULOMB_CNT1,2[COULOMB_CNT]. */
#define MC33772C_ADC2_OFFSET_COMP_CC_N_OVF_SHIFT             (0xCU)
#define MC33772C_ADC2_OFFSET_COMP_CC_N_OVF_MASK              (0x1000U)
#define MC33772C_ADC2_OFFSET_COMP_CC_N_OVF(x)                ((uint16_t)((uint16_t)(x) << MC33772C_ADC2_OFFSET_COMP_CC_N_OVF_SHIFT) & MC33772C_ADC2_OFFSET_COMP_CC_N_OVF_MASK)

/* Enumerated value DISABLED: No underflow. */
#define MC33772C_ADC2_OFFSET_COMP_CC_N_OVF_DISABLED_ENUM_VAL (0x0U)

/* Enumerated value ENABLED: COULOMB_CNT1,2[COULOMB_CNT] went in underflow. */
#define MC33772C_ADC2_OFFSET_COMP_CC_N_OVF_ENABLED_ENUM_VAL  (0x1U)

/* Field CC_P_OVF: Overflow indicator on the COULOMB_CNT1,2[COULOMB_CNT]. */
#define MC33772C_ADC2_OFFSET_COMP_CC_P_OVF_SHIFT             (0xDU)
#define MC33772C_ADC2_OFFSET_COMP_CC_P_OVF_MASK              (0x2000U)
#define MC33772C_ADC2_OFFSET_COMP_CC_P_OVF(x)                ((uint16_t)((uint16_t)(x) << MC33772C_ADC2_OFFSET_COMP_CC_P_OVF_SHIFT) & MC33772C_ADC2_OFFSET_COMP_CC_P_OVF_MASK)

/* Enumerated value DISABLED: No overflow. */
#define MC33772C_ADC2_OFFSET_COMP_CC_P_OVF_DISABLED_ENUM_VAL (0x0U)

/* Enumerated value ENABLED: COULOMB_CNT1,2[COULOMB_CNT] went in overflow. */
#define MC33772C_ADC2_OFFSET_COMP_CC_P_OVF_ENABLED_ENUM_VAL  (0x1U)

/* Field FREE_CNT: Configuration of the free running coulomb counters. */
#define MC33772C_ADC2_OFFSET_COMP_FREE_CNT_SHIFT             (0xEU)
#define MC33772C_ADC2_OFFSET_COMP_FREE_CNT_MASK              (0x4000U)
#define MC33772C_ADC2_OFFSET_COMP_FREE_CNT(x)                ((uint16_t)((uint16_t)(x) << MC33772C_ADC2_OFFSET_COMP_FREE_CNT_SHIFT) & MC33772C_ADC2_OFFSET_COMP_FREE_CNT_MASK)

/* Enumerated value CLAMP: No free-running, coulomb counters clamp on min/max values. */
#define MC33772C_ADC2_OFFSET_COMP_FREE_CNT_CLAMP_ENUM_VAL    (0x0U)

/* Enumerated value ROLL_OVER: Free-running mode. No clamp but rollover. */
#define MC33772C_ADC2_OFFSET_COMP_FREE_CNT_ROLL_OVER_ENUM_VAL (0x1U)

/* Field CC_RST CFG: Configuration of the action linked to the read of coulomb count results. */
#define MC33772C_ADC2_OFFSET_COMP_CC_RST_CFG_SHIFT           (0xFU)
#define MC33772C_ADC2_OFFSET_COMP_CC_RST_CFG_MASK            (0x8000U)
#define MC33772C_ADC2_OFFSET_COMP_CC_RST_CFG(x)              ((uint16_t)((uint16_t)(x) << MC33772C_ADC2_OFFSET_COMP_CC_RST_CFG_SHIFT) & MC33772C_ADC2_OFFSET_COMP_CC_RST_CFG_MASK)

/* Enumerated value NO_ACTION: No linked action. */
#define MC33772C_ADC2_OFFSET_COMP_CC_RST_CFG_NO_ACTION_ENUM_VAL (0x0U)

/* Enumerated value CC_RESET: Reading any CC register (from @ $2D to @ $2F) also resets the coulomb counters. */
#define MC33772C_ADC2_OFFSET_COMP_CC_RST_CFG_CC_RESET_ENUM_VAL (0x1U)

/* --------------------------------------------------------------------------
 * OV_UV_EN (read-write): Cell select register.
 * -------------------------------------------------------------------------- */
#define MC33772C_OV_UV_EN_OFFSET                             (0x8U)
#define MC33772C_OV_UV_EN_POR_VAL                            (0x3FU)

/* Field CT1_OVUV_EN: Enable or disable ADC data to be compared with thresholds for OV/UV. If disabled no OVUV fault is set. */
#define MC33772C_OV_UV_EN_CT1_OVUV_EN_SHIFT                  (0x0U)
#define MC33772C_OV_UV_EN_CT1_OVUV_EN_MASK                   (0x1U)
#define MC33772C_OV_UV_EN_CT1_OVUV_EN(x)                     ((uint16_t)((uint16_t)(x) << MC33772C_OV_UV_EN_CT1_OVUV_EN_SHIFT) & MC33772C_OV_UV_EN_CT1_OVUV_EN_MASK)

/* Enumerated value DISABLED: OVUV disabled */
#define MC33772C_OV_UV_EN_CT1_OVUV_EN_DISABLED_ENUM_VAL      (0x0U)

/* Enumerated value ENABLED: OVUV is enabled */
#define MC33772C_OV_UV_EN_CT1_OVUV_EN_ENABLED_ENUM_VAL       (0x1U)

/* Field CT2_OVUV_EN: Enable or disable ADC data to be compared with thresholds for OV/UV. If disabled no OVUV fault is set. */
#define MC33772C_OV_UV_EN_CT2_OVUV_EN_SHIFT                  (0x1U)
#define MC33772C_OV_UV_EN_CT2_OVUV_EN_MASK                   (0x2U)
#define MC33772C_OV_UV_EN_CT2_OVUV_EN(x)                     ((uint16_t)((uint16_t)(x) << MC33772C_OV_UV_EN_CT2_OVUV_EN_SHIFT) & MC33772C_OV_UV_EN_CT2_OVUV_EN_MASK)

/* Enumerated value DISABLED: OVUV disabled */
#define MC33772C_OV_UV_EN_CT2_OVUV_EN_DISABLED_ENUM_VAL      (0x0U)

/* Enumerated value ENABLED: OVUV is enabled */
#define MC33772C_OV_UV_EN_CT2_OVUV_EN_ENABLED_ENUM_VAL       (0x1U)

/* Field CT3_OVUV_EN: Enable or disable ADC data to be compared with thresholds for OV/UV. If disabled no OVUV fault is set. */
#define MC33772C_OV_UV_EN_CT3_OVUV_EN_SHIFT                  (0x2U)
#define MC33772C_OV_UV_EN_CT3_OVUV_EN_MASK                   (0x4U)
#define MC33772C_OV_UV_EN_CT3_OVUV_EN(x)                     ((uint16_t)((uint16_t)(x) << MC33772C_OV_UV_EN_CT3_OVUV_EN_SHIFT) & MC33772C_OV_UV_EN_CT3_OVUV_EN_MASK)

/* Enumerated value DISABLED: OVUV disabled */
#define MC33772C_OV_UV_EN_CT3_OVUV_EN_DISABLED_ENUM_VAL      (0x0U)

/* Enumerated value ENABLED: OVUV is enabled */
#define MC33772C_OV_UV_EN_CT3_OVUV_EN_ENABLED_ENUM_VAL       (0x1U)

/* Field CT4_OVUV_EN: Enable or disable ADC data to be compared with thresholds for OV/UV. If disabled no OVUV fault is set. */
#define MC33772C_OV_UV_EN_CT4_OVUV_EN_SHIFT                  (0x3U)
#define MC33772C_OV_UV_EN_CT4_OVUV_EN_MASK                   (0x8U)
#define MC33772C_OV_UV_EN_CT4_OVUV_EN(x)                     ((uint16_t)((uint16_t)(x) << MC33772C_OV_UV_EN_CT4_OVUV_EN_SHIFT) & MC33772C_OV_UV_EN_CT4_OVUV_EN_MASK)

/* Enumerated value DISABLED: OVUV disabled */
#define MC33772C_OV_UV_EN_CT4_OVUV_EN_DISABLED_ENUM_VAL      (0x0U)

/* Enumerated value ENABLED: OVUV is enabled */
#define MC33772C_OV_UV_EN_CT4_OVUV_EN_ENABLED_ENUM_VAL       (0x1U)

/* Field CT5_OVUV_EN: Enable or disable ADC data to be compared with thresholds for OV/UV. If disabled no OVUV fault is set. */
#define MC33772C_OV_UV_EN_CT5_OVUV_EN_SHIFT                  (0x4U)
#define MC33772C_OV_UV_EN_CT5_OVUV_EN_MASK                   (0x10U)
#define MC33772C_OV_UV_EN_CT5_OVUV_EN(x)                     ((uint16_t)((uint16_t)(x) << MC33772C_OV_UV_EN_CT5_OVUV_EN_SHIFT) & MC33772C_OV_UV_EN_CT5_OVUV_EN_MASK)

/* Enumerated value DISABLED: OVUV disabled */
#define MC33772C_OV_UV_EN_CT5_OVUV_EN_DISABLED_ENUM_VAL      (0x0U)

/* Enumerated value ENABLED: OVUV is enabled */
#define MC33772C_OV_UV_EN_CT5_OVUV_EN_ENABLED_ENUM_VAL       (0x1U)

/* Field CT6_OVUV_EN: Enable or disable ADC data to be compared with thresholds for OV/UV. If disabled no OVUV fault is set. */
#define MC33772C_OV_UV_EN_CT6_OVUV_EN_SHIFT                  (0x5U)
#define MC33772C_OV_UV_EN_CT6_OVUV_EN_MASK                   (0x20U)
#define MC33772C_OV_UV_EN_CT6_OVUV_EN(x)                     ((uint16_t)((uint16_t)(x) << MC33772C_OV_UV_EN_CT6_OVUV_EN_SHIFT) & MC33772C_OV_UV_EN_CT6_OVUV_EN_MASK)

/* Enumerated value DISABLED: OVUV disabled */
#define MC33772C_OV_UV_EN_CT6_OVUV_EN_DISABLED_ENUM_VAL      (0x0U)

/* Enumerated value ENABLED: OVUV is enabled */
#define MC33772C_OV_UV_EN_CT6_OVUV_EN_ENABLED_ENUM_VAL       (0x1U)

/* Field COMMON_UV_TH: All CTx measurement use the common undervoltage threshold register for comparison. */
#define MC33772C_OV_UV_EN_COMMON_UV_TH_SHIFT                 (0xEU)
#define MC33772C_OV_UV_EN_COMMON_UV_TH_MASK                  (0x4000U)
#define MC33772C_OV_UV_EN_COMMON_UV_TH(x)                    ((uint16_t)((uint16_t)(x) << MC33772C_OV_UV_EN_COMMON_UV_TH_SHIFT) & MC33772C_OV_UV_EN_COMMON_UV_TH_MASK)

/* Enumerated value INDIVIDUAL: Use individual threshold register. */
#define MC33772C_OV_UV_EN_COMMON_UV_TH_INDIVIDUAL_ENUM_VAL   (0x0U)

/* Enumerated value COMMON: Use common threshold register. */
#define MC33772C_OV_UV_EN_COMMON_UV_TH_COMMON_ENUM_VAL       (0x1U)

/* Field COMMON_OV_TH: All CTx measurement use the common overvoltage threshold register for comparison. */
#define MC33772C_OV_UV_EN_COMMON_OV_TH_SHIFT                 (0xFU)
#define MC33772C_OV_UV_EN_COMMON_OV_TH_MASK                  (0x8000U)
#define MC33772C_OV_UV_EN_COMMON_OV_TH(x)                    ((uint16_t)((uint16_t)(x) << MC33772C_OV_UV_EN_COMMON_OV_TH_SHIFT) & MC33772C_OV_UV_EN_COMMON_OV_TH_MASK)

/* Enumerated value INDIVIDUAL: Use individual threshold register. */
#define MC33772C_OV_UV_EN_COMMON_OV_TH_INDIVIDUAL_ENUM_VAL   (0x0U)

/* Enumerated value COMMON: Use common threshold register. */
#define MC33772C_OV_UV_EN_COMMON_OV_TH_COMMON_ENUM_VAL       (0x1U)

/* --------------------------------------------------------------------------
 * CELL_OV_FLT (read-only): Cell terminal overvoltage fault register.
 * -------------------------------------------------------------------------- */
#define MC33772C_CELL_OV_FLT_OFFSET                          (0x9U)
#define MC33772C_CELL_OV_FLT_POR_VAL                         (0x0U)

/* Field CT1_OV_FLT (read-only): Contains the status of the overvoltage fault for the cell terminal. Register is updated with each internal and system controller on-demand conversion cycle. */
#define MC33772C_CELL_OV_FLT_CT1_OV_FLT_SHIFT                (0x0U)
#define MC33772C_CELL_OV_FLT_CT1_OV_FLT_MASK                 (0x1U)
#define MC33772C_CELL_OV_FLT_CT1_OV_FLT(x)                   ((uint16_t)((uint16_t)(x) << MC33772C_CELL_OV_FLT_CT1_OV_FLT_SHIFT) & MC33772C_CELL_OV_FLT_CT1_OV_FLT_MASK)

/* Enumerated value NO_OVERVOLTAGE: No Cell Terminal overvoltage. */
#define MC33772C_CELL_OV_FLT_CT1_OV_FLT_NO_OVERVOLTAGE_ENUM_VAL (0x0U)

/* Enumerated value OVERVOLTAGE: Cell Terminal overvoltage detected on terminal. */
#define MC33772C_CELL_OV_FLT_CT1_OV_FLT_OVERVOLTAGE_ENUM_VAL (0x1U)

/* Field CT2_OV_FLT (read-only): Contains the status of the overvoltage fault for the cell terminal. Register is updated with each internal and system controller on-demand conversion cycle. */
#define MC33772C_CELL_OV_FLT_CT2_OV_FLT_SHIFT                (0x1U)
#define MC33772C_CELL_OV_FLT_CT2_OV_FLT_MASK                 (0x2U)
#define MC33772C_CELL_OV_FLT_CT2_OV_FLT(x)                   ((uint16_t)((uint16_t)(x) << MC33772C_CELL_OV_FLT_CT2_OV_FLT_SHIFT) & MC33772C_CELL_OV_FLT_CT2_OV_FLT_MASK)

/* Enumerated value NO_OVERVOLTAGE: No Cell Terminal overvoltage. */
#define MC33772C_CELL_OV_FLT_CT2_OV_FLT_NO_OVERVOLTAGE_ENUM_VAL (0x0U)

/* Enumerated value OVERVOLTAGE: Cell Terminal overvoltage detected on terminal. */
#define MC33772C_CELL_OV_FLT_CT2_OV_FLT_OVERVOLTAGE_ENUM_VAL (0x1U)

/* Field CT3_OV_FLT (read-only): Contains the status of the overvoltage fault for the cell terminal. Register is updated with each internal and system controller on-demand conversion cycle. */
#define MC33772C_CELL_OV_FLT_CT3_OV_FLT_SHIFT                (0x2U)
#define MC33772C_CELL_OV_FLT_CT3_OV_FLT_MASK                 (0x4U)
#define MC33772C_CELL_OV_FLT_CT3_OV_FLT(x)                   ((uint16_t)((uint16_t)(x) << MC33772C_CELL_OV_FLT_CT3_OV_FLT_SHIFT) & MC33772C_CELL_OV_FLT_CT3_OV_FLT_MASK)

/* Enumerated value NO_OVERVOLTAGE: No Cell Terminal overvoltage. */
#define MC33772C_CELL_OV_FLT_CT3_OV_FLT_NO_OVERVOLTAGE_ENUM_VAL (0x0U)

/* Enumerated value OVERVOLTAGE: Cell Terminal overvoltage detected on terminal. */
#define MC33772C_CELL_OV_FLT_CT3_OV_FLT_OVERVOLTAGE_ENUM_VAL (0x1U)

/* Field CT4_OV_FLT (read-only): Contains the status of the overvoltage fault for the cell terminal. Register is updated with each internal and system controller on-demand conversion cycle. */
#define MC33772C_CELL_OV_FLT_CT4_OV_FLT_SHIFT                (0x3U)
#define MC33772C_CELL_OV_FLT_CT4_OV_FLT_MASK                 (0x8U)
#define MC33772C_CELL_OV_FLT_CT4_OV_FLT(x)                   ((uint16_t)((uint16_t)(x) << MC33772C_CELL_OV_FLT_CT4_OV_FLT_SHIFT) & MC33772C_CELL_OV_FLT_CT4_OV_FLT_MASK)

/* Enumerated value NO_OVERVOLTAGE: No Cell Terminal overvoltage. */
#define MC33772C_CELL_OV_FLT_CT4_OV_FLT_NO_OVERVOLTAGE_ENUM_VAL (0x0U)

/* Enumerated value OVERVOLTAGE: Cell Terminal overvoltage detected on terminal. */
#define MC33772C_CELL_OV_FLT_CT4_OV_FLT_OVERVOLTAGE_ENUM_VAL (0x1U)

/* Field CT5_OV_FLT (read-only): Contains the status of the overvoltage fault for the cell terminal. Register is updated with each internal and system controller on-demand conversion cycle. */
#define MC33772C_CELL_OV_FLT_CT5_OV_FLT_SHIFT                (0x4U)
#define MC33772C_CELL_OV_FLT_CT5_OV_FLT_MASK                 (0x10U)
#define MC33772C_CELL_OV_FLT_CT5_OV_FLT(x)                   ((uint16_t)((uint16_t)(x) << MC33772C_CELL_OV_FLT_CT5_OV_FLT_SHIFT) & MC33772C_CELL_OV_FLT_CT5_OV_FLT_MASK)

/* Enumerated value NO_OVERVOLTAGE: No Cell Terminal overvoltage. */
#define MC33772C_CELL_OV_FLT_CT5_OV_FLT_NO_OVERVOLTAGE_ENUM_VAL (0x0U)

/* Enumerated value OVERVOLTAGE: Cell Terminal overvoltage detected on terminal. */
#define MC33772C_CELL_OV_FLT_CT5_OV_FLT_OVERVOLTAGE_ENUM_VAL (0x1U)

/* Field CT6_OV_FLT (read-only): Contains the status of the overvoltage fault for the cell terminal. Register is updated with each internal and system controller on-demand conversion cycle. */
#define MC33772C_CELL_OV_FLT_CT6_OV_FLT_SHIFT                (0x5U)
#define MC33772C_CELL_OV_FLT_CT6_OV_FLT_MASK                 (0x20U)
#define MC33772C_CELL_OV_FLT_CT6_OV_FLT(x)                   ((uint16_t)((uint16_t)(x) << MC33772C_CELL_OV_FLT_CT6_OV_FLT_SHIFT) & MC33772C_CELL_OV_FLT_CT6_OV_FLT_MASK)

/* Enumerated value NO_OVERVOLTAGE: No Cell Terminal overvoltage. */
#define MC33772C_CELL_OV_FLT_CT6_OV_FLT_NO_OVERVOLTAGE_ENUM_VAL (0x0U)

/* Enumerated value OVERVOLTAGE: Cell Terminal overvoltage detected on terminal. */
#define MC33772C_CELL_OV_FLT_CT6_OV_FLT_OVERVOLTAGE_ENUM_VAL (0x1U)

/* --------------------------------------------------------------------------
 * CELL_UV_FLT (read-only): Cell terminal undervoltage fault register.
 * -------------------------------------------------------------------------- */
#define MC33772C_CELL_UV_FLT_OFFSET                          (0xAU)
#define MC33772C_CELL_UV_FLT_POR_VAL                         (0x0U)

/* Field CT1_UV_FLT (read-only): Contains the status of the undervoltage fault for the cell terminal. Register is updated with each internal and system controller on-demand conversion cycle. */
#define MC33772C_CELL_UV_FLT_CT1_UV_FLT_SHIFT                (0x0U)
#define MC33772C_CELL_UV_FLT_CT1_UV_FLT_MASK                 (0x1U)
#define MC33772C_CELL_UV_FLT_CT1_UV_FLT(x)                   ((uint16_t)((uint16_t)(x) << MC33772C_CELL_UV_FLT_CT1_UV_FLT_SHIFT) & MC33772C_CELL_UV_FLT_CT1_UV_FLT_MASK)

/* Enumerated value NO_UNDERVOLTAGE: No Cell Terminal undervoltage. */
#define MC33772C_CELL_UV_FLT_CT1_UV_FLT_NO_UNDERVOLTAGE_ENUM_VAL (0x0U)

/* Enumerated value UNDERVOLTAGE: Cell Terminal undervoltage detected. */
#define MC33772C_CELL_UV_FLT_CT1_UV_FLT_UNDERVOLTAGE_ENUM_VAL (0x1U)

/* Field CT2_UV_FLT (read-only): Contains the status of the undervoltage fault for the cell terminal. Register is updated with each internal and system controller on-demand conversion cycle. */
#define MC33772C_CELL_UV_FLT_CT2_UV_FLT_SHIFT                (0x1U)
#define MC33772C_CELL_UV_FLT_CT2_UV_FLT_MASK                 (0x2U)
#define MC33772C_CELL_UV_FLT_CT2_UV_FLT(x)                   ((uint16_t)((uint16_t)(x) << MC33772C_CELL_UV_FLT_CT2_UV_FLT_SHIFT) & MC33772C_CELL_UV_FLT_CT2_UV_FLT_MASK)

/* Enumerated value NO_UNDERVOLTAGE: No Cell Terminal undervoltage. */
#define MC33772C_CELL_UV_FLT_CT2_UV_FLT_NO_UNDERVOLTAGE_ENUM_VAL (0x0U)

/* Enumerated value UNDERVOLTAGE: Cell Terminal undervoltage detected. */
#define MC33772C_CELL_UV_FLT_CT2_UV_FLT_UNDERVOLTAGE_ENUM_VAL (0x1U)

/* Field CT3_UV_FLT (read-only): Contains the status of the undervoltage fault for the cell terminal. Register is updated with each internal and system controller on-demand conversion cycle. */
#define MC33772C_CELL_UV_FLT_CT3_UV_FLT_SHIFT                (0x2U)
#define MC33772C_CELL_UV_FLT_CT3_UV_FLT_MASK                 (0x4U)
#define MC33772C_CELL_UV_FLT_CT3_UV_FLT(x)                   ((uint16_t)((uint16_t)(x) << MC33772C_CELL_UV_FLT_CT3_UV_FLT_SHIFT) & MC33772C_CELL_UV_FLT_CT3_UV_FLT_MASK)

/* Enumerated value NO_UNDERVOLTAGE: No Cell Terminal undervoltage. */
#define MC33772C_CELL_UV_FLT_CT3_UV_FLT_NO_UNDERVOLTAGE_ENUM_VAL (0x0U)

/* Enumerated value UNDERVOLTAGE: Cell Terminal undervoltage detected. */
#define MC33772C_CELL_UV_FLT_CT3_UV_FLT_UNDERVOLTAGE_ENUM_VAL (0x1U)

/* Field CT4_UV_FLT (read-only): Contains the status of the undervoltage fault for the cell terminal. Register is updated with each internal and system controller on-demand conversion cycle. */
#define MC33772C_CELL_UV_FLT_CT4_UV_FLT_SHIFT                (0x3U)
#define MC33772C_CELL_UV_FLT_CT4_UV_FLT_MASK                 (0x8U)
#define MC33772C_CELL_UV_FLT_CT4_UV_FLT(x)                   ((uint16_t)((uint16_t)(x) << MC33772C_CELL_UV_FLT_CT4_UV_FLT_SHIFT) & MC33772C_CELL_UV_FLT_CT4_UV_FLT_MASK)

/* Enumerated value NO_UNDERVOLTAGE: No Cell Terminal undervoltage. */
#define MC33772C_CELL_UV_FLT_CT4_UV_FLT_NO_UNDERVOLTAGE_ENUM_VAL (0x0U)

/* Enumerated value UNDERVOLTAGE: Cell Terminal undervoltage detected. */
#define MC33772C_CELL_UV_FLT_CT4_UV_FLT_UNDERVOLTAGE_ENUM_VAL (0x1U)

/* Field CT5_UV_FLT (read-only): Contains the status of the undervoltage fault for the cell terminal. Register is updated with each internal and system controller on-demand conversion cycle. */
#define MC33772C_CELL_UV_FLT_CT5_UV_FLT_SHIFT                (0x4U)
#define MC33772C_CELL_UV_FLT_CT5_UV_FLT_MASK                 (0x10U)
#define MC33772C_CELL_UV_FLT_CT5_UV_FLT(x)                   ((uint16_t)((uint16_t)(x) << MC33772C_CELL_UV_FLT_CT5_UV_FLT_SHIFT) & MC33772C_CELL_UV_FLT_CT5_UV_FLT_MASK)

/* Enumerated value NO_UNDERVOLTAGE: No Cell Terminal undervoltage. */
#define MC33772C_CELL_UV_FLT_CT5_UV_FLT_NO_UNDERVOLTAGE_ENUM_VAL (0x0U)

/* Enumerated value UNDERVOLTAGE: Cell Terminal undervoltage detected. */
#define MC33772C_CELL_UV_FLT_CT5_UV_FLT_UNDERVOLTAGE_ENUM_VAL (0x1U)

/* Field CT6_UV_FLT (read-only): Contains the status of the undervoltage fault for the cell terminal. Register is updated with each internal and system controller on-demand conversion cycle. */
#define MC33772C_CELL_UV_FLT_CT6_UV_FLT_SHIFT                (0x5U)
#define MC33772C_CELL_UV_FLT_CT6_UV_FLT_MASK                 (0x20U)
#define MC33772C_CELL_UV_FLT_CT6_UV_FLT(x)                   ((uint16_t)((uint16_t)(x) << MC33772C_CELL_UV_FLT_CT6_UV_FLT_SHIFT) & MC33772C_CELL_UV_FLT_CT6_UV_FLT_MASK)

/* Enumerated value NO_UNDERVOLTAGE: No Cell Terminal undervoltage. */
#define MC33772C_CELL_UV_FLT_CT6_UV_FLT_NO_UNDERVOLTAGE_ENUM_VAL (0x0U)

/* Enumerated value UNDERVOLTAGE: Cell Terminal undervoltage detected. */
#define MC33772C_CELL_UV_FLT_CT6_UV_FLT_UNDERVOLTAGE_ENUM_VAL (0x1U)

/* --------------------------------------------------------------------------
 * TPL_CFG (read-only): Configures up and down transmitter.
 * -------------------------------------------------------------------------- */
#define MC33772C_TPL_CFG_OFFSET                              (0xBU)
#define MC33772C_TPL_CFG_POR_VAL                             (0x6262U)

/* Field TPL_TX_CFG_2 (read-only): RDTX_IN (Reserved). */
#define MC33772C_TPL_CFG_TPL_TX_CFG_2_SHIFT                  (0x0U)
#define MC33772C_TPL_CFG_TPL_TX_CFG_2_MASK                   (0xFFU)
#define MC33772C_TPL_CFG_TPL_TX_CFG_2(x)                     ((uint16_t)((uint16_t)(x) << MC33772C_TPL_CFG_TPL_TX_CFG_2_SHIFT) & MC33772C_TPL_CFG_TPL_TX_CFG_2_MASK)

/* Field TPL_TX_CFG_1 (read-only): RDTX_OUT (Reserved). */
#define MC33772C_TPL_CFG_TPL_TX_CFG_1_SHIFT                  (0x8U)
#define MC33772C_TPL_CFG_TPL_TX_CFG_1_MASK                   (0xFF00U)
#define MC33772C_TPL_CFG_TPL_TX_CFG_1(x)                     ((uint16_t)((uint16_t)(x) << MC33772C_TPL_CFG_TPL_TX_CFG_1_SHIFT) & MC33772C_TPL_CFG_TPL_TX_CFG_1_MASK)

/* --------------------------------------------------------------------------
 * CB1_CFG (read-write): Cell balance configuration register
 * -------------------------------------------------------------------------- */
#define MC33772C_CB1_CFG_OFFSET                              (0xCU)
#define MC33772C_CB1_CFG_POR_VAL                             (0x0U)

/* Field CB_TIMER: Cell balance timer in minutes (0 means 30 seconds). */
#define MC33772C_CB1_CFG_CB_TIMER_SHIFT                      (0x0U)
#define MC33772C_CB1_CFG_CB_TIMER_MASK                       (0x1FFU)
#define MC33772C_CB1_CFG_CB_TIMER(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_CB1_CFG_CB_TIMER_SHIFT) & MC33772C_CB1_CFG_CB_TIMER_MASK)

/* Field CB_EN: Cell balance enable. */
#define MC33772C_CB1_CFG_CB_EN_SHIFT                         (0x9U)
#define MC33772C_CB1_CFG_CB_EN_MASK                          (0x200U)
#define MC33772C_CB1_CFG_CB_EN(x)                            ((uint16_t)((uint16_t)(x) << MC33772C_CB1_CFG_CB_EN_SHIFT) & MC33772C_CB1_CFG_CB_EN_MASK)

/* Enumerated value DISABLED: Cell balance driver disabled. */
#define MC33772C_CB1_CFG_CB_EN_DISABLED_ENUM_VAL             (0x0U)

/* Enumerated value ENABLED: Cell balance is enabled or re-launched if overwritten (restarts the timer count from zero and enables the driver). */
#define MC33772C_CB1_CFG_CB_EN_ENABLED_ENUM_VAL              (0x1U)

/* Field CB_STS: Cell balance driver status. */
#define MC33772C_CB1_CFG_CB_STS_SHIFT                        (0x9U)
#define MC33772C_CB1_CFG_CB_STS_MASK                         (0x200U)
#define MC33772C_CB1_CFG_CB_STS(x)                           ((uint16_t)((uint16_t)(x) << MC33772C_CB1_CFG_CB_STS_SHIFT) & MC33772C_CB1_CFG_CB_STS_MASK)

/* Enumerated value DISABLED: Cell balance driver is off. */
#define MC33772C_CB1_CFG_CB_STS_DISABLED_ENUM_VAL            (0x0U)

/* Enumerated value ENABLED: Cell balance driver is on. */
#define MC33772C_CB1_CFG_CB_STS_ENABLED_ENUM_VAL             (0x1U)

/* --------------------------------------------------------------------------
 * CB2_CFG (read-write): Cell balance configuration register
 * -------------------------------------------------------------------------- */
#define MC33772C_CB2_CFG_OFFSET                              (0xDU)
#define MC33772C_CB2_CFG_POR_VAL                             (0x0U)

/* Field CB_TIMER: Cell balance timer in minutes (0 means 30 seconds). */
#define MC33772C_CB2_CFG_CB_TIMER_SHIFT                      (0x0U)
#define MC33772C_CB2_CFG_CB_TIMER_MASK                       (0x1FFU)
#define MC33772C_CB2_CFG_CB_TIMER(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_CB2_CFG_CB_TIMER_SHIFT) & MC33772C_CB2_CFG_CB_TIMER_MASK)

/* Field CB_EN: Cell balance enable. */
#define MC33772C_CB2_CFG_CB_EN_SHIFT                         (0x9U)
#define MC33772C_CB2_CFG_CB_EN_MASK                          (0x200U)
#define MC33772C_CB2_CFG_CB_EN(x)                            ((uint16_t)((uint16_t)(x) << MC33772C_CB2_CFG_CB_EN_SHIFT) & MC33772C_CB2_CFG_CB_EN_MASK)

/* Enumerated value DISABLED: Cell balance driver disabled. */
#define MC33772C_CB2_CFG_CB_EN_DISABLED_ENUM_VAL             (0x0U)

/* Enumerated value ENABLED: Cell balance is enabled or re-launched if overwritten (restarts the timer count from zero and enables the driver). */
#define MC33772C_CB2_CFG_CB_EN_ENABLED_ENUM_VAL              (0x1U)

/* Field CB_STS: Cell balance driver status. */
#define MC33772C_CB2_CFG_CB_STS_SHIFT                        (0x9U)
#define MC33772C_CB2_CFG_CB_STS_MASK                         (0x200U)
#define MC33772C_CB2_CFG_CB_STS(x)                           ((uint16_t)((uint16_t)(x) << MC33772C_CB2_CFG_CB_STS_SHIFT) & MC33772C_CB2_CFG_CB_STS_MASK)

/* Enumerated value DISABLED: Cell balance driver is off. */
#define MC33772C_CB2_CFG_CB_STS_DISABLED_ENUM_VAL            (0x0U)

/* Enumerated value ENABLED: Cell balance driver is on. */
#define MC33772C_CB2_CFG_CB_STS_ENABLED_ENUM_VAL             (0x1U)

/* --------------------------------------------------------------------------
 * CB3_CFG (read-write): Cell balance configuration register
 * -------------------------------------------------------------------------- */
#define MC33772C_CB3_CFG_OFFSET                              (0xEU)
#define MC33772C_CB3_CFG_POR_VAL                             (0x0U)

/* Field CB_TIMER: Cell balance timer in minutes (0 means 30 seconds). */
#define MC33772C_CB3_CFG_CB_TIMER_SHIFT                      (0x0U)
#define MC33772C_CB3_CFG_CB_TIMER_MASK                       (0x1FFU)
#define MC33772C_CB3_CFG_CB_TIMER(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_CB3_CFG_CB_TIMER_SHIFT) & MC33772C_CB3_CFG_CB_TIMER_MASK)

/* Field CB_EN: Cell balance enable. */
#define MC33772C_CB3_CFG_CB_EN_SHIFT                         (0x9U)
#define MC33772C_CB3_CFG_CB_EN_MASK                          (0x200U)
#define MC33772C_CB3_CFG_CB_EN(x)                            ((uint16_t)((uint16_t)(x) << MC33772C_CB3_CFG_CB_EN_SHIFT) & MC33772C_CB3_CFG_CB_EN_MASK)

/* Enumerated value DISABLED: Cell balance driver disabled. */
#define MC33772C_CB3_CFG_CB_EN_DISABLED_ENUM_VAL             (0x0U)

/* Enumerated value ENABLED: Cell balance is enabled or re-launched if overwritten (restarts the timer count from zero and enables the driver). */
#define MC33772C_CB3_CFG_CB_EN_ENABLED_ENUM_VAL              (0x1U)

/* Field CB_STS: Cell balance driver status. */
#define MC33772C_CB3_CFG_CB_STS_SHIFT                        (0x9U)
#define MC33772C_CB3_CFG_CB_STS_MASK                         (0x200U)
#define MC33772C_CB3_CFG_CB_STS(x)                           ((uint16_t)((uint16_t)(x) << MC33772C_CB3_CFG_CB_STS_SHIFT) & MC33772C_CB3_CFG_CB_STS_MASK)

/* Enumerated value DISABLED: Cell balance driver is off. */
#define MC33772C_CB3_CFG_CB_STS_DISABLED_ENUM_VAL            (0x0U)

/* Enumerated value ENABLED: Cell balance driver is on. */
#define MC33772C_CB3_CFG_CB_STS_ENABLED_ENUM_VAL             (0x1U)

/* --------------------------------------------------------------------------
 * CB4_CFG (read-write): Cell balance configuration register
 * -------------------------------------------------------------------------- */
#define MC33772C_CB4_CFG_OFFSET                              (0xFU)
#define MC33772C_CB4_CFG_POR_VAL                             (0x0U)

/* Field CB_TIMER: Cell balance timer in minutes (0 means 30 seconds). */
#define MC33772C_CB4_CFG_CB_TIMER_SHIFT                      (0x0U)
#define MC33772C_CB4_CFG_CB_TIMER_MASK                       (0x1FFU)
#define MC33772C_CB4_CFG_CB_TIMER(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_CB4_CFG_CB_TIMER_SHIFT) & MC33772C_CB4_CFG_CB_TIMER_MASK)

/* Field CB_EN: Cell balance enable. */
#define MC33772C_CB4_CFG_CB_EN_SHIFT                         (0x9U)
#define MC33772C_CB4_CFG_CB_EN_MASK                          (0x200U)
#define MC33772C_CB4_CFG_CB_EN(x)                            ((uint16_t)((uint16_t)(x) << MC33772C_CB4_CFG_CB_EN_SHIFT) & MC33772C_CB4_CFG_CB_EN_MASK)

/* Enumerated value DISABLED: Cell balance driver disabled. */
#define MC33772C_CB4_CFG_CB_EN_DISABLED_ENUM_VAL             (0x0U)

/* Enumerated value ENABLED: Cell balance is enabled or re-launched if overwritten (restarts the timer count from zero and enables the driver). */
#define MC33772C_CB4_CFG_CB_EN_ENABLED_ENUM_VAL              (0x1U)

/* Field CB_STS: Cell balance driver status. */
#define MC33772C_CB4_CFG_CB_STS_SHIFT                        (0x9U)
#define MC33772C_CB4_CFG_CB_STS_MASK                         (0x200U)
#define MC33772C_CB4_CFG_CB_STS(x)                           ((uint16_t)((uint16_t)(x) << MC33772C_CB4_CFG_CB_STS_SHIFT) & MC33772C_CB4_CFG_CB_STS_MASK)

/* Enumerated value DISABLED: Cell balance driver is off. */
#define MC33772C_CB4_CFG_CB_STS_DISABLED_ENUM_VAL            (0x0U)

/* Enumerated value ENABLED: Cell balance driver is on. */
#define MC33772C_CB4_CFG_CB_STS_ENABLED_ENUM_VAL             (0x1U)

/* --------------------------------------------------------------------------
 * CB5_CFG (read-write): Cell balance configuration register
 * -------------------------------------------------------------------------- */
#define MC33772C_CB5_CFG_OFFSET                              (0x10U)
#define MC33772C_CB5_CFG_POR_VAL                             (0x0U)

/* Field CB_TIMER: Cell balance timer in minutes (0 means 30 seconds). */
#define MC33772C_CB5_CFG_CB_TIMER_SHIFT                      (0x0U)
#define MC33772C_CB5_CFG_CB_TIMER_MASK                       (0x1FFU)
#define MC33772C_CB5_CFG_CB_TIMER(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_CB5_CFG_CB_TIMER_SHIFT) & MC33772C_CB5_CFG_CB_TIMER_MASK)

/* Field CB_EN: Cell balance enable. */
#define MC33772C_CB5_CFG_CB_EN_SHIFT                         (0x9U)
#define MC33772C_CB5_CFG_CB_EN_MASK                          (0x200U)
#define MC33772C_CB5_CFG_CB_EN(x)                            ((uint16_t)((uint16_t)(x) << MC33772C_CB5_CFG_CB_EN_SHIFT) & MC33772C_CB5_CFG_CB_EN_MASK)

/* Enumerated value DISABLED: Cell balance driver disabled. */
#define MC33772C_CB5_CFG_CB_EN_DISABLED_ENUM_VAL             (0x0U)

/* Enumerated value ENABLED: Cell balance is enabled or re-launched if overwritten (restarts the timer count from zero and enables the driver). */
#define MC33772C_CB5_CFG_CB_EN_ENABLED_ENUM_VAL              (0x1U)

/* Field CB_STS: Cell balance driver status. */
#define MC33772C_CB5_CFG_CB_STS_SHIFT                        (0x9U)
#define MC33772C_CB5_CFG_CB_STS_MASK                         (0x200U)
#define MC33772C_CB5_CFG_CB_STS(x)                           ((uint16_t)((uint16_t)(x) << MC33772C_CB5_CFG_CB_STS_SHIFT) & MC33772C_CB5_CFG_CB_STS_MASK)

/* Enumerated value DISABLED: Cell balance driver is off. */
#define MC33772C_CB5_CFG_CB_STS_DISABLED_ENUM_VAL            (0x0U)

/* Enumerated value ENABLED: Cell balance driver is on. */
#define MC33772C_CB5_CFG_CB_STS_ENABLED_ENUM_VAL             (0x1U)

/* --------------------------------------------------------------------------
 * CB6_CFG (read-write): Cell balance configuration register
 * -------------------------------------------------------------------------- */
#define MC33772C_CB6_CFG_OFFSET                              (0x11U)
#define MC33772C_CB6_CFG_POR_VAL                             (0x0U)

/* Field CB_TIMER: Cell balance timer in minutes (0 means 30 seconds). */
#define MC33772C_CB6_CFG_CB_TIMER_SHIFT                      (0x0U)
#define MC33772C_CB6_CFG_CB_TIMER_MASK                       (0x1FFU)
#define MC33772C_CB6_CFG_CB_TIMER(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_CB6_CFG_CB_TIMER_SHIFT) & MC33772C_CB6_CFG_CB_TIMER_MASK)

/* Field CB_EN: Cell balance enable. */
#define MC33772C_CB6_CFG_CB_EN_SHIFT                         (0x9U)
#define MC33772C_CB6_CFG_CB_EN_MASK                          (0x200U)
#define MC33772C_CB6_CFG_CB_EN(x)                            ((uint16_t)((uint16_t)(x) << MC33772C_CB6_CFG_CB_EN_SHIFT) & MC33772C_CB6_CFG_CB_EN_MASK)

/* Enumerated value DISABLED: Cell balance driver disabled. */
#define MC33772C_CB6_CFG_CB_EN_DISABLED_ENUM_VAL             (0x0U)

/* Enumerated value ENABLED: Cell balance is enabled or re-launched if overwritten (restarts the timer count from zero and enables the driver). */
#define MC33772C_CB6_CFG_CB_EN_ENABLED_ENUM_VAL              (0x1U)

/* Field CB_STS: Cell balance driver status. */
#define MC33772C_CB6_CFG_CB_STS_SHIFT                        (0x9U)
#define MC33772C_CB6_CFG_CB_STS_MASK                         (0x200U)
#define MC33772C_CB6_CFG_CB_STS(x)                           ((uint16_t)((uint16_t)(x) << MC33772C_CB6_CFG_CB_STS_SHIFT) & MC33772C_CB6_CFG_CB_STS_MASK)

/* Enumerated value DISABLED: Cell balance driver is off. */
#define MC33772C_CB6_CFG_CB_STS_DISABLED_ENUM_VAL            (0x0U)

/* Enumerated value ENABLED: Cell balance driver is on. */
#define MC33772C_CB6_CFG_CB_STS_ENABLED_ENUM_VAL             (0x1U)

/* --------------------------------------------------------------------------
 * CB_OPEN_FLT (read-only): Cell balance open load fault detection register.
 * -------------------------------------------------------------------------- */
#define MC33772C_CB_OPEN_FLT_OFFSET                          (0x1AU)
#define MC33772C_CB_OPEN_FLT_POR_VAL                         (0x0U)

/* Field CB1_OPEN_FLT (read-only): Cell balancing open load detection - (info) Logic OR of CBx_OPEN_FLT is provided in the FAULT2_STATUS[CB_OPEN_FLT]. */
#define MC33772C_CB_OPEN_FLT_CB1_OPEN_FLT_SHIFT              (0x0U)
#define MC33772C_CB_OPEN_FLT_CB1_OPEN_FLT_MASK               (0x1U)
#define MC33772C_CB_OPEN_FLT_CB1_OPEN_FLT(x)                 ((uint16_t)((uint16_t)(x) << MC33772C_CB_OPEN_FLT_CB1_OPEN_FLT_SHIFT) & MC33772C_CB_OPEN_FLT_CB1_OPEN_FLT_MASK)

/* Enumerated value NO_FAULT: No open load cell balance fault detected. */
#define MC33772C_CB_OPEN_FLT_CB1_OPEN_FLT_NO_FAULT_ENUM_VAL  (0x0U)

/* Enumerated value FAULT: Off state open load detected. */
#define MC33772C_CB_OPEN_FLT_CB1_OPEN_FLT_FAULT_ENUM_VAL     (0x1U)

/* Field CB2_OPEN_FLT (read-only): Cell balancing open load detection - (info) Logic OR of CBx_OPEN_FLT is provided in the FAULT2_STATUS[CB_OPEN_FLT]. */
#define MC33772C_CB_OPEN_FLT_CB2_OPEN_FLT_SHIFT              (0x1U)
#define MC33772C_CB_OPEN_FLT_CB2_OPEN_FLT_MASK               (0x2U)
#define MC33772C_CB_OPEN_FLT_CB2_OPEN_FLT(x)                 ((uint16_t)((uint16_t)(x) << MC33772C_CB_OPEN_FLT_CB2_OPEN_FLT_SHIFT) & MC33772C_CB_OPEN_FLT_CB2_OPEN_FLT_MASK)

/* Enumerated value NO_FAULT: No open load cell balance fault detected. */
#define MC33772C_CB_OPEN_FLT_CB2_OPEN_FLT_NO_FAULT_ENUM_VAL  (0x0U)

/* Enumerated value FAULT: Off state open load detected. */
#define MC33772C_CB_OPEN_FLT_CB2_OPEN_FLT_FAULT_ENUM_VAL     (0x1U)

/* Field CB3_OPEN_FLT (read-only): Cell balancing open load detection - (info) Logic OR of CBx_OPEN_FLT is provided in the FAULT2_STATUS[CB_OPEN_FLT]. */
#define MC33772C_CB_OPEN_FLT_CB3_OPEN_FLT_SHIFT              (0x2U)
#define MC33772C_CB_OPEN_FLT_CB3_OPEN_FLT_MASK               (0x4U)
#define MC33772C_CB_OPEN_FLT_CB3_OPEN_FLT(x)                 ((uint16_t)((uint16_t)(x) << MC33772C_CB_OPEN_FLT_CB3_OPEN_FLT_SHIFT) & MC33772C_CB_OPEN_FLT_CB3_OPEN_FLT_MASK)

/* Enumerated value NO_FAULT: No open load cell balance fault detected. */
#define MC33772C_CB_OPEN_FLT_CB3_OPEN_FLT_NO_FAULT_ENUM_VAL  (0x0U)

/* Enumerated value FAULT: Off state open load detected. */
#define MC33772C_CB_OPEN_FLT_CB3_OPEN_FLT_FAULT_ENUM_VAL     (0x1U)

/* Field CB4_OPEN_FLT (read-only): Cell balancing open load detection - (info) Logic OR of CBx_OPEN_FLT is provided in the FAULT2_STATUS[CB_OPEN_FLT]. */
#define MC33772C_CB_OPEN_FLT_CB4_OPEN_FLT_SHIFT              (0x3U)
#define MC33772C_CB_OPEN_FLT_CB4_OPEN_FLT_MASK               (0x8U)
#define MC33772C_CB_OPEN_FLT_CB4_OPEN_FLT(x)                 ((uint16_t)((uint16_t)(x) << MC33772C_CB_OPEN_FLT_CB4_OPEN_FLT_SHIFT) & MC33772C_CB_OPEN_FLT_CB4_OPEN_FLT_MASK)

/* Enumerated value NO_FAULT: No open load cell balance fault detected. */
#define MC33772C_CB_OPEN_FLT_CB4_OPEN_FLT_NO_FAULT_ENUM_VAL  (0x0U)

/* Enumerated value FAULT: Off state open load detected. */
#define MC33772C_CB_OPEN_FLT_CB4_OPEN_FLT_FAULT_ENUM_VAL     (0x1U)

/* Field CB5_OPEN_FLT (read-only): Cell balancing open load detection - (info) Logic OR of CBx_OPEN_FLT is provided in the FAULT2_STATUS[CB_OPEN_FLT]. */
#define MC33772C_CB_OPEN_FLT_CB5_OPEN_FLT_SHIFT              (0x4U)
#define MC33772C_CB_OPEN_FLT_CB5_OPEN_FLT_MASK               (0x10U)
#define MC33772C_CB_OPEN_FLT_CB5_OPEN_FLT(x)                 ((uint16_t)((uint16_t)(x) << MC33772C_CB_OPEN_FLT_CB5_OPEN_FLT_SHIFT) & MC33772C_CB_OPEN_FLT_CB5_OPEN_FLT_MASK)

/* Enumerated value NO_FAULT: No open load cell balance fault detected. */
#define MC33772C_CB_OPEN_FLT_CB5_OPEN_FLT_NO_FAULT_ENUM_VAL  (0x0U)

/* Enumerated value FAULT: Off state open load detected. */
#define MC33772C_CB_OPEN_FLT_CB5_OPEN_FLT_FAULT_ENUM_VAL     (0x1U)

/* Field CB6_OPEN_FLT (read-only): Cell balancing open load detection - (info) Logic OR of CBx_OPEN_FLT is provided in the FAULT2_STATUS[CB_OPEN_FLT]. */
#define MC33772C_CB_OPEN_FLT_CB6_OPEN_FLT_SHIFT              (0x5U)
#define MC33772C_CB_OPEN_FLT_CB6_OPEN_FLT_MASK               (0x20U)
#define MC33772C_CB_OPEN_FLT_CB6_OPEN_FLT(x)                 ((uint16_t)((uint16_t)(x) << MC33772C_CB_OPEN_FLT_CB6_OPEN_FLT_SHIFT) & MC33772C_CB_OPEN_FLT_CB6_OPEN_FLT_MASK)

/* Enumerated value NO_FAULT: No open load cell balance fault detected. */
#define MC33772C_CB_OPEN_FLT_CB6_OPEN_FLT_NO_FAULT_ENUM_VAL  (0x0U)

/* Enumerated value FAULT: Off state open load detected. */
#define MC33772C_CB_OPEN_FLT_CB6_OPEN_FLT_FAULT_ENUM_VAL     (0x1U)

/* --------------------------------------------------------------------------
 * CB_SHORT_FLT (read-only): Cell balance shorted load fault detection register.
 * -------------------------------------------------------------------------- */
#define MC33772C_CB_SHORT_FLT_OFFSET                         (0x1BU)
#define MC33772C_CB_SHORT_FLT_POR_VAL                        (0x0U)

/* Field CB1_SHORT_FLT (read-only): Cell balancing shorted load fault detection - (info) CBx_SHORT_FLT Ored is provided in the FAULT2[CB_SHORT_FLT]. */
#define MC33772C_CB_SHORT_FLT_CB1_SHORT_FLT_SHIFT            (0x0U)
#define MC33772C_CB_SHORT_FLT_CB1_SHORT_FLT_MASK             (0x1U)
#define MC33772C_CB_SHORT_FLT_CB1_SHORT_FLT(x)               ((uint16_t)((uint16_t)(x) << MC33772C_CB_SHORT_FLT_CB1_SHORT_FLT_SHIFT) & MC33772C_CB_SHORT_FLT_CB1_SHORT_FLT_MASK)

/* Enumerated value NO_FAULT: No shorted load cell balance fault detected. */
#define MC33772C_CB_SHORT_FLT_CB1_SHORT_FLT_NO_FAULT_ENUM_VAL (0x0U)

/* Enumerated value FAULT: Shorted load fault detected. */
#define MC33772C_CB_SHORT_FLT_CB1_SHORT_FLT_FAULT_ENUM_VAL   (0x1U)

/* Field CB2_SHORT_FLT (read-only): Cell balancing shorted load fault detection - (info) CBx_SHORT_FLT Ored is provided in the FAULT2[CB_SHORT_FLT]. */
#define MC33772C_CB_SHORT_FLT_CB2_SHORT_FLT_SHIFT            (0x1U)
#define MC33772C_CB_SHORT_FLT_CB2_SHORT_FLT_MASK             (0x2U)
#define MC33772C_CB_SHORT_FLT_CB2_SHORT_FLT(x)               ((uint16_t)((uint16_t)(x) << MC33772C_CB_SHORT_FLT_CB2_SHORT_FLT_SHIFT) & MC33772C_CB_SHORT_FLT_CB2_SHORT_FLT_MASK)

/* Enumerated value NO_FAULT: No shorted load cell balance fault detected. */
#define MC33772C_CB_SHORT_FLT_CB2_SHORT_FLT_NO_FAULT_ENUM_VAL (0x0U)

/* Enumerated value FAULT: Shorted load fault detected. */
#define MC33772C_CB_SHORT_FLT_CB2_SHORT_FLT_FAULT_ENUM_VAL   (0x1U)

/* Field CB3_SHORT_FLT (read-only): Cell balancing shorted load fault detection - (info) CBx_SHORT_FLT Ored is provided in the FAULT2[CB_SHORT_FLT]. */
#define MC33772C_CB_SHORT_FLT_CB3_SHORT_FLT_SHIFT            (0x2U)
#define MC33772C_CB_SHORT_FLT_CB3_SHORT_FLT_MASK             (0x4U)
#define MC33772C_CB_SHORT_FLT_CB3_SHORT_FLT(x)               ((uint16_t)((uint16_t)(x) << MC33772C_CB_SHORT_FLT_CB3_SHORT_FLT_SHIFT) & MC33772C_CB_SHORT_FLT_CB3_SHORT_FLT_MASK)

/* Enumerated value NO_FAULT: No shorted load cell balance fault detected. */
#define MC33772C_CB_SHORT_FLT_CB3_SHORT_FLT_NO_FAULT_ENUM_VAL (0x0U)

/* Enumerated value FAULT: Shorted load fault detected. */
#define MC33772C_CB_SHORT_FLT_CB3_SHORT_FLT_FAULT_ENUM_VAL   (0x1U)

/* Field CB4_SHORT_FLT (read-only): Cell balancing shorted load fault detection - (info) CBx_SHORT_FLT Ored is provided in the FAULT2[CB_SHORT_FLT]. */
#define MC33772C_CB_SHORT_FLT_CB4_SHORT_FLT_SHIFT            (0x3U)
#define MC33772C_CB_SHORT_FLT_CB4_SHORT_FLT_MASK             (0x8U)
#define MC33772C_CB_SHORT_FLT_CB4_SHORT_FLT(x)               ((uint16_t)((uint16_t)(x) << MC33772C_CB_SHORT_FLT_CB4_SHORT_FLT_SHIFT) & MC33772C_CB_SHORT_FLT_CB4_SHORT_FLT_MASK)

/* Enumerated value NO_FAULT: No shorted load cell balance fault detected. */
#define MC33772C_CB_SHORT_FLT_CB4_SHORT_FLT_NO_FAULT_ENUM_VAL (0x0U)

/* Enumerated value FAULT: Shorted load fault detected. */
#define MC33772C_CB_SHORT_FLT_CB4_SHORT_FLT_FAULT_ENUM_VAL   (0x1U)

/* Field CB5_SHORT_FLT (read-only): Cell balancing shorted load fault detection - (info) CBx_SHORT_FLT Ored is provided in the FAULT2[CB_SHORT_FLT]. */
#define MC33772C_CB_SHORT_FLT_CB5_SHORT_FLT_SHIFT            (0x4U)
#define MC33772C_CB_SHORT_FLT_CB5_SHORT_FLT_MASK             (0x10U)
#define MC33772C_CB_SHORT_FLT_CB5_SHORT_FLT(x)               ((uint16_t)((uint16_t)(x) << MC33772C_CB_SHORT_FLT_CB5_SHORT_FLT_SHIFT) & MC33772C_CB_SHORT_FLT_CB5_SHORT_FLT_MASK)

/* Enumerated value NO_FAULT: No shorted load cell balance fault detected. */
#define MC33772C_CB_SHORT_FLT_CB5_SHORT_FLT_NO_FAULT_ENUM_VAL (0x0U)

/* Enumerated value FAULT: Shorted load fault detected. */
#define MC33772C_CB_SHORT_FLT_CB5_SHORT_FLT_FAULT_ENUM_VAL   (0x1U)

/* Field CB6_SHORT_FLT (read-only): Cell balancing shorted load fault detection - (info) CBx_SHORT_FLT Ored is provided in the FAULT2[CB_SHORT_FLT]. */
#define MC33772C_CB_SHORT_FLT_CB6_SHORT_FLT_SHIFT            (0x5U)
#define MC33772C_CB_SHORT_FLT_CB6_SHORT_FLT_MASK             (0x20U)
#define MC33772C_CB_SHORT_FLT_CB6_SHORT_FLT(x)               ((uint16_t)((uint16_t)(x) << MC33772C_CB_SHORT_FLT_CB6_SHORT_FLT_SHIFT) & MC33772C_CB_SHORT_FLT_CB6_SHORT_FLT_MASK)

/* Enumerated value NO_FAULT: No shorted load cell balance fault detected. */
#define MC33772C_CB_SHORT_FLT_CB6_SHORT_FLT_NO_FAULT_ENUM_VAL (0x0U)

/* Enumerated value FAULT: Shorted load fault detected. */
#define MC33772C_CB_SHORT_FLT_CB6_SHORT_FLT_FAULT_ENUM_VAL   (0x1U)

/* --------------------------------------------------------------------------
 * CB_DRV_STS (read-only): Cell balance driver on/off status register.
 * -------------------------------------------------------------------------- */
#define MC33772C_CB_DRV_STS_OFFSET                           (0x1CU)
#define MC33772C_CB_DRV_STS_POR_VAL                          (0x0U)

/* Field CB1_STS (read-only): Contains the state of the cell balance driver. */
#define MC33772C_CB_DRV_STS_CB1_STS_SHIFT                    (0x0U)
#define MC33772C_CB_DRV_STS_CB1_STS_MASK                     (0x1U)
#define MC33772C_CB_DRV_STS_CB1_STS(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_CB_DRV_STS_CB1_STS_SHIFT) & MC33772C_CB_DRV_STS_CB1_STS_MASK)

/* Enumerated value False: Driver is off. */
#define MC33772C_CB_DRV_STS_CB1_STS_FALSE_ENUM_VAL           (0x0U)

/* Enumerated value True: Driver is on. */
#define MC33772C_CB_DRV_STS_CB1_STS_TRUE_ENUM_VAL            (0x1U)

/* Field CB2_STS (read-only): Contains the state of the cell balance driver. */
#define MC33772C_CB_DRV_STS_CB2_STS_SHIFT                    (0x1U)
#define MC33772C_CB_DRV_STS_CB2_STS_MASK                     (0x2U)
#define MC33772C_CB_DRV_STS_CB2_STS(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_CB_DRV_STS_CB2_STS_SHIFT) & MC33772C_CB_DRV_STS_CB2_STS_MASK)

/* Enumerated value False: Driver is off. */
#define MC33772C_CB_DRV_STS_CB2_STS_FALSE_ENUM_VAL           (0x0U)

/* Enumerated value True: Driver is on. */
#define MC33772C_CB_DRV_STS_CB2_STS_TRUE_ENUM_VAL            (0x1U)

/* Field CB3_STS (read-only): Contains the state of the cell balance driver. */
#define MC33772C_CB_DRV_STS_CB3_STS_SHIFT                    (0x2U)
#define MC33772C_CB_DRV_STS_CB3_STS_MASK                     (0x4U)
#define MC33772C_CB_DRV_STS_CB3_STS(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_CB_DRV_STS_CB3_STS_SHIFT) & MC33772C_CB_DRV_STS_CB3_STS_MASK)

/* Enumerated value False: Driver is off. */
#define MC33772C_CB_DRV_STS_CB3_STS_FALSE_ENUM_VAL           (0x0U)

/* Enumerated value True: Driver is on. */
#define MC33772C_CB_DRV_STS_CB3_STS_TRUE_ENUM_VAL            (0x1U)

/* Field CB4_STS (read-only): Contains the state of the cell balance driver. */
#define MC33772C_CB_DRV_STS_CB4_STS_SHIFT                    (0x3U)
#define MC33772C_CB_DRV_STS_CB4_STS_MASK                     (0x8U)
#define MC33772C_CB_DRV_STS_CB4_STS(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_CB_DRV_STS_CB4_STS_SHIFT) & MC33772C_CB_DRV_STS_CB4_STS_MASK)

/* Enumerated value False: Driver is off. */
#define MC33772C_CB_DRV_STS_CB4_STS_FALSE_ENUM_VAL           (0x0U)

/* Enumerated value True: Driver is on. */
#define MC33772C_CB_DRV_STS_CB4_STS_TRUE_ENUM_VAL            (0x1U)

/* Field CB5_STS (read-only): Contains the state of the cell balance driver. */
#define MC33772C_CB_DRV_STS_CB5_STS_SHIFT                    (0x4U)
#define MC33772C_CB_DRV_STS_CB5_STS_MASK                     (0x10U)
#define MC33772C_CB_DRV_STS_CB5_STS(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_CB_DRV_STS_CB5_STS_SHIFT) & MC33772C_CB_DRV_STS_CB5_STS_MASK)

/* Enumerated value False: Driver is off. */
#define MC33772C_CB_DRV_STS_CB5_STS_FALSE_ENUM_VAL           (0x0U)

/* Enumerated value True: Driver is on. */
#define MC33772C_CB_DRV_STS_CB5_STS_TRUE_ENUM_VAL            (0x1U)

/* Field CB6_STS (read-only): Contains the state of the cell balance driver. */
#define MC33772C_CB_DRV_STS_CB6_STS_SHIFT                    (0x5U)
#define MC33772C_CB_DRV_STS_CB6_STS_MASK                     (0x20U)
#define MC33772C_CB_DRV_STS_CB6_STS(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_CB_DRV_STS_CB6_STS_SHIFT) & MC33772C_CB_DRV_STS_CB6_STS_MASK)

/* Enumerated value False: Driver is off. */
#define MC33772C_CB_DRV_STS_CB6_STS_FALSE_ENUM_VAL           (0x0U)

/* Enumerated value True: Driver is on. */
#define MC33772C_CB_DRV_STS_CB6_STS_TRUE_ENUM_VAL            (0x1U)

/* --------------------------------------------------------------------------
 * GPIO_CFG1 (read-write): GPIO configuration register 1.
 * -------------------------------------------------------------------------- */
#define MC33772C_GPIO_CFG1_OFFSET                            (0x1DU)
#define MC33772C_GPIO_CFG1_POR_VAL                           (0x0U)

/* Field GPIO0_CFG: Register controls the configuration of the GPIO port. */
#define MC33772C_GPIO_CFG1_GPIO0_CFG_SHIFT                   (0x0U)
#define MC33772C_GPIO_CFG1_GPIO0_CFG_MASK                    (0x3U)
#define MC33772C_GPIO_CFG1_GPIO0_CFG(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_CFG1_GPIO0_CFG_SHIFT) & MC33772C_GPIO_CFG1_GPIO0_CFG_MASK)

/* Enumerated value ANALOG_RATIO: GPIOx configured as analog input for ratiometric measurement. */
#define MC33772C_GPIO_CFG1_GPIO0_CFG_ANALOG_RATIO_ENUM_VAL   (0x0U)

/* Enumerated value ANALOG_ABS: GPIOx configured as analog input for absolute measurement. */
#define MC33772C_GPIO_CFG1_GPIO0_CFG_ANALOG_ABS_ENUM_VAL     (0x1U)

/* Enumerated value DIGITAL_IN: GPIOx configured as digital input. */
#define MC33772C_GPIO_CFG1_GPIO0_CFG_DIGITAL_IN_ENUM_VAL     (0x2U)

/* Enumerated value DIGITAL_OUT: GPIOx configured as digital output. */
#define MC33772C_GPIO_CFG1_GPIO0_CFG_DIGITAL_OUT_ENUM_VAL    (0x3U)

/* Field GPIO1_CFG: Register controls the configuration of the GPIO port. */
#define MC33772C_GPIO_CFG1_GPIO1_CFG_SHIFT                   (0x2U)
#define MC33772C_GPIO_CFG1_GPIO1_CFG_MASK                    (0xCU)
#define MC33772C_GPIO_CFG1_GPIO1_CFG(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_CFG1_GPIO1_CFG_SHIFT) & MC33772C_GPIO_CFG1_GPIO1_CFG_MASK)

/* Enumerated value ANALOG_RATIO: GPIOx configured as analog input for ratiometric measurement. */
#define MC33772C_GPIO_CFG1_GPIO1_CFG_ANALOG_RATIO_ENUM_VAL   (0x0U)

/* Enumerated value ANALOG_ABS: GPIOx configured as analog input for absolute measurement. */
#define MC33772C_GPIO_CFG1_GPIO1_CFG_ANALOG_ABS_ENUM_VAL     (0x1U)

/* Enumerated value DIGITAL_IN: GPIOx configured as digital input. */
#define MC33772C_GPIO_CFG1_GPIO1_CFG_DIGITAL_IN_ENUM_VAL     (0x2U)

/* Enumerated value DIGITAL_OUT: GPIOx configured as digital output. */
#define MC33772C_GPIO_CFG1_GPIO1_CFG_DIGITAL_OUT_ENUM_VAL    (0x3U)

/* Field GPIO2_CFG: Register controls the configuration of the GPIO port. */
#define MC33772C_GPIO_CFG1_GPIO2_CFG_SHIFT                   (0x4U)
#define MC33772C_GPIO_CFG1_GPIO2_CFG_MASK                    (0x30U)
#define MC33772C_GPIO_CFG1_GPIO2_CFG(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_CFG1_GPIO2_CFG_SHIFT) & MC33772C_GPIO_CFG1_GPIO2_CFG_MASK)

/* Enumerated value ANALOG_RATIO: GPIOx configured as analog input for ratiometric measurement. */
#define MC33772C_GPIO_CFG1_GPIO2_CFG_ANALOG_RATIO_ENUM_VAL   (0x0U)

/* Enumerated value ANALOG_ABS: GPIOx configured as analog input for absolute measurement. */
#define MC33772C_GPIO_CFG1_GPIO2_CFG_ANALOG_ABS_ENUM_VAL     (0x1U)

/* Enumerated value DIGITAL_IN: GPIOx configured as digital input. */
#define MC33772C_GPIO_CFG1_GPIO2_CFG_DIGITAL_IN_ENUM_VAL     (0x2U)

/* Enumerated value DIGITAL_OUT: GPIOx configured as digital output. */
#define MC33772C_GPIO_CFG1_GPIO2_CFG_DIGITAL_OUT_ENUM_VAL    (0x3U)

/* Field GPIO3_CFG: Register controls the configuration of the GPIO port. */
#define MC33772C_GPIO_CFG1_GPIO3_CFG_SHIFT                   (0x6U)
#define MC33772C_GPIO_CFG1_GPIO3_CFG_MASK                    (0xC0U)
#define MC33772C_GPIO_CFG1_GPIO3_CFG(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_CFG1_GPIO3_CFG_SHIFT) & MC33772C_GPIO_CFG1_GPIO3_CFG_MASK)

/* Enumerated value ANALOG_RATIO: GPIOx configured as analog input for ratiometric measurement. */
#define MC33772C_GPIO_CFG1_GPIO3_CFG_ANALOG_RATIO_ENUM_VAL   (0x0U)

/* Enumerated value ANALOG_ABS: GPIOx configured as analog input for absolute measurement. */
#define MC33772C_GPIO_CFG1_GPIO3_CFG_ANALOG_ABS_ENUM_VAL     (0x1U)

/* Enumerated value DIGITAL_IN: GPIOx configured as digital input. */
#define MC33772C_GPIO_CFG1_GPIO3_CFG_DIGITAL_IN_ENUM_VAL     (0x2U)

/* Enumerated value DIGITAL_OUT: GPIOx configured as digital output. */
#define MC33772C_GPIO_CFG1_GPIO3_CFG_DIGITAL_OUT_ENUM_VAL    (0x3U)

/* Field GPIO4_CFG: Register controls the configuration of the GPIO port. */
#define MC33772C_GPIO_CFG1_GPIO4_CFG_SHIFT                   (0x8U)
#define MC33772C_GPIO_CFG1_GPIO4_CFG_MASK                    (0x300U)
#define MC33772C_GPIO_CFG1_GPIO4_CFG(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_CFG1_GPIO4_CFG_SHIFT) & MC33772C_GPIO_CFG1_GPIO4_CFG_MASK)

/* Enumerated value ANALOG_RATIO: GPIOx configured as analog input for ratiometric measurement. */
#define MC33772C_GPIO_CFG1_GPIO4_CFG_ANALOG_RATIO_ENUM_VAL   (0x0U)

/* Enumerated value ANALOG_ABS: GPIOx configured as analog input for absolute measurement. */
#define MC33772C_GPIO_CFG1_GPIO4_CFG_ANALOG_ABS_ENUM_VAL     (0x1U)

/* Enumerated value DIGITAL_IN: GPIOx configured as digital input. */
#define MC33772C_GPIO_CFG1_GPIO4_CFG_DIGITAL_IN_ENUM_VAL     (0x2U)

/* Enumerated value DIGITAL_OUT: GPIOx configured as digital output. */
#define MC33772C_GPIO_CFG1_GPIO4_CFG_DIGITAL_OUT_ENUM_VAL    (0x3U)

/* Field GPIO5_CFG: Register controls the configuration of the GPIO port. */
#define MC33772C_GPIO_CFG1_GPIO5_CFG_SHIFT                   (0xAU)
#define MC33772C_GPIO_CFG1_GPIO5_CFG_MASK                    (0xC00U)
#define MC33772C_GPIO_CFG1_GPIO5_CFG(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_CFG1_GPIO5_CFG_SHIFT) & MC33772C_GPIO_CFG1_GPIO5_CFG_MASK)

/* Enumerated value ANALOG_RATIO: GPIOx configured as analog input for ratiometric measurement. */
#define MC33772C_GPIO_CFG1_GPIO5_CFG_ANALOG_RATIO_ENUM_VAL   (0x0U)

/* Enumerated value ANALOG_ABS: GPIOx configured as analog input for absolute measurement. */
#define MC33772C_GPIO_CFG1_GPIO5_CFG_ANALOG_ABS_ENUM_VAL     (0x1U)

/* Enumerated value DIGITAL_IN: GPIOx configured as digital input. */
#define MC33772C_GPIO_CFG1_GPIO5_CFG_DIGITAL_IN_ENUM_VAL     (0x2U)

/* Enumerated value DIGITAL_OUT: GPIOx configured as digital output. */
#define MC33772C_GPIO_CFG1_GPIO5_CFG_DIGITAL_OUT_ENUM_VAL    (0x3U)

/* Field GPIO6_CFG: Register controls the configuration of the GPIO port. */
#define MC33772C_GPIO_CFG1_GPIO6_CFG_SHIFT                   (0xCU)
#define MC33772C_GPIO_CFG1_GPIO6_CFG_MASK                    (0x3000U)
#define MC33772C_GPIO_CFG1_GPIO6_CFG(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_CFG1_GPIO6_CFG_SHIFT) & MC33772C_GPIO_CFG1_GPIO6_CFG_MASK)

/* Enumerated value ANALOG_RATIO: GPIOx configured as analog input for ratiometric measurement. */
#define MC33772C_GPIO_CFG1_GPIO6_CFG_ANALOG_RATIO_ENUM_VAL   (0x0U)

/* Enumerated value ANALOG_ABS: GPIOx configured as analog input for absolute measurement. */
#define MC33772C_GPIO_CFG1_GPIO6_CFG_ANALOG_ABS_ENUM_VAL     (0x1U)

/* Enumerated value DIGITAL_IN: GPIOx configured as digital input. */
#define MC33772C_GPIO_CFG1_GPIO6_CFG_DIGITAL_IN_ENUM_VAL     (0x2U)

/* Enumerated value DIGITAL_OUT: GPIOx configured as digital output. */
#define MC33772C_GPIO_CFG1_GPIO6_CFG_DIGITAL_OUT_ENUM_VAL    (0x3U)

/* --------------------------------------------------------------------------
 * GPIO_CFG2 (read-write): GPIO configuration register 2.
 * -------------------------------------------------------------------------- */
#define MC33772C_GPIO_CFG2_OFFSET                            (0x1EU)
#define MC33772C_GPIO_CFG2_POR_VAL                           (0x0U)

/* Field GPIO0_DR: GPIOx pin drive. Ignored when GPIOx_CFG = 0b11. */
#define MC33772C_GPIO_CFG2_GPIO0_DR_SHIFT                    (0x0U)
#define MC33772C_GPIO_CFG2_GPIO0_DR_MASK                     (0x1U)
#define MC33772C_GPIO_CFG2_GPIO0_DR(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_CFG2_GPIO0_DR_SHIFT) & MC33772C_GPIO_CFG2_GPIO0_DR_MASK)

/* Enumerated value LOW: Drive GPIOx to low level. */
#define MC33772C_GPIO_CFG2_GPIO0_DR_LOW_ENUM_VAL             (0x0U)

/* Enumerated value HIGH: Drive GPIOx to high level. */
#define MC33772C_GPIO_CFG2_GPIO0_DR_HIGH_ENUM_VAL            (0x1U)

/* Field GPIO1_DR: GPIOx pin drive. Ignored when GPIOx_CFG = 0b11. */
#define MC33772C_GPIO_CFG2_GPIO1_DR_SHIFT                    (0x1U)
#define MC33772C_GPIO_CFG2_GPIO1_DR_MASK                     (0x2U)
#define MC33772C_GPIO_CFG2_GPIO1_DR(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_CFG2_GPIO1_DR_SHIFT) & MC33772C_GPIO_CFG2_GPIO1_DR_MASK)

/* Enumerated value LOW: Drive GPIOx to low level. */
#define MC33772C_GPIO_CFG2_GPIO1_DR_LOW_ENUM_VAL             (0x0U)

/* Enumerated value HIGH: Drive GPIOx to high level. */
#define MC33772C_GPIO_CFG2_GPIO1_DR_HIGH_ENUM_VAL            (0x1U)

/* Field GPIO2_DR: GPIOx pin drive. Ignored when GPIOx_CFG = 0b11. */
#define MC33772C_GPIO_CFG2_GPIO2_DR_SHIFT                    (0x2U)
#define MC33772C_GPIO_CFG2_GPIO2_DR_MASK                     (0x4U)
#define MC33772C_GPIO_CFG2_GPIO2_DR(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_CFG2_GPIO2_DR_SHIFT) & MC33772C_GPIO_CFG2_GPIO2_DR_MASK)

/* Enumerated value LOW: Drive GPIOx to low level. */
#define MC33772C_GPIO_CFG2_GPIO2_DR_LOW_ENUM_VAL             (0x0U)

/* Enumerated value HIGH: Drive GPIOx to high level. */
#define MC33772C_GPIO_CFG2_GPIO2_DR_HIGH_ENUM_VAL            (0x1U)

/* Field GPIO3_DR: GPIOx pin drive. Ignored when GPIOx_CFG = 0b11. */
#define MC33772C_GPIO_CFG2_GPIO3_DR_SHIFT                    (0x3U)
#define MC33772C_GPIO_CFG2_GPIO3_DR_MASK                     (0x8U)
#define MC33772C_GPIO_CFG2_GPIO3_DR(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_CFG2_GPIO3_DR_SHIFT) & MC33772C_GPIO_CFG2_GPIO3_DR_MASK)

/* Enumerated value LOW: Drive GPIOx to low level. */
#define MC33772C_GPIO_CFG2_GPIO3_DR_LOW_ENUM_VAL             (0x0U)

/* Enumerated value HIGH: Drive GPIOx to high level. */
#define MC33772C_GPIO_CFG2_GPIO3_DR_HIGH_ENUM_VAL            (0x1U)

/* Field GPIO4_DR: GPIOx pin drive. Ignored when GPIOx_CFG = 0b11. */
#define MC33772C_GPIO_CFG2_GPIO4_DR_SHIFT                    (0x4U)
#define MC33772C_GPIO_CFG2_GPIO4_DR_MASK                     (0x10U)
#define MC33772C_GPIO_CFG2_GPIO4_DR(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_CFG2_GPIO4_DR_SHIFT) & MC33772C_GPIO_CFG2_GPIO4_DR_MASK)

/* Enumerated value LOW: Drive GPIOx to low level. */
#define MC33772C_GPIO_CFG2_GPIO4_DR_LOW_ENUM_VAL             (0x0U)

/* Enumerated value HIGH: Drive GPIOx to high level. */
#define MC33772C_GPIO_CFG2_GPIO4_DR_HIGH_ENUM_VAL            (0x1U)

/* Field GPIO5_DR: GPIOx pin drive. Ignored when GPIOx_CFG = 0b11. */
#define MC33772C_GPIO_CFG2_GPIO5_DR_SHIFT                    (0x5U)
#define MC33772C_GPIO_CFG2_GPIO5_DR_MASK                     (0x20U)
#define MC33772C_GPIO_CFG2_GPIO5_DR(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_CFG2_GPIO5_DR_SHIFT) & MC33772C_GPIO_CFG2_GPIO5_DR_MASK)

/* Enumerated value LOW: Drive GPIOx to low level. */
#define MC33772C_GPIO_CFG2_GPIO5_DR_LOW_ENUM_VAL             (0x0U)

/* Enumerated value HIGH: Drive GPIOx to high level. */
#define MC33772C_GPIO_CFG2_GPIO5_DR_HIGH_ENUM_VAL            (0x1U)

/* Field GPIO6_DR: GPIOx pin drive. Ignored when GPIOx_CFG = 0b11. */
#define MC33772C_GPIO_CFG2_GPIO6_DR_SHIFT                    (0x6U)
#define MC33772C_GPIO_CFG2_GPIO6_DR_MASK                     (0x40U)
#define MC33772C_GPIO_CFG2_GPIO6_DR(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_CFG2_GPIO6_DR_SHIFT) & MC33772C_GPIO_CFG2_GPIO6_DR_MASK)

/* Enumerated value LOW: Drive GPIOx to low level. */
#define MC33772C_GPIO_CFG2_GPIO6_DR_LOW_ENUM_VAL             (0x0U)

/* Enumerated value HIGH: Drive GPIOx to high level. */
#define MC33772C_GPIO_CFG2_GPIO6_DR_HIGH_ENUM_VAL            (0x1U)

/* Field GPIO0_FLT_ACT: GPIO0 activate fault output pin. Valid only when GPIO0_CFG = 0b10. */
#define MC33772C_GPIO_CFG2_GPIO0_FLT_ACT_SHIFT               (0x7U)
#define MC33772C_GPIO_CFG2_GPIO0_FLT_ACT_MASK                (0x80U)
#define MC33772C_GPIO_CFG2_GPIO0_FLT_ACT(x)                  ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_CFG2_GPIO0_FLT_ACT_SHIFT) & MC33772C_GPIO_CFG2_GPIO0_FLT_ACT_MASK)

/* Enumerated value DISABLED: Does not activate FAULT pin when GPIO0 is configured as an input and is logic 1. */
#define MC33772C_GPIO_CFG2_GPIO0_FLT_ACT_DISABLED_ENUM_VAL   (0x0U)

/* Enumerated value ENABLED: Activates the FAULT pin when GPIO is configured as an input and is logic 1. */
#define MC33772C_GPIO_CFG2_GPIO0_FLT_ACT_ENABLED_ENUM_VAL    (0x1U)

/* Field GPIO0_WU: GPIO0 wake-up capability. Valid only when GPIO0_CFG = 0b10. */
#define MC33772C_GPIO_CFG2_GPIO0_WU_SHIFT                    (0x8U)
#define MC33772C_GPIO_CFG2_GPIO0_WU_MASK                     (0x100U)
#define MC33772C_GPIO_CFG2_GPIO0_WU(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_CFG2_GPIO0_WU_SHIFT) & MC33772C_GPIO_CFG2_GPIO0_WU_MASK)

/* Enumerated value NO_WAKEUP: No wake-up capability. */
#define MC33772C_GPIO_CFG2_GPIO0_WU_NO_WAKEUP_ENUM_VAL       (0x0U)

/* Enumerated value WAKEUP: Wake-up on any edge, transitioning the system from sleep to normal. */
#define MC33772C_GPIO_CFG2_GPIO0_WU_WAKEUP_ENUM_VAL          (0x1U)

/* Field GPIO2_SOC: GPIO2 used as ADC1_A/ADC1_B start-of-conversion. Requires GPIO2_CFG = 0b10. */
#define MC33772C_GPIO_CFG2_GPIO2_SOC_SHIFT                   (0x9U)
#define MC33772C_GPIO_CFG2_GPIO2_SOC_MASK                    (0x200U)
#define MC33772C_GPIO_CFG2_GPIO2_SOC(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_CFG2_GPIO2_SOC_SHIFT) & MC33772C_GPIO_CFG2_GPIO2_SOC_MASK)

/* Enumerated value ADC_TRG_DISABLED: GPIO2 port ADC trigger is disabled. */
#define MC33772C_GPIO_CFG2_GPIO2_SOC_ADC_TRG_DISABLED_ENUM_VAL (0x0U)

/* Enumerated value ADC_TRG_ENABLED: GPIO2 port ADC trigger is enabled. A rising edge on GPIO2 triggers an ADC1-A and ADC1-B conversion - only when in normal mode. */
#define MC33772C_GPIO_CFG2_GPIO2_SOC_ADC_TRG_ENABLED_ENUM_VAL (0x1U)

/* --------------------------------------------------------------------------
 * GPIO_STS (read-only): GPIO status register.
 * -------------------------------------------------------------------------- */
#define MC33772C_GPIO_STS_OFFSET                             (0x1FU)
#define MC33772C_GPIO_STS_POR_VAL                            (0x0U)

/* Field GPIO0_ST (read-only): Real time GPIOx status. */
#define MC33772C_GPIO_STS_GPIO0_ST_SHIFT                     (0x0U)
#define MC33772C_GPIO_STS_GPIO0_ST_MASK                      (0x1U)
#define MC33772C_GPIO_STS_GPIO0_ST(x)                        ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_STS_GPIO0_ST_SHIFT) & MC33772C_GPIO_STS_GPIO0_ST_MASK)

/* Enumerated value REPORT_LOW: Report GPIOx at low level. */
#define MC33772C_GPIO_STS_GPIO0_ST_REPORT_LOW_ENUM_VAL       (0x0U)

/* Enumerated value REPORT_HIGH: Report GPIOx at high level. */
#define MC33772C_GPIO_STS_GPIO0_ST_REPORT_HIGH_ENUM_VAL      (0x1U)

/* Field GPIO1_ST (read-only): Real time GPIOx status. */
#define MC33772C_GPIO_STS_GPIO1_ST_SHIFT                     (0x1U)
#define MC33772C_GPIO_STS_GPIO1_ST_MASK                      (0x2U)
#define MC33772C_GPIO_STS_GPIO1_ST(x)                        ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_STS_GPIO1_ST_SHIFT) & MC33772C_GPIO_STS_GPIO1_ST_MASK)

/* Enumerated value REPORT_LOW: Report GPIOx at low level. */
#define MC33772C_GPIO_STS_GPIO1_ST_REPORT_LOW_ENUM_VAL       (0x0U)

/* Enumerated value REPORT_HIGH: Report GPIOx at high level. */
#define MC33772C_GPIO_STS_GPIO1_ST_REPORT_HIGH_ENUM_VAL      (0x1U)

/* Field GPIO2_ST (read-only): Real time GPIOx status. */
#define MC33772C_GPIO_STS_GPIO2_ST_SHIFT                     (0x2U)
#define MC33772C_GPIO_STS_GPIO2_ST_MASK                      (0x4U)
#define MC33772C_GPIO_STS_GPIO2_ST(x)                        ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_STS_GPIO2_ST_SHIFT) & MC33772C_GPIO_STS_GPIO2_ST_MASK)

/* Enumerated value REPORT_LOW: Report GPIOx at low level. */
#define MC33772C_GPIO_STS_GPIO2_ST_REPORT_LOW_ENUM_VAL       (0x0U)

/* Enumerated value REPORT_HIGH: Report GPIOx at high level. */
#define MC33772C_GPIO_STS_GPIO2_ST_REPORT_HIGH_ENUM_VAL      (0x1U)

/* Field GPIO3_ST (read-only): Real time GPIOx status. */
#define MC33772C_GPIO_STS_GPIO3_ST_SHIFT                     (0x3U)
#define MC33772C_GPIO_STS_GPIO3_ST_MASK                      (0x8U)
#define MC33772C_GPIO_STS_GPIO3_ST(x)                        ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_STS_GPIO3_ST_SHIFT) & MC33772C_GPIO_STS_GPIO3_ST_MASK)

/* Enumerated value REPORT_LOW: Report GPIOx at low level. */
#define MC33772C_GPIO_STS_GPIO3_ST_REPORT_LOW_ENUM_VAL       (0x0U)

/* Enumerated value REPORT_HIGH: Report GPIOx at high level. */
#define MC33772C_GPIO_STS_GPIO3_ST_REPORT_HIGH_ENUM_VAL      (0x1U)

/* Field GPIO4_ST (read-only): Real time GPIOx status. */
#define MC33772C_GPIO_STS_GPIO4_ST_SHIFT                     (0x4U)
#define MC33772C_GPIO_STS_GPIO4_ST_MASK                      (0x10U)
#define MC33772C_GPIO_STS_GPIO4_ST(x)                        ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_STS_GPIO4_ST_SHIFT) & MC33772C_GPIO_STS_GPIO4_ST_MASK)

/* Enumerated value REPORT_LOW: Report GPIOx at low level. */
#define MC33772C_GPIO_STS_GPIO4_ST_REPORT_LOW_ENUM_VAL       (0x0U)

/* Enumerated value REPORT_HIGH: Report GPIOx at high level. */
#define MC33772C_GPIO_STS_GPIO4_ST_REPORT_HIGH_ENUM_VAL      (0x1U)

/* Field GPIO5_ST (read-only): Real time GPIOx status. */
#define MC33772C_GPIO_STS_GPIO5_ST_SHIFT                     (0x5U)
#define MC33772C_GPIO_STS_GPIO5_ST_MASK                      (0x20U)
#define MC33772C_GPIO_STS_GPIO5_ST(x)                        ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_STS_GPIO5_ST_SHIFT) & MC33772C_GPIO_STS_GPIO5_ST_MASK)

/* Enumerated value REPORT_LOW: Report GPIOx at low level. */
#define MC33772C_GPIO_STS_GPIO5_ST_REPORT_LOW_ENUM_VAL       (0x0U)

/* Enumerated value REPORT_HIGH: Report GPIOx at high level. */
#define MC33772C_GPIO_STS_GPIO5_ST_REPORT_HIGH_ENUM_VAL      (0x1U)

/* Field GPIO6_ST (read-only): Real time GPIOx status. */
#define MC33772C_GPIO_STS_GPIO6_ST_SHIFT                     (0x6U)
#define MC33772C_GPIO_STS_GPIO6_ST_MASK                      (0x40U)
#define MC33772C_GPIO_STS_GPIO6_ST(x)                        ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_STS_GPIO6_ST_SHIFT) & MC33772C_GPIO_STS_GPIO6_ST_MASK)

/* Enumerated value REPORT_LOW: Report GPIOx at low level. */
#define MC33772C_GPIO_STS_GPIO6_ST_REPORT_LOW_ENUM_VAL       (0x0U)

/* Enumerated value REPORT_HIGH: Report GPIOx at high level. */
#define MC33772C_GPIO_STS_GPIO6_ST_REPORT_HIGH_ENUM_VAL      (0x1U)

/* Field GPIO0_H (read-only): The GPIOx_H bits detects and latches the low to high transition occurring on the GPIOx input. */
#define MC33772C_GPIO_STS_GPIO0_H_SHIFT                      (0x8U)
#define MC33772C_GPIO_STS_GPIO0_H_MASK                       (0x100U)
#define MC33772C_GPIO_STS_GPIO0_H(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_STS_GPIO0_H_SHIFT) & MC33772C_GPIO_STS_GPIO0_H_MASK)

/* Enumerated value NOT_DETECTED: No high state detected. */
#define MC33772C_GPIO_STS_GPIO0_H_NOT_DETECTED_ENUM_VAL      (0x0U)

/* Enumerated value DETECTED: A high state has been detected. */
#define MC33772C_GPIO_STS_GPIO0_H_DETECTED_ENUM_VAL          (0x1U)

/* Field GPIO1_H (read-only): The GPIOx_H bits detects and latches the low to high transition occurring on the GPIOx input. */
#define MC33772C_GPIO_STS_GPIO1_H_SHIFT                      (0x9U)
#define MC33772C_GPIO_STS_GPIO1_H_MASK                       (0x200U)
#define MC33772C_GPIO_STS_GPIO1_H(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_STS_GPIO1_H_SHIFT) & MC33772C_GPIO_STS_GPIO1_H_MASK)

/* Enumerated value NOT_DETECTED: No high state detected. */
#define MC33772C_GPIO_STS_GPIO1_H_NOT_DETECTED_ENUM_VAL      (0x0U)

/* Enumerated value DETECTED: A high state has been detected. */
#define MC33772C_GPIO_STS_GPIO1_H_DETECTED_ENUM_VAL          (0x1U)

/* Field GPIO2_H (read-only): The GPIOx_H bits detects and latches the low to high transition occurring on the GPIOx input. */
#define MC33772C_GPIO_STS_GPIO2_H_SHIFT                      (0xAU)
#define MC33772C_GPIO_STS_GPIO2_H_MASK                       (0x400U)
#define MC33772C_GPIO_STS_GPIO2_H(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_STS_GPIO2_H_SHIFT) & MC33772C_GPIO_STS_GPIO2_H_MASK)

/* Enumerated value NOT_DETECTED: No high state detected. */
#define MC33772C_GPIO_STS_GPIO2_H_NOT_DETECTED_ENUM_VAL      (0x0U)

/* Enumerated value DETECTED: A high state has been detected. */
#define MC33772C_GPIO_STS_GPIO2_H_DETECTED_ENUM_VAL          (0x1U)

/* Field GPIO3_H (read-only): The GPIOx_H bits detects and latches the low to high transition occurring on the GPIOx input. */
#define MC33772C_GPIO_STS_GPIO3_H_SHIFT                      (0xBU)
#define MC33772C_GPIO_STS_GPIO3_H_MASK                       (0x800U)
#define MC33772C_GPIO_STS_GPIO3_H(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_STS_GPIO3_H_SHIFT) & MC33772C_GPIO_STS_GPIO3_H_MASK)

/* Enumerated value NOT_DETECTED: No high state detected. */
#define MC33772C_GPIO_STS_GPIO3_H_NOT_DETECTED_ENUM_VAL      (0x0U)

/* Enumerated value DETECTED: A high state has been detected. */
#define MC33772C_GPIO_STS_GPIO3_H_DETECTED_ENUM_VAL          (0x1U)

/* Field GPIO4_H (read-only): The GPIOx_H bits detects and latches the low to high transition occurring on the GPIOx input. */
#define MC33772C_GPIO_STS_GPIO4_H_SHIFT                      (0xCU)
#define MC33772C_GPIO_STS_GPIO4_H_MASK                       (0x1000U)
#define MC33772C_GPIO_STS_GPIO4_H(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_STS_GPIO4_H_SHIFT) & MC33772C_GPIO_STS_GPIO4_H_MASK)

/* Enumerated value NOT_DETECTED: No high state detected. */
#define MC33772C_GPIO_STS_GPIO4_H_NOT_DETECTED_ENUM_VAL      (0x0U)

/* Enumerated value DETECTED: A high state has been detected. */
#define MC33772C_GPIO_STS_GPIO4_H_DETECTED_ENUM_VAL          (0x1U)

/* Field GPIO5_H (read-only): The GPIOx_H bits detects and latches the low to high transition occurring on the GPIOx input. */
#define MC33772C_GPIO_STS_GPIO5_H_SHIFT                      (0xDU)
#define MC33772C_GPIO_STS_GPIO5_H_MASK                       (0x2000U)
#define MC33772C_GPIO_STS_GPIO5_H(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_STS_GPIO5_H_SHIFT) & MC33772C_GPIO_STS_GPIO5_H_MASK)

/* Enumerated value NOT_DETECTED: No high state detected. */
#define MC33772C_GPIO_STS_GPIO5_H_NOT_DETECTED_ENUM_VAL      (0x0U)

/* Enumerated value DETECTED: A high state has been detected. */
#define MC33772C_GPIO_STS_GPIO5_H_DETECTED_ENUM_VAL          (0x1U)

/* Field GPIO6_H (read-only): The GPIOx_H bits detects and latches the low to high transition occurring on the GPIOx input. */
#define MC33772C_GPIO_STS_GPIO6_H_SHIFT                      (0xEU)
#define MC33772C_GPIO_STS_GPIO6_H_MASK                       (0x4000U)
#define MC33772C_GPIO_STS_GPIO6_H(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_STS_GPIO6_H_SHIFT) & MC33772C_GPIO_STS_GPIO6_H_MASK)

/* Enumerated value NOT_DETECTED: No high state detected. */
#define MC33772C_GPIO_STS_GPIO6_H_NOT_DETECTED_ENUM_VAL      (0x0U)

/* Enumerated value DETECTED: A high state has been detected. */
#define MC33772C_GPIO_STS_GPIO6_H_DETECTED_ENUM_VAL          (0x1U)

/* --------------------------------------------------------------------------
 * AN_OT_UT_FLT (read-only): Overtemperature/undertemperature fault register.
 * -------------------------------------------------------------------------- */
#define MC33772C_AN_OT_UT_FLT_OFFSET                         (0x20U)
#define MC33772C_AN_OT_UT_FLT_POR_VAL                        (0x0U)

/* Field AN0_UT (read-only): Undertemperature detection for ANx - ANx_UT ored is provided in FAULT1_STATUS[AN_UT_FLT]. */
#define MC33772C_AN_OT_UT_FLT_AN0_UT_SHIFT                   (0x0U)
#define MC33772C_AN_OT_UT_FLT_AN0_UT_MASK                    (0x1U)
#define MC33772C_AN_OT_UT_FLT_AN0_UT(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_AN_OT_UT_FLT_AN0_UT_SHIFT) & MC33772C_AN_OT_UT_FLT_AN0_UT_MASK)

/* Enumerated value NO_UNDERTEMP: No undertemperature fault detected. */
#define MC33772C_AN_OT_UT_FLT_AN0_UT_NO_UNDERTEMP_ENUM_VAL   (0x0U)

/* Enumerated value UNDERTEMP: Undertemperature fault detected. */
#define MC33772C_AN_OT_UT_FLT_AN0_UT_UNDERTEMP_ENUM_VAL      (0x1U)

/* Field AN1_UT (read-only): Undertemperature detection for ANx - ANx_UT ored is provided in FAULT1_STATUS[AN_UT_FLT]. */
#define MC33772C_AN_OT_UT_FLT_AN1_UT_SHIFT                   (0x1U)
#define MC33772C_AN_OT_UT_FLT_AN1_UT_MASK                    (0x2U)
#define MC33772C_AN_OT_UT_FLT_AN1_UT(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_AN_OT_UT_FLT_AN1_UT_SHIFT) & MC33772C_AN_OT_UT_FLT_AN1_UT_MASK)

/* Enumerated value NO_UNDERTEMP: No undertemperature fault detected. */
#define MC33772C_AN_OT_UT_FLT_AN1_UT_NO_UNDERTEMP_ENUM_VAL   (0x0U)

/* Enumerated value UNDERTEMP: Undertemperature fault detected. */
#define MC33772C_AN_OT_UT_FLT_AN1_UT_UNDERTEMP_ENUM_VAL      (0x1U)

/* Field AN2_UT (read-only): Undertemperature detection for ANx - ANx_UT ored is provided in FAULT1_STATUS[AN_UT_FLT]. */
#define MC33772C_AN_OT_UT_FLT_AN2_UT_SHIFT                   (0x2U)
#define MC33772C_AN_OT_UT_FLT_AN2_UT_MASK                    (0x4U)
#define MC33772C_AN_OT_UT_FLT_AN2_UT(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_AN_OT_UT_FLT_AN2_UT_SHIFT) & MC33772C_AN_OT_UT_FLT_AN2_UT_MASK)

/* Enumerated value NO_UNDERTEMP: No undertemperature fault detected. */
#define MC33772C_AN_OT_UT_FLT_AN2_UT_NO_UNDERTEMP_ENUM_VAL   (0x0U)

/* Enumerated value UNDERTEMP: Undertemperature fault detected. */
#define MC33772C_AN_OT_UT_FLT_AN2_UT_UNDERTEMP_ENUM_VAL      (0x1U)

/* Field AN3_UT (read-only): Undertemperature detection for ANx - ANx_UT ored is provided in FAULT1_STATUS[AN_UT_FLT]. */
#define MC33772C_AN_OT_UT_FLT_AN3_UT_SHIFT                   (0x3U)
#define MC33772C_AN_OT_UT_FLT_AN3_UT_MASK                    (0x8U)
#define MC33772C_AN_OT_UT_FLT_AN3_UT(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_AN_OT_UT_FLT_AN3_UT_SHIFT) & MC33772C_AN_OT_UT_FLT_AN3_UT_MASK)

/* Enumerated value NO_UNDERTEMP: No undertemperature fault detected. */
#define MC33772C_AN_OT_UT_FLT_AN3_UT_NO_UNDERTEMP_ENUM_VAL   (0x0U)

/* Enumerated value UNDERTEMP: Undertemperature fault detected. */
#define MC33772C_AN_OT_UT_FLT_AN3_UT_UNDERTEMP_ENUM_VAL      (0x1U)

/* Field AN4_UT (read-only): Undertemperature detection for ANx - ANx_UT ored is provided in FAULT1_STATUS[AN_UT_FLT]. */
#define MC33772C_AN_OT_UT_FLT_AN4_UT_SHIFT                   (0x4U)
#define MC33772C_AN_OT_UT_FLT_AN4_UT_MASK                    (0x10U)
#define MC33772C_AN_OT_UT_FLT_AN4_UT(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_AN_OT_UT_FLT_AN4_UT_SHIFT) & MC33772C_AN_OT_UT_FLT_AN4_UT_MASK)

/* Enumerated value NO_UNDERTEMP: No undertemperature fault detected. */
#define MC33772C_AN_OT_UT_FLT_AN4_UT_NO_UNDERTEMP_ENUM_VAL   (0x0U)

/* Enumerated value UNDERTEMP: Undertemperature fault detected. */
#define MC33772C_AN_OT_UT_FLT_AN4_UT_UNDERTEMP_ENUM_VAL      (0x1U)

/* Field AN5_UT (read-only): Undertemperature detection for ANx - ANx_UT ored is provided in FAULT1_STATUS[AN_UT_FLT]. */
#define MC33772C_AN_OT_UT_FLT_AN5_UT_SHIFT                   (0x5U)
#define MC33772C_AN_OT_UT_FLT_AN5_UT_MASK                    (0x20U)
#define MC33772C_AN_OT_UT_FLT_AN5_UT(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_AN_OT_UT_FLT_AN5_UT_SHIFT) & MC33772C_AN_OT_UT_FLT_AN5_UT_MASK)

/* Enumerated value NO_UNDERTEMP: No undertemperature fault detected. */
#define MC33772C_AN_OT_UT_FLT_AN5_UT_NO_UNDERTEMP_ENUM_VAL   (0x0U)

/* Enumerated value UNDERTEMP: Undertemperature fault detected. */
#define MC33772C_AN_OT_UT_FLT_AN5_UT_UNDERTEMP_ENUM_VAL      (0x1U)

/* Field AN6_UT (read-only): Undertemperature detection for ANx - ANx_UT ored is provided in FAULT1_STATUS[AN_UT_FLT]. */
#define MC33772C_AN_OT_UT_FLT_AN6_UT_SHIFT                   (0x6U)
#define MC33772C_AN_OT_UT_FLT_AN6_UT_MASK                    (0x40U)
#define MC33772C_AN_OT_UT_FLT_AN6_UT(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_AN_OT_UT_FLT_AN6_UT_SHIFT) & MC33772C_AN_OT_UT_FLT_AN6_UT_MASK)

/* Enumerated value NO_UNDERTEMP: No undertemperature fault detected. */
#define MC33772C_AN_OT_UT_FLT_AN6_UT_NO_UNDERTEMP_ENUM_VAL   (0x0U)

/* Enumerated value UNDERTEMP: Undertemperature fault detected. */
#define MC33772C_AN_OT_UT_FLT_AN6_UT_UNDERTEMP_ENUM_VAL      (0x1U)

/* Field AN0_OT (read-only): Overtemperature detection for ANx - ANx_OT ored is provided in FAULT1_STATUS[AN_OT_FLT]. */
#define MC33772C_AN_OT_UT_FLT_AN0_OT_SHIFT                   (0x8U)
#define MC33772C_AN_OT_UT_FLT_AN0_OT_MASK                    (0x100U)
#define MC33772C_AN_OT_UT_FLT_AN0_OT(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_AN_OT_UT_FLT_AN0_OT_SHIFT) & MC33772C_AN_OT_UT_FLT_AN0_OT_MASK)

/* Enumerated value REPORT_LOW: No high state detected. */
#define MC33772C_AN_OT_UT_FLT_AN0_OT_REPORT_LOW_ENUM_VAL     (0x0U)

/* Enumerated value REPORT_HIGH: A high state has been detected. */
#define MC33772C_AN_OT_UT_FLT_AN0_OT_REPORT_HIGH_ENUM_VAL    (0x1U)

/* Field AN1_OT (read-only): Overtemperature detection for ANx - ANx_OT ored is provided in FAULT1_STATUS[AN_OT_FLT]. */
#define MC33772C_AN_OT_UT_FLT_AN1_OT_SHIFT                   (0x9U)
#define MC33772C_AN_OT_UT_FLT_AN1_OT_MASK                    (0x200U)
#define MC33772C_AN_OT_UT_FLT_AN1_OT(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_AN_OT_UT_FLT_AN1_OT_SHIFT) & MC33772C_AN_OT_UT_FLT_AN1_OT_MASK)

/* Enumerated value REPORT_LOW: No high state detected. */
#define MC33772C_AN_OT_UT_FLT_AN1_OT_REPORT_LOW_ENUM_VAL     (0x0U)

/* Enumerated value REPORT_HIGH: A high state has been detected. */
#define MC33772C_AN_OT_UT_FLT_AN1_OT_REPORT_HIGH_ENUM_VAL    (0x1U)

/* Field AN2_OT (read-only): Overtemperature detection for ANx - ANx_OT ored is provided in FAULT1_STATUS[AN_OT_FLT]. */
#define MC33772C_AN_OT_UT_FLT_AN2_OT_SHIFT                   (0xAU)
#define MC33772C_AN_OT_UT_FLT_AN2_OT_MASK                    (0x400U)
#define MC33772C_AN_OT_UT_FLT_AN2_OT(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_AN_OT_UT_FLT_AN2_OT_SHIFT) & MC33772C_AN_OT_UT_FLT_AN2_OT_MASK)

/* Enumerated value REPORT_LOW: No high state detected. */
#define MC33772C_AN_OT_UT_FLT_AN2_OT_REPORT_LOW_ENUM_VAL     (0x0U)

/* Enumerated value REPORT_HIGH: A high state has been detected. */
#define MC33772C_AN_OT_UT_FLT_AN2_OT_REPORT_HIGH_ENUM_VAL    (0x1U)

/* Field AN3_OT (read-only): Overtemperature detection for ANx - ANx_OT ored is provided in FAULT1_STATUS[AN_OT_FLT]. */
#define MC33772C_AN_OT_UT_FLT_AN3_OT_SHIFT                   (0xBU)
#define MC33772C_AN_OT_UT_FLT_AN3_OT_MASK                    (0x800U)
#define MC33772C_AN_OT_UT_FLT_AN3_OT(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_AN_OT_UT_FLT_AN3_OT_SHIFT) & MC33772C_AN_OT_UT_FLT_AN3_OT_MASK)

/* Enumerated value REPORT_LOW: No high state detected. */
#define MC33772C_AN_OT_UT_FLT_AN3_OT_REPORT_LOW_ENUM_VAL     (0x0U)

/* Enumerated value REPORT_HIGH: A high state has been detected. */
#define MC33772C_AN_OT_UT_FLT_AN3_OT_REPORT_HIGH_ENUM_VAL    (0x1U)

/* Field AN4_OT (read-only): Overtemperature detection for ANx - ANx_OT ored is provided in FAULT1_STATUS[AN_OT_FLT]. */
#define MC33772C_AN_OT_UT_FLT_AN4_OT_SHIFT                   (0xCU)
#define MC33772C_AN_OT_UT_FLT_AN4_OT_MASK                    (0x1000U)
#define MC33772C_AN_OT_UT_FLT_AN4_OT(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_AN_OT_UT_FLT_AN4_OT_SHIFT) & MC33772C_AN_OT_UT_FLT_AN4_OT_MASK)

/* Enumerated value REPORT_LOW: No high state detected. */
#define MC33772C_AN_OT_UT_FLT_AN4_OT_REPORT_LOW_ENUM_VAL     (0x0U)

/* Enumerated value REPORT_HIGH: A high state has been detected. */
#define MC33772C_AN_OT_UT_FLT_AN4_OT_REPORT_HIGH_ENUM_VAL    (0x1U)

/* Field AN5_OT (read-only): Overtemperature detection for ANx - ANx_OT ored is provided in FAULT1_STATUS[AN_OT_FLT]. */
#define MC33772C_AN_OT_UT_FLT_AN5_OT_SHIFT                   (0xDU)
#define MC33772C_AN_OT_UT_FLT_AN5_OT_MASK                    (0x2000U)
#define MC33772C_AN_OT_UT_FLT_AN5_OT(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_AN_OT_UT_FLT_AN5_OT_SHIFT) & MC33772C_AN_OT_UT_FLT_AN5_OT_MASK)

/* Enumerated value REPORT_LOW: No high state detected. */
#define MC33772C_AN_OT_UT_FLT_AN5_OT_REPORT_LOW_ENUM_VAL     (0x0U)

/* Enumerated value REPORT_HIGH: A high state has been detected. */
#define MC33772C_AN_OT_UT_FLT_AN5_OT_REPORT_HIGH_ENUM_VAL    (0x1U)

/* Field AN6_OT (read-only): Overtemperature detection for ANx - ANx_OT ored is provided in FAULT1_STATUS[AN_OT_FLT]. */
#define MC33772C_AN_OT_UT_FLT_AN6_OT_SHIFT                   (0xEU)
#define MC33772C_AN_OT_UT_FLT_AN6_OT_MASK                    (0x4000U)
#define MC33772C_AN_OT_UT_FLT_AN6_OT(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_AN_OT_UT_FLT_AN6_OT_SHIFT) & MC33772C_AN_OT_UT_FLT_AN6_OT_MASK)

/* Enumerated value REPORT_LOW: No high state detected. */
#define MC33772C_AN_OT_UT_FLT_AN6_OT_REPORT_LOW_ENUM_VAL     (0x0U)

/* Enumerated value REPORT_HIGH: A high state has been detected. */
#define MC33772C_AN_OT_UT_FLT_AN6_OT_REPORT_HIGH_ENUM_VAL    (0x1U)

/* --------------------------------------------------------------------------
 * GPIO_SHORT_ANx_OPEN_STS (read-only): GPIO open short register.
 * -------------------------------------------------------------------------- */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_OFFSET              (0x21U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_POR_VAL             (0x0U)

/* Field AN0_OPEN (read-only): Analog inputs open load detection. ANx_OPEN ored is provided in FAULT2_STATUS[AN_OPEN_FLT]. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN0_OPEN_SHIFT      (0x0U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN0_OPEN_MASK       (0x1U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN0_OPEN(x)         ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN0_OPEN_SHIFT) & MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN0_OPEN_MASK)

/* Enumerated value NO_OPEN_LOAD: No open load detected. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN0_OPEN_NO_OPEN_LOAD_ENUM_VAL (0x0U)

/* Enumerated value OPEN_LOAD: Open load detected. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN0_OPEN_OPEN_LOAD_ENUM_VAL (0x1U)

/* Field AN1_OPEN (read-only): Analog inputs open load detection. ANx_OPEN ored is provided in FAULT2_STATUS[AN_OPEN_FLT]. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN1_OPEN_SHIFT      (0x1U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN1_OPEN_MASK       (0x2U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN1_OPEN(x)         ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN1_OPEN_SHIFT) & MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN1_OPEN_MASK)

/* Enumerated value NO_OPEN_LOAD: No open load detected. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN1_OPEN_NO_OPEN_LOAD_ENUM_VAL (0x0U)

/* Enumerated value OPEN_LOAD: Open load detected. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN1_OPEN_OPEN_LOAD_ENUM_VAL (0x1U)

/* Field AN2_OPEN (read-only): Analog inputs open load detection. ANx_OPEN ored is provided in FAULT2_STATUS[AN_OPEN_FLT]. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN2_OPEN_SHIFT      (0x2U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN2_OPEN_MASK       (0x4U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN2_OPEN(x)         ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN2_OPEN_SHIFT) & MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN2_OPEN_MASK)

/* Enumerated value NO_OPEN_LOAD: No open load detected. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN2_OPEN_NO_OPEN_LOAD_ENUM_VAL (0x0U)

/* Enumerated value OPEN_LOAD: Open load detected. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN2_OPEN_OPEN_LOAD_ENUM_VAL (0x1U)

/* Field AN3_OPEN (read-only): Analog inputs open load detection. ANx_OPEN ored is provided in FAULT2_STATUS[AN_OPEN_FLT]. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN3_OPEN_SHIFT      (0x3U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN3_OPEN_MASK       (0x8U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN3_OPEN(x)         ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN3_OPEN_SHIFT) & MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN3_OPEN_MASK)

/* Enumerated value NO_OPEN_LOAD: No open load detected. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN3_OPEN_NO_OPEN_LOAD_ENUM_VAL (0x0U)

/* Enumerated value OPEN_LOAD: Open load detected. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN3_OPEN_OPEN_LOAD_ENUM_VAL (0x1U)

/* Field AN4_OPEN (read-only): Analog inputs open load detection. ANx_OPEN ored is provided in FAULT2_STATUS[AN_OPEN_FLT]. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN4_OPEN_SHIFT      (0x4U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN4_OPEN_MASK       (0x10U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN4_OPEN(x)         ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN4_OPEN_SHIFT) & MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN4_OPEN_MASK)

/* Enumerated value NO_OPEN_LOAD: No open load detected. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN4_OPEN_NO_OPEN_LOAD_ENUM_VAL (0x0U)

/* Enumerated value OPEN_LOAD: Open load detected. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN4_OPEN_OPEN_LOAD_ENUM_VAL (0x1U)

/* Field AN5_OPEN (read-only): Analog inputs open load detection. ANx_OPEN ored is provided in FAULT2_STATUS[AN_OPEN_FLT]. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN5_OPEN_SHIFT      (0x5U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN5_OPEN_MASK       (0x20U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN5_OPEN(x)         ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN5_OPEN_SHIFT) & MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN5_OPEN_MASK)

/* Enumerated value NO_OPEN_LOAD: No open load detected. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN5_OPEN_NO_OPEN_LOAD_ENUM_VAL (0x0U)

/* Enumerated value OPEN_LOAD: Open load detected. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN5_OPEN_OPEN_LOAD_ENUM_VAL (0x1U)

/* Field AN6_OPEN (read-only): Analog inputs open load detection. ANx_OPEN ored is provided in FAULT2_STATUS[AN_OPEN_FLT]. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN6_OPEN_SHIFT      (0x6U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN6_OPEN_MASK       (0x40U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN6_OPEN(x)         ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN6_OPEN_SHIFT) & MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN6_OPEN_MASK)

/* Enumerated value NO_OPEN_LOAD: No open load detected. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN6_OPEN_NO_OPEN_LOAD_ENUM_VAL (0x0U)

/* Enumerated value OPEN_LOAD: Open load detected. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_AN6_OPEN_OPEN_LOAD_ENUM_VAL (0x1U)

/* Field GPIO0_SH (read-only): GPIOx short detection GPIOx_SH ored is provided in FAULT2_STATUS[GPIO_SHORT_FLT]. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO0_SH_SHIFT      (0x8U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO0_SH_MASK       (0x100U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO0_SH(x)         ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO0_SH_SHIFT) & MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO0_SH_MASK)

/* Enumerated value NO_SHORT: No short detected. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO0_SH_NO_SHORT_ENUM_VAL (0x0U)

/* Enumerated value SHORT: Short detected, pad sense is different from pad command. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO0_SH_SHORT_ENUM_VAL (0x1U)

/* Field GPIO1_SH (read-only): GPIOx short detection GPIOx_SH ored is provided in FAULT2_STATUS[GPIO_SHORT_FLT]. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO1_SH_SHIFT      (0x9U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO1_SH_MASK       (0x200U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO1_SH(x)         ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO1_SH_SHIFT) & MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO1_SH_MASK)

/* Enumerated value NO_SHORT: No short detected. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO1_SH_NO_SHORT_ENUM_VAL (0x0U)

/* Enumerated value SHORT: Short detected, pad sense is different from pad command. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO1_SH_SHORT_ENUM_VAL (0x1U)

/* Field GPIO2_SH (read-only): GPIOx short detection GPIOx_SH ored is provided in FAULT2_STATUS[GPIO_SHORT_FLT]. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO2_SH_SHIFT      (0xAU)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO2_SH_MASK       (0x400U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO2_SH(x)         ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO2_SH_SHIFT) & MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO2_SH_MASK)

/* Enumerated value NO_SHORT: No short detected. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO2_SH_NO_SHORT_ENUM_VAL (0x0U)

/* Enumerated value SHORT: Short detected, pad sense is different from pad command. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO2_SH_SHORT_ENUM_VAL (0x1U)

/* Field GPIO3_SH (read-only): GPIOx short detection GPIOx_SH ored is provided in FAULT2_STATUS[GPIO_SHORT_FLT]. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO3_SH_SHIFT      (0xBU)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO3_SH_MASK       (0x800U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO3_SH(x)         ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO3_SH_SHIFT) & MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO3_SH_MASK)

/* Enumerated value NO_SHORT: No short detected. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO3_SH_NO_SHORT_ENUM_VAL (0x0U)

/* Enumerated value SHORT: Short detected, pad sense is different from pad command. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO3_SH_SHORT_ENUM_VAL (0x1U)

/* Field GPIO4_SH (read-only): GPIOx short detection GPIOx_SH ored is provided in FAULT2_STATUS[GPIO_SHORT_FLT]. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO4_SH_SHIFT      (0xCU)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO4_SH_MASK       (0x1000U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO4_SH(x)         ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO4_SH_SHIFT) & MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO4_SH_MASK)

/* Enumerated value NO_SHORT: No short detected. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO4_SH_NO_SHORT_ENUM_VAL (0x0U)

/* Enumerated value SHORT: Short detected, pad sense is different from pad command. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO4_SH_SHORT_ENUM_VAL (0x1U)

/* Field GPIO5_SH (read-only): GPIOx short detection GPIOx_SH ored is provided in FAULT2_STATUS[GPIO_SHORT_FLT]. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO5_SH_SHIFT      (0xDU)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO5_SH_MASK       (0x2000U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO5_SH(x)         ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO5_SH_SHIFT) & MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO5_SH_MASK)

/* Enumerated value NO_SHORT: No short detected. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO5_SH_NO_SHORT_ENUM_VAL (0x0U)

/* Enumerated value SHORT: Short detected, pad sense is different from pad command. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO5_SH_SHORT_ENUM_VAL (0x1U)

/* Field GPIO6_SH (read-only): GPIOx short detection GPIOx_SH ored is provided in FAULT2_STATUS[GPIO_SHORT_FLT]. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO6_SH_SHIFT      (0xEU)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO6_SH_MASK       (0x4000U)
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO6_SH(x)         ((uint16_t)((uint16_t)(x) << MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO6_SH_SHIFT) & MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO6_SH_MASK)

/* Enumerated value NO_SHORT: No short detected. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO6_SH_NO_SHORT_ENUM_VAL (0x0U)

/* Enumerated value SHORT: Short detected, pad sense is different from pad command. */
#define MC33772C_GPIO_SHORT_ANX_OPEN_STS_GPIO6_SH_SHORT_ENUM_VAL (0x1U)

/* --------------------------------------------------------------------------
 * I_STATUS (read-only): Current measurement status register.
 * -------------------------------------------------------------------------- */
#define MC33772C_I_STATUS_OFFSET                             (0x22U)
#define MC33772C_I_STATUS_POR_VAL                            (0x0U)

/* Field PGA_DAC (read-only): DAC code to be provided to the PGA (for offset cancellation), calculated through an autozero phase. */
#define MC33772C_I_STATUS_PGA_DAC_SHIFT                      (0x8U)
#define MC33772C_I_STATUS_PGA_DAC_MASK                       (0xFF00U)
#define MC33772C_I_STATUS_PGA_DAC(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_I_STATUS_PGA_DAC_SHIFT) & MC33772C_I_STATUS_PGA_DAC_MASK)

/* Enumerated value INIT_VALUE: DAC code is initially all zeros. */
#define MC33772C_I_STATUS_PGA_DAC_INIT_VALUE_ENUM_VAL        (0x0U)

/* --------------------------------------------------------------------------
 * COM_STATUS (read-only): Communication status register.
 * -------------------------------------------------------------------------- */
#define MC33772C_COM_STATUS_OFFSET                           (0x23U)
#define MC33772C_COM_STATUS_POR_VAL                          (0x0U)

/* Field COM_ERR_COUNT (read-only): Number of communication errors detected. */
#define MC33772C_COM_STATUS_COM_ERR_COUNT_SHIFT              (0x8U)
#define MC33772C_COM_STATUS_COM_ERR_COUNT_MASK               (0xFF00U)
#define MC33772C_COM_STATUS_COM_ERR_COUNT(x)                 ((uint16_t)((uint16_t)(x) << MC33772C_COM_STATUS_COM_ERR_COUNT_SHIFT) & MC33772C_COM_STATUS_COM_ERR_COUNT_MASK)

/* Enumerated value COMM_ERR_OVF: 255 communication errors have been detected. Overflow of counter sets FAULT1_STATUS[COM_ERR_OVR_FLT]. Count remains at 255 until cleared by controller. */
#define MC33772C_COM_STATUS_COM_ERR_COUNT_COMM_ERR_OVF_ENUM_VAL (0xFFU)

/* --------------------------------------------------------------------------
 * FAULT1_STATUS (read-write): Fault status register 1.
 * -------------------------------------------------------------------------- */
#define MC33772C_FAULT1_STATUS_OFFSET                        (0x24U)
#define MC33772C_FAULT1_STATUS_POR_VAL                       (0x8000U)

/* Field CT_UV_FLT (read-only): Cell terminal undervoltage detection. */
#define MC33772C_FAULT1_STATUS_CT_UV_FLT_SHIFT               (0x0U)
#define MC33772C_FAULT1_STATUS_CT_UV_FLT_MASK                (0x1U)
#define MC33772C_FAULT1_STATUS_CT_UV_FLT(x)                  ((uint16_t)((uint16_t)(x) << MC33772C_FAULT1_STATUS_CT_UV_FLT_SHIFT) & MC33772C_FAULT1_STATUS_CT_UV_FLT_MASK)

/* Enumerated value NO_UNDERVOLTAGE: No undervoltage detected. */
#define MC33772C_FAULT1_STATUS_CT_UV_FLT_NO_UNDERVOLTAGE_ENUM_VAL (0x0U)

/* Enumerated value UNDERVOLTAGE: Undervoltage detection in at least one of the 14 cell terminals. */
#define MC33772C_FAULT1_STATUS_CT_UV_FLT_UNDERVOLTAGE_ENUM_VAL (0x1U)

/* Field CT_OV_FLT (read-only): Cell terminal overvoltage detection. */
#define MC33772C_FAULT1_STATUS_CT_OV_FLT_SHIFT               (0x1U)
#define MC33772C_FAULT1_STATUS_CT_OV_FLT_MASK                (0x2U)
#define MC33772C_FAULT1_STATUS_CT_OV_FLT(x)                  ((uint16_t)((uint16_t)(x) << MC33772C_FAULT1_STATUS_CT_OV_FLT_SHIFT) & MC33772C_FAULT1_STATUS_CT_OV_FLT_MASK)

/* Enumerated value NO_OVERVOLTAGE: No overvoltage detected. */
#define MC33772C_FAULT1_STATUS_CT_OV_FLT_NO_OVERVOLTAGE_ENUM_VAL (0x0U)

/* Enumerated value OVERVOLTAGE: Overvoltage detected in one or more of the 14 cell terminals. */
#define MC33772C_FAULT1_STATUS_CT_OV_FLT_OVERVOLTAGE_ENUM_VAL (0x1U)

/* Field AN_UT_FLT (read-only): Analog inputs undertemperature detection. */
#define MC33772C_FAULT1_STATUS_AN_UT_FLT_SHIFT               (0x2U)
#define MC33772C_FAULT1_STATUS_AN_UT_FLT_MASK                (0x4U)
#define MC33772C_FAULT1_STATUS_AN_UT_FLT(x)                  ((uint16_t)((uint16_t)(x) << MC33772C_FAULT1_STATUS_AN_UT_FLT_SHIFT) & MC33772C_FAULT1_STATUS_AN_UT_FLT_MASK)

/* Enumerated value NO_UNDERTEMP: No undertemperature detected. */
#define MC33772C_FAULT1_STATUS_AN_UT_FLT_NO_UNDERTEMP_ENUM_VAL (0x0U)

/* Enumerated value UNDERTEMP: Undertemperature detected in at least one of the seven analog inputs. */
#define MC33772C_FAULT1_STATUS_AN_UT_FLT_UNDERTEMP_ENUM_VAL  (0x1U)

/* Field AN_OT_FLT (read-only): Analog input overtemperature detection. */
#define MC33772C_FAULT1_STATUS_AN_OT_FLT_SHIFT               (0x3U)
#define MC33772C_FAULT1_STATUS_AN_OT_FLT_MASK                (0x8U)
#define MC33772C_FAULT1_STATUS_AN_OT_FLT(x)                  ((uint16_t)((uint16_t)(x) << MC33772C_FAULT1_STATUS_AN_OT_FLT_SHIFT) & MC33772C_FAULT1_STATUS_AN_OT_FLT_MASK)

/* Enumerated value NO_OVERTEMP: No overtemperature detected. */
#define MC33772C_FAULT1_STATUS_AN_OT_FLT_NO_OVERTEMP_ENUM_VAL (0x0U)

/* Enumerated value OVERTEMP: Overtemperature detected in one or more of the ANx analog inputs. */
#define MC33772C_FAULT1_STATUS_AN_OT_FLT_OVERTEMP_ENUM_VAL   (0x1U)

/* Field IS_OC_FLT: ISENSE overcurrent detected (sleep mode only). */
#define MC33772C_FAULT1_STATUS_IS_OC_FLT_SHIFT               (0x4U)
#define MC33772C_FAULT1_STATUS_IS_OC_FLT_MASK                (0x10U)
#define MC33772C_FAULT1_STATUS_IS_OC_FLT(x)                  ((uint16_t)((uint16_t)(x) << MC33772C_FAULT1_STATUS_IS_OC_FLT_SHIFT) & MC33772C_FAULT1_STATUS_IS_OC_FLT_MASK)

/* Enumerated value NO_OVERCURRENT: No overcurrent detected. */
#define MC33772C_FAULT1_STATUS_IS_OC_FLT_NO_OVERCURRENT_ENUM_VAL (0x0U)

/* Enumerated value OVERCURRENT: Overcurrent detected from ISENSE inputs. */
#define MC33772C_FAULT1_STATUS_IS_OC_FLT_OVERCURRENT_ENUM_VAL (0x1U)

/* Field IS_OL_FLT: ISENSE pins open load detected. */
#define MC33772C_FAULT1_STATUS_IS_OL_FLT_SHIFT               (0x5U)
#define MC33772C_FAULT1_STATUS_IS_OL_FLT_MASK                (0x20U)
#define MC33772C_FAULT1_STATUS_IS_OL_FLT(x)                  ((uint16_t)((uint16_t)(x) << MC33772C_FAULT1_STATUS_IS_OL_FLT_SHIFT) & MC33772C_FAULT1_STATUS_IS_OL_FLT_MASK)

/* Enumerated value NO_OPEN_LOAD: No open load detected. */
#define MC33772C_FAULT1_STATUS_IS_OL_FLT_NO_OPEN_LOAD_ENUM_VAL (0x0U)

/* Enumerated value OPEN_LOAD: Open load detected in one or both ISENSE pins. */
#define MC33772C_FAULT1_STATUS_IS_OL_FLT_OPEN_LOAD_ENUM_VAL  (0x1U)

/* Field I2C_ERR_FLT: I2C communication error during the transfer from EEPROM to the IC. */
#define MC33772C_FAULT1_STATUS_I2C_ERR_FLT_SHIFT             (0x6U)
#define MC33772C_FAULT1_STATUS_I2C_ERR_FLT_MASK              (0x40U)
#define MC33772C_FAULT1_STATUS_I2C_ERR_FLT(x)                ((uint16_t)((uint16_t)(x) << MC33772C_FAULT1_STATUS_I2C_ERR_FLT_SHIFT) & MC33772C_FAULT1_STATUS_I2C_ERR_FLT_MASK)

/* Enumerated value NO_ERROR: No Error. */
#define MC33772C_FAULT1_STATUS_I2C_ERR_FLT_NO_ERROR_ENUM_VAL (0x0U)

/* Enumerated value ERROR: Error detected. */
#define MC33772C_FAULT1_STATUS_I2C_ERR_FLT_ERROR_ENUM_VAL    (0x1U)

/* Field GPIO0_WUP_FLT: GPIO0 wake-up notification */
#define MC33772C_FAULT1_STATUS_GPIO0_WUP_FLT_SHIFT           (0x7U)
#define MC33772C_FAULT1_STATUS_GPIO0_WUP_FLT_MASK            (0x80U)
#define MC33772C_FAULT1_STATUS_GPIO0_WUP_FLT(x)              ((uint16_t)((uint16_t)(x) << MC33772C_FAULT1_STATUS_GPIO0_WUP_FLT_SHIFT) & MC33772C_FAULT1_STATUS_GPIO0_WUP_FLT_MASK)

/* Enumerated value NO_WAKEUP: No wake-up. */
#define MC33772C_FAULT1_STATUS_GPIO0_WUP_FLT_NO_WAKEUP_ENUM_VAL (0x0U)

/* Enumerated value WAKEUP: GPIO0 wake-up detected. */
#define MC33772C_FAULT1_STATUS_GPIO0_WUP_FLT_WAKEUP_ENUM_VAL (0x1U)

/* Field CSB_WUP_FLT: CSB wake-up notification. */
#define MC33772C_FAULT1_STATUS_CSB_WUP_FLT_SHIFT             (0x8U)
#define MC33772C_FAULT1_STATUS_CSB_WUP_FLT_MASK              (0x100U)
#define MC33772C_FAULT1_STATUS_CSB_WUP_FLT(x)                ((uint16_t)((uint16_t)(x) << MC33772C_FAULT1_STATUS_CSB_WUP_FLT_SHIFT) & MC33772C_FAULT1_STATUS_CSB_WUP_FLT_MASK)

/* Enumerated value NO_WAKEUP: No wake-up. */
#define MC33772C_FAULT1_STATUS_CSB_WUP_FLT_NO_WAKEUP_ENUM_VAL (0x0U)

/* Enumerated value WAKEUP: CSB wake-up detected. */
#define MC33772C_FAULT1_STATUS_CSB_WUP_FLT_WAKEUP_ENUM_VAL   (0x1U)

/* Field COM_ERR_FLT: Communication error detected. */
#define MC33772C_FAULT1_STATUS_COM_ERR_FLT_SHIFT             (0x9U)
#define MC33772C_FAULT1_STATUS_COM_ERR_FLT_MASK              (0x200U)
#define MC33772C_FAULT1_STATUS_COM_ERR_FLT(x)                ((uint16_t)((uint16_t)(x) << MC33772C_FAULT1_STATUS_COM_ERR_FLT_SHIFT) & MC33772C_FAULT1_STATUS_COM_ERR_FLT_MASK)

/* Enumerated value NO_ERROR: No error. */
#define MC33772C_FAULT1_STATUS_COM_ERR_FLT_NO_ERROR_ENUM_VAL (0x0U)

/* Enumerated value ERROR: An error has been detected during a communication. */
#define MC33772C_FAULT1_STATUS_COM_ERR_FLT_ERROR_ENUM_VAL    (0x1U)

/* Field COM_LOSS_FLT: In normal mode, each slave device must receive a local message within the programmed period or COM_LOSS_FLT flag is set. */
#define MC33772C_FAULT1_STATUS_COM_LOSS_FLT_SHIFT            (0xAU)
#define MC33772C_FAULT1_STATUS_COM_LOSS_FLT_MASK             (0x400U)
#define MC33772C_FAULT1_STATUS_COM_LOSS_FLT(x)               ((uint16_t)((uint16_t)(x) << MC33772C_FAULT1_STATUS_COM_LOSS_FLT_SHIFT) & MC33772C_FAULT1_STATUS_COM_LOSS_FLT_MASK)

/* Enumerated value NO_ERROR: No error. */
#define MC33772C_FAULT1_STATUS_COM_LOSS_FLT_NO_ERROR_ENUM_VAL (0x0U)

/* Enumerated value ERROR: Communication loss detected after a reset due to a communication loss. */
#define MC33772C_FAULT1_STATUS_COM_LOSS_FLT_ERROR_ENUM_VAL   (0x1U)

/* Field VPWR_LV_FLT: VPWR low-voltage notification. */
#define MC33772C_FAULT1_STATUS_VPWR_LV_FLT_SHIFT             (0xBU)
#define MC33772C_FAULT1_STATUS_VPWR_LV_FLT_MASK              (0x800U)
#define MC33772C_FAULT1_STATUS_VPWR_LV_FLT(x)                ((uint16_t)((uint16_t)(x) << MC33772C_FAULT1_STATUS_VPWR_LV_FLT_SHIFT) & MC33772C_FAULT1_STATUS_VPWR_LV_FLT_MASK)

/* Enumerated value NO_LV_DETECTED: No low-voltage (VPWR > VPWR(LV_FLAG)) detected. */
#define MC33772C_FAULT1_STATUS_VPWR_LV_FLT_NO_LV_DETECTED_ENUM_VAL (0x0U)

/* Enumerated value LV_DETECTED: Low-voltage detected (VPWR < VPWR(LV_FLAG), timing filtered). */
#define MC33772C_FAULT1_STATUS_VPWR_LV_FLT_LV_DETECTED_ENUM_VAL (0x1U)

/* Field VPWR_OV_FLT: VPWR overvoltage notification. */
#define MC33772C_FAULT1_STATUS_VPWR_OV_FLT_SHIFT             (0xCU)
#define MC33772C_FAULT1_STATUS_VPWR_OV_FLT_MASK              (0x1000U)
#define MC33772C_FAULT1_STATUS_VPWR_OV_FLT(x)                ((uint16_t)((uint16_t)(x) << MC33772C_FAULT1_STATUS_VPWR_OV_FLT_SHIFT) & MC33772C_FAULT1_STATUS_VPWR_OV_FLT_MASK)

/* Enumerated value NO_OVERVOLTAGE: No overvoltage (VPWR < VPWR(OV_FLAG)) detected. */
#define MC33772C_FAULT1_STATUS_VPWR_OV_FLT_NO_OVERVOLTAGE_ENUM_VAL (0x0U)

/* Enumerated value OVERVOLTAGE: Overvoltage detected (VPWR > VPWR(OV_FLAG), timing filtered). */
#define MC33772C_FAULT1_STATUS_VPWR_OV_FLT_OVERVOLTAGE_ENUM_VAL (0x1U)

/* Field COM_ERR_OVR_FLT: Overflow indicator on the COM_STATUS[COM_ERR_COUNT]. */
#define MC33772C_FAULT1_STATUS_COM_ERR_OVR_FLT_SHIFT         (0xDU)
#define MC33772C_FAULT1_STATUS_COM_ERR_OVR_FLT_MASK          (0x2000U)
#define MC33772C_FAULT1_STATUS_COM_ERR_OVR_FLT(x)            ((uint16_t)((uint16_t)(x) << MC33772C_FAULT1_STATUS_COM_ERR_OVR_FLT_SHIFT) & MC33772C_FAULT1_STATUS_COM_ERR_OVR_FLT_MASK)

/* Enumerated value NO_ERROR: No error. */
#define MC33772C_FAULT1_STATUS_COM_ERR_OVR_FLT_NO_ERROR_ENUM_VAL (0x0U)

/* Enumerated value ERROR: COM_STATUS[COM_ERR_COUNT] went in overflow. */
#define MC33772C_FAULT1_STATUS_COM_ERR_OVR_FLT_ERROR_ENUM_VAL (0x1U)

/* Field RESET_FLT: RESET Indication (nonmaskable). */
#define MC33772C_FAULT1_STATUS_RESET_FLT_SHIFT               (0xEU)
#define MC33772C_FAULT1_STATUS_RESET_FLT_MASK                (0x4000U)
#define MC33772C_FAULT1_STATUS_RESET_FLT(x)                  ((uint16_t)((uint16_t)(x) << MC33772C_FAULT1_STATUS_RESET_FLT_SHIFT) & MC33772C_FAULT1_STATUS_RESET_FLT_MASK)

/* Enumerated value NO_RESET: No reset. */
#define MC33772C_FAULT1_STATUS_RESET_FLT_NO_RESET_ENUM_VAL   (0x0U)

/* Enumerated value RESET: Device has been reset through the RESET pin or by a write command setting the SYS_CFG1[SOFT_RST] or by a communication loss or an oscillator monitoring fault. */
#define MC33772C_FAULT1_STATUS_RESET_FLT_RESET_ENUM_VAL      (0x1U)

/* Field POR: Power on reset indication (POR). Clear on write 0. */
#define MC33772C_FAULT1_STATUS_POR_SHIFT                     (0xFU)
#define MC33772C_FAULT1_STATUS_POR_MASK                      (0x8000U)
#define MC33772C_FAULT1_STATUS_POR(x)                        ((uint16_t)((uint16_t)(x) << MC33772C_FAULT1_STATUS_POR_SHIFT) & MC33772C_FAULT1_STATUS_POR_MASK)

/* Enumerated value NO_POR: No POR. */
#define MC33772C_FAULT1_STATUS_POR_NO_POR_ENUM_VAL           (0x0U)

/* Enumerated value POR: Power-on reset. */
#define MC33772C_FAULT1_STATUS_POR_POR_ENUM_VAL              (0x1U)

/* --------------------------------------------------------------------------
 * FAULT2_STATUS (read-write): Fault status register 2.
 * -------------------------------------------------------------------------- */
#define MC33772C_FAULT2_STATUS_OFFSET                        (0x25U)
#define MC33772C_FAULT2_STATUS_POR_VAL                       (0x0U)

/* Field FUSE_ERR_FLT: Error in the loading of fuses. */
#define MC33772C_FAULT2_STATUS_FUSE_ERR_FLT_SHIFT            (0x0U)
#define MC33772C_FAULT2_STATUS_FUSE_ERR_FLT_MASK             (0x1U)
#define MC33772C_FAULT2_STATUS_FUSE_ERR_FLT(x)               ((uint16_t)((uint16_t)(x) << MC33772C_FAULT2_STATUS_FUSE_ERR_FLT_SHIFT) & MC33772C_FAULT2_STATUS_FUSE_ERR_FLT_MASK)

/* Enumerated value NO_ERROR: No error. */
#define MC33772C_FAULT2_STATUS_FUSE_ERR_FLT_NO_ERROR_ENUM_VAL (0x0U)

/* Enumerated value ERROR: The lock bit was not set after loading, meaning transfer of the fuse values is aborted. */
#define MC33772C_FAULT2_STATUS_FUSE_ERR_FLT_ERROR_ENUM_VAL   (0x1U)

/* Field DED_ERR_FLT: ECC error, double error detection. */
#define MC33772C_FAULT2_STATUS_DED_ERR_FLT_SHIFT             (0x1U)
#define MC33772C_FAULT2_STATUS_DED_ERR_FLT_MASK              (0x2U)
#define MC33772C_FAULT2_STATUS_DED_ERR_FLT(x)                ((uint16_t)((uint16_t)(x) << MC33772C_FAULT2_STATUS_DED_ERR_FLT_SHIFT) & MC33772C_FAULT2_STATUS_DED_ERR_FLT_MASK)

/* Enumerated value NO_ERROR: No error. */
#define MC33772C_FAULT2_STATUS_DED_ERR_FLT_NO_ERROR_ENUM_VAL (0x0U)

/* Enumerated value ERROR: A double error has been detected (and only one corrected) in the fuses. */
#define MC33772C_FAULT2_STATUS_DED_ERR_FLT_ERROR_ENUM_VAL    (0x1U)

/* Field OSC_ERR_FLT: Low-power oscillator error. */
#define MC33772C_FAULT2_STATUS_OSC_ERR_FLT_SHIFT             (0x2U)
#define MC33772C_FAULT2_STATUS_OSC_ERR_FLT_MASK              (0x4U)
#define MC33772C_FAULT2_STATUS_OSC_ERR_FLT(x)                ((uint16_t)((uint16_t)(x) << MC33772C_FAULT2_STATUS_OSC_ERR_FLT_SHIFT) & MC33772C_FAULT2_STATUS_OSC_ERR_FLT_MASK)

/* Enumerated value NO_ERROR: No error. */
#define MC33772C_FAULT2_STATUS_OSC_ERR_FLT_NO_ERROR_ENUM_VAL (0x0U)

/* Enumerated value ERROR: The low-power oscillator frequency is out of range after a reset due to an oscillator monitoring fault. */
#define MC33772C_FAULT2_STATUS_OSC_ERR_FLT_ERROR_ENUM_VAL    (0x1U)

/* Field CB_OPEN_FLT: Cell balancing open load detection. */
#define MC33772C_FAULT2_STATUS_CB_OPEN_FLT_SHIFT             (0x3U)
#define MC33772C_FAULT2_STATUS_CB_OPEN_FLT_MASK              (0x8U)
#define MC33772C_FAULT2_STATUS_CB_OPEN_FLT(x)                ((uint16_t)((uint16_t)(x) << MC33772C_FAULT2_STATUS_CB_OPEN_FLT_SHIFT) & MC33772C_FAULT2_STATUS_CB_OPEN_FLT_MASK)

/* Enumerated value NO_ERROR: No cell balance open load detected. */
#define MC33772C_FAULT2_STATUS_CB_OPEN_FLT_NO_ERROR_ENUM_VAL (0x0U)

/* Enumerated value ERROR: Off state open load detected in one or more of the 14 cell balancing switches. */
#define MC33772C_FAULT2_STATUS_CB_OPEN_FLT_ERROR_ENUM_VAL    (0x1U)

/* Field CB_SHORT_FLT: Cell balance short-circuit detection. */
#define MC33772C_FAULT2_STATUS_CB_SHORT_FLT_SHIFT            (0x4U)
#define MC33772C_FAULT2_STATUS_CB_SHORT_FLT_MASK             (0x10U)
#define MC33772C_FAULT2_STATUS_CB_SHORT_FLT(x)               ((uint16_t)((uint16_t)(x) << MC33772C_FAULT2_STATUS_CB_SHORT_FLT_SHIFT) & MC33772C_FAULT2_STATUS_CB_SHORT_FLT_MASK)

/* Enumerated value NO_ERROR: No short-circuit detected. */
#define MC33772C_FAULT2_STATUS_CB_SHORT_FLT_NO_ERROR_ENUM_VAL (0x0U)

/* Enumerated value ERROR: On state short-circuit detected in one or more of the 14 cell balancing switches. */
#define MC33772C_FAULT2_STATUS_CB_SHORT_FLT_ERROR_ENUM_VAL   (0x1U)

/* Field GPIO_SHORT_FLT: GPIO short detection. */
#define MC33772C_FAULT2_STATUS_GPIO_SHORT_FLT_SHIFT          (0x5U)
#define MC33772C_FAULT2_STATUS_GPIO_SHORT_FLT_MASK           (0x20U)
#define MC33772C_FAULT2_STATUS_GPIO_SHORT_FLT(x)             ((uint16_t)((uint16_t)(x) << MC33772C_FAULT2_STATUS_GPIO_SHORT_FLT_SHIFT) & MC33772C_FAULT2_STATUS_GPIO_SHORT_FLT_MASK)

/* Enumerated value NO_ERROR: No short detected. */
#define MC33772C_FAULT2_STATUS_GPIO_SHORT_FLT_NO_ERROR_ENUM_VAL (0x0U)

/* Enumerated value ERROR: Short detected in one or more of the seven GPIOs, pad sense is different from pad command. */
#define MC33772C_FAULT2_STATUS_GPIO_SHORT_FLT_ERROR_ENUM_VAL (0x1U)

/* Field AN_OPEN_FLT: Analog inputs open load detection. */
#define MC33772C_FAULT2_STATUS_AN_OPEN_FLT_SHIFT             (0x6U)
#define MC33772C_FAULT2_STATUS_AN_OPEN_FLT_MASK              (0x40U)
#define MC33772C_FAULT2_STATUS_AN_OPEN_FLT(x)                ((uint16_t)((uint16_t)(x) << MC33772C_FAULT2_STATUS_AN_OPEN_FLT_SHIFT) & MC33772C_FAULT2_STATUS_AN_OPEN_FLT_MASK)

/* Enumerated value NO_ERROR: No open load detected. */
#define MC33772C_FAULT2_STATUS_AN_OPEN_FLT_NO_ERROR_ENUM_VAL (0x0U)

/* Enumerated value ERROR: Open load detected in one of the seven analog inputs. */
#define MC33772C_FAULT2_STATUS_AN_OPEN_FLT_ERROR_ENUM_VAL    (0x1U)

/* Field IDLE_MODE_FLT: IDLE mode notification. */
#define MC33772C_FAULT2_STATUS_IDLE_MODE_FLT_SHIFT           (0x7U)
#define MC33772C_FAULT2_STATUS_IDLE_MODE_FLT_MASK            (0x80U)
#define MC33772C_FAULT2_STATUS_IDLE_MODE_FLT(x)              ((uint16_t)((uint16_t)(x) << MC33772C_FAULT2_STATUS_IDLE_MODE_FLT_SHIFT) & MC33772C_FAULT2_STATUS_IDLE_MODE_FLT_MASK)

/* Enumerated value NO_NOTIFICATION: No notification. */
#define MC33772C_FAULT2_STATUS_IDLE_MODE_FLT_NO_NOTIFICATION_ENUM_VAL (0x0U)

/* Enumerated value NOTIFICATION: The system has transitioned through idle mode. */
#define MC33772C_FAULT2_STATUS_IDLE_MODE_FLT_NOTIFICATION_ENUM_VAL (0x1U)

/* Field IC_TSD_FLT: IC thermal limitation notification. */
#define MC33772C_FAULT2_STATUS_IC_TSD_FLT_SHIFT              (0x8U)
#define MC33772C_FAULT2_STATUS_IC_TSD_FLT_MASK               (0x100U)
#define MC33772C_FAULT2_STATUS_IC_TSD_FLT(x)                 ((uint16_t)((uint16_t)(x) << MC33772C_FAULT2_STATUS_IC_TSD_FLT_SHIFT) & MC33772C_FAULT2_STATUS_IC_TSD_FLT_MASK)

/* Enumerated value NO_ERROR: No thermal limitation detected. */
#define MC33772C_FAULT2_STATUS_IC_TSD_FLT_NO_ERROR_ENUM_VAL  (0x0U)

/* Enumerated value ERROR: Thermal limitation detected. */
#define MC33772C_FAULT2_STATUS_IC_TSD_FLT_ERROR_ENUM_VAL     (0x1U)

/* Field GND_LOSS_FLT: Loss of ground has been detected on DGND or AGND. */
#define MC33772C_FAULT2_STATUS_GND_LOSS_FLT_SHIFT            (0x9U)
#define MC33772C_FAULT2_STATUS_GND_LOSS_FLT_MASK             (0x200U)
#define MC33772C_FAULT2_STATUS_GND_LOSS_FLT(x)               ((uint16_t)((uint16_t)(x) << MC33772C_FAULT2_STATUS_GND_LOSS_FLT_SHIFT) & MC33772C_FAULT2_STATUS_GND_LOSS_FLT_MASK)

/* Enumerated value NO_ERROR: No error. */
#define MC33772C_FAULT2_STATUS_GND_LOSS_FLT_NO_ERROR_ENUM_VAL (0x0U)

/* Enumerated value ERROR: Loss of ground detected. */
#define MC33772C_FAULT2_STATUS_GND_LOSS_FLT_ERROR_ENUM_VAL   (0x1U)

/* Field ADC1_A_FLT: ADC1_A fault notification. */
#define MC33772C_FAULT2_STATUS_ADC1_A_FLT_SHIFT              (0xAU)
#define MC33772C_FAULT2_STATUS_ADC1_A_FLT_MASK               (0x400U)
#define MC33772C_FAULT2_STATUS_ADC1_A_FLT(x)                 ((uint16_t)((uint16_t)(x) << MC33772C_FAULT2_STATUS_ADC1_A_FLT_SHIFT) & MC33772C_FAULT2_STATUS_ADC1_A_FLT_MASK)

/* Enumerated value NO_ERROR: No fault detected. */
#define MC33772C_FAULT2_STATUS_ADC1_A_FLT_NO_ERROR_ENUM_VAL  (0x0U)

/* Enumerated value ERROR: ADC1_A fault (over or undervoltage has been detected on MEAS_VBG_DIAG_ADC1A). */
#define MC33772C_FAULT2_STATUS_ADC1_A_FLT_ERROR_ENUM_VAL     (0x1U)

/* Field ADC1_B_FLT: ADC1_B fault notification. */
#define MC33772C_FAULT2_STATUS_ADC1_B_FLT_SHIFT              (0xBU)
#define MC33772C_FAULT2_STATUS_ADC1_B_FLT_MASK               (0x800U)
#define MC33772C_FAULT2_STATUS_ADC1_B_FLT(x)                 ((uint16_t)((uint16_t)(x) << MC33772C_FAULT2_STATUS_ADC1_B_FLT_SHIFT) & MC33772C_FAULT2_STATUS_ADC1_B_FLT_MASK)

/* Enumerated value NO_ERROR: No fault detected. */
#define MC33772C_FAULT2_STATUS_ADC1_B_FLT_NO_ERROR_ENUM_VAL  (0x0U)

/* Enumerated value ERROR: ADC1_B fault (over or undervoltage has been detected on MEAS_VBG_DIAG_ADC1B). */
#define MC33772C_FAULT2_STATUS_ADC1_B_FLT_ERROR_ENUM_VAL     (0x1U)

/* Field VANA_UV_FLT: VANA undervoltage notification. */
#define MC33772C_FAULT2_STATUS_VANA_UV_FLT_SHIFT             (0xCU)
#define MC33772C_FAULT2_STATUS_VANA_UV_FLT_MASK              (0x1000U)
#define MC33772C_FAULT2_STATUS_VANA_UV_FLT(x)                ((uint16_t)((uint16_t)(x) << MC33772C_FAULT2_STATUS_VANA_UV_FLT_SHIFT) & MC33772C_FAULT2_STATUS_VANA_UV_FLT_MASK)

/* Enumerated value NO_ERROR: No undervoltage detected. */
#define MC33772C_FAULT2_STATUS_VANA_UV_FLT_NO_ERROR_ENUM_VAL (0x0U)

/* Enumerated value ERROR: Undervoltage has been detected on the VANA supply. */
#define MC33772C_FAULT2_STATUS_VANA_UV_FLT_ERROR_ENUM_VAL    (0x1U)

/* Field VANA_OV_FLT: VANA overvoltage notification. */
#define MC33772C_FAULT2_STATUS_VANA_OV_FLT_SHIFT             (0xDU)
#define MC33772C_FAULT2_STATUS_VANA_OV_FLT_MASK              (0x2000U)
#define MC33772C_FAULT2_STATUS_VANA_OV_FLT(x)                ((uint16_t)((uint16_t)(x) << MC33772C_FAULT2_STATUS_VANA_OV_FLT_SHIFT) & MC33772C_FAULT2_STATUS_VANA_OV_FLT_MASK)

/* Enumerated value NO_ERROR: No overvoltage detected. */
#define MC33772C_FAULT2_STATUS_VANA_OV_FLT_NO_ERROR_ENUM_VAL (0x0U)

/* Enumerated value ERROR: Overvoltage has been detected on the VANA supply. */
#define MC33772C_FAULT2_STATUS_VANA_OV_FLT_ERROR_ENUM_VAL    (0x1U)

/* Field VCOM_UV_FLT: VCOM undervoltage notification. */
#define MC33772C_FAULT2_STATUS_VCOM_UV_FLT_SHIFT             (0xEU)
#define MC33772C_FAULT2_STATUS_VCOM_UV_FLT_MASK              (0x4000U)
#define MC33772C_FAULT2_STATUS_VCOM_UV_FLT(x)                ((uint16_t)((uint16_t)(x) << MC33772C_FAULT2_STATUS_VCOM_UV_FLT_SHIFT) & MC33772C_FAULT2_STATUS_VCOM_UV_FLT_MASK)

/* Enumerated value NO_ERROR: No undervoltage detected. */
#define MC33772C_FAULT2_STATUS_VCOM_UV_FLT_NO_ERROR_ENUM_VAL (0x0U)

/* Enumerated value ERROR: Undervoltage has been detected on VCOM supply. */
#define MC33772C_FAULT2_STATUS_VCOM_UV_FLT_ERROR_ENUM_VAL    (0x1U)

/* Field VCOM_OV_FLT: VCOM overvoltage notification. */
#define MC33772C_FAULT2_STATUS_VCOM_OV_FLT_SHIFT             (0xFU)
#define MC33772C_FAULT2_STATUS_VCOM_OV_FLT_MASK              (0x8000U)
#define MC33772C_FAULT2_STATUS_VCOM_OV_FLT(x)                ((uint16_t)((uint16_t)(x) << MC33772C_FAULT2_STATUS_VCOM_OV_FLT_SHIFT) & MC33772C_FAULT2_STATUS_VCOM_OV_FLT_MASK)

/* Enumerated value NO_ERROR: No overvoltage detected. */
#define MC33772C_FAULT2_STATUS_VCOM_OV_FLT_NO_ERROR_ENUM_VAL (0x0U)

/* Enumerated value ERROR: Overvoltage has been detected on VCOM supply. */
#define MC33772C_FAULT2_STATUS_VCOM_OV_FLT_ERROR_ENUM_VAL    (0x1U)

/* --------------------------------------------------------------------------
 * FAULT3_STATUS (read-write): Fault status register 3.
 * -------------------------------------------------------------------------- */
#define MC33772C_FAULT3_STATUS_OFFSET                        (0x26U)
#define MC33772C_FAULT3_STATUS_POR_VAL                       (0x0U)

/* Field EOT_CB1: End of time cell balancing notification - indicates when a cell balance timer has expired and driver has been shutoff. */
#define MC33772C_FAULT3_STATUS_EOT_CB1_SHIFT                 (0x0U)
#define MC33772C_FAULT3_STATUS_EOT_CB1_MASK                  (0x1U)
#define MC33772C_FAULT3_STATUS_EOT_CB1(x)                    ((uint16_t)((uint16_t)(x) << MC33772C_FAULT3_STATUS_EOT_CB1_SHIFT) & MC33772C_FAULT3_STATUS_EOT_CB1_MASK)

/* Enumerated value NO_TIMEOUT: Cell balance timer has not timed out. */
#define MC33772C_FAULT3_STATUS_EOT_CB1_NO_TIMEOUT_ENUM_VAL   (0x0U)

/* Enumerated value TIMEOUT: Cell balance timer has timed out. */
#define MC33772C_FAULT3_STATUS_EOT_CB1_TIMEOUT_ENUM_VAL      (0x1U)

/* Field EOT_CB2: End of time cell balancing notification - indicates when a cell balance timer has expired and driver has been shutoff. */
#define MC33772C_FAULT3_STATUS_EOT_CB2_SHIFT                 (0x1U)
#define MC33772C_FAULT3_STATUS_EOT_CB2_MASK                  (0x2U)
#define MC33772C_FAULT3_STATUS_EOT_CB2(x)                    ((uint16_t)((uint16_t)(x) << MC33772C_FAULT3_STATUS_EOT_CB2_SHIFT) & MC33772C_FAULT3_STATUS_EOT_CB2_MASK)

/* Enumerated value NO_TIMEOUT: Cell balance timer has not timed out. */
#define MC33772C_FAULT3_STATUS_EOT_CB2_NO_TIMEOUT_ENUM_VAL   (0x0U)

/* Enumerated value TIMEOUT: Cell balance timer has timed out. */
#define MC33772C_FAULT3_STATUS_EOT_CB2_TIMEOUT_ENUM_VAL      (0x1U)

/* Field EOT_CB3: End of time cell balancing notification - indicates when a cell balance timer has expired and driver has been shutoff. */
#define MC33772C_FAULT3_STATUS_EOT_CB3_SHIFT                 (0x2U)
#define MC33772C_FAULT3_STATUS_EOT_CB3_MASK                  (0x4U)
#define MC33772C_FAULT3_STATUS_EOT_CB3(x)                    ((uint16_t)((uint16_t)(x) << MC33772C_FAULT3_STATUS_EOT_CB3_SHIFT) & MC33772C_FAULT3_STATUS_EOT_CB3_MASK)

/* Enumerated value NO_TIMEOUT: Cell balance timer has not timed out. */
#define MC33772C_FAULT3_STATUS_EOT_CB3_NO_TIMEOUT_ENUM_VAL   (0x0U)

/* Enumerated value TIMEOUT: Cell balance timer has timed out. */
#define MC33772C_FAULT3_STATUS_EOT_CB3_TIMEOUT_ENUM_VAL      (0x1U)

/* Field EOT_CB4: End of time cell balancing notification - indicates when a cell balance timer has expired and driver has been shutoff. */
#define MC33772C_FAULT3_STATUS_EOT_CB4_SHIFT                 (0x3U)
#define MC33772C_FAULT3_STATUS_EOT_CB4_MASK                  (0x8U)
#define MC33772C_FAULT3_STATUS_EOT_CB4(x)                    ((uint16_t)((uint16_t)(x) << MC33772C_FAULT3_STATUS_EOT_CB4_SHIFT) & MC33772C_FAULT3_STATUS_EOT_CB4_MASK)

/* Enumerated value NO_TIMEOUT: Cell balance timer has not timed out. */
#define MC33772C_FAULT3_STATUS_EOT_CB4_NO_TIMEOUT_ENUM_VAL   (0x0U)

/* Enumerated value TIMEOUT: Cell balance timer has timed out. */
#define MC33772C_FAULT3_STATUS_EOT_CB4_TIMEOUT_ENUM_VAL      (0x1U)

/* Field EOT_CB5: End of time cell balancing notification - indicates when a cell balance timer has expired and driver has been shutoff. */
#define MC33772C_FAULT3_STATUS_EOT_CB5_SHIFT                 (0x4U)
#define MC33772C_FAULT3_STATUS_EOT_CB5_MASK                  (0x10U)
#define MC33772C_FAULT3_STATUS_EOT_CB5(x)                    ((uint16_t)((uint16_t)(x) << MC33772C_FAULT3_STATUS_EOT_CB5_SHIFT) & MC33772C_FAULT3_STATUS_EOT_CB5_MASK)

/* Enumerated value NO_TIMEOUT: Cell balance timer has not timed out. */
#define MC33772C_FAULT3_STATUS_EOT_CB5_NO_TIMEOUT_ENUM_VAL   (0x0U)

/* Enumerated value TIMEOUT: Cell balance timer has timed out. */
#define MC33772C_FAULT3_STATUS_EOT_CB5_TIMEOUT_ENUM_VAL      (0x1U)

/* Field EOT_CB6: End of time cell balancing notification - indicates when a cell balance timer has expired and driver has been shutoff. */
#define MC33772C_FAULT3_STATUS_EOT_CB6_SHIFT                 (0x5U)
#define MC33772C_FAULT3_STATUS_EOT_CB6_MASK                  (0x20U)
#define MC33772C_FAULT3_STATUS_EOT_CB6(x)                    ((uint16_t)((uint16_t)(x) << MC33772C_FAULT3_STATUS_EOT_CB6_SHIFT) & MC33772C_FAULT3_STATUS_EOT_CB6_MASK)

/* Enumerated value NO_TIMEOUT: Cell balance timer has not timed out. */
#define MC33772C_FAULT3_STATUS_EOT_CB6_NO_TIMEOUT_ENUM_VAL   (0x0U)

/* Enumerated value TIMEOUT: Cell balance timer has timed out. */
#define MC33772C_FAULT3_STATUS_EOT_CB6_TIMEOUT_ENUM_VAL      (0x1U)

/* Field VCP_UV: VCP undervoltage detection. */
#define MC33772C_FAULT3_STATUS_VCP_UV_SHIFT                  (0xDU)
#define MC33772C_FAULT3_STATUS_VCP_UV_MASK                   (0x2000U)
#define MC33772C_FAULT3_STATUS_VCP_UV(x)                     ((uint16_t)((uint16_t)(x) << MC33772C_FAULT3_STATUS_VCP_UV_SHIFT) & MC33772C_FAULT3_STATUS_VCP_UV_MASK)

/* Enumerated value NO_UNDERVOLTAGE: No undervoltage detected. */
#define MC33772C_FAULT3_STATUS_VCP_UV_NO_UNDERVOLTAGE_ENUM_VAL (0x0U)

/* Enumerated value UNDERVOLTAGE: Undervoltage detected on VCP supply. */
#define MC33772C_FAULT3_STATUS_VCP_UV_UNDERVOLTAGE_ENUM_VAL  (0x1U)

/* Field DIAG_TO_FLT: Timeout of diagnostic state. */
#define MC33772C_FAULT3_STATUS_DIAG_TO_FLT_SHIFT             (0xEU)
#define MC33772C_FAULT3_STATUS_DIAG_TO_FLT_MASK              (0x4000U)
#define MC33772C_FAULT3_STATUS_DIAG_TO_FLT(x)                ((uint16_t)((uint16_t)(x) << MC33772C_FAULT3_STATUS_DIAG_TO_FLT_SHIFT) & MC33772C_FAULT3_STATUS_DIAG_TO_FLT_MASK)

/* Enumerated value NO_TIMEOUT: No timeout. */
#define MC33772C_FAULT3_STATUS_DIAG_TO_FLT_NO_TIMEOUT_ENUM_VAL (0x0U)

/* Enumerated value TIMEOUT: The system has exited itself from diagnostic state after timeout. */
#define MC33772C_FAULT3_STATUS_DIAG_TO_FLT_TIMEOUT_ENUM_VAL  (0x1U)

/* Field CC_OVR_FLT: Overflow indicator on the COULOMB_CNT1,2[COULOMB_CNT] or CC_NB_SAMPLES. */
#define MC33772C_FAULT3_STATUS_CC_OVR_FLT_SHIFT              (0xFU)
#define MC33772C_FAULT3_STATUS_CC_OVR_FLT_MASK               (0x8000U)
#define MC33772C_FAULT3_STATUS_CC_OVR_FLT(x)                 ((uint16_t)((uint16_t)(x) << MC33772C_FAULT3_STATUS_CC_OVR_FLT_SHIFT) & MC33772C_FAULT3_STATUS_CC_OVR_FLT_MASK)

/* Enumerated value NO_ERROR: No error. */
#define MC33772C_FAULT3_STATUS_CC_OVR_FLT_NO_ERROR_ENUM_VAL  (0x0U)

/* Enumerated value ERROR: COULOMB_CNT1,2[COULOMB_CNT] or CC_NB_SAMPLES went in overflow. */
#define MC33772C_FAULT3_STATUS_CC_OVR_FLT_ERROR_ENUM_VAL     (0x1U)

/* --------------------------------------------------------------------------
 * FAULT_MASK1 (read-write): Fault mask register 1
 * -------------------------------------------------------------------------- */
#define MC33772C_FAULT_MASK1_OFFSET                          (0x27U)
#define MC33772C_FAULT_MASK1_POR_VAL                         (0x0U)

/* Field CT_UV_FLT_MASK_0_F: Prevent the corresponding flags in FAULT1_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK1_CT_UV_FLT_MASK_0_F_SHIFT        (0x0U)
#define MC33772C_FAULT_MASK1_CT_UV_FLT_MASK_0_F_MASK         (0x1U)
#define MC33772C_FAULT_MASK1_CT_UV_FLT_MASK_0_F(x)           ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK1_CT_UV_FLT_MASK_0_F_SHIFT) & MC33772C_FAULT_MASK1_CT_UV_FLT_MASK_0_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK1_CT_UV_FLT_MASK_0_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK1_CT_UV_FLT_MASK_0_F_MASKED_ENUM_VAL (0x1U)

/* Field CT_OV_FLT_MASK_1_F: Prevent the corresponding flags in FAULT1_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK1_CT_OV_FLT_MASK_1_F_SHIFT        (0x1U)
#define MC33772C_FAULT_MASK1_CT_OV_FLT_MASK_1_F_MASK         (0x2U)
#define MC33772C_FAULT_MASK1_CT_OV_FLT_MASK_1_F(x)           ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK1_CT_OV_FLT_MASK_1_F_SHIFT) & MC33772C_FAULT_MASK1_CT_OV_FLT_MASK_1_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK1_CT_OV_FLT_MASK_1_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK1_CT_OV_FLT_MASK_1_F_MASKED_ENUM_VAL (0x1U)

/* Field AN_UT_FLT_MASK_2_F: Prevent the corresponding flags in FAULT1_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK1_AN_UT_FLT_MASK_2_F_SHIFT        (0x2U)
#define MC33772C_FAULT_MASK1_AN_UT_FLT_MASK_2_F_MASK         (0x4U)
#define MC33772C_FAULT_MASK1_AN_UT_FLT_MASK_2_F(x)           ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK1_AN_UT_FLT_MASK_2_F_SHIFT) & MC33772C_FAULT_MASK1_AN_UT_FLT_MASK_2_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK1_AN_UT_FLT_MASK_2_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK1_AN_UT_FLT_MASK_2_F_MASKED_ENUM_VAL (0x1U)

/* Field AN_OT_FLT_MASK_3_F: Prevent the corresponding flags in FAULT1_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK1_AN_OT_FLT_MASK_3_F_SHIFT        (0x3U)
#define MC33772C_FAULT_MASK1_AN_OT_FLT_MASK_3_F_MASK         (0x8U)
#define MC33772C_FAULT_MASK1_AN_OT_FLT_MASK_3_F(x)           ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK1_AN_OT_FLT_MASK_3_F_SHIFT) & MC33772C_FAULT_MASK1_AN_OT_FLT_MASK_3_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK1_AN_OT_FLT_MASK_3_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK1_AN_OT_FLT_MASK_3_F_MASKED_ENUM_VAL (0x1U)

/* Field IS_OC_FLT_MASK_4_F: Prevent the corresponding flags in FAULT1_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK1_IS_OC_FLT_MASK_4_F_SHIFT        (0x4U)
#define MC33772C_FAULT_MASK1_IS_OC_FLT_MASK_4_F_MASK         (0x10U)
#define MC33772C_FAULT_MASK1_IS_OC_FLT_MASK_4_F(x)           ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK1_IS_OC_FLT_MASK_4_F_SHIFT) & MC33772C_FAULT_MASK1_IS_OC_FLT_MASK_4_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK1_IS_OC_FLT_MASK_4_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK1_IS_OC_FLT_MASK_4_F_MASKED_ENUM_VAL (0x1U)

/* Field IS_OL_FLT_MASK_5_F: Prevent the corresponding flags in FAULT1_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK1_IS_OL_FLT_MASK_5_F_SHIFT        (0x5U)
#define MC33772C_FAULT_MASK1_IS_OL_FLT_MASK_5_F_MASK         (0x20U)
#define MC33772C_FAULT_MASK1_IS_OL_FLT_MASK_5_F(x)           ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK1_IS_OL_FLT_MASK_5_F_SHIFT) & MC33772C_FAULT_MASK1_IS_OL_FLT_MASK_5_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK1_IS_OL_FLT_MASK_5_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK1_IS_OL_FLT_MASK_5_F_MASKED_ENUM_VAL (0x1U)

/* Field I2C_ERR_FLT_MASK_6_F: Prevent the corresponding flags in FAULT1_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK1_I2C_ERR_FLT_MASK_6_F_SHIFT      (0x6U)
#define MC33772C_FAULT_MASK1_I2C_ERR_FLT_MASK_6_F_MASK       (0x40U)
#define MC33772C_FAULT_MASK1_I2C_ERR_FLT_MASK_6_F(x)         ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK1_I2C_ERR_FLT_MASK_6_F_SHIFT) & MC33772C_FAULT_MASK1_I2C_ERR_FLT_MASK_6_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK1_I2C_ERR_FLT_MASK_6_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK1_I2C_ERR_FLT_MASK_6_F_MASKED_ENUM_VAL (0x1U)

/* Field GPIO0_WUP_FLT_MASK_7_F: Prevent the corresponding flags in FAULT1_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK1_GPIO0_WUP_FLT_MASK_7_F_SHIFT    (0x7U)
#define MC33772C_FAULT_MASK1_GPIO0_WUP_FLT_MASK_7_F_MASK     (0x80U)
#define MC33772C_FAULT_MASK1_GPIO0_WUP_FLT_MASK_7_F(x)       ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK1_GPIO0_WUP_FLT_MASK_7_F_SHIFT) & MC33772C_FAULT_MASK1_GPIO0_WUP_FLT_MASK_7_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK1_GPIO0_WUP_FLT_MASK_7_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK1_GPIO0_WUP_FLT_MASK_7_F_MASKED_ENUM_VAL (0x1U)

/* Field CSB_WUP_FLT_MASK_8_F: Prevent the corresponding flags in FAULT1_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK1_CSB_WUP_FLT_MASK_8_F_SHIFT      (0x8U)
#define MC33772C_FAULT_MASK1_CSB_WUP_FLT_MASK_8_F_MASK       (0x100U)
#define MC33772C_FAULT_MASK1_CSB_WUP_FLT_MASK_8_F(x)         ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK1_CSB_WUP_FLT_MASK_8_F_SHIFT) & MC33772C_FAULT_MASK1_CSB_WUP_FLT_MASK_8_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK1_CSB_WUP_FLT_MASK_8_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK1_CSB_WUP_FLT_MASK_8_F_MASKED_ENUM_VAL (0x1U)

/* Field COM_ERR_FLT_MASK_9_F: Prevent the corresponding flags in FAULT1_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK1_COM_ERR_FLT_MASK_9_F_SHIFT      (0x9U)
#define MC33772C_FAULT_MASK1_COM_ERR_FLT_MASK_9_F_MASK       (0x200U)
#define MC33772C_FAULT_MASK1_COM_ERR_FLT_MASK_9_F(x)         ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK1_COM_ERR_FLT_MASK_9_F_SHIFT) & MC33772C_FAULT_MASK1_COM_ERR_FLT_MASK_9_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK1_COM_ERR_FLT_MASK_9_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK1_COM_ERR_FLT_MASK_9_F_MASKED_ENUM_VAL (0x1U)

/* Field COM_LOSS_FLT_MASK_10_F: Prevent the corresponding flags in FAULT1_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK1_COM_LOSS_FLT_MASK_10_F_SHIFT    (0xAU)
#define MC33772C_FAULT_MASK1_COM_LOSS_FLT_MASK_10_F_MASK     (0x400U)
#define MC33772C_FAULT_MASK1_COM_LOSS_FLT_MASK_10_F(x)       ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK1_COM_LOSS_FLT_MASK_10_F_SHIFT) & MC33772C_FAULT_MASK1_COM_LOSS_FLT_MASK_10_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK1_COM_LOSS_FLT_MASK_10_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK1_COM_LOSS_FLT_MASK_10_F_MASKED_ENUM_VAL (0x1U)

/* Field VPWR_LV_FLT_MASK_11_F: Prevent the corresponding flags in FAULT1_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK1_VPWR_LV_FLT_MASK_11_F_SHIFT     (0xBU)
#define MC33772C_FAULT_MASK1_VPWR_LV_FLT_MASK_11_F_MASK      (0x800U)
#define MC33772C_FAULT_MASK1_VPWR_LV_FLT_MASK_11_F(x)        ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK1_VPWR_LV_FLT_MASK_11_F_SHIFT) & MC33772C_FAULT_MASK1_VPWR_LV_FLT_MASK_11_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK1_VPWR_LV_FLT_MASK_11_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK1_VPWR_LV_FLT_MASK_11_F_MASKED_ENUM_VAL (0x1U)

/* Field VPWR_OV_FLT_MASK_12_F: Prevent the corresponding flags in FAULT1_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK1_VPWR_OV_FLT_MASK_12_F_SHIFT     (0xCU)
#define MC33772C_FAULT_MASK1_VPWR_OV_FLT_MASK_12_F_MASK      (0x1000U)
#define MC33772C_FAULT_MASK1_VPWR_OV_FLT_MASK_12_F(x)        ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK1_VPWR_OV_FLT_MASK_12_F_SHIFT) & MC33772C_FAULT_MASK1_VPWR_OV_FLT_MASK_12_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK1_VPWR_OV_FLT_MASK_12_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK1_VPWR_OV_FLT_MASK_12_F_MASKED_ENUM_VAL (0x1U)

/* --------------------------------------------------------------------------
 * FAULT_MASK2 (read-write): Fault mask register 2.
 * -------------------------------------------------------------------------- */
#define MC33772C_FAULT_MASK2_OFFSET                          (0x28U)
#define MC33772C_FAULT_MASK2_POR_VAL                         (0x0U)

/* Field FUSE_ERR_FLT_MASK_0_F: Prevent the corresponding flags in FAULT2_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK2_FUSE_ERR_FLT_MASK_0_F_SHIFT     (0x0U)
#define MC33772C_FAULT_MASK2_FUSE_ERR_FLT_MASK_0_F_MASK      (0x1U)
#define MC33772C_FAULT_MASK2_FUSE_ERR_FLT_MASK_0_F(x)        ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK2_FUSE_ERR_FLT_MASK_0_F_SHIFT) & MC33772C_FAULT_MASK2_FUSE_ERR_FLT_MASK_0_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK2_FUSE_ERR_FLT_MASK_0_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK2_FUSE_ERR_FLT_MASK_0_F_MASKED_ENUM_VAL (0x1U)

/* Field DED_ERR_FLT_MASK_1_F: Prevent the corresponding flags in FAULT2_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK2_DED_ERR_FLT_MASK_1_F_SHIFT      (0x1U)
#define MC33772C_FAULT_MASK2_DED_ERR_FLT_MASK_1_F_MASK       (0x2U)
#define MC33772C_FAULT_MASK2_DED_ERR_FLT_MASK_1_F(x)         ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK2_DED_ERR_FLT_MASK_1_F_SHIFT) & MC33772C_FAULT_MASK2_DED_ERR_FLT_MASK_1_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK2_DED_ERR_FLT_MASK_1_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK2_DED_ERR_FLT_MASK_1_F_MASKED_ENUM_VAL (0x1U)

/* Field OSC_ERR_FLT_MASK_2_F: Prevent the corresponding flags in FAULT2_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK2_OSC_ERR_FLT_MASK_2_F_SHIFT      (0x2U)
#define MC33772C_FAULT_MASK2_OSC_ERR_FLT_MASK_2_F_MASK       (0x4U)
#define MC33772C_FAULT_MASK2_OSC_ERR_FLT_MASK_2_F(x)         ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK2_OSC_ERR_FLT_MASK_2_F_SHIFT) & MC33772C_FAULT_MASK2_OSC_ERR_FLT_MASK_2_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK2_OSC_ERR_FLT_MASK_2_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK2_OSC_ERR_FLT_MASK_2_F_MASKED_ENUM_VAL (0x1U)

/* Field CB_OPEN_FLT_MASK_3_F: Prevent the corresponding flags in FAULT2_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK2_CB_OPEN_FLT_MASK_3_F_SHIFT      (0x3U)
#define MC33772C_FAULT_MASK2_CB_OPEN_FLT_MASK_3_F_MASK       (0x8U)
#define MC33772C_FAULT_MASK2_CB_OPEN_FLT_MASK_3_F(x)         ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK2_CB_OPEN_FLT_MASK_3_F_SHIFT) & MC33772C_FAULT_MASK2_CB_OPEN_FLT_MASK_3_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK2_CB_OPEN_FLT_MASK_3_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK2_CB_OPEN_FLT_MASK_3_F_MASKED_ENUM_VAL (0x1U)

/* Field CB_SHORT_FLT_MASK_4_F: Prevent the corresponding flags in FAULT2_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK2_CB_SHORT_FLT_MASK_4_F_SHIFT     (0x4U)
#define MC33772C_FAULT_MASK2_CB_SHORT_FLT_MASK_4_F_MASK      (0x10U)
#define MC33772C_FAULT_MASK2_CB_SHORT_FLT_MASK_4_F(x)        ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK2_CB_SHORT_FLT_MASK_4_F_SHIFT) & MC33772C_FAULT_MASK2_CB_SHORT_FLT_MASK_4_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK2_CB_SHORT_FLT_MASK_4_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK2_CB_SHORT_FLT_MASK_4_F_MASKED_ENUM_VAL (0x1U)

/* Field GPIO_SHORT_FLT_MASK_5_F: Prevent the corresponding flags in FAULT2_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK2_GPIO_SHORT_FLT_MASK_5_F_SHIFT   (0x5U)
#define MC33772C_FAULT_MASK2_GPIO_SHORT_FLT_MASK_5_F_MASK    (0x20U)
#define MC33772C_FAULT_MASK2_GPIO_SHORT_FLT_MASK_5_F(x)      ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK2_GPIO_SHORT_FLT_MASK_5_F_SHIFT) & MC33772C_FAULT_MASK2_GPIO_SHORT_FLT_MASK_5_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK2_GPIO_SHORT_FLT_MASK_5_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK2_GPIO_SHORT_FLT_MASK_5_F_MASKED_ENUM_VAL (0x1U)

/* Field AN_OPEN_FLT_MASK_6_F: Prevent the corresponding flags in FAULT2_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK2_AN_OPEN_FLT_MASK_6_F_SHIFT      (0x6U)
#define MC33772C_FAULT_MASK2_AN_OPEN_FLT_MASK_6_F_MASK       (0x40U)
#define MC33772C_FAULT_MASK2_AN_OPEN_FLT_MASK_6_F(x)         ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK2_AN_OPEN_FLT_MASK_6_F_SHIFT) & MC33772C_FAULT_MASK2_AN_OPEN_FLT_MASK_6_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK2_AN_OPEN_FLT_MASK_6_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK2_AN_OPEN_FLT_MASK_6_F_MASKED_ENUM_VAL (0x1U)

/* Field GND_LOSS_FLT_MASK_9_F: Prevent the corresponding flags in FAULT2_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK2_GND_LOSS_FLT_MASK_9_F_SHIFT     (0x9U)
#define MC33772C_FAULT_MASK2_GND_LOSS_FLT_MASK_9_F_MASK      (0x200U)
#define MC33772C_FAULT_MASK2_GND_LOSS_FLT_MASK_9_F(x)        ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK2_GND_LOSS_FLT_MASK_9_F_SHIFT) & MC33772C_FAULT_MASK2_GND_LOSS_FLT_MASK_9_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK2_GND_LOSS_FLT_MASK_9_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK2_GND_LOSS_FLT_MASK_9_F_MASKED_ENUM_VAL (0x1U)

/* Field ADC1_A_FLT_MASK_10_F: Prevent the corresponding flags in FAULT2_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK2_ADC1_A_FLT_MASK_10_F_SHIFT      (0xAU)
#define MC33772C_FAULT_MASK2_ADC1_A_FLT_MASK_10_F_MASK       (0x400U)
#define MC33772C_FAULT_MASK2_ADC1_A_FLT_MASK_10_F(x)         ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK2_ADC1_A_FLT_MASK_10_F_SHIFT) & MC33772C_FAULT_MASK2_ADC1_A_FLT_MASK_10_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK2_ADC1_A_FLT_MASK_10_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK2_ADC1_A_FLT_MASK_10_F_MASKED_ENUM_VAL (0x1U)

/* Field ADC1_B_FLT_MASK_11_F: Prevent the corresponding flags in FAULT2_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK2_ADC1_B_FLT_MASK_11_F_SHIFT      (0xBU)
#define MC33772C_FAULT_MASK2_ADC1_B_FLT_MASK_11_F_MASK       (0x800U)
#define MC33772C_FAULT_MASK2_ADC1_B_FLT_MASK_11_F(x)         ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK2_ADC1_B_FLT_MASK_11_F_SHIFT) & MC33772C_FAULT_MASK2_ADC1_B_FLT_MASK_11_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK2_ADC1_B_FLT_MASK_11_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK2_ADC1_B_FLT_MASK_11_F_MASKED_ENUM_VAL (0x1U)

/* Field VANA_UV_FLT_MASK_12_F: Prevent the corresponding flags in FAULT2_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK2_VANA_UV_FLT_MASK_12_F_SHIFT     (0xCU)
#define MC33772C_FAULT_MASK2_VANA_UV_FLT_MASK_12_F_MASK      (0x1000U)
#define MC33772C_FAULT_MASK2_VANA_UV_FLT_MASK_12_F(x)        ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK2_VANA_UV_FLT_MASK_12_F_SHIFT) & MC33772C_FAULT_MASK2_VANA_UV_FLT_MASK_12_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK2_VANA_UV_FLT_MASK_12_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK2_VANA_UV_FLT_MASK_12_F_MASKED_ENUM_VAL (0x1U)

/* Field VANA_OV_FLT_MASK_13_F: Prevent the corresponding flags in FAULT2_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK2_VANA_OV_FLT_MASK_13_F_SHIFT     (0xDU)
#define MC33772C_FAULT_MASK2_VANA_OV_FLT_MASK_13_F_MASK      (0x2000U)
#define MC33772C_FAULT_MASK2_VANA_OV_FLT_MASK_13_F(x)        ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK2_VANA_OV_FLT_MASK_13_F_SHIFT) & MC33772C_FAULT_MASK2_VANA_OV_FLT_MASK_13_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK2_VANA_OV_FLT_MASK_13_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK2_VANA_OV_FLT_MASK_13_F_MASKED_ENUM_VAL (0x1U)

/* Field VCOM_UV_FLT_MASK_14_F: Prevent the corresponding flags in FAULT2_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK2_VCOM_UV_FLT_MASK_14_F_SHIFT     (0xEU)
#define MC33772C_FAULT_MASK2_VCOM_UV_FLT_MASK_14_F_MASK      (0x4000U)
#define MC33772C_FAULT_MASK2_VCOM_UV_FLT_MASK_14_F(x)        ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK2_VCOM_UV_FLT_MASK_14_F_SHIFT) & MC33772C_FAULT_MASK2_VCOM_UV_FLT_MASK_14_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK2_VCOM_UV_FLT_MASK_14_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK2_VCOM_UV_FLT_MASK_14_F_MASKED_ENUM_VAL (0x1U)

/* Field VCOM_OV_FLT_MASK_15_F: Prevent the corresponding flags in FAULT2_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK2_VCOM_OV_FLT_MASK_15_F_SHIFT     (0xFU)
#define MC33772C_FAULT_MASK2_VCOM_OV_FLT_MASK_15_F_MASK      (0x8000U)
#define MC33772C_FAULT_MASK2_VCOM_OV_FLT_MASK_15_F(x)        ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK2_VCOM_OV_FLT_MASK_15_F_SHIFT) & MC33772C_FAULT_MASK2_VCOM_OV_FLT_MASK_15_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK2_VCOM_OV_FLT_MASK_15_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK2_VCOM_OV_FLT_MASK_15_F_MASKED_ENUM_VAL (0x1U)

/* --------------------------------------------------------------------------
 * FAULT_MASK3 (read-write): Fault mask register 3.
 * -------------------------------------------------------------------------- */
#define MC33772C_FAULT_MASK3_OFFSET                          (0x29U)
#define MC33772C_FAULT_MASK3_POR_VAL                         (0x0U)

/* Field EOT_CB1_MASK_0_F: Prevent the corresponding flags in FAULT3_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK3_EOT_CB1_MASK_0_F_SHIFT          (0x0U)
#define MC33772C_FAULT_MASK3_EOT_CB1_MASK_0_F_MASK           (0x1U)
#define MC33772C_FAULT_MASK3_EOT_CB1_MASK_0_F(x)             ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK3_EOT_CB1_MASK_0_F_SHIFT) & MC33772C_FAULT_MASK3_EOT_CB1_MASK_0_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK3_EOT_CB1_MASK_0_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK3_EOT_CB1_MASK_0_F_MASKED_ENUM_VAL (0x1U)

/* Field EOT_CB2_MASK_1_F: Prevent the corresponding flags in FAULT3_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK3_EOT_CB2_MASK_1_F_SHIFT          (0x1U)
#define MC33772C_FAULT_MASK3_EOT_CB2_MASK_1_F_MASK           (0x2U)
#define MC33772C_FAULT_MASK3_EOT_CB2_MASK_1_F(x)             ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK3_EOT_CB2_MASK_1_F_SHIFT) & MC33772C_FAULT_MASK3_EOT_CB2_MASK_1_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK3_EOT_CB2_MASK_1_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK3_EOT_CB2_MASK_1_F_MASKED_ENUM_VAL (0x1U)

/* Field EOT_CB3_MASK_2_F: Prevent the corresponding flags in FAULT3_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK3_EOT_CB3_MASK_2_F_SHIFT          (0x2U)
#define MC33772C_FAULT_MASK3_EOT_CB3_MASK_2_F_MASK           (0x4U)
#define MC33772C_FAULT_MASK3_EOT_CB3_MASK_2_F(x)             ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK3_EOT_CB3_MASK_2_F_SHIFT) & MC33772C_FAULT_MASK3_EOT_CB3_MASK_2_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK3_EOT_CB3_MASK_2_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK3_EOT_CB3_MASK_2_F_MASKED_ENUM_VAL (0x1U)

/* Field EOT_CB4_MASK_3_F: Prevent the corresponding flags in FAULT3_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK3_EOT_CB4_MASK_3_F_SHIFT          (0x3U)
#define MC33772C_FAULT_MASK3_EOT_CB4_MASK_3_F_MASK           (0x8U)
#define MC33772C_FAULT_MASK3_EOT_CB4_MASK_3_F(x)             ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK3_EOT_CB4_MASK_3_F_SHIFT) & MC33772C_FAULT_MASK3_EOT_CB4_MASK_3_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK3_EOT_CB4_MASK_3_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK3_EOT_CB4_MASK_3_F_MASKED_ENUM_VAL (0x1U)

/* Field EOT_CB5_MASK_4_F: Prevent the corresponding flags in FAULT3_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK3_EOT_CB5_MASK_4_F_SHIFT          (0x4U)
#define MC33772C_FAULT_MASK3_EOT_CB5_MASK_4_F_MASK           (0x10U)
#define MC33772C_FAULT_MASK3_EOT_CB5_MASK_4_F(x)             ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK3_EOT_CB5_MASK_4_F_SHIFT) & MC33772C_FAULT_MASK3_EOT_CB5_MASK_4_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK3_EOT_CB5_MASK_4_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK3_EOT_CB5_MASK_4_F_MASKED_ENUM_VAL (0x1U)

/* Field EOT_CB6_MASK_5_F: Prevent the corresponding flags in FAULT3_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK3_EOT_CB6_MASK_5_F_SHIFT          (0x5U)
#define MC33772C_FAULT_MASK3_EOT_CB6_MASK_5_F_MASK           (0x20U)
#define MC33772C_FAULT_MASK3_EOT_CB6_MASK_5_F(x)             ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK3_EOT_CB6_MASK_5_F_SHIFT) & MC33772C_FAULT_MASK3_EOT_CB6_MASK_5_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK3_EOT_CB6_MASK_5_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK3_EOT_CB6_MASK_5_F_MASKED_ENUM_VAL (0x1U)

/* Field VCP_UV_MASK_13_F: Prevent the corresponding flags in FAULT3_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK3_VCP_UV_MASK_13_F_SHIFT          (0xDU)
#define MC33772C_FAULT_MASK3_VCP_UV_MASK_13_F_MASK           (0x2000U)
#define MC33772C_FAULT_MASK3_VCP_UV_MASK_13_F(x)             ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK3_VCP_UV_MASK_13_F_SHIFT) & MC33772C_FAULT_MASK3_VCP_UV_MASK_13_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK3_VCP_UV_MASK_13_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK3_VCP_UV_MASK_13_F_MASKED_ENUM_VAL (0x1U)

/* Field DIAG_TO_FLT_MASK_14_F: Prevent the corresponding flags in FAULT3_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK3_DIAG_TO_FLT_MASK_14_F_SHIFT     (0xEU)
#define MC33772C_FAULT_MASK3_DIAG_TO_FLT_MASK_14_F_MASK      (0x4000U)
#define MC33772C_FAULT_MASK3_DIAG_TO_FLT_MASK_14_F(x)        ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK3_DIAG_TO_FLT_MASK_14_F_SHIFT) & MC33772C_FAULT_MASK3_DIAG_TO_FLT_MASK_14_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK3_DIAG_TO_FLT_MASK_14_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK3_DIAG_TO_FLT_MASK_14_F_MASKED_ENUM_VAL (0x1U)

/* Field CC_OVR_FLT_MASK_15_F: Prevent the corresponding flags in FAULT3_STATUS to activate the FAULT pin. */
#define MC33772C_FAULT_MASK3_CC_OVR_FLT_MASK_15_F_SHIFT      (0xFU)
#define MC33772C_FAULT_MASK3_CC_OVR_FLT_MASK_15_F_MASK       (0x8000U)
#define MC33772C_FAULT_MASK3_CC_OVR_FLT_MASK_15_F(x)         ((uint16_t)((uint16_t)(x) << MC33772C_FAULT_MASK3_CC_OVR_FLT_MASK_15_F_SHIFT) & MC33772C_FAULT_MASK3_CC_OVR_FLT_MASK_15_F_MASK)

/* Enumerated value NOT_MASKED: The flag activates the FAULT pin. */
#define MC33772C_FAULT_MASK3_CC_OVR_FLT_MASK_15_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No fault pin activation. */
#define MC33772C_FAULT_MASK3_CC_OVR_FLT_MASK_15_F_MASKED_ENUM_VAL (0x1U)

/* --------------------------------------------------------------------------
 * WAKEUP_MASK1 (read-write): Wake-up mask register 1.
 * -------------------------------------------------------------------------- */
#define MC33772C_WAKEUP_MASK1_OFFSET                         (0x2AU)
#define MC33772C_WAKEUP_MASK1_POR_VAL                        (0x189FU)

/* Field CT_UV_FLT_MASK_0_F: Prevent the corresponding flags in FAULT1_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK1_CT_UV_FLT_MASK_0_F_SHIFT       (0x0U)
#define MC33772C_WAKEUP_MASK1_CT_UV_FLT_MASK_0_F_MASK        (0x1U)
#define MC33772C_WAKEUP_MASK1_CT_UV_FLT_MASK_0_F(x)          ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK1_CT_UV_FLT_MASK_0_F_SHIFT) & MC33772C_WAKEUP_MASK1_CT_UV_FLT_MASK_0_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK1_CT_UV_FLT_MASK_0_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK1_CT_UV_FLT_MASK_0_F_MASKED_ENUM_VAL (0x1U)

/* Field CT_OV_FLT_MASK_1_F: Prevent the corresponding flags in FAULT1_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK1_CT_OV_FLT_MASK_1_F_SHIFT       (0x1U)
#define MC33772C_WAKEUP_MASK1_CT_OV_FLT_MASK_1_F_MASK        (0x2U)
#define MC33772C_WAKEUP_MASK1_CT_OV_FLT_MASK_1_F(x)          ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK1_CT_OV_FLT_MASK_1_F_SHIFT) & MC33772C_WAKEUP_MASK1_CT_OV_FLT_MASK_1_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK1_CT_OV_FLT_MASK_1_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK1_CT_OV_FLT_MASK_1_F_MASKED_ENUM_VAL (0x1U)

/* Field AN_UT_FLT_MASK_2_F: Prevent the corresponding flags in FAULT1_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK1_AN_UT_FLT_MASK_2_F_SHIFT       (0x2U)
#define MC33772C_WAKEUP_MASK1_AN_UT_FLT_MASK_2_F_MASK        (0x4U)
#define MC33772C_WAKEUP_MASK1_AN_UT_FLT_MASK_2_F(x)          ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK1_AN_UT_FLT_MASK_2_F_SHIFT) & MC33772C_WAKEUP_MASK1_AN_UT_FLT_MASK_2_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK1_AN_UT_FLT_MASK_2_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK1_AN_UT_FLT_MASK_2_F_MASKED_ENUM_VAL (0x1U)

/* Field AN_OT_FLT_MASK_3_F: Prevent the corresponding flags in FAULT1_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK1_AN_OT_FLT_MASK_3_F_SHIFT       (0x3U)
#define MC33772C_WAKEUP_MASK1_AN_OT_FLT_MASK_3_F_MASK        (0x8U)
#define MC33772C_WAKEUP_MASK1_AN_OT_FLT_MASK_3_F(x)          ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK1_AN_OT_FLT_MASK_3_F_SHIFT) & MC33772C_WAKEUP_MASK1_AN_OT_FLT_MASK_3_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK1_AN_OT_FLT_MASK_3_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK1_AN_OT_FLT_MASK_3_F_MASKED_ENUM_VAL (0x1U)

/* Field IS_OC_FLT_MASK_4_F: Prevent the corresponding flags in FAULT1_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK1_IS_OC_FLT_MASK_4_F_SHIFT       (0x4U)
#define MC33772C_WAKEUP_MASK1_IS_OC_FLT_MASK_4_F_MASK        (0x10U)
#define MC33772C_WAKEUP_MASK1_IS_OC_FLT_MASK_4_F(x)          ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK1_IS_OC_FLT_MASK_4_F_SHIFT) & MC33772C_WAKEUP_MASK1_IS_OC_FLT_MASK_4_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK1_IS_OC_FLT_MASK_4_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK1_IS_OC_FLT_MASK_4_F_MASKED_ENUM_VAL (0x1U)

/* Field GPIO0_WUP_FLT_MASK_7_F: Prevent the corresponding flags in FAULT1_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK1_GPIO0_WUP_FLT_MASK_7_F_SHIFT   (0x7U)
#define MC33772C_WAKEUP_MASK1_GPIO0_WUP_FLT_MASK_7_F_MASK    (0x80U)
#define MC33772C_WAKEUP_MASK1_GPIO0_WUP_FLT_MASK_7_F(x)      ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK1_GPIO0_WUP_FLT_MASK_7_F_SHIFT) & MC33772C_WAKEUP_MASK1_GPIO0_WUP_FLT_MASK_7_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK1_GPIO0_WUP_FLT_MASK_7_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK1_GPIO0_WUP_FLT_MASK_7_F_MASKED_ENUM_VAL (0x1U)

/* Field VPWR_LV_FLT_MASK_11_F: Prevent the corresponding flags in FAULT1_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK1_VPWR_LV_FLT_MASK_11_F_SHIFT    (0xBU)
#define MC33772C_WAKEUP_MASK1_VPWR_LV_FLT_MASK_11_F_MASK     (0x800U)
#define MC33772C_WAKEUP_MASK1_VPWR_LV_FLT_MASK_11_F(x)       ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK1_VPWR_LV_FLT_MASK_11_F_SHIFT) & MC33772C_WAKEUP_MASK1_VPWR_LV_FLT_MASK_11_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK1_VPWR_LV_FLT_MASK_11_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK1_VPWR_LV_FLT_MASK_11_F_MASKED_ENUM_VAL (0x1U)

/* Field VPWR_OV_FLT_MASK_12_F: Prevent the corresponding flags in FAULT1_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK1_VPWR_OV_FLT_MASK_12_F_SHIFT    (0xCU)
#define MC33772C_WAKEUP_MASK1_VPWR_OV_FLT_MASK_12_F_MASK     (0x1000U)
#define MC33772C_WAKEUP_MASK1_VPWR_OV_FLT_MASK_12_F(x)       ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK1_VPWR_OV_FLT_MASK_12_F_SHIFT) & MC33772C_WAKEUP_MASK1_VPWR_OV_FLT_MASK_12_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK1_VPWR_OV_FLT_MASK_12_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK1_VPWR_OV_FLT_MASK_12_F_MASKED_ENUM_VAL (0x1U)

/* --------------------------------------------------------------------------
 * WAKEUP_MASK2 (read-write): Wake-up mask register 2.
 * -------------------------------------------------------------------------- */
#define MC33772C_WAKEUP_MASK2_OFFSET                         (0x2BU)
#define MC33772C_WAKEUP_MASK2_POR_VAL                        (0xFF36U)

/* Field DED_ERR_FLT_MASK_1_F: Prevent the corresponding flags in FAULT2_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK2_DED_ERR_FLT_MASK_1_F_SHIFT     (0x1U)
#define MC33772C_WAKEUP_MASK2_DED_ERR_FLT_MASK_1_F_MASK      (0x2U)
#define MC33772C_WAKEUP_MASK2_DED_ERR_FLT_MASK_1_F(x)        ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK2_DED_ERR_FLT_MASK_1_F_SHIFT) & MC33772C_WAKEUP_MASK2_DED_ERR_FLT_MASK_1_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK2_DED_ERR_FLT_MASK_1_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK2_DED_ERR_FLT_MASK_1_F_MASKED_ENUM_VAL (0x1U)

/* Field OSC_ERR_FLT_MASK_2_F: Prevent the corresponding flags in FAULT2_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK2_OSC_ERR_FLT_MASK_2_F_SHIFT     (0x2U)
#define MC33772C_WAKEUP_MASK2_OSC_ERR_FLT_MASK_2_F_MASK      (0x4U)
#define MC33772C_WAKEUP_MASK2_OSC_ERR_FLT_MASK_2_F(x)        ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK2_OSC_ERR_FLT_MASK_2_F_SHIFT) & MC33772C_WAKEUP_MASK2_OSC_ERR_FLT_MASK_2_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK2_OSC_ERR_FLT_MASK_2_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK2_OSC_ERR_FLT_MASK_2_F_MASKED_ENUM_VAL (0x1U)

/* Field CB_SHORT_FLT_MASK_4_F: Prevent the corresponding flags in FAULT2_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK2_CB_SHORT_FLT_MASK_4_F_SHIFT    (0x4U)
#define MC33772C_WAKEUP_MASK2_CB_SHORT_FLT_MASK_4_F_MASK     (0x10U)
#define MC33772C_WAKEUP_MASK2_CB_SHORT_FLT_MASK_4_F(x)       ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK2_CB_SHORT_FLT_MASK_4_F_SHIFT) & MC33772C_WAKEUP_MASK2_CB_SHORT_FLT_MASK_4_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK2_CB_SHORT_FLT_MASK_4_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK2_CB_SHORT_FLT_MASK_4_F_MASKED_ENUM_VAL (0x1U)

/* Field GPIO_SHORT_FLT_MASK_5_F: Prevent the corresponding flags in FAULT2_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK2_GPIO_SHORT_FLT_MASK_5_F_SHIFT  (0x5U)
#define MC33772C_WAKEUP_MASK2_GPIO_SHORT_FLT_MASK_5_F_MASK   (0x20U)
#define MC33772C_WAKEUP_MASK2_GPIO_SHORT_FLT_MASK_5_F(x)     ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK2_GPIO_SHORT_FLT_MASK_5_F_SHIFT) & MC33772C_WAKEUP_MASK2_GPIO_SHORT_FLT_MASK_5_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK2_GPIO_SHORT_FLT_MASK_5_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK2_GPIO_SHORT_FLT_MASK_5_F_MASKED_ENUM_VAL (0x1U)

/* Field IC_TSD_FLT_MASK_8_F: Prevent the corresponding flags in FAULT2_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK2_IC_TSD_FLT_MASK_8_F_SHIFT      (0x8U)
#define MC33772C_WAKEUP_MASK2_IC_TSD_FLT_MASK_8_F_MASK       (0x100U)
#define MC33772C_WAKEUP_MASK2_IC_TSD_FLT_MASK_8_F(x)         ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK2_IC_TSD_FLT_MASK_8_F_SHIFT) & MC33772C_WAKEUP_MASK2_IC_TSD_FLT_MASK_8_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK2_IC_TSD_FLT_MASK_8_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK2_IC_TSD_FLT_MASK_8_F_MASKED_ENUM_VAL (0x1U)

/* Field GND_LOSS_FLT_MASK_9_F: Prevent the corresponding flags in FAULT2_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK2_GND_LOSS_FLT_MASK_9_F_SHIFT    (0x9U)
#define MC33772C_WAKEUP_MASK2_GND_LOSS_FLT_MASK_9_F_MASK     (0x200U)
#define MC33772C_WAKEUP_MASK2_GND_LOSS_FLT_MASK_9_F(x)       ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK2_GND_LOSS_FLT_MASK_9_F_SHIFT) & MC33772C_WAKEUP_MASK2_GND_LOSS_FLT_MASK_9_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK2_GND_LOSS_FLT_MASK_9_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK2_GND_LOSS_FLT_MASK_9_F_MASKED_ENUM_VAL (0x1U)

/* Field ADC1_A_FLT_MASK_10_F: Prevent the corresponding flags in FAULT2_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK2_ADC1_A_FLT_MASK_10_F_SHIFT     (0xAU)
#define MC33772C_WAKEUP_MASK2_ADC1_A_FLT_MASK_10_F_MASK      (0x400U)
#define MC33772C_WAKEUP_MASK2_ADC1_A_FLT_MASK_10_F(x)        ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK2_ADC1_A_FLT_MASK_10_F_SHIFT) & MC33772C_WAKEUP_MASK2_ADC1_A_FLT_MASK_10_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK2_ADC1_A_FLT_MASK_10_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK2_ADC1_A_FLT_MASK_10_F_MASKED_ENUM_VAL (0x1U)

/* Field ADC1_B_FLT_MASK_11_F: Prevent the corresponding flags in FAULT2_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK2_ADC1_B_FLT_MASK_11_F_SHIFT     (0xBU)
#define MC33772C_WAKEUP_MASK2_ADC1_B_FLT_MASK_11_F_MASK      (0x800U)
#define MC33772C_WAKEUP_MASK2_ADC1_B_FLT_MASK_11_F(x)        ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK2_ADC1_B_FLT_MASK_11_F_SHIFT) & MC33772C_WAKEUP_MASK2_ADC1_B_FLT_MASK_11_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK2_ADC1_B_FLT_MASK_11_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK2_ADC1_B_FLT_MASK_11_F_MASKED_ENUM_VAL (0x1U)

/* Field VANA_UV_FLT_MASK_12_F: Prevent the corresponding flags in FAULT2_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK2_VANA_UV_FLT_MASK_12_F_SHIFT    (0xCU)
#define MC33772C_WAKEUP_MASK2_VANA_UV_FLT_MASK_12_F_MASK     (0x1000U)
#define MC33772C_WAKEUP_MASK2_VANA_UV_FLT_MASK_12_F(x)       ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK2_VANA_UV_FLT_MASK_12_F_SHIFT) & MC33772C_WAKEUP_MASK2_VANA_UV_FLT_MASK_12_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK2_VANA_UV_FLT_MASK_12_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK2_VANA_UV_FLT_MASK_12_F_MASKED_ENUM_VAL (0x1U)

/* Field VANA_OV_FLT_MASK_13_F: Prevent the corresponding flags in FAULT2_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK2_VANA_OV_FLT_MASK_13_F_SHIFT    (0xDU)
#define MC33772C_WAKEUP_MASK2_VANA_OV_FLT_MASK_13_F_MASK     (0x2000U)
#define MC33772C_WAKEUP_MASK2_VANA_OV_FLT_MASK_13_F(x)       ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK2_VANA_OV_FLT_MASK_13_F_SHIFT) & MC33772C_WAKEUP_MASK2_VANA_OV_FLT_MASK_13_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK2_VANA_OV_FLT_MASK_13_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK2_VANA_OV_FLT_MASK_13_F_MASKED_ENUM_VAL (0x1U)

/* Field VCOM_UV_FLT_MASK_14_F: Prevent the corresponding flags in FAULT2_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK2_VCOM_UV_FLT_MASK_14_F_SHIFT    (0xEU)
#define MC33772C_WAKEUP_MASK2_VCOM_UV_FLT_MASK_14_F_MASK     (0x4000U)
#define MC33772C_WAKEUP_MASK2_VCOM_UV_FLT_MASK_14_F(x)       ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK2_VCOM_UV_FLT_MASK_14_F_SHIFT) & MC33772C_WAKEUP_MASK2_VCOM_UV_FLT_MASK_14_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK2_VCOM_UV_FLT_MASK_14_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK2_VCOM_UV_FLT_MASK_14_F_MASKED_ENUM_VAL (0x1U)

/* Field VCOM_OV_FLT_MASK_15_F: Prevent the corresponding flags in FAULT2_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK2_VCOM_OV_FLT_MASK_15_F_SHIFT    (0xFU)
#define MC33772C_WAKEUP_MASK2_VCOM_OV_FLT_MASK_15_F_MASK     (0x8000U)
#define MC33772C_WAKEUP_MASK2_VCOM_OV_FLT_MASK_15_F(x)       ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK2_VCOM_OV_FLT_MASK_15_F_SHIFT) & MC33772C_WAKEUP_MASK2_VCOM_OV_FLT_MASK_15_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK2_VCOM_OV_FLT_MASK_15_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK2_VCOM_OV_FLT_MASK_15_F_MASKED_ENUM_VAL (0x1U)

/* --------------------------------------------------------------------------
 * WAKEUP_MASK3 (read-write): Wake-up mask register 3.
 * -------------------------------------------------------------------------- */
#define MC33772C_WAKEUP_MASK3_OFFSET                         (0x2CU)
#define MC33772C_WAKEUP_MASK3_POR_VAL                        (0xA03FU)

/* Field EOT_CB1_MASK_0_F: Prevent the corresponding flags in FAULT3_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK3_EOT_CB1_MASK_0_F_SHIFT         (0x0U)
#define MC33772C_WAKEUP_MASK3_EOT_CB1_MASK_0_F_MASK          (0x1U)
#define MC33772C_WAKEUP_MASK3_EOT_CB1_MASK_0_F(x)            ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK3_EOT_CB1_MASK_0_F_SHIFT) & MC33772C_WAKEUP_MASK3_EOT_CB1_MASK_0_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK3_EOT_CB1_MASK_0_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK3_EOT_CB1_MASK_0_F_MASKED_ENUM_VAL (0x1U)

/* Field EOT_CB2_MASK_1_F: Prevent the corresponding flags in FAULT3_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK3_EOT_CB2_MASK_1_F_SHIFT         (0x1U)
#define MC33772C_WAKEUP_MASK3_EOT_CB2_MASK_1_F_MASK          (0x2U)
#define MC33772C_WAKEUP_MASK3_EOT_CB2_MASK_1_F(x)            ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK3_EOT_CB2_MASK_1_F_SHIFT) & MC33772C_WAKEUP_MASK3_EOT_CB2_MASK_1_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK3_EOT_CB2_MASK_1_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK3_EOT_CB2_MASK_1_F_MASKED_ENUM_VAL (0x1U)

/* Field EOT_CB3_MASK_2_F: Prevent the corresponding flags in FAULT3_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK3_EOT_CB3_MASK_2_F_SHIFT         (0x2U)
#define MC33772C_WAKEUP_MASK3_EOT_CB3_MASK_2_F_MASK          (0x4U)
#define MC33772C_WAKEUP_MASK3_EOT_CB3_MASK_2_F(x)            ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK3_EOT_CB3_MASK_2_F_SHIFT) & MC33772C_WAKEUP_MASK3_EOT_CB3_MASK_2_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK3_EOT_CB3_MASK_2_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK3_EOT_CB3_MASK_2_F_MASKED_ENUM_VAL (0x1U)

/* Field EOT_CB4_MASK_3_F: Prevent the corresponding flags in FAULT3_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK3_EOT_CB4_MASK_3_F_SHIFT         (0x3U)
#define MC33772C_WAKEUP_MASK3_EOT_CB4_MASK_3_F_MASK          (0x8U)
#define MC33772C_WAKEUP_MASK3_EOT_CB4_MASK_3_F(x)            ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK3_EOT_CB4_MASK_3_F_SHIFT) & MC33772C_WAKEUP_MASK3_EOT_CB4_MASK_3_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK3_EOT_CB4_MASK_3_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK3_EOT_CB4_MASK_3_F_MASKED_ENUM_VAL (0x1U)

/* Field EOT_CB5_MASK_4_F: Prevent the corresponding flags in FAULT3_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK3_EOT_CB5_MASK_4_F_SHIFT         (0x4U)
#define MC33772C_WAKEUP_MASK3_EOT_CB5_MASK_4_F_MASK          (0x10U)
#define MC33772C_WAKEUP_MASK3_EOT_CB5_MASK_4_F(x)            ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK3_EOT_CB5_MASK_4_F_SHIFT) & MC33772C_WAKEUP_MASK3_EOT_CB5_MASK_4_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK3_EOT_CB5_MASK_4_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK3_EOT_CB5_MASK_4_F_MASKED_ENUM_VAL (0x1U)

/* Field EOT_CB6_MASK_5_F: Prevent the corresponding flags in FAULT3_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK3_EOT_CB6_MASK_5_F_SHIFT         (0x5U)
#define MC33772C_WAKEUP_MASK3_EOT_CB6_MASK_5_F_MASK          (0x20U)
#define MC33772C_WAKEUP_MASK3_EOT_CB6_MASK_5_F(x)            ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK3_EOT_CB6_MASK_5_F_SHIFT) & MC33772C_WAKEUP_MASK3_EOT_CB6_MASK_5_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK3_EOT_CB6_MASK_5_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK3_EOT_CB6_MASK_5_F_MASKED_ENUM_VAL (0x1U)

/* Field VCP_UV_MASK_13_F: Prevent the corresponding flags in FAULT3_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK3_VCP_UV_MASK_13_F_SHIFT         (0xDU)
#define MC33772C_WAKEUP_MASK3_VCP_UV_MASK_13_F_MASK          (0x2000U)
#define MC33772C_WAKEUP_MASK3_VCP_UV_MASK_13_F(x)            ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK3_VCP_UV_MASK_13_F_SHIFT) & MC33772C_WAKEUP_MASK3_VCP_UV_MASK_13_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK3_VCP_UV_MASK_13_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK3_VCP_UV_MASK_13_F_MASKED_ENUM_VAL (0x1U)

/* Field CC_OVR_FLT_MASK_15_F: Prevent the corresponding flags in FAULT1_STATUS to wake-up the device. */
#define MC33772C_WAKEUP_MASK3_CC_OVR_FLT_MASK_15_F_SHIFT     (0xFU)
#define MC33772C_WAKEUP_MASK3_CC_OVR_FLT_MASK_15_F_MASK      (0x8000U)
#define MC33772C_WAKEUP_MASK3_CC_OVR_FLT_MASK_15_F(x)        ((uint16_t)((uint16_t)(x) << MC33772C_WAKEUP_MASK3_CC_OVR_FLT_MASK_15_F_SHIFT) & MC33772C_WAKEUP_MASK3_CC_OVR_FLT_MASK_15_F_MASK)

/* Enumerated value NOT_MASKED: The flag wakes the device up, when active. */
#define MC33772C_WAKEUP_MASK3_CC_OVR_FLT_MASK_15_F_NOT_MASKED_ENUM_VAL (0x0U)

/* Enumerated value MASKED: No wake-up is possible by this source. */
#define MC33772C_WAKEUP_MASK3_CC_OVR_FLT_MASK_15_F_MASKED_ENUM_VAL (0x1U)

/* --------------------------------------------------------------------------
 * CC_NB_SAMPLES (read-only): Coulomb count number of samples register.
 * -------------------------------------------------------------------------- */
#define MC33772C_CC_NB_SAMPLES_OFFSET                        (0x2DU)
#define MC33772C_CC_NB_SAMPLES_POR_VAL                       (0x0U)

/* Field CC_NB_SAMPLES (read-only): Number of samples accumulated for the coulomb count value. */
#define MC33772C_CC_NB_SAMPLES_CC_NB_SAMPLES_SHIFT           (0x0U)
#define MC33772C_CC_NB_SAMPLES_CC_NB_SAMPLES_MASK            (0xFFFFU)
#define MC33772C_CC_NB_SAMPLES_CC_NB_SAMPLES(x)              ((uint16_t)((uint16_t)(x) << MC33772C_CC_NB_SAMPLES_CC_NB_SAMPLES_SHIFT) & MC33772C_CC_NB_SAMPLES_CC_NB_SAMPLES_MASK)

/* --------------------------------------------------------------------------
 * COULOMB_CNT1 (read-only): Coulomb count register.
 * -------------------------------------------------------------------------- */
#define MC33772C_COULOMB_CNT1_OFFSET                         (0x2EU)
#define MC33772C_COULOMB_CNT1_POR_VAL                        (0x0U)

/* Field COULOMB_CNT_MSB (read-only): Coulomb counting accumulator. */
#define MC33772C_COULOMB_CNT1_COULOMB_CNT_MSB_SHIFT          (0x0U)
#define MC33772C_COULOMB_CNT1_COULOMB_CNT_MSB_MASK           (0xFFFFU)
#define MC33772C_COULOMB_CNT1_COULOMB_CNT_MSB(x)             ((uint16_t)((uint16_t)(x) << MC33772C_COULOMB_CNT1_COULOMB_CNT_MSB_SHIFT) & MC33772C_COULOMB_CNT1_COULOMB_CNT_MSB_MASK)

/* --------------------------------------------------------------------------
 * COULOMB_CNT2 (read-only): Coulomb count register.
 * -------------------------------------------------------------------------- */
#define MC33772C_COULOMB_CNT2_OFFSET                         (0x2FU)
#define MC33772C_COULOMB_CNT2_POR_VAL                        (0x0U)

/* Field COULOMB_CNT_LSB (read-only): Coulomb counting accumulator. */
#define MC33772C_COULOMB_CNT2_COULOMB_CNT_LSB_SHIFT          (0x0U)
#define MC33772C_COULOMB_CNT2_COULOMB_CNT_LSB_MASK           (0xFFFFU)
#define MC33772C_COULOMB_CNT2_COULOMB_CNT_LSB(x)             ((uint16_t)((uint16_t)(x) << MC33772C_COULOMB_CNT2_COULOMB_CNT_LSB_SHIFT) & MC33772C_COULOMB_CNT2_COULOMB_CNT_LSB_MASK)

/* --------------------------------------------------------------------------
 * MEAS_ISENSE1 (read-only): Current measurement register 1.
 * -------------------------------------------------------------------------- */
#define MC33772C_MEAS_ISENSE1_OFFSET                         (0x30U)
#define MC33772C_MEAS_ISENSE1_POR_VAL                        (0x0U)

/* Field MEAS_I_MSB (read-only): ISENSE value, compensated in gain and temp, signed. */
#define MC33772C_MEAS_ISENSE1_MEAS_I_MSB_SHIFT               (0x0U)
#define MC33772C_MEAS_ISENSE1_MEAS_I_MSB_MASK                (0x7FFFU)
#define MC33772C_MEAS_ISENSE1_MEAS_I_MSB(x)                  ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_ISENSE1_MEAS_I_MSB_SHIFT) & MC33772C_MEAS_ISENSE1_MEAS_I_MSB_MASK)

/* Field DATA_RDY (read-only): This bit is set when the conversion is complete and the register is updated. The DATA_RDY bit is cleared when a request to convert is received either through the SOC or GPIO2 convert trigger. */
#define MC33772C_MEAS_ISENSE1_DATA_RDY_SHIFT                 (0xFU)
#define MC33772C_MEAS_ISENSE1_DATA_RDY_MASK                  (0x8000U)
#define MC33772C_MEAS_ISENSE1_DATA_RDY(x)                    ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_ISENSE1_DATA_RDY_SHIFT) & MC33772C_MEAS_ISENSE1_DATA_RDY_MASK)

/* Enumerated value IN_PROGRESS: A new sequence of conversions is currently running. */
#define MC33772C_MEAS_ISENSE1_DATA_RDY_IN_PROGRESS_ENUM_VAL  (0x0U)

/* Enumerated value DATA_RDY: A data is available in MEAS_ISENSE1. */
#define MC33772C_MEAS_ISENSE1_DATA_RDY_DATA_RDY_ENUM_VAL     (0x1U)

/* --------------------------------------------------------------------------
 * MEAS_ISENSE2 (read-only): Current measurement register 2.
 * -------------------------------------------------------------------------- */
#define MC33772C_MEAS_ISENSE2_OFFSET                         (0x31U)
#define MC33772C_MEAS_ISENSE2_POR_VAL                        (0x0U)

/* Field MEAS_I_LSB (read-only): ISENSE value, compensated in gain and temp, signed. */
#define MC33772C_MEAS_ISENSE2_MEAS_I_LSB_SHIFT               (0x0U)
#define MC33772C_MEAS_ISENSE2_MEAS_I_LSB_MASK                (0xFU)
#define MC33772C_MEAS_ISENSE2_MEAS_I_LSB(x)                  ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_ISENSE2_MEAS_I_LSB_SHIFT) & MC33772C_MEAS_ISENSE2_MEAS_I_LSB_MASK)

/* Field PGA_GCHANGE (read-only): PGA gain change information during ISENSE on-demand conversion. */
#define MC33772C_MEAS_ISENSE2_PGA_GCHANGE_SHIFT              (0x6U)
#define MC33772C_MEAS_ISENSE2_PGA_GCHANGE_MASK               (0x40U)
#define MC33772C_MEAS_ISENSE2_PGA_GCHANGE(x)                 ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_ISENSE2_PGA_GCHANGE_SHIFT) & MC33772C_MEAS_ISENSE2_PGA_GCHANGE_MASK)

/* Enumerated value NO_CHANGE: No gain change during ISENSE on-demand measurement; result is accurate. */
#define MC33772C_MEAS_ISENSE2_PGA_GCHANGE_NO_CHANGE_ENUM_VAL (0x0U)

/* Enumerated value CHANGED: The PGA gain has changed between the two chopped measurements. */
#define MC33772C_MEAS_ISENSE2_PGA_GCHANGE_CHANGED_ENUM_VAL   (0x1U)

/* Field ADC2_SAT (read-only): ADC2 saturation information. */
#define MC33772C_MEAS_ISENSE2_ADC2_SAT_SHIFT                 (0x7U)
#define MC33772C_MEAS_ISENSE2_ADC2_SAT_MASK                  (0x80U)
#define MC33772C_MEAS_ISENSE2_ADC2_SAT(x)                    ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_ISENSE2_ADC2_SAT_SHIFT) & MC33772C_MEAS_ISENSE2_ADC2_SAT_MASK)

/* Enumerated value NO_SATURATION: No saturation reported. */
#define MC33772C_MEAS_ISENSE2_ADC2_SAT_NO_SATURATION_ENUM_VAL (0x0U)

/* Enumerated value SATURATED: ADC2 has saturated during the ISENSE on-demand conversion. */
#define MC33772C_MEAS_ISENSE2_ADC2_SAT_SATURATED_ENUM_VAL    (0x1U)

/* Field PGA_GAIN (read-only): Report the current gain of the ADC2 programmable gain amplifier (automatically settled or not). */
#define MC33772C_MEAS_ISENSE2_PGA_GAIN_SHIFT                 (0x8U)
#define MC33772C_MEAS_ISENSE2_PGA_GAIN_MASK                  (0x300U)
#define MC33772C_MEAS_ISENSE2_PGA_GAIN(x)                    ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_ISENSE2_PGA_GAIN_SHIFT) & MC33772C_MEAS_ISENSE2_PGA_GAIN_MASK)

/* Enumerated value 4: 4 */
#define MC33772C_MEAS_ISENSE2_PGA_GAIN_4_ENUM_VAL            (0x0U)

/* Enumerated value 16: 16 */
#define MC33772C_MEAS_ISENSE2_PGA_GAIN_16_ENUM_VAL           (0x1U)

/* Enumerated value 64: 64 */
#define MC33772C_MEAS_ISENSE2_PGA_GAIN_64_ENUM_VAL           (0x2U)

/* Enumerated value 256: 256 */
#define MC33772C_MEAS_ISENSE2_PGA_GAIN_256_ENUM_VAL          (0x3U)

/* Field DATA_RDY (read-only): This bit is set when the conversion is complete and the register is updated. The DATA_RDY bit is cleared when a request to convert is received either through the SOC or GPIO2 convert trigger. */
#define MC33772C_MEAS_ISENSE2_DATA_RDY_SHIFT                 (0xFU)
#define MC33772C_MEAS_ISENSE2_DATA_RDY_MASK                  (0x8000U)
#define MC33772C_MEAS_ISENSE2_DATA_RDY(x)                    ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_ISENSE2_DATA_RDY_SHIFT) & MC33772C_MEAS_ISENSE2_DATA_RDY_MASK)

/* Enumerated value IN_PROGRESS: A new sequence of conversions is currently running. */
#define MC33772C_MEAS_ISENSE2_DATA_RDY_IN_PROGRESS_ENUM_VAL  (0x0U)

/* Enumerated value DATA_RDY: Data is available in MEAS_ISENSE2. */
#define MC33772C_MEAS_ISENSE2_DATA_RDY_DATA_RDY_ENUM_VAL     (0x1U)

/* --------------------------------------------------------------------------
 * MEAS_STACK (read-only): Stack voltage measurement.
 * -------------------------------------------------------------------------- */
#define MC33772C_MEAS_STACK_OFFSET                           (0x32U)
#define MC33772C_MEAS_STACK_POR_VAL                          (0x0U)

/* Field MEAS_STACK (read-only): Value is unsigned, resolution is VCT_ANx_RES independently on the selected resolution of ADC_CFG. */
#define MC33772C_MEAS_STACK_MEAS_STACK_SHIFT                 (0x0U)
#define MC33772C_MEAS_STACK_MEAS_STACK_MASK                  (0x7FFFU)
#define MC33772C_MEAS_STACK_MEAS_STACK(x)                    ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_STACK_MEAS_STACK_SHIFT) & MC33772C_MEAS_STACK_MEAS_STACK_MASK)

/* Field DATA_RDY (read-only): This bit is set when the conversion is complete and the register is updated. The DATA_RDY bit is cleared when a request to convert is received either through the SOC or GPIO2 convert trigger. */
#define MC33772C_MEAS_STACK_DATA_RDY_SHIFT                   (0xFU)
#define MC33772C_MEAS_STACK_DATA_RDY_MASK                    (0x8000U)
#define MC33772C_MEAS_STACK_DATA_RDY(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_STACK_DATA_RDY_SHIFT) & MC33772C_MEAS_STACK_DATA_RDY_MASK)

/* Enumerated value IN_PROGRESS: A new sequence of conversions is currently running. */
#define MC33772C_MEAS_STACK_DATA_RDY_IN_PROGRESS_ENUM_VAL    (0x0U)

/* Enumerated value DATA_RDY: A data is available. */
#define MC33772C_MEAS_STACK_DATA_RDY_DATA_RDY_ENUM_VAL       (0x1U)

/* --------------------------------------------------------------------------
 * MEAS_CELL6 (read-only): Cell 6 voltage measurement.
 * -------------------------------------------------------------------------- */
#define MC33772C_MEAS_CELL6_OFFSET                           (0x3BU)
#define MC33772C_MEAS_CELL6_POR_VAL                          (0x0U)

/* Field MEAS_CELL (read-only): Value is unsigned, resolution is VCT_ANx_RES independently on the selected resolution of ADC_CFG. */
#define MC33772C_MEAS_CELL6_MEAS_CELL_SHIFT                  (0x0U)
#define MC33772C_MEAS_CELL6_MEAS_CELL_MASK                   (0x7FFFU)
#define MC33772C_MEAS_CELL6_MEAS_CELL(x)                     ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_CELL6_MEAS_CELL_SHIFT) & MC33772C_MEAS_CELL6_MEAS_CELL_MASK)

/* Field DATA_RDY (read-only): This bit is set when the conversion is complete and the register is updated. The DATA_RDY bit is cleared when a request to convert is received either through the SOC or GPIO2 convert trigger. */
#define MC33772C_MEAS_CELL6_DATA_RDY_SHIFT                   (0xFU)
#define MC33772C_MEAS_CELL6_DATA_RDY_MASK                    (0x8000U)
#define MC33772C_MEAS_CELL6_DATA_RDY(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_CELL6_DATA_RDY_SHIFT) & MC33772C_MEAS_CELL6_DATA_RDY_MASK)

/* Enumerated value IN_PROGRESS: A new sequence of conversions is currently running. */
#define MC33772C_MEAS_CELL6_DATA_RDY_IN_PROGRESS_ENUM_VAL    (0x0U)

/* Enumerated value DATA_RDY: A data is available. */
#define MC33772C_MEAS_CELL6_DATA_RDY_DATA_RDY_ENUM_VAL       (0x1U)

/* --------------------------------------------------------------------------
 * MEAS_CELL5 (read-only): Cell 5 voltage measurement.
 * -------------------------------------------------------------------------- */
#define MC33772C_MEAS_CELL5_OFFSET                           (0x3CU)
#define MC33772C_MEAS_CELL5_POR_VAL                          (0x0U)

/* Field MEAS_CELL (read-only): Value is unsigned, resolution is VCT_ANx_RES independently on the selected resolution of ADC_CFG. */
#define MC33772C_MEAS_CELL5_MEAS_CELL_SHIFT                  (0x0U)
#define MC33772C_MEAS_CELL5_MEAS_CELL_MASK                   (0x7FFFU)
#define MC33772C_MEAS_CELL5_MEAS_CELL(x)                     ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_CELL5_MEAS_CELL_SHIFT) & MC33772C_MEAS_CELL5_MEAS_CELL_MASK)

/* Field DATA_RDY (read-only): This bit is set when the conversion is complete and the register is updated. The DATA_RDY bit is cleared when a request to convert is received either through the SOC or GPIO2 convert trigger. */
#define MC33772C_MEAS_CELL5_DATA_RDY_SHIFT                   (0xFU)
#define MC33772C_MEAS_CELL5_DATA_RDY_MASK                    (0x8000U)
#define MC33772C_MEAS_CELL5_DATA_RDY(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_CELL5_DATA_RDY_SHIFT) & MC33772C_MEAS_CELL5_DATA_RDY_MASK)

/* Enumerated value IN_PROGRESS: A new sequence of conversions is currently running. */
#define MC33772C_MEAS_CELL5_DATA_RDY_IN_PROGRESS_ENUM_VAL    (0x0U)

/* Enumerated value DATA_RDY: A data is available. */
#define MC33772C_MEAS_CELL5_DATA_RDY_DATA_RDY_ENUM_VAL       (0x1U)

/* --------------------------------------------------------------------------
 * MEAS_CELL4 (read-only): Cell 4 voltage measurement.
 * -------------------------------------------------------------------------- */
#define MC33772C_MEAS_CELL4_OFFSET                           (0x3DU)
#define MC33772C_MEAS_CELL4_POR_VAL                          (0x0U)

/* Field MEAS_CELL (read-only): Value is unsigned, resolution is VCT_ANx_RES independently on the selected resolution of ADC_CFG. */
#define MC33772C_MEAS_CELL4_MEAS_CELL_SHIFT                  (0x0U)
#define MC33772C_MEAS_CELL4_MEAS_CELL_MASK                   (0x7FFFU)
#define MC33772C_MEAS_CELL4_MEAS_CELL(x)                     ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_CELL4_MEAS_CELL_SHIFT) & MC33772C_MEAS_CELL4_MEAS_CELL_MASK)

/* Field DATA_RDY (read-only): This bit is set when the conversion is complete and the register is updated. The DATA_RDY bit is cleared when a request to convert is received either through the SOC or GPIO2 convert trigger. */
#define MC33772C_MEAS_CELL4_DATA_RDY_SHIFT                   (0xFU)
#define MC33772C_MEAS_CELL4_DATA_RDY_MASK                    (0x8000U)
#define MC33772C_MEAS_CELL4_DATA_RDY(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_CELL4_DATA_RDY_SHIFT) & MC33772C_MEAS_CELL4_DATA_RDY_MASK)

/* Enumerated value IN_PROGRESS: A new sequence of conversions is currently running. */
#define MC33772C_MEAS_CELL4_DATA_RDY_IN_PROGRESS_ENUM_VAL    (0x0U)

/* Enumerated value DATA_RDY: A data is available. */
#define MC33772C_MEAS_CELL4_DATA_RDY_DATA_RDY_ENUM_VAL       (0x1U)

/* --------------------------------------------------------------------------
 * MEAS_CELL3 (read-only): Cell 3 voltage measurement.
 * -------------------------------------------------------------------------- */
#define MC33772C_MEAS_CELL3_OFFSET                           (0x3EU)
#define MC33772C_MEAS_CELL3_POR_VAL                          (0x0U)

/* Field MEAS_CELL (read-only): Value is unsigned, resolution is VCT_ANx_RES independently on the selected resolution of ADC_CFG. */
#define MC33772C_MEAS_CELL3_MEAS_CELL_SHIFT                  (0x0U)
#define MC33772C_MEAS_CELL3_MEAS_CELL_MASK                   (0x7FFFU)
#define MC33772C_MEAS_CELL3_MEAS_CELL(x)                     ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_CELL3_MEAS_CELL_SHIFT) & MC33772C_MEAS_CELL3_MEAS_CELL_MASK)

/* Field DATA_RDY (read-only): This bit is set when the conversion is complete and the register is updated. The DATA_RDY bit is cleared when a request to convert is received either through the SOC or GPIO2 convert trigger. */
#define MC33772C_MEAS_CELL3_DATA_RDY_SHIFT                   (0xFU)
#define MC33772C_MEAS_CELL3_DATA_RDY_MASK                    (0x8000U)
#define MC33772C_MEAS_CELL3_DATA_RDY(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_CELL3_DATA_RDY_SHIFT) & MC33772C_MEAS_CELL3_DATA_RDY_MASK)

/* Enumerated value IN_PROGRESS: A new sequence of conversions is currently running. */
#define MC33772C_MEAS_CELL3_DATA_RDY_IN_PROGRESS_ENUM_VAL    (0x0U)

/* Enumerated value DATA_RDY: A data is available. */
#define MC33772C_MEAS_CELL3_DATA_RDY_DATA_RDY_ENUM_VAL       (0x1U)

/* --------------------------------------------------------------------------
 * MEAS_CELL2 (read-only): Cell 2 voltage measurement.
 * -------------------------------------------------------------------------- */
#define MC33772C_MEAS_CELL2_OFFSET                           (0x3FU)
#define MC33772C_MEAS_CELL2_POR_VAL                          (0x0U)

/* Field MEAS_CELL (read-only): Value is unsigned, resolution is VCT_ANx_RES independently on the selected resolution of ADC_CFG. */
#define MC33772C_MEAS_CELL2_MEAS_CELL_SHIFT                  (0x0U)
#define MC33772C_MEAS_CELL2_MEAS_CELL_MASK                   (0x7FFFU)
#define MC33772C_MEAS_CELL2_MEAS_CELL(x)                     ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_CELL2_MEAS_CELL_SHIFT) & MC33772C_MEAS_CELL2_MEAS_CELL_MASK)

/* Field DATA_RDY (read-only): This bit is set when the conversion is complete and the register is updated. The DATA_RDY bit is cleared when a request to convert is received either through the SOC or GPIO2 convert trigger. */
#define MC33772C_MEAS_CELL2_DATA_RDY_SHIFT                   (0xFU)
#define MC33772C_MEAS_CELL2_DATA_RDY_MASK                    (0x8000U)
#define MC33772C_MEAS_CELL2_DATA_RDY(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_CELL2_DATA_RDY_SHIFT) & MC33772C_MEAS_CELL2_DATA_RDY_MASK)

/* Enumerated value IN_PROGRESS: A new sequence of conversions is currently running. */
#define MC33772C_MEAS_CELL2_DATA_RDY_IN_PROGRESS_ENUM_VAL    (0x0U)

/* Enumerated value DATA_RDY: A data is available. */
#define MC33772C_MEAS_CELL2_DATA_RDY_DATA_RDY_ENUM_VAL       (0x1U)

/* --------------------------------------------------------------------------
 * MEAS_CELL1 (read-only): Cell 1 voltage measurement.
 * -------------------------------------------------------------------------- */
#define MC33772C_MEAS_CELL1_OFFSET                           (0x40U)
#define MC33772C_MEAS_CELL1_POR_VAL                          (0x0U)

/* Field MEAS_CELL (read-only): Value is unsigned, resolution is VCT_ANx_RES independently on the selected resolution of ADC_CFG. */
#define MC33772C_MEAS_CELL1_MEAS_CELL_SHIFT                  (0x0U)
#define MC33772C_MEAS_CELL1_MEAS_CELL_MASK                   (0x7FFFU)
#define MC33772C_MEAS_CELL1_MEAS_CELL(x)                     ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_CELL1_MEAS_CELL_SHIFT) & MC33772C_MEAS_CELL1_MEAS_CELL_MASK)

/* Field DATA_RDY (read-only): This bit is set when the conversion is complete and the register is updated. The DATA_RDY bit is cleared when a request to convert is received either through the SOC or GPIO2 convert trigger. */
#define MC33772C_MEAS_CELL1_DATA_RDY_SHIFT                   (0xFU)
#define MC33772C_MEAS_CELL1_DATA_RDY_MASK                    (0x8000U)
#define MC33772C_MEAS_CELL1_DATA_RDY(x)                      ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_CELL1_DATA_RDY_SHIFT) & MC33772C_MEAS_CELL1_DATA_RDY_MASK)

/* Enumerated value IN_PROGRESS: A new sequence of conversions is currently running. */
#define MC33772C_MEAS_CELL1_DATA_RDY_IN_PROGRESS_ENUM_VAL    (0x0U)

/* Enumerated value DATA_RDY: A data is available. */
#define MC33772C_MEAS_CELL1_DATA_RDY_DATA_RDY_ENUM_VAL       (0x1U)

/* --------------------------------------------------------------------------
 * MEAS_AN6 (read-only): AN voltage measurement.
 * -------------------------------------------------------------------------- */
#define MC33772C_MEAS_AN6_OFFSET                             (0x41U)
#define MC33772C_MEAS_AN6_POR_VAL                            (0x0U)

/* Field MEAS_AN (read-only): Value is unsigned, resolution is VCT_ANx_RES independently on the selected resolution of ADC_CFG. */
#define MC33772C_MEAS_AN6_MEAS_AN_SHIFT                      (0x0U)
#define MC33772C_MEAS_AN6_MEAS_AN_MASK                       (0x7FFFU)
#define MC33772C_MEAS_AN6_MEAS_AN(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_AN6_MEAS_AN_SHIFT) & MC33772C_MEAS_AN6_MEAS_AN_MASK)

/* Field DATA_RDY (read-only): This bit is set when the conversion is complete and the register is updated. The DATA_RDY bit is cleared when a request to convert is received either through the SOC or GPIO2 convert trigger. */
#define MC33772C_MEAS_AN6_DATA_RDY_SHIFT                     (0xFU)
#define MC33772C_MEAS_AN6_DATA_RDY_MASK                      (0x8000U)
#define MC33772C_MEAS_AN6_DATA_RDY(x)                        ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_AN6_DATA_RDY_SHIFT) & MC33772C_MEAS_AN6_DATA_RDY_MASK)

/* Enumerated value IN_PROGRESS: A new sequence of conversions is currently running. */
#define MC33772C_MEAS_AN6_DATA_RDY_IN_PROGRESS_ENUM_VAL      (0x0U)

/* Enumerated value DATA_RDY: A data is available. */
#define MC33772C_MEAS_AN6_DATA_RDY_DATA_RDY_ENUM_VAL         (0x1U)

/* --------------------------------------------------------------------------
 * MEAS_AN5 (read-only): AN voltage measurement.
 * -------------------------------------------------------------------------- */
#define MC33772C_MEAS_AN5_OFFSET                             (0x42U)
#define MC33772C_MEAS_AN5_POR_VAL                            (0x0U)

/* Field MEAS_AN (read-only): Value is unsigned, resolution is VCT_ANx_RES independently on the selected resolution of ADC_CFG. */
#define MC33772C_MEAS_AN5_MEAS_AN_SHIFT                      (0x0U)
#define MC33772C_MEAS_AN5_MEAS_AN_MASK                       (0x7FFFU)
#define MC33772C_MEAS_AN5_MEAS_AN(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_AN5_MEAS_AN_SHIFT) & MC33772C_MEAS_AN5_MEAS_AN_MASK)

/* Field DATA_RDY (read-only): This bit is set when the conversion is complete and the register is updated. The DATA_RDY bit is cleared when a request to convert is received either through the SOC or GPIO2 convert trigger. */
#define MC33772C_MEAS_AN5_DATA_RDY_SHIFT                     (0xFU)
#define MC33772C_MEAS_AN5_DATA_RDY_MASK                      (0x8000U)
#define MC33772C_MEAS_AN5_DATA_RDY(x)                        ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_AN5_DATA_RDY_SHIFT) & MC33772C_MEAS_AN5_DATA_RDY_MASK)

/* Enumerated value IN_PROGRESS: A new sequence of conversions is currently running. */
#define MC33772C_MEAS_AN5_DATA_RDY_IN_PROGRESS_ENUM_VAL      (0x0U)

/* Enumerated value DATA_RDY: A data is available. */
#define MC33772C_MEAS_AN5_DATA_RDY_DATA_RDY_ENUM_VAL         (0x1U)

/* --------------------------------------------------------------------------
 * MEAS_AN4 (read-only): AN voltage measurement.
 * -------------------------------------------------------------------------- */
#define MC33772C_MEAS_AN4_OFFSET                             (0x43U)
#define MC33772C_MEAS_AN4_POR_VAL                            (0x0U)

/* Field MEAS_AN (read-only): Value is unsigned, resolution is VCT_ANx_RES independently on the selected resolution of ADC_CFG. */
#define MC33772C_MEAS_AN4_MEAS_AN_SHIFT                      (0x0U)
#define MC33772C_MEAS_AN4_MEAS_AN_MASK                       (0x7FFFU)
#define MC33772C_MEAS_AN4_MEAS_AN(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_AN4_MEAS_AN_SHIFT) & MC33772C_MEAS_AN4_MEAS_AN_MASK)

/* Field DATA_RDY (read-only): This bit is set when the conversion is complete and the register is updated. The DATA_RDY bit is cleared when a request to convert is received either through the SOC or GPIO2 convert trigger. */
#define MC33772C_MEAS_AN4_DATA_RDY_SHIFT                     (0xFU)
#define MC33772C_MEAS_AN4_DATA_RDY_MASK                      (0x8000U)
#define MC33772C_MEAS_AN4_DATA_RDY(x)                        ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_AN4_DATA_RDY_SHIFT) & MC33772C_MEAS_AN4_DATA_RDY_MASK)

/* Enumerated value IN_PROGRESS: A new sequence of conversions is currently running. */
#define MC33772C_MEAS_AN4_DATA_RDY_IN_PROGRESS_ENUM_VAL      (0x0U)

/* Enumerated value DATA_RDY: A data is available. */
#define MC33772C_MEAS_AN4_DATA_RDY_DATA_RDY_ENUM_VAL         (0x1U)

/* --------------------------------------------------------------------------
 * MEAS_AN3 (read-only): AN voltage measurement.
 * -------------------------------------------------------------------------- */
#define MC33772C_MEAS_AN3_OFFSET                             (0x44U)
#define MC33772C_MEAS_AN3_POR_VAL                            (0x0U)

/* Field MEAS_AN (read-only): Value is unsigned, resolution is VCT_ANx_RES independently on the selected resolution of ADC_CFG. */
#define MC33772C_MEAS_AN3_MEAS_AN_SHIFT                      (0x0U)
#define MC33772C_MEAS_AN3_MEAS_AN_MASK                       (0x7FFFU)
#define MC33772C_MEAS_AN3_MEAS_AN(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_AN3_MEAS_AN_SHIFT) & MC33772C_MEAS_AN3_MEAS_AN_MASK)

/* Field DATA_RDY (read-only): This bit is set when the conversion is complete and the register is updated. The DATA_RDY bit is cleared when a request to convert is received either through the SOC or GPIO2 convert trigger. */
#define MC33772C_MEAS_AN3_DATA_RDY_SHIFT                     (0xFU)
#define MC33772C_MEAS_AN3_DATA_RDY_MASK                      (0x8000U)
#define MC33772C_MEAS_AN3_DATA_RDY(x)                        ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_AN3_DATA_RDY_SHIFT) & MC33772C_MEAS_AN3_DATA_RDY_MASK)

/* Enumerated value IN_PROGRESS: A new sequence of conversions is currently running. */
#define MC33772C_MEAS_AN3_DATA_RDY_IN_PROGRESS_ENUM_VAL      (0x0U)

/* Enumerated value DATA_RDY: A data is available. */
#define MC33772C_MEAS_AN3_DATA_RDY_DATA_RDY_ENUM_VAL         (0x1U)

/* --------------------------------------------------------------------------
 * MEAS_AN2 (read-only): AN voltage measurement.
 * -------------------------------------------------------------------------- */
#define MC33772C_MEAS_AN2_OFFSET                             (0x45U)
#define MC33772C_MEAS_AN2_POR_VAL                            (0x0U)

/* Field MEAS_AN (read-only): Value is unsigned, resolution is VCT_ANx_RES independently on the selected resolution of ADC_CFG. */
#define MC33772C_MEAS_AN2_MEAS_AN_SHIFT                      (0x0U)
#define MC33772C_MEAS_AN2_MEAS_AN_MASK                       (0x7FFFU)
#define MC33772C_MEAS_AN2_MEAS_AN(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_AN2_MEAS_AN_SHIFT) & MC33772C_MEAS_AN2_MEAS_AN_MASK)

/* Field DATA_RDY (read-only): This bit is set when the conversion is complete and the register is updated. The DATA_RDY bit is cleared when a request to convert is received either through the SOC or GPIO2 convert trigger. */
#define MC33772C_MEAS_AN2_DATA_RDY_SHIFT                     (0xFU)
#define MC33772C_MEAS_AN2_DATA_RDY_MASK                      (0x8000U)
#define MC33772C_MEAS_AN2_DATA_RDY(x)                        ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_AN2_DATA_RDY_SHIFT) & MC33772C_MEAS_AN2_DATA_RDY_MASK)

/* Enumerated value IN_PROGRESS: A new sequence of conversions is currently running. */
#define MC33772C_MEAS_AN2_DATA_RDY_IN_PROGRESS_ENUM_VAL      (0x0U)

/* Enumerated value DATA_RDY: A data is available. */
#define MC33772C_MEAS_AN2_DATA_RDY_DATA_RDY_ENUM_VAL         (0x1U)

/* --------------------------------------------------------------------------
 * MEAS_AN1 (read-only): AN voltage measurement.
 * -------------------------------------------------------------------------- */
#define MC33772C_MEAS_AN1_OFFSET                             (0x46U)
#define MC33772C_MEAS_AN1_POR_VAL                            (0x0U)

/* Field MEAS_AN (read-only): Value is unsigned, resolution is VCT_ANx_RES independently on the selected resolution of ADC_CFG. */
#define MC33772C_MEAS_AN1_MEAS_AN_SHIFT                      (0x0U)
#define MC33772C_MEAS_AN1_MEAS_AN_MASK                       (0x7FFFU)
#define MC33772C_MEAS_AN1_MEAS_AN(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_AN1_MEAS_AN_SHIFT) & MC33772C_MEAS_AN1_MEAS_AN_MASK)

/* Field DATA_RDY (read-only): This bit is set when the conversion is complete and the register is updated. The DATA_RDY bit is cleared when a request to convert is received either through the SOC or GPIO2 convert trigger. */
#define MC33772C_MEAS_AN1_DATA_RDY_SHIFT                     (0xFU)
#define MC33772C_MEAS_AN1_DATA_RDY_MASK                      (0x8000U)
#define MC33772C_MEAS_AN1_DATA_RDY(x)                        ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_AN1_DATA_RDY_SHIFT) & MC33772C_MEAS_AN1_DATA_RDY_MASK)

/* Enumerated value IN_PROGRESS: A new sequence of conversions is currently running. */
#define MC33772C_MEAS_AN1_DATA_RDY_IN_PROGRESS_ENUM_VAL      (0x0U)

/* Enumerated value DATA_RDY: A data is available. */
#define MC33772C_MEAS_AN1_DATA_RDY_DATA_RDY_ENUM_VAL         (0x1U)

/* --------------------------------------------------------------------------
 * MEAS_AN0 (read-only): AN voltage measurement.
 * -------------------------------------------------------------------------- */
#define MC33772C_MEAS_AN0_OFFSET                             (0x47U)
#define MC33772C_MEAS_AN0_POR_VAL                            (0x0U)

/* Field MEAS_AN (read-only): Value is unsigned, resolution is VCT_ANx_RES independently on the selected resolution of ADC_CFG. */
#define MC33772C_MEAS_AN0_MEAS_AN_SHIFT                      (0x0U)
#define MC33772C_MEAS_AN0_MEAS_AN_MASK                       (0x7FFFU)
#define MC33772C_MEAS_AN0_MEAS_AN(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_AN0_MEAS_AN_SHIFT) & MC33772C_MEAS_AN0_MEAS_AN_MASK)

/* Field DATA_RDY (read-only): This bit is set when the conversion is complete and the register is updated. The DATA_RDY bit is cleared when a request to convert is received either through the SOC or GPIO2 convert trigger. */
#define MC33772C_MEAS_AN0_DATA_RDY_SHIFT                     (0xFU)
#define MC33772C_MEAS_AN0_DATA_RDY_MASK                      (0x8000U)
#define MC33772C_MEAS_AN0_DATA_RDY(x)                        ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_AN0_DATA_RDY_SHIFT) & MC33772C_MEAS_AN0_DATA_RDY_MASK)

/* Enumerated value IN_PROGRESS: A new sequence of conversions is currently running. */
#define MC33772C_MEAS_AN0_DATA_RDY_IN_PROGRESS_ENUM_VAL      (0x0U)

/* Enumerated value DATA_RDY: A data is available. */
#define MC33772C_MEAS_AN0_DATA_RDY_DATA_RDY_ENUM_VAL         (0x1U)

/* --------------------------------------------------------------------------
 * MEAS_IC_TEMP (read-only): IC temperature measurement.
 * -------------------------------------------------------------------------- */
#define MC33772C_MEAS_IC_TEMP_OFFSET                         (0x48U)
#define MC33772C_MEAS_IC_TEMP_POR_VAL                        (0x0U)

/* Field MEAS_IC_TEMP (read-only): Value is unsigned, resolution is VCT_ANx_RES independently on the selected resolution of ADC_CFG. */
#define MC33772C_MEAS_IC_TEMP_MEAS_IC_TEMP_SHIFT             (0x0U)
#define MC33772C_MEAS_IC_TEMP_MEAS_IC_TEMP_MASK              (0x7FFFU)
#define MC33772C_MEAS_IC_TEMP_MEAS_IC_TEMP(x)                ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_IC_TEMP_MEAS_IC_TEMP_SHIFT) & MC33772C_MEAS_IC_TEMP_MEAS_IC_TEMP_MASK)

/* Field DATA_RDY (read-only): This bit is set when the conversion is complete and the register is updated. The DATA_RDY bit is cleared when a request to convert is received either through the SOC or GPIO2 convert trigger. */
#define MC33772C_MEAS_IC_TEMP_DATA_RDY_SHIFT                 (0xFU)
#define MC33772C_MEAS_IC_TEMP_DATA_RDY_MASK                  (0x8000U)
#define MC33772C_MEAS_IC_TEMP_DATA_RDY(x)                    ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_IC_TEMP_DATA_RDY_SHIFT) & MC33772C_MEAS_IC_TEMP_DATA_RDY_MASK)

/* Enumerated value IN_PROGRESS: A new sequence of conversions is currently running. */
#define MC33772C_MEAS_IC_TEMP_DATA_RDY_IN_PROGRESS_ENUM_VAL  (0x0U)

/* Enumerated value DATA_RDY: A data is available. */
#define MC33772C_MEAS_IC_TEMP_DATA_RDY_DATA_RDY_ENUM_VAL     (0x1U)

/* --------------------------------------------------------------------------
 * MEAS_VBG_DIAG_ADC1A (read-only): ADC1A voltage reference measurement
 * -------------------------------------------------------------------------- */
#define MC33772C_MEAS_VBG_DIAG_ADC1A_OFFSET                  (0x49U)
#define MC33772C_MEAS_VBG_DIAG_ADC1A_POR_VAL                 (0x0U)

/* Field MEAS_VBG_DIAG_ADC1A (read-only): Value is unsigned, resolution is VCT_ANx_RES independently on the selected resolution of ADC_CFG. */
#define MC33772C_MEAS_VBG_DIAG_ADC1A_MEAS_VBG_DIAG_ADC1A_SHIFT (0x0U)
#define MC33772C_MEAS_VBG_DIAG_ADC1A_MEAS_VBG_DIAG_ADC1A_MASK (0x7FFFU)
#define MC33772C_MEAS_VBG_DIAG_ADC1A_MEAS_VBG_DIAG_ADC1A(x)  ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_VBG_DIAG_ADC1A_MEAS_VBG_DIAG_ADC1A_SHIFT) & MC33772C_MEAS_VBG_DIAG_ADC1A_MEAS_VBG_DIAG_ADC1A_MASK)

/* Field DATA_RDY (read-only): This bit is set when the conversion is complete and the register is updated. The DATA_RDY bit is cleared when a request to convert is received either through the SOC or GPIO2 convert trigger. */
#define MC33772C_MEAS_VBG_DIAG_ADC1A_DATA_RDY_SHIFT          (0xFU)
#define MC33772C_MEAS_VBG_DIAG_ADC1A_DATA_RDY_MASK           (0x8000U)
#define MC33772C_MEAS_VBG_DIAG_ADC1A_DATA_RDY(x)             ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_VBG_DIAG_ADC1A_DATA_RDY_SHIFT) & MC33772C_MEAS_VBG_DIAG_ADC1A_DATA_RDY_MASK)

/* Enumerated value IN_PROGRESS: A new sequence of conversions is currently running. */
#define MC33772C_MEAS_VBG_DIAG_ADC1A_DATA_RDY_IN_PROGRESS_ENUM_VAL (0x0U)

/* Enumerated value DATA_RDY: A data is available. */
#define MC33772C_MEAS_VBG_DIAG_ADC1A_DATA_RDY_DATA_RDY_ENUM_VAL (0x1U)

/* --------------------------------------------------------------------------
 * MEAS_VBG_DIAG_ADC1B (read-only): ADC1B voltage reference measurement
 * -------------------------------------------------------------------------- */
#define MC33772C_MEAS_VBG_DIAG_ADC1B_OFFSET                  (0x4AU)
#define MC33772C_MEAS_VBG_DIAG_ADC1B_POR_VAL                 (0x0U)

/* Field MEAS_VBG_DIAG_ADC1B (read-only): Value is unsigned, resolution is VCT_ANx_RES independently on the selected resolution of ADC_CFG. */
#define MC33772C_MEAS_VBG_DIAG_ADC1B_MEAS_VBG_DIAG_ADC1B_SHIFT (0x0U)
#define MC33772C_MEAS_VBG_DIAG_ADC1B_MEAS_VBG_DIAG_ADC1B_MASK (0x7FFFU)
#define MC33772C_MEAS_VBG_DIAG_ADC1B_MEAS_VBG_DIAG_ADC1B(x)  ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_VBG_DIAG_ADC1B_MEAS_VBG_DIAG_ADC1B_SHIFT) & MC33772C_MEAS_VBG_DIAG_ADC1B_MEAS_VBG_DIAG_ADC1B_MASK)

/* Field DATA_RDY (read-only): This bit is set when the conversion is complete and the register is updated. The DATA_RDY bit is cleared when a request to convert is received either through the SOC or GPIO2 convert trigger. */
#define MC33772C_MEAS_VBG_DIAG_ADC1B_DATA_RDY_SHIFT          (0xFU)
#define MC33772C_MEAS_VBG_DIAG_ADC1B_DATA_RDY_MASK           (0x8000U)
#define MC33772C_MEAS_VBG_DIAG_ADC1B_DATA_RDY(x)             ((uint16_t)((uint16_t)(x) << MC33772C_MEAS_VBG_DIAG_ADC1B_DATA_RDY_SHIFT) & MC33772C_MEAS_VBG_DIAG_ADC1B_DATA_RDY_MASK)

/* Enumerated value IN_PROGRESS: A new sequence of conversions is currently running. */
#define MC33772C_MEAS_VBG_DIAG_ADC1B_DATA_RDY_IN_PROGRESS_ENUM_VAL (0x0U)

/* Enumerated value DATA_RDY: A data is available. */
#define MC33772C_MEAS_VBG_DIAG_ADC1B_DATA_RDY_DATA_RDY_ENUM_VAL (0x1U)

/* --------------------------------------------------------------------------
 * TH_ALL_CT (read-write): Overvoltage and undervoltage threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_ALL_CT_OFFSET                            (0x4BU)
#define MC33772C_TH_ALL_CT_POR_VAL                           (0xD780U)

/* Field ALL_CT_UV_TH: Undervoltage threshold setting for all cell terminals. Enabled through register OV_UV_EN. */
#define MC33772C_TH_ALL_CT_ALL_CT_UV_TH_SHIFT                (0x0U)
#define MC33772C_TH_ALL_CT_ALL_CT_UV_TH_MASK                 (0xFFU)
#define MC33772C_TH_ALL_CT_ALL_CT_UV_TH(x)                   ((uint16_t)((uint16_t)(x) << MC33772C_TH_ALL_CT_ALL_CT_UV_TH_SHIFT) & MC33772C_TH_ALL_CT_ALL_CT_UV_TH_MASK)

/* Enumerated value DEFAULT: Default undervoltage threshold set to 2.5 V */
#define MC33772C_TH_ALL_CT_ALL_CT_UV_TH_DEFAULT_ENUM_VAL     (0x80U)

/* Field ALL_CT_OV_TH: Overvoltage threshold setting for all cell terminals. Enabled through register OV_UV_EN. */
#define MC33772C_TH_ALL_CT_ALL_CT_OV_TH_SHIFT                (0x8U)
#define MC33772C_TH_ALL_CT_ALL_CT_OV_TH_MASK                 (0xFF00U)
#define MC33772C_TH_ALL_CT_ALL_CT_OV_TH(x)                   ((uint16_t)((uint16_t)(x) << MC33772C_TH_ALL_CT_ALL_CT_OV_TH_SHIFT) & MC33772C_TH_ALL_CT_ALL_CT_OV_TH_MASK)

/* Enumerated value DEFAULT: Default overvoltage threshold set to 4.2 V */
#define MC33772C_TH_ALL_CT_ALL_CT_OV_TH_DEFAULT_ENUM_VAL     (0xD7U)

/* --------------------------------------------------------------------------
 * TH_CT6 (read-write): Overvoltage and undervoltage threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_CT6_OFFSET                               (0x54U)
#define MC33772C_TH_CT6_POR_VAL                              (0xD780U)

/* Field CT_UV_TH: Undervoltage threshold setting for individual cell terminals. OV_UV_EN[COMMON_UV_TH] bit must be logic 0 and OV_UV_EN[CTx_OVUV_EN] bit must be logic 1 to use TH_CTx register as threshold. */
#define MC33772C_TH_CT6_CT_UV_TH_SHIFT                       (0x0U)
#define MC33772C_TH_CT6_CT_UV_TH_MASK                        (0xFFU)
#define MC33772C_TH_CT6_CT_UV_TH(x)                          ((uint16_t)((uint16_t)(x) << MC33772C_TH_CT6_CT_UV_TH_SHIFT) & MC33772C_TH_CT6_CT_UV_TH_MASK)

/* Enumerated value DEFAULT: Default undervoltage threshold set to 2.5 V */
#define MC33772C_TH_CT6_CT_UV_TH_DEFAULT_ENUM_VAL            (0x80U)

/* Field CT_OV_TH: Overvoltage threshold setting for individual cell terminals. OV_UV_EN[COMMON_OV_TH] bit must be logic 0 and OV_UV_EN[CTx_OVUV_EN] bit must be logic 1 to use TH_CTx register as threshold. */
#define MC33772C_TH_CT6_CT_OV_TH_SHIFT                       (0x8U)
#define MC33772C_TH_CT6_CT_OV_TH_MASK                        (0xFF00U)
#define MC33772C_TH_CT6_CT_OV_TH(x)                          ((uint16_t)((uint16_t)(x) << MC33772C_TH_CT6_CT_OV_TH_SHIFT) & MC33772C_TH_CT6_CT_OV_TH_MASK)

/* Enumerated value DEFAULT: Default overvoltage threshold set to 4.2 V */
#define MC33772C_TH_CT6_CT_OV_TH_DEFAULT_ENUM_VAL            (0xD7U)

/* --------------------------------------------------------------------------
 * TH_CT5 (read-write): Overvoltage and undervoltage threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_CT5_OFFSET                               (0x55U)
#define MC33772C_TH_CT5_POR_VAL                              (0xD780U)

/* Field CT_UV_TH: Undervoltage threshold setting for individual cell terminals. OV_UV_EN[COMMON_UV_TH] bit must be logic 0 and OV_UV_EN[CTx_OVUV_EN] bit must be logic 1 to use TH_CTx register as threshold. */
#define MC33772C_TH_CT5_CT_UV_TH_SHIFT                       (0x0U)
#define MC33772C_TH_CT5_CT_UV_TH_MASK                        (0xFFU)
#define MC33772C_TH_CT5_CT_UV_TH(x)                          ((uint16_t)((uint16_t)(x) << MC33772C_TH_CT5_CT_UV_TH_SHIFT) & MC33772C_TH_CT5_CT_UV_TH_MASK)

/* Enumerated value DEFAULT: Default undervoltage threshold set to 2.5 V */
#define MC33772C_TH_CT5_CT_UV_TH_DEFAULT_ENUM_VAL            (0x80U)

/* Field CT_OV_TH: Overvoltage threshold setting for individual cell terminals. OV_UV_EN[COMMON_OV_TH] bit must be logic 0 and OV_UV_EN[CTx_OVUV_EN] bit must be logic 1 to use TH_CTx register as threshold. */
#define MC33772C_TH_CT5_CT_OV_TH_SHIFT                       (0x8U)
#define MC33772C_TH_CT5_CT_OV_TH_MASK                        (0xFF00U)
#define MC33772C_TH_CT5_CT_OV_TH(x)                          ((uint16_t)((uint16_t)(x) << MC33772C_TH_CT5_CT_OV_TH_SHIFT) & MC33772C_TH_CT5_CT_OV_TH_MASK)

/* Enumerated value DEFAULT: Default overvoltage threshold set to 4.2 V */
#define MC33772C_TH_CT5_CT_OV_TH_DEFAULT_ENUM_VAL            (0xD7U)

/* --------------------------------------------------------------------------
 * TH_CT4 (read-write): Overvoltage and undervoltage threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_CT4_OFFSET                               (0x56U)
#define MC33772C_TH_CT4_POR_VAL                              (0xD780U)

/* Field CT_UV_TH: Undervoltage threshold setting for individual cell terminals. OV_UV_EN[COMMON_UV_TH] bit must be logic 0 and OV_UV_EN[CTx_OVUV_EN] bit must be logic 1 to use TH_CTx register as threshold. */
#define MC33772C_TH_CT4_CT_UV_TH_SHIFT                       (0x0U)
#define MC33772C_TH_CT4_CT_UV_TH_MASK                        (0xFFU)
#define MC33772C_TH_CT4_CT_UV_TH(x)                          ((uint16_t)((uint16_t)(x) << MC33772C_TH_CT4_CT_UV_TH_SHIFT) & MC33772C_TH_CT4_CT_UV_TH_MASK)

/* Enumerated value DEFAULT: Default undervoltage threshold set to 2.5 V */
#define MC33772C_TH_CT4_CT_UV_TH_DEFAULT_ENUM_VAL            (0x80U)

/* Field CT_OV_TH: Overvoltage threshold setting for individual cell terminals. OV_UV_EN[COMMON_OV_TH] bit must be logic 0 and OV_UV_EN[CTx_OVUV_EN] bit must be logic 1 to use TH_CTx register as threshold. */
#define MC33772C_TH_CT4_CT_OV_TH_SHIFT                       (0x8U)
#define MC33772C_TH_CT4_CT_OV_TH_MASK                        (0xFF00U)
#define MC33772C_TH_CT4_CT_OV_TH(x)                          ((uint16_t)((uint16_t)(x) << MC33772C_TH_CT4_CT_OV_TH_SHIFT) & MC33772C_TH_CT4_CT_OV_TH_MASK)

/* Enumerated value DEFAULT: Default overvoltage threshold set to 4.2 V */
#define MC33772C_TH_CT4_CT_OV_TH_DEFAULT_ENUM_VAL            (0xD7U)

/* --------------------------------------------------------------------------
 * TH_CT3 (read-write): Overvoltage and undervoltage threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_CT3_OFFSET                               (0x57U)
#define MC33772C_TH_CT3_POR_VAL                              (0xD780U)

/* Field CT_UV_TH: Undervoltage threshold setting for individual cell terminals. OV_UV_EN[COMMON_UV_TH] bit must be logic 0 and OV_UV_EN[CTx_OVUV_EN] bit must be logic 1 to use TH_CTx register as threshold. */
#define MC33772C_TH_CT3_CT_UV_TH_SHIFT                       (0x0U)
#define MC33772C_TH_CT3_CT_UV_TH_MASK                        (0xFFU)
#define MC33772C_TH_CT3_CT_UV_TH(x)                          ((uint16_t)((uint16_t)(x) << MC33772C_TH_CT3_CT_UV_TH_SHIFT) & MC33772C_TH_CT3_CT_UV_TH_MASK)

/* Enumerated value DEFAULT: Default undervoltage threshold set to 2.5 V */
#define MC33772C_TH_CT3_CT_UV_TH_DEFAULT_ENUM_VAL            (0x80U)

/* Field CT_OV_TH: Overvoltage threshold setting for individual cell terminals. OV_UV_EN[COMMON_OV_TH] bit must be logic 0 and OV_UV_EN[CTx_OVUV_EN] bit must be logic 1 to use TH_CTx register as threshold. */
#define MC33772C_TH_CT3_CT_OV_TH_SHIFT                       (0x8U)
#define MC33772C_TH_CT3_CT_OV_TH_MASK                        (0xFF00U)
#define MC33772C_TH_CT3_CT_OV_TH(x)                          ((uint16_t)((uint16_t)(x) << MC33772C_TH_CT3_CT_OV_TH_SHIFT) & MC33772C_TH_CT3_CT_OV_TH_MASK)

/* Enumerated value DEFAULT: Default overvoltage threshold set to 4.2 V */
#define MC33772C_TH_CT3_CT_OV_TH_DEFAULT_ENUM_VAL            (0xD7U)

/* --------------------------------------------------------------------------
 * TH_CT2 (read-write): Overvoltage and undervoltage threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_CT2_OFFSET                               (0x58U)
#define MC33772C_TH_CT2_POR_VAL                              (0xD780U)

/* Field CT_UV_TH: Undervoltage threshold setting for individual cell terminals. OV_UV_EN[COMMON_UV_TH] bit must be logic 0 and OV_UV_EN[CTx_OVUV_EN] bit must be logic 1 to use TH_CTx register as threshold. */
#define MC33772C_TH_CT2_CT_UV_TH_SHIFT                       (0x0U)
#define MC33772C_TH_CT2_CT_UV_TH_MASK                        (0xFFU)
#define MC33772C_TH_CT2_CT_UV_TH(x)                          ((uint16_t)((uint16_t)(x) << MC33772C_TH_CT2_CT_UV_TH_SHIFT) & MC33772C_TH_CT2_CT_UV_TH_MASK)

/* Enumerated value DEFAULT: Default undervoltage threshold set to 2.5 V */
#define MC33772C_TH_CT2_CT_UV_TH_DEFAULT_ENUM_VAL            (0x80U)

/* Field CT_OV_TH: Overvoltage threshold setting for individual cell terminals. OV_UV_EN[COMMON_OV_TH] bit must be logic 0 and OV_UV_EN[CTx_OVUV_EN] bit must be logic 1 to use TH_CTx register as threshold. */
#define MC33772C_TH_CT2_CT_OV_TH_SHIFT                       (0x8U)
#define MC33772C_TH_CT2_CT_OV_TH_MASK                        (0xFF00U)
#define MC33772C_TH_CT2_CT_OV_TH(x)                          ((uint16_t)((uint16_t)(x) << MC33772C_TH_CT2_CT_OV_TH_SHIFT) & MC33772C_TH_CT2_CT_OV_TH_MASK)

/* Enumerated value DEFAULT: Default overvoltage threshold set to 4.2 V */
#define MC33772C_TH_CT2_CT_OV_TH_DEFAULT_ENUM_VAL            (0xD7U)

/* --------------------------------------------------------------------------
 * TH_CT1 (read-write): Overvoltage and undervoltage threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_CT1_OFFSET                               (0x59U)
#define MC33772C_TH_CT1_POR_VAL                              (0xD780U)

/* Field CT_UV_TH: Undervoltage threshold setting for individual cell terminals. OV_UV_EN[COMMON_UV_TH] bit must be logic 0 and OV_UV_EN[CTx_OVUV_EN] bit must be logic 1 to use TH_CTx register as threshold. */
#define MC33772C_TH_CT1_CT_UV_TH_SHIFT                       (0x0U)
#define MC33772C_TH_CT1_CT_UV_TH_MASK                        (0xFFU)
#define MC33772C_TH_CT1_CT_UV_TH(x)                          ((uint16_t)((uint16_t)(x) << MC33772C_TH_CT1_CT_UV_TH_SHIFT) & MC33772C_TH_CT1_CT_UV_TH_MASK)

/* Enumerated value DEFAULT: Default undervoltage threshold set to 2.5 V */
#define MC33772C_TH_CT1_CT_UV_TH_DEFAULT_ENUM_VAL            (0x80U)

/* Field CT_OV_TH: Overvoltage threshold setting for individual cell terminals. OV_UV_EN[COMMON_OV_TH] bit must be logic 0 and OV_UV_EN[CTx_OVUV_EN] bit must be logic 1 to use TH_CTx register as threshold. */
#define MC33772C_TH_CT1_CT_OV_TH_SHIFT                       (0x8U)
#define MC33772C_TH_CT1_CT_OV_TH_MASK                        (0xFF00U)
#define MC33772C_TH_CT1_CT_OV_TH(x)                          ((uint16_t)((uint16_t)(x) << MC33772C_TH_CT1_CT_OV_TH_SHIFT) & MC33772C_TH_CT1_CT_OV_TH_MASK)

/* Enumerated value DEFAULT: Default overvoltage threshold set to 4.2 V */
#define MC33772C_TH_CT1_CT_OV_TH_DEFAULT_ENUM_VAL            (0xD7U)

/* --------------------------------------------------------------------------
 * TH_AN6_OT (read-write): Overtemperature threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_AN6_OT_OFFSET                            (0x5AU)
#define MC33772C_TH_AN6_OT_POR_VAL                           (0xEDU)

/* Field AN_OT_TH: Overtemperature threshold setting for analog input. */
#define MC33772C_TH_AN6_OT_AN_OT_TH_SHIFT                    (0x0U)
#define MC33772C_TH_AN6_OT_AN_OT_TH_MASK                     (0x3FFU)
#define MC33772C_TH_AN6_OT_AN_OT_TH(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_TH_AN6_OT_AN_OT_TH_SHIFT) & MC33772C_TH_AN6_OT_AN_OT_TH_MASK)

/* Enumerated value DEFAULT: Overtemperature default set to 1.16 V */
#define MC33772C_TH_AN6_OT_AN_OT_TH_DEFAULT_ENUM_VAL         (0xEDU)

/* --------------------------------------------------------------------------
 * TH_AN5_OT (read-write): Overtemperature threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_AN5_OT_OFFSET                            (0x5BU)
#define MC33772C_TH_AN5_OT_POR_VAL                           (0xEDU)

/* Field AN_OT_TH: Overtemperature threshold setting for analog input. */
#define MC33772C_TH_AN5_OT_AN_OT_TH_SHIFT                    (0x0U)
#define MC33772C_TH_AN5_OT_AN_OT_TH_MASK                     (0x3FFU)
#define MC33772C_TH_AN5_OT_AN_OT_TH(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_TH_AN5_OT_AN_OT_TH_SHIFT) & MC33772C_TH_AN5_OT_AN_OT_TH_MASK)

/* Enumerated value DEFAULT: Overtemperature default set to 1.16 V */
#define MC33772C_TH_AN5_OT_AN_OT_TH_DEFAULT_ENUM_VAL         (0xEDU)

/* --------------------------------------------------------------------------
 * TH_AN4_OT (read-write): Overtemperature threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_AN4_OT_OFFSET                            (0x5CU)
#define MC33772C_TH_AN4_OT_POR_VAL                           (0xEDU)

/* Field AN_OT_TH: Overtemperature threshold setting for analog input. */
#define MC33772C_TH_AN4_OT_AN_OT_TH_SHIFT                    (0x0U)
#define MC33772C_TH_AN4_OT_AN_OT_TH_MASK                     (0x3FFU)
#define MC33772C_TH_AN4_OT_AN_OT_TH(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_TH_AN4_OT_AN_OT_TH_SHIFT) & MC33772C_TH_AN4_OT_AN_OT_TH_MASK)

/* Enumerated value DEFAULT: Overtemperature default set to 1.16 V */
#define MC33772C_TH_AN4_OT_AN_OT_TH_DEFAULT_ENUM_VAL         (0xEDU)

/* --------------------------------------------------------------------------
 * TH_AN3_OT (read-write): Overtemperature threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_AN3_OT_OFFSET                            (0x5DU)
#define MC33772C_TH_AN3_OT_POR_VAL                           (0xEDU)

/* Field AN_OT_TH: Overtemperature threshold setting for analog input. */
#define MC33772C_TH_AN3_OT_AN_OT_TH_SHIFT                    (0x0U)
#define MC33772C_TH_AN3_OT_AN_OT_TH_MASK                     (0x3FFU)
#define MC33772C_TH_AN3_OT_AN_OT_TH(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_TH_AN3_OT_AN_OT_TH_SHIFT) & MC33772C_TH_AN3_OT_AN_OT_TH_MASK)

/* Enumerated value DEFAULT: Overtemperature default set to 1.16 V */
#define MC33772C_TH_AN3_OT_AN_OT_TH_DEFAULT_ENUM_VAL         (0xEDU)

/* --------------------------------------------------------------------------
 * TH_AN2_OT (read-write): Overtemperature threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_AN2_OT_OFFSET                            (0x5EU)
#define MC33772C_TH_AN2_OT_POR_VAL                           (0xEDU)

/* Field AN_OT_TH: Overtemperature threshold setting for analog input. */
#define MC33772C_TH_AN2_OT_AN_OT_TH_SHIFT                    (0x0U)
#define MC33772C_TH_AN2_OT_AN_OT_TH_MASK                     (0x3FFU)
#define MC33772C_TH_AN2_OT_AN_OT_TH(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_TH_AN2_OT_AN_OT_TH_SHIFT) & MC33772C_TH_AN2_OT_AN_OT_TH_MASK)

/* Enumerated value DEFAULT: Overtemperature default set to 1.16 V */
#define MC33772C_TH_AN2_OT_AN_OT_TH_DEFAULT_ENUM_VAL         (0xEDU)

/* --------------------------------------------------------------------------
 * TH_AN1_OT (read-write): Overtemperature threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_AN1_OT_OFFSET                            (0x5FU)
#define MC33772C_TH_AN1_OT_POR_VAL                           (0xEDU)

/* Field AN_OT_TH: Overtemperature threshold setting for analog input. */
#define MC33772C_TH_AN1_OT_AN_OT_TH_SHIFT                    (0x0U)
#define MC33772C_TH_AN1_OT_AN_OT_TH_MASK                     (0x3FFU)
#define MC33772C_TH_AN1_OT_AN_OT_TH(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_TH_AN1_OT_AN_OT_TH_SHIFT) & MC33772C_TH_AN1_OT_AN_OT_TH_MASK)

/* Enumerated value DEFAULT: Overtemperature default set to 1.16 V */
#define MC33772C_TH_AN1_OT_AN_OT_TH_DEFAULT_ENUM_VAL         (0xEDU)

/* --------------------------------------------------------------------------
 * TH_AN0_OT (read-write): Overtemperature threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_AN0_OT_OFFSET                            (0x60U)
#define MC33772C_TH_AN0_OT_POR_VAL                           (0xEDU)

/* Field AN_OT_TH: Overtemperature threshold setting for analog input. */
#define MC33772C_TH_AN0_OT_AN_OT_TH_SHIFT                    (0x0U)
#define MC33772C_TH_AN0_OT_AN_OT_TH_MASK                     (0x3FFU)
#define MC33772C_TH_AN0_OT_AN_OT_TH(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_TH_AN0_OT_AN_OT_TH_SHIFT) & MC33772C_TH_AN0_OT_AN_OT_TH_MASK)

/* Enumerated value DEFAULT: Overtemperature default set to 1.16 V */
#define MC33772C_TH_AN0_OT_AN_OT_TH_DEFAULT_ENUM_VAL         (0xEDU)

/* --------------------------------------------------------------------------
 * TH_AN6_UT (read-write): Undertemperature threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_AN6_UT_OFFSET                            (0x61U)
#define MC33772C_TH_AN6_UT_POR_VAL                           (0x30EU)

/* Field AN_UT_TH: Undertemperature threshold setting for analog input. */
#define MC33772C_TH_AN6_UT_AN_UT_TH_SHIFT                    (0x0U)
#define MC33772C_TH_AN6_UT_AN_UT_TH_MASK                     (0x3FFU)
#define MC33772C_TH_AN6_UT_AN_UT_TH(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_TH_AN6_UT_AN_UT_TH_SHIFT) & MC33772C_TH_AN6_UT_AN_UT_TH_MASK)

/* Enumerated value DEFAULT: Undertemperature default set to 3.82 V */
#define MC33772C_TH_AN6_UT_AN_UT_TH_DEFAULT_ENUM_VAL         (0x30EU)

/* --------------------------------------------------------------------------
 * TH_AN5_UT (read-write): Undertemperature threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_AN5_UT_OFFSET                            (0x62U)
#define MC33772C_TH_AN5_UT_POR_VAL                           (0x30EU)

/* Field AN_UT_TH: Undertemperature threshold setting for analog input. */
#define MC33772C_TH_AN5_UT_AN_UT_TH_SHIFT                    (0x0U)
#define MC33772C_TH_AN5_UT_AN_UT_TH_MASK                     (0x3FFU)
#define MC33772C_TH_AN5_UT_AN_UT_TH(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_TH_AN5_UT_AN_UT_TH_SHIFT) & MC33772C_TH_AN5_UT_AN_UT_TH_MASK)

/* Enumerated value DEFAULT: Undertemperature default set to 3.82 V */
#define MC33772C_TH_AN5_UT_AN_UT_TH_DEFAULT_ENUM_VAL         (0x30EU)

/* --------------------------------------------------------------------------
 * TH_AN4_UT (read-write): Undertemperature threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_AN4_UT_OFFSET                            (0x63U)
#define MC33772C_TH_AN4_UT_POR_VAL                           (0x30EU)

/* Field AN_UT_TH: Undertemperature threshold setting for analog input. */
#define MC33772C_TH_AN4_UT_AN_UT_TH_SHIFT                    (0x0U)
#define MC33772C_TH_AN4_UT_AN_UT_TH_MASK                     (0x3FFU)
#define MC33772C_TH_AN4_UT_AN_UT_TH(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_TH_AN4_UT_AN_UT_TH_SHIFT) & MC33772C_TH_AN4_UT_AN_UT_TH_MASK)

/* Enumerated value DEFAULT: Undertemperature default set to 3.82 V */
#define MC33772C_TH_AN4_UT_AN_UT_TH_DEFAULT_ENUM_VAL         (0x30EU)

/* --------------------------------------------------------------------------
 * TH_AN3_UT (read-write): Undertemperature threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_AN3_UT_OFFSET                            (0x64U)
#define MC33772C_TH_AN3_UT_POR_VAL                           (0x30EU)

/* Field AN_UT_TH: Undertemperature threshold setting for analog input. */
#define MC33772C_TH_AN3_UT_AN_UT_TH_SHIFT                    (0x0U)
#define MC33772C_TH_AN3_UT_AN_UT_TH_MASK                     (0x3FFU)
#define MC33772C_TH_AN3_UT_AN_UT_TH(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_TH_AN3_UT_AN_UT_TH_SHIFT) & MC33772C_TH_AN3_UT_AN_UT_TH_MASK)

/* Enumerated value DEFAULT: Undertemperature default set to 3.82 V */
#define MC33772C_TH_AN3_UT_AN_UT_TH_DEFAULT_ENUM_VAL         (0x30EU)

/* --------------------------------------------------------------------------
 * TH_AN2_UT (read-write): Undertemperature threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_AN2_UT_OFFSET                            (0x65U)
#define MC33772C_TH_AN2_UT_POR_VAL                           (0x30EU)

/* Field AN_UT_TH: Undertemperature threshold setting for analog input. */
#define MC33772C_TH_AN2_UT_AN_UT_TH_SHIFT                    (0x0U)
#define MC33772C_TH_AN2_UT_AN_UT_TH_MASK                     (0x3FFU)
#define MC33772C_TH_AN2_UT_AN_UT_TH(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_TH_AN2_UT_AN_UT_TH_SHIFT) & MC33772C_TH_AN2_UT_AN_UT_TH_MASK)

/* Enumerated value DEFAULT: Undertemperature default set to 3.82 V */
#define MC33772C_TH_AN2_UT_AN_UT_TH_DEFAULT_ENUM_VAL         (0x30EU)

/* --------------------------------------------------------------------------
 * TH_AN1_UT (read-write): Undertemperature threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_AN1_UT_OFFSET                            (0x66U)
#define MC33772C_TH_AN1_UT_POR_VAL                           (0x30EU)

/* Field AN_UT_TH: Undertemperature threshold setting for analog input. */
#define MC33772C_TH_AN1_UT_AN_UT_TH_SHIFT                    (0x0U)
#define MC33772C_TH_AN1_UT_AN_UT_TH_MASK                     (0x3FFU)
#define MC33772C_TH_AN1_UT_AN_UT_TH(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_TH_AN1_UT_AN_UT_TH_SHIFT) & MC33772C_TH_AN1_UT_AN_UT_TH_MASK)

/* Enumerated value DEFAULT: Undertemperature default set to 3.82 V */
#define MC33772C_TH_AN1_UT_AN_UT_TH_DEFAULT_ENUM_VAL         (0x30EU)

/* --------------------------------------------------------------------------
 * TH_AN0_UT (read-write): Undertemperature threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_AN0_UT_OFFSET                            (0x67U)
#define MC33772C_TH_AN0_UT_POR_VAL                           (0x30EU)

/* Field AN_UT_TH: Undertemperature threshold setting for analog input. */
#define MC33772C_TH_AN0_UT_AN_UT_TH_SHIFT                    (0x0U)
#define MC33772C_TH_AN0_UT_AN_UT_TH_MASK                     (0x3FFU)
#define MC33772C_TH_AN0_UT_AN_UT_TH(x)                       ((uint16_t)((uint16_t)(x) << MC33772C_TH_AN0_UT_AN_UT_TH_SHIFT) & MC33772C_TH_AN0_UT_AN_UT_TH_MASK)

/* Enumerated value DEFAULT: Undertemperature default set to 3.82 V */
#define MC33772C_TH_AN0_UT_AN_UT_TH_DEFAULT_ENUM_VAL         (0x30EU)

/* --------------------------------------------------------------------------
 * TH_ISENSE_OC (read-write): Overcurrent threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_ISENSE_OC_OFFSET                         (0x68U)
#define MC33772C_TH_ISENSE_OC_POR_VAL                        (0x0U)

/* Field TH_ISENSE_OC: Sleep mode ISENSE overcurrent threshold, unsigned. Resolution is 1.2 uV/LSB. */
#define MC33772C_TH_ISENSE_OC_TH_ISENSE_OC_SHIFT             (0x0U)
#define MC33772C_TH_ISENSE_OC_TH_ISENSE_OC_MASK              (0xFFFU)
#define MC33772C_TH_ISENSE_OC_TH_ISENSE_OC(x)                ((uint16_t)((uint16_t)(x) << MC33772C_TH_ISENSE_OC_TH_ISENSE_OC_SHIFT) & MC33772C_TH_ISENSE_OC_TH_ISENSE_OC_MASK)

/* --------------------------------------------------------------------------
 * TH_COULOMB_CNT_MSB (read-write): Over coulomb counter threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_COULOMB_CNT_MSB_OFFSET                   (0x69U)
#define MC33772C_TH_COULOMB_CNT_MSB_POR_VAL                  (0x0U)

/* Field TH_COULOMB_CNT_MSB: Over coulomb counting accumulator threshold (MSB). */
#define MC33772C_TH_COULOMB_CNT_MSB_TH_COULOMB_CNT_MSB_SHIFT (0x0U)
#define MC33772C_TH_COULOMB_CNT_MSB_TH_COULOMB_CNT_MSB_MASK  (0xFFFFU)
#define MC33772C_TH_COULOMB_CNT_MSB_TH_COULOMB_CNT_MSB(x)    ((uint16_t)((uint16_t)(x) << MC33772C_TH_COULOMB_CNT_MSB_TH_COULOMB_CNT_MSB_SHIFT) & MC33772C_TH_COULOMB_CNT_MSB_TH_COULOMB_CNT_MSB_MASK)

/* --------------------------------------------------------------------------
 * TH_COULOMB_CNT_LSB (read-write): Over coulomb counter threshold register.
 * -------------------------------------------------------------------------- */
#define MC33772C_TH_COULOMB_CNT_LSB_OFFSET                   (0x6AU)
#define MC33772C_TH_COULOMB_CNT_LSB_POR_VAL                  (0x0U)

/* Field TH_COULOMB_CNT_LSB: Over coulomb counting accumulator threshold (LSB). */
#define MC33772C_TH_COULOMB_CNT_LSB_TH_COULOMB_CNT_LSB_SHIFT (0x0U)
#define MC33772C_TH_COULOMB_CNT_LSB_TH_COULOMB_CNT_LSB_MASK  (0xFFFFU)
#define MC33772C_TH_COULOMB_CNT_LSB_TH_COULOMB_CNT_LSB(x)    ((uint16_t)((uint16_t)(x) << MC33772C_TH_COULOMB_CNT_LSB_TH_COULOMB_CNT_LSB_SHIFT) & MC33772C_TH_COULOMB_CNT_LSB_TH_COULOMB_CNT_LSB_MASK)

/* --------------------------------------------------------------------------
 * SILICON_REV (read-only): Silicon revision register.
 * -------------------------------------------------------------------------- */
#define MC33772C_SILICON_REV_OFFSET                          (0x6BU)
#define MC33772C_SILICON_REV_POR_VAL                         (0x0U)

/* Field MREV (read-only): Metal mask revision. */
#define MC33772C_SILICON_REV_MREV_SHIFT                      (0x0U)
#define MC33772C_SILICON_REV_MREV_MASK                       (0x7U)
#define MC33772C_SILICON_REV_MREV(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_SILICON_REV_MREV_SHIFT) & MC33772C_SILICON_REV_MREV_MASK)

/* Field FREV (read-only): Full mask revision. */
#define MC33772C_SILICON_REV_FREV_SHIFT                      (0x3U)
#define MC33772C_SILICON_REV_FREV_MASK                       (0x38U)
#define MC33772C_SILICON_REV_FREV(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_SILICON_REV_FREV_SHIFT) & MC33772C_SILICON_REV_FREV_MASK)

/* --------------------------------------------------------------------------
 * EEPROM_CTRL (read-write): EEPROM communication register.
 * -------------------------------------------------------------------------- */
#define MC33772C_EEPROM_CTRL_OFFSET                          (0x6CU)
#define MC33772C_EEPROM_CTRL_POR_VAL                         (0x0U)

/* Field READ_DATA (read-only): Data read in the EEPROM at address given by EEPROM_ADD. */
#define MC33772C_EEPROM_CTRL_READ_DATA_SHIFT                 (0x0U)
#define MC33772C_EEPROM_CTRL_READ_DATA_MASK                  (0xFFU)
#define MC33772C_EEPROM_CTRL_READ_DATA(x)                    ((uint16_t)((uint16_t)(x) << MC33772C_EEPROM_CTRL_READ_DATA_SHIFT) & MC33772C_EEPROM_CTRL_READ_DATA_MASK)

/* Field DATA_TO_WRITE (write-only): Data to be written into the EEPROM. */
#define MC33772C_EEPROM_CTRL_DATA_TO_WRITE_SHIFT             (0x0U)
#define MC33772C_EEPROM_CTRL_DATA_TO_WRITE_MASK              (0xFFU)
#define MC33772C_EEPROM_CTRL_DATA_TO_WRITE(x)                ((uint16_t)((uint16_t)(x) << MC33772C_EEPROM_CTRL_DATA_TO_WRITE_SHIFT) & MC33772C_EEPROM_CTRL_DATA_TO_WRITE_MASK)

/* Field EEPROM_ADD (write-only): EEPROM address to read or write. */
#define MC33772C_EEPROM_CTRL_EEPROM_ADD_SHIFT                (0x8U)
#define MC33772C_EEPROM_CTRL_EEPROM_ADD_MASK                 (0x7F00U)
#define MC33772C_EEPROM_CTRL_EEPROM_ADD(x)                   ((uint16_t)((uint16_t)(x) << MC33772C_EEPROM_CTRL_EEPROM_ADD_SHIFT) & MC33772C_EEPROM_CTRL_EEPROM_ADD_MASK)

/* Field EE_PRESENT (read-only): EEPROM detection. */
#define MC33772C_EEPROM_CTRL_EE_PRESENT_SHIFT                (0xDU)
#define MC33772C_EEPROM_CTRL_EE_PRESENT_MASK                 (0x2000U)
#define MC33772C_EEPROM_CTRL_EE_PRESENT(x)                   ((uint16_t)((uint16_t)(x) << MC33772C_EEPROM_CTRL_EE_PRESENT_SHIFT) & MC33772C_EEPROM_CTRL_EE_PRESENT_MASK)

/* Enumerated value NOT_DETECTED: No EEPROM detected. */
#define MC33772C_EEPROM_CTRL_EE_PRESENT_NOT_DETECTED_ENUM_VAL (0x0U)

/* Enumerated value DETECTED: EEPROM has been detected and present. */
#define MC33772C_EEPROM_CTRL_EE_PRESENT_DETECTED_ENUM_VAL    (0x1U)

/* Field ERROR (read-only): EEPROM communication error bit.. */
#define MC33772C_EEPROM_CTRL_ERROR_SHIFT                     (0xEU)
#define MC33772C_EEPROM_CTRL_ERROR_MASK                      (0x4000U)
#define MC33772C_EEPROM_CTRL_ERROR(x)                        ((uint16_t)((uint16_t)(x) << MC33772C_EEPROM_CTRL_ERROR_SHIFT) & MC33772C_EEPROM_CTRL_ERROR_MASK)

/* Enumerated value NO_ERROR: No error occurred during the communication to EEPROM. */
#define MC33772C_EEPROM_CTRL_ERROR_NO_ERROR_ENUM_VAL         (0x0U)

/* Enumerated value ERROR: An error occurred during the communication to EEPROM. */
#define MC33772C_EEPROM_CTRL_ERROR_ERROR_ENUM_VAL            (0x1U)

/* Field BUSY (read-only): Busy bit. */
#define MC33772C_EEPROM_CTRL_BUSY_SHIFT                      (0xFU)
#define MC33772C_EEPROM_CTRL_BUSY_MASK                       (0x8000U)
#define MC33772C_EEPROM_CTRL_BUSY(x)                         ((uint16_t)((uint16_t)(x) << MC33772C_EEPROM_CTRL_BUSY_SHIFT) & MC33772C_EEPROM_CTRL_BUSY_MASK)

/* Enumerated value COMPLETED: Indicates the IC has completed the EEPROM read or write operation. */
#define MC33772C_EEPROM_CTRL_BUSY_COMPLETED_ENUM_VAL         (0x0U)

/* Enumerated value IN_PROGRESS: Indicates the IC is in the process of performing the EEPROM read or write operation. */
#define MC33772C_EEPROM_CTRL_BUSY_IN_PROGRESS_ENUM_VAL       (0x1U)

/* Field R_W (write-only): Read/write bit, directs the 33771 to read or write from EEPROM. */
#define MC33772C_EEPROM_CTRL_R_W_SHIFT                       (0xFU)
#define MC33772C_EEPROM_CTRL_R_W_MASK                        (0x8000U)
#define MC33772C_EEPROM_CTRL_R_W(x)                          ((uint16_t)((uint16_t)(x) << MC33772C_EEPROM_CTRL_R_W_SHIFT) & MC33772C_EEPROM_CTRL_R_W_MASK)

/* Enumerated value WRITE: Write. */
#define MC33772C_EEPROM_CTRL_R_W_WRITE_ENUM_VAL              (0x0U)

/* Enumerated value READ: Read. */
#define MC33772C_EEPROM_CTRL_R_W_READ_ENUM_VAL               (0x1U)

/* --------------------------------------------------------------------------
 * DED_ENCODE1 (read-only): ECC signature 1 register.
 * -------------------------------------------------------------------------- */
#define MC33772C_DED_ENCODE1_OFFSET                          (0x6DU)
#define MC33772C_DED_ENCODE1_POR_VAL                         (0x0U)

/* Field DED_HAMMING_COUT1_23_8 (read-only): Reports the 16 MSBits to encode in the fuse matrix (ECC). */
#define MC33772C_DED_ENCODE1_DED_HAMMING_COUT1_23_8_SHIFT    (0x0U)
#define MC33772C_DED_ENCODE1_DED_HAMMING_COUT1_23_8_MASK     (0xFFFFU)
#define MC33772C_DED_ENCODE1_DED_HAMMING_COUT1_23_8(x)       ((uint16_t)((uint16_t)(x) << MC33772C_DED_ENCODE1_DED_HAMMING_COUT1_23_8_SHIFT) & MC33772C_DED_ENCODE1_DED_HAMMING_COUT1_23_8_MASK)

/* --------------------------------------------------------------------------
 * DED_ENCODE2 (read-only): ECC signature 2 register.
 * -------------------------------------------------------------------------- */
#define MC33772C_DED_ENCODE2_OFFSET                          (0x6EU)
#define MC33772C_DED_ENCODE2_POR_VAL                         (0x0U)

/* Field DED_HAMMING_COUT_1_7_0 (read-only): Report the 8 LSBits to encode in the fuse matrix (ECC). */
#define MC33772C_DED_ENCODE2_DED_HAMMING_COUT_1_7_0_SHIFT    (0x8U)
#define MC33772C_DED_ENCODE2_DED_HAMMING_COUT_1_7_0_MASK     (0xFF00U)
#define MC33772C_DED_ENCODE2_DED_HAMMING_COUT_1_7_0(x)       ((uint16_t)((uint16_t)(x) << MC33772C_DED_ENCODE2_DED_HAMMING_COUT_1_7_0_SHIFT) & MC33772C_DED_ENCODE2_DED_HAMMING_COUT_1_7_0_MASK)

/* --------------------------------------------------------------------------
 * FUSE_MIRROR_DATA (read-write): FUSE mirror and data control.
 * -------------------------------------------------------------------------- */
#define MC33772C_FUSE_MIRROR_DATA_OFFSET                     (0x6FU)
#define MC33772C_FUSE_MIRROR_DATA_POR_VAL                    (0x0U)

/* Field FMR_DATA: Fuse mirror data to read or write. */
#define MC33772C_FUSE_MIRROR_DATA_FMR_DATA_SHIFT             (0x0U)
#define MC33772C_FUSE_MIRROR_DATA_FMR_DATA_MASK              (0xFFFFU)
#define MC33772C_FUSE_MIRROR_DATA_FMR_DATA(x)                ((uint16_t)((uint16_t)(x) << MC33772C_FUSE_MIRROR_DATA_FMR_DATA_SHIFT) & MC33772C_FUSE_MIRROR_DATA_FMR_DATA_MASK)

/* --------------------------------------------------------------------------
 * FUSE_MIRROR_CNTL (read-write): FUSE mirror and data control
 * -------------------------------------------------------------------------- */
#define MC33772C_FUSE_MIRROR_CNTL_OFFSET                     (0x70U)
#define MC33772C_FUSE_MIRROR_CNTL_POR_VAL                    (0x0U)

/* Field FST_ST (read-only): Fuse state control. Read in this register enables to trace the current state. */
#define MC33772C_FUSE_MIRROR_CNTL_FST_ST_SHIFT               (0x0U)
#define MC33772C_FUSE_MIRROR_CNTL_FST_ST_MASK                (0x7U)
#define MC33772C_FUSE_MIRROR_CNTL_FST_ST(x)                  ((uint16_t)((uint16_t)(x) << MC33772C_FUSE_MIRROR_CNTL_FST_ST_SHIFT) & MC33772C_FUSE_MIRROR_CNTL_FST_ST_MASK)

/* Field FST (write-only): Fuse state control. write to this register controls the switching of the fuse state machine. Read in this register enables tracing the current state. */
#define MC33772C_FUSE_MIRROR_CNTL_FST_SHIFT                  (0x0U)
#define MC33772C_FUSE_MIRROR_CNTL_FST_MASK                   (0x7U)
#define MC33772C_FUSE_MIRROR_CNTL_FST(x)                     ((uint16_t)((uint16_t)(x) << MC33772C_FUSE_MIRROR_CNTL_FST_SHIFT) & MC33772C_FUSE_MIRROR_CNTL_FST_MASK)

/* Enumerated value SPI_WRITE_ENABLE: Enable SPI write. */
#define MC33772C_FUSE_MIRROR_CNTL_FST_SPI_WRITE_ENABLE_ENUM_VAL (0x0U)

/* Enumerated value LP: FUSE_MIRROR_CNTL to low-power. */
#define MC33772C_FUSE_MIRROR_CNTL_FST_LP_ENUM_VAL            (0x4U)

/* Field FSTM (write-only): Fuse state write mask. This bit controls the write access to the FST[2:0] bits. */
#define MC33772C_FUSE_MIRROR_CNTL_FSTM_SHIFT                 (0x3U)
#define MC33772C_FUSE_MIRROR_CNTL_FSTM_MASK                  (0x8U)
#define MC33772C_FUSE_MIRROR_CNTL_FSTM(x)                    ((uint16_t)((uint16_t)(x) << MC33772C_FUSE_MIRROR_CNTL_FSTM_SHIFT) & MC33772C_FUSE_MIRROR_CNTL_FSTM_MASK)

/* Enumerated value LOCKED: Writing in FST bits has no effect. */
#define MC33772C_FUSE_MIRROR_CNTL_FSTM_LOCKED_ENUM_VAL       (0x0U)

/* Enumerated value UNLOCKED: FST bits are unlocked for writing. */
#define MC33772C_FUSE_MIRROR_CNTL_FSTM_UNLOCKED_ENUM_VAL     (0x1U)

/* Field FMR_ADDR: Fuse mirror register address. */
#define MC33772C_FUSE_MIRROR_CNTL_FMR_ADDR_SHIFT             (0x8U)
#define MC33772C_FUSE_MIRROR_CNTL_FMR_ADDR_MASK              (0x1F00U)
#define MC33772C_FUSE_MIRROR_CNTL_FMR_ADDR(x)                ((uint16_t)((uint16_t)(x) << MC33772C_FUSE_MIRROR_CNTL_FMR_ADDR_SHIFT) & MC33772C_FUSE_MIRROR_CNTL_FMR_ADDR_MASK)

/* Field SEC_ERR_FLT (read-only): ECC error, single error correction. */
#define MC33772C_FUSE_MIRROR_CNTL_SEC_ERR_FLT_SHIFT          (0xFU)
#define MC33772C_FUSE_MIRROR_CNTL_SEC_ERR_FLT_MASK           (0x8000U)
#define MC33772C_FUSE_MIRROR_CNTL_SEC_ERR_FLT(x)             ((uint16_t)((uint16_t)(x) << MC33772C_FUSE_MIRROR_CNTL_SEC_ERR_FLT_SHIFT) & MC33772C_FUSE_MIRROR_CNTL_SEC_ERR_FLT_MASK)

/* Enumerated value NO_ERROR: No error. */
#define MC33772C_FUSE_MIRROR_CNTL_SEC_ERR_FLT_NO_ERROR_ENUM_VAL (0x0U)

/* Enumerated value ERROR: A single error has been detected and corrected. The IC is usable, must not be considered defective. */
#define MC33772C_FUSE_MIRROR_CNTL_SEC_ERR_FLT_ERROR_ENUM_VAL (0x1U)



#ifdef __cplusplus
}
#endif

#endif
