#ifndef __SPL06_H
#define __SPL06_H

#include "stm32f4xx_hal.h"  // 使用HAL库头文件

// I2C句柄声明 - 需要在主程序中定义并初始化
extern I2C_HandleTypeDef hi2c1;  // 假设使用I2C1，可根据实际情况修改

#define SPL06_IIC_ADDR  0x77 // SPL06的I2C地址
#define SPL06_IIC_ADDR_WRITE  (SPL06_IIC_ADDR << 1)  // 写地址
#define SPL06_IIC_ADDR_READ   ((SPL06_IIC_ADDR << 1) | 0x01)  // 读地址

// 压力测量速率(sample/sec),Background模式使用
#define  PM_RATE_1          (0<<4)      // 1 measurements pr. sec.
#define  PM_RATE_2          (1<<4)      // 2 measurements pr. sec.
#define  PM_RATE_4          (2<<4)      // 4 measurements pr. sec.            
#define  PM_RATE_8          (3<<4)      // 8 measurements pr. sec.
#define  PM_RATE_16         (4<<4)      // 16 measurements pr. sec.
#define  PM_RATE_32         (5<<4)      // 32 measurements pr. sec.
#define  PM_RATE_64         (6<<4)      // 64 measurements pr. sec.
#define  PM_RATE_128        (7<<4)      // 128 measurements pr. sec.

// 压力过采样次数(times),Background模式使用
#define PM_PRC_1            0       // Sigle         kP=524288   ,3.6ms
#define PM_PRC_2            1       // 2 times       kP=1572864  ,5.2ms
#define PM_PRC_4            2       // 4 times       kP=3670016  ,8.4ms
#define PM_PRC_8            3       // 8 times       kP=7864320  ,14.8ms
#define PM_PRC_16           4       // 16 times      kP=253952   ,27.6ms
#define PM_PRC_32           5       // 32 times      kP=516096   ,53.2ms
#define PM_PRC_64           6       // 64 times      kP=1040384  ,104.4ms
#define PM_PRC_128          7       // 128 times     kP=2088960  ,206.8ms

// 温度测量速率(sample/sec),Background模式使用
#define  TMP_RATE_1         (0<<4)      // 1 measurements pr. sec.
#define  TMP_RATE_2         (1<<4)      // 2 measurements pr. sec.
#define  TMP_RATE_4         (2<<4)      // 4 measurements pr. sec.            
#define  TMP_RATE_8         (3<<4)      // 8 measurements pr. sec.
#define  TMP_RATE_16        (4<<4)      // 16 measurements pr. sec.
#define  TMP_RATE_32        (5<<4)      // 32 measurements pr. sec.
#define  TMP_RATE_64        (6<<4)      // 64 measurements pr. sec.
#define  TMP_RATE_128       (7<<4)      // 128 measurements pr. sec.

// 温度过采样次数(times),Background模式使用
#define TMP_PRC_1           0       // Sigle
#define TMP_PRC_2           1       // 2 times
#define TMP_PRC_4           2       // 4 times
#define TMP_PRC_8           3       // 8 times
#define TMP_PRC_16          4       // 16 times
#define TMP_PRC_32          5       // 32 times
#define TMP_PRC_64          6       // 64 times
#define TMP_PRC_128         7       // 128 times

// SPL06_MEAS_CFG
#define MEAS_COEF_RDY       0x80
#define MEAS_SENSOR_RDY     0x40        // 传感器准备好信号
#define MEAS_TMP_RDY        0x20        // 新的温度数据
#define MEAS_PRS_RDY        0x10        // 新的压力数据

#define MEAS_CTRL_Standby               0x00    // 待机模式
#define MEAS_CTRL_PressMeasure          0x01    // 单次压力测量
#define MEAS_CTRL_TempMeasure           0x02    // 单次温度测量
#define MEAS_CTRL_ContinuousPress       0x05    // 连续压力测量
#define MEAS_CTRL_ContinuousTemp        0x06    // 连续温度测量
#define MEAS_CTRL_ContinuousPressTemp   0x07    // 连续压力温度测量

// FIFO_STS
#define SPL06_FIFO_FULL     0x02
#define SPL06_FIFO_EMPTY    0x01

// INT_STS
#define SPL06_INT_FIFO_FULL     0x04
#define SPL06_INT_TMP           0x02
#define SPL06_INT_PRS           0x01

// CFG_REG
#define SPL06_CFG_T_SHIFT   0x08    // oversampling times>8时必须使用
#define SPL06_CFG_P_SHIFT   0x04

// 寄存器地址定义
#define SP06_PSR_B2     0x00        // 压力值
#define SP06_PSR_B1     0x01
#define SP06_PSR_B0     0x02
#define SP06_TMP_B2     0x03        // 温度值
#define SP06_TMP_B1     0x04
#define SP06_TMP_B0     0x05

#define SP06_PSR_CFG    0x06        // 压力测量配置
#define SP06_TMP_CFG    0x07        // 温度测量配置
#define SP06_MEAS_CFG   0x08        // 测量模式配置

#define SP06_CFG_REG    0x09
#define SP06_INT_STS    0x0A
#define SP06_FIFO_STS   0x0B

#define SP06_RESET      0x0C
#define SP06_ID         0x0D

#define SP06_COEF       0x10        // -0x21
#define SP06_COEF_SRCE  0x28

// 全局变量声明
extern float temp, press;

// 函数声明
uint8_t spl06_init(void);
void spl06_start(uint8_t mode);
void spl06_config_temperature(uint8_t rate, uint8_t oversampling);
void spl06_config_pressure(uint8_t rate, uint8_t oversampling);
int32_t spl06_get_pressure_adc(void);
int32_t spl06_get_temperature_adc(void);
void spl06_update_pressure(void);
void spl06_update(void);
float spl06_get_temperature(void);
float spl06_get_pressure(void);
uint8_t spl06_write_reg(uint8_t reg_addr, uint8_t reg_val);
uint8_t spl06_read_reg(uint8_t reg_addr, uint8_t *buf);
uint8_t spl06_read_buffer(uint8_t reg_addr, void *buffer, uint16_t len);

#endif /* __SPL06_H */