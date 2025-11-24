#include "spl06.h"

// 校准系数
int16_t C0;
int16_t C1;
int32_t C00;
int32_t C10;
int16_t C01;
int16_t C11;
int16_t C20;
int16_t C21;
int16_t C30;

// 原始数据
float raw_temp, raw_press;

// 缩放系数
float _kT, _kP;

// 最终温度和压力值
float temp, press;

/**
 * @brief  初始化SPL06传感器
 * @retval 成功返回0，失败返回1
 */
uint8_t spl06_init(void)
{
    uint8_t coef[18];
    uint8_t id;

    // 复位传感器
    if(spl06_write_reg(SP06_RESET, 0x89))
    {
        printf("spl06 reset fail\r\n");
        return 1;
    }
    
    // 读取传感器ID
    spl06_read_reg(SP06_ID, &id);
    
    // 检查ID是否正确
    if(id != 0x1D)
    {
        while(1)
        {
            // 可以在这里添加错误处理，比如点亮错误指示灯
            HAL_Delay(100); // 延时100ms
        }
    }

    HAL_Delay(100);  // 复位后系统准备需要等待40ms

    // 读取校准系数
    spl06_read_buffer(SP06_COEF, coef, 18);
    
    // 计算校准系数
    C0 = ((int16_t)coef[0] << 4) + ((coef[1] & 0xF0) >> 4);
    C0 = (C0 & 0x0800) ? (0xF000 | C0) : C0;
    
    C1 = ((int16_t)(coef[1] & 0x0F) << 8) + coef[2];
    C1 = (C1 & 0x0800) ? (0xF000 | C1) : C1;
    
    C00 = ((int32_t)coef[3] << 12) + ((uint32_t)coef[4] << 4) + (coef[5] >> 4);
    C10 = ((int32_t)(coef[5] & 0x0F) << 16) + ((uint32_t)coef[6] << 8) + coef[7];
    
    C00 = (C00 & 0x080000) ? (0xFFF00000 | C00) : C00;
    C10 = (C10 & 0x080000) ? (0xFFF00000 | C10) : C10;
    
    C01 = ((int16_t)coef[8] << 8) + coef[9];
    C11 = ((int16_t)coef[10] << 8) + coef[11];
    C11 = (C11 & 0x0800) ? (0xF000 | C11) : C11;
    
    C20 = ((int16_t)coef[12] << 8) + coef[13];
    C20 = (C20 & 0x0800) ? (0xF000 | C20) : C20;
    
    C21 = ((int16_t)coef[14] << 8) + coef[15];
    C21 = (C21 & 0x0800) ? (0xF000 | C21) : C21;
    
    C30 = ((int16_t)coef[16] << 8) + coef[17];
    C30 = (C30 & 0x0800) ? (0xF000 | C30) : C30;

    // 配置压力测量
    spl06_config_pressure(PM_RATE_128, PM_PRC_64);
    
    // 配置温度测量
    spl06_config_temperature(PM_RATE_8, TMP_PRC_8);

    // 开始连续测量压力和温度
    spl06_start(MEAS_CTRL_ContinuousPressTemp);
    
    HAL_Delay(20);  // 等待传感器稳定

    return 0;
}

/**
 * @brief  设置传感器工作模式
 * @param  mode: 测量模式
 */
void spl06_start(uint8_t mode)
{
    spl06_write_reg(SP06_MEAS_CFG, mode);
}

/**
 * @brief  配置温度测量参数
 * @param  rate: 测量速率
 * @param  oversampling: 过采样次数
 */
void spl06_config_temperature(uint8_t rate, uint8_t oversampling)
{
    // 根据过采样次数设置缩放系数
    switch(oversampling)
    {
        case TMP_PRC_1:
            _kT = 524288;
            break;
        case TMP_PRC_2:
            _kT = 1572864;
            break;
        case TMP_PRC_4:
            _kT = 3670016;
            break;
        case TMP_PRC_8:
            _kT = 7864320;
            break;
        case TMP_PRC_16:
            _kT = 253952;
            break;
        case TMP_PRC_32:
            _kT = 516096;
            break;
        case TMP_PRC_64:
            _kT = 1040384;
            break;
        case TMP_PRC_128:
            _kT = 2088960;
            break;
    }

    // 配置温度测量寄存器
    spl06_write_reg(SP06_TMP_CFG, rate | oversampling | 0x80);  // 温度每秒8次采样
    
    // 当过采样次数大于8时，需要设置移位标志
    if(oversampling > TMP_PRC_8)
    {
        uint8_t temp;
        spl06_read_reg(SP06_CFG_REG, &temp);
        spl06_write_reg(SP06_CFG_REG, temp | SPL06_CFG_T_SHIFT);
    }
}

/**
 * @brief  配置压力测量参数
 * @param  rate: 测量速率
 * @param  oversampling: 过采样次数
 */
