#ifndef PAC1942_H
#define PAC1942_H

#include "i2c.h"
#include "delay.h"
#include <unistd.h>

#define PAC1942_REG_REFRESH 0x00            // REFRESH
#define PAC1942_REG_CTRL 0x01               // CTRL
#define PAC1942_REG_NEG_PWR_FSR 0x1D        // NEG_PWR_FSR
#define PAC1942_REG_VBUS_CH1 0x07           // VBUS_CH1
#define PAC1942_REG_VBUS_CH2 0x08           // VBUS_CH2
#define PAC1942_REG_VSENS_CH1 0x0B          // VSENS_CH1
#define PAC1942_REG_VSENS_CH2 0x0C          // VSENS_CH2
#define PAC1942_REG_VBUS_CH1_AVG 0x0F       // VBUS_CH1 (Rolling Average)
#define PAC1942_REG_VBUS_CH2_AVG 0x10       // VBUS_CH2 (Rolling Average)
#define PAC1942_REG_VSENS_CH1_AVG 0x13      // VSENS_CH1 (Rolling Average)
#define PAC1942_REG_VSENS_CH2_AVG 0x14      // VSENS_CH2 (Rolling Average)
#define PAC1942_REG_POWER_CH1 0x17          // ṔOWER_CH1
#define PAC1942_REG_POWER_CH2 0x18          // POWER_CH2
#define PAC1942_REG_ACC_CONFIG 0x25         // Accumulator Config
#define PAC1942_REG_ACC_COUNT 0x02          // Accumulator Counter
#define PAC1942_REG_ACC_VACC1 0x03          // Accumulator Value1
#define PAC1942_REG_ACC_VACC2 0x04          // Accumulator Value2

#define PAC1942_DEFAULT_ID 0b01101001
#define PAC1942_REG_PRODUCT_ID 0xFD

#define PAC1942_DEFAULT_MANU 0x54
#define PAC1942_REG_MANU_ID 0xFE


enum SAMPLE_MODE{
    SPS_1024_ADAPTIVE = 0x0, //default
    SPS_256_ADAPTIVE = 0x1,
    SPS_64_ADAPTIVE = 0x2,
    SPS_8_ADAPTIVE = 0x3,
    SPS_1024 = 0x4,
    SPS_256 = 0x5,
    SPS_64 = 0x6,
    SPS_8 = 0x7
};

enum CONFIG_VBUS{
    UNIPOLAR_9V, //default
    BIPOLAR_9V,
    BIPOLAR_4V5
};

enum CONFIG_VSENSE{
    UNIPOLAR_100mV, //default
    BIPOLAR_100mV,
    BIPOLAR_50mV
};

enum CONFIG_ACC{
    VPOWER,
    VSENSE,
    VBUS
};

class PAC1942{
public:
    PAC1942(I2C& i2c_dev);
    bool init(int8_t adcId);
    void setSampleMode(enum SAMPLE_MODE samples);
    void configChannels(enum CONFIG_VBUS vbusCH1, enum CONFIG_VSENSE vsensCH1, enum CONFIG_VBUS vbusCH2, enum CONFIG_VSENSE vsensCH2);
    void configAccumulator(enum CONFIG_ACC configCH1,enum CONFIG_ACC configCH2);
    void refresh(void);//refresh before reading values
    float getVoltageCH1(void);
    float getVoltageCH2(void);
    float getCurrentCH1(void);
    float getCurrentCH2(void);
    float getPowerCH1(void);
    float getPowerCH2(void);
    float getAccCurrentCH1(void);
    float getAccCurrentCH2(void);    
    uint16_t getVoltageCH1Raw(void);
    uint16_t getVoltageCH2Raw(void);
    uint16_t getCurrentCH1Raw(void);
    uint16_t getCurrentCH2Raw(void);
private:
    I2C i2c;
    int8_t deviceId;
    CONFIG_VBUS configBusCH1;
    CONFIG_VBUS configBusCH2;
    CONFIG_VSENSE configSenseCH1;
    CONFIG_VSENSE configSenseCH2;
};

#endif