void spl06_config_pressure(uint8_t rate, uint8_t oversampling)
{
    // 根据过采样次数设置缩放系数
    switch(oversampling)
    {
        case PM_PRC_1:
            _kP = 524288;
            break;
        case PM_PRC_2:
            _kP = 1572864;
            break;
        case PM_PRC_4:
            _kP = 3670016;
            break;
        case PM_PRC_8:
            _kP = 7864320;
            break;
        case PM_PRC_16:
            _kP = 253952;
            break;
        case PM_PRC_32:
            _kP = 516096;
            break;
        case PM_PRC_64:
            _kP = 1040384;
            break;
        case PM_PRC_128:
            _kP = 2088960;
            break;
    }

    // 配置压力测量寄存器
    spl06_write_reg(SP06_PSR_CFG, rate | oversampling);
    
    // 当过采样次数大于8时，需要设置移位标志
    if(oversampling > PM_PRC_8)
    {
        uint8_t temp;
        spl06_read_reg(SP06_CFG_REG, &temp);
        spl06_write_reg(SP06_CFG_REG, temp | SPL06_CFG_P_SHIFT);
    }
}

/**
 * @brief  读取压力原始ADC值
 * @retval 压力ADC值
 */
int32_t spl06_get_pressure_adc()
{
    uint8_t buf[3];
    int32_t adc;

    spl06_read_buffer(SP06_PSR_B2, buf, 3);
    adc = (int32_t)(buf[0] << 16) + (buf[1] << 8) + buf[2];
    
    // 符号扩展
    adc = (adc & 0x800000) ? (0xFF000000 | adc) : adc;

    return adc;
}

/**
 * @brief  读取温度原始ADC值
 * @retval 温度ADC值
 */
int32_t spl06_get_temperature_adc()
{
    uint8_t buf[3];
    int32_t adc;

    spl06_read_buffer(SP06_TMP_B2, buf, 3);
    adc = (int32_t)(buf[0] << 16) + (buf[1] << 8) + buf[2];

    return adc;
}

/**
 * @brief  根据校准系数计算温度和压力值
 */
void spl06_update_pressure()
{
    float Traw_src, Praw_src;
    float qua2, qua3;
    
    Traw_src = raw_temp / _kT;
    Praw_src = raw_press / _kP;
    
    // 计算温度
    temp = 0.5f * C0 + Traw_src * C1;

    // 计算压力
    qua2 = C10 + Praw_src * (C20 + Praw_src * C30);
    qua3 = Traw_src * Praw_src * (C11 + Praw_src * C21);
    press = C00 + Praw_src * qua2 + Traw_src * C01 + qua3;
}

/**
 * @brief  更新传感器数据
 */
void spl06_update()
{
    raw_temp = spl06_get_temperature_adc();
    raw_press = spl06_get_pressure_adc();
    spl06_update_pressure();
}

/**
 * @brief  获取温度值
 * @retval 温度值(°C)
 */
float spl06_get_temperature()
{
    return temp;
}

/**
 * @brief  获取压力值
 * @retval 压力值(Pa)
 */
float spl06_get_pressure()
{
    return press;
}

/**
 * @brief  向传感器寄存器写入数据
 * @param  reg_addr: 寄存器地址
 * @param  reg_val: 要写入的值
 * @retval 成功返回0，失败返回1
 */
uint8_t spl06_write_reg(uint8_t reg_addr, uint8_t reg_val)
{
    uint8_t data[2];
    data[0] = reg_addr;
    data[1] = reg_val;
    
    if(HAL_I2C_Master_Transmit(&hi2c1, SPL06_IIC_ADDR_WRITE, data, 2, 1000) != HAL_OK)
    {
        return 1; // 写入失败
    }
    
    return 0; // 写入成功
}

/**
 * @brief  从传感器寄存器读取数据
 * @param  reg_addr: 寄存器地址
 * @param  buf: 存储读取数据的缓冲区
 * @retval 成功返回0，失败返回1
 */
uint8_t spl06_read_reg(uint8_t reg_addr, uint8_t *buf)
{
    if(HAL_I2C_Master_Transmit(&hi2c1, SPL06_IIC_ADDR_WRITE, &reg_addr, 1, 1000) != HAL_OK)
    {
        return 1; // 写入地址失败
    }
    
    if(HAL_I2C_Master_Receive(&hi2c1, SPL06_IIC_ADDR_READ, buf, 1, 1000) != HAL_OK)
    {
        return 1; // 读取数据失败
    }
    
    return 0; // 读取成功
}

/**
 * @brief  从传感器寄存器读取多个数据
 * @param  reg_addr: 寄存器地址
 * @param  buffer: 存储读取数据的缓冲区
 * @param  len: 要读取的数据长度
 * @retval 成功返回0，失败返回1
 */
uint8_t spl06_read_buffer(uint8_t reg_addr, void *buffer, uint16_t len)
{
    if(HAL_I2C_Master_Transmit(&hi2c1, SPL06_IIC_ADDR_WRITE, &reg_addr, 1, 1000) != HAL_OK)
    {
        return 1; // 写入地址失败
    }
    
    if(HAL_I2C_Master_Receive(&hi2c1, SPL06_IIC_ADDR_READ, (uint8_t *)buffer, len, 1000) != HAL_OK)
    {
        return 1; // 读取数据失败
    }
    
    return 0; // 读取成功
}