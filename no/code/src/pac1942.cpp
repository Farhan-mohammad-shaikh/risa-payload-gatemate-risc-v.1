#include "pac1942.h"
#include "logging.h"

PAC1942::PAC1942(I2C& i2c_dev): i2c(i2c_dev) {}

bool PAC1942::init(int8_t adcId) {
    deviceId = adcId;

    /* I2C TEST ERSATZ*/
    i2c.start();
    uint8_t result = i2c.writeAddress(deviceId, RD);
    i2c.stop();

    if(result == ACK)
    {
        return true;
    }

    LOGWARN("NO ACK FROM ADDR 0x%X", deviceId);
    
    return false;
}

void PAC1942::refresh(void)
{
    i2c.writeRegister8(deviceId, PAC1942_REG_REFRESH, 0x00);
    delayUS(1000); //Wait 1ms (according to the data sheet)
}

#if 1

void PAC1942::setSampleMode(enum SAMPLE_MODE samples){
    uint16_t ctrl = i2c.readRegister16(deviceId, PAC1942_REG_CTRL);
    ctrl =  (samples << 12) | (ctrl & 0xFFF);
    i2c.writeRegister16(deviceId, PAC1942_REG_CTRL, ctrl);
    refresh();
}

void PAC1942::configChannels(enum CONFIG_VBUS vbusCH1, enum CONFIG_VSENSE vsensCH1, enum CONFIG_VBUS vbusCH2, enum CONFIG_VSENSE vsensCH2){
    uint16_t negPwrFsr = i2c.readRegister16(deviceId, PAC1942_REG_NEG_PWR_FSR);
    negPwrFsr =  (vsensCH1 << 14) | (negPwrFsr & 0x3FFF);
    negPwrFsr =  (vsensCH2 << 12) | (negPwrFsr & 0xCFFF);
    negPwrFsr =  (vbusCH1 << 6) | (negPwrFsr & 0xFF3F);
    negPwrFsr =  (vbusCH2 << 4) | (negPwrFsr & 0xFFCF);
    configBusCH1 = vbusCH1;
    configSenseCH1 = vsensCH1;
    configBusCH2 = vbusCH2;
    configSenseCH2 = vsensCH2;
    i2c.writeRegister16(deviceId, PAC1942_REG_NEG_PWR_FSR, negPwrFsr);
}

void PAC1942::configAccumulator(enum CONFIG_ACC configCH1,enum CONFIG_ACC configCH2){
    uint8_t accConfig = i2c.readRegister8(deviceId, PAC1942_REG_ACC_CONFIG);
    accConfig =  (configCH1 << 6) | (accConfig & 0x1F);
    accConfig =  (configCH2 << 4) | (accConfig & 0xCF);
    i2c.writeRegister8(deviceId, PAC1942_REG_ACC_CONFIG, accConfig);
}

float PAC1942::getVoltageCH1(void){
    int16_t vBusCh1 = i2c.readRegister16(deviceId, PAC1942_REG_VBUS_CH1_AVG);

    switch(configBusCH1){
        case UNIPOLAR_9V: return 9.0f * vBusCh1/65536.0f; break;
        case BIPOLAR_9V: return 18.0f * vBusCh1/65536.0f; break;
        case BIPOLAR_4V5: return 4.5f * vBusCh1/32768.0f; break;
    }

    return 0.0f;
}

uint16_t PAC1942::getVoltageCH1Raw(void){
    return i2c.readRegister16(deviceId, PAC1942_REG_VBUS_CH1_AVG);
}

float PAC1942::getVoltageCH2(void){
    int16_t vBusCh2 = i2c.readRegister16(deviceId, PAC1942_REG_VBUS_CH2_AVG);

    switch(configBusCH2){
        case UNIPOLAR_9V: return 9.0f * vBusCh2/65536.0f; break;
        case BIPOLAR_9V: return 18.0f * vBusCh2/65536.0f; break;
        case BIPOLAR_4V5: return 4.50f * vBusCh2/32768.0f; break;
    }

    return 0.0f;
}

uint16_t PAC1942::getVoltageCH2Raw(void){
    return i2c.readRegister16(deviceId, PAC1942_REG_VBUS_CH2_AVG);
}


float PAC1942::getCurrentCH1(void){
    int16_t vSens1 = i2c.readRegister16(deviceId, PAC1942_REG_VSENS_CH1_AVG);

    switch(configSenseCH1){
        case UNIPOLAR_100mV: return 100.0f * vSens1/65536.0f; break;
        case BIPOLAR_100mV: return 200.0f * vSens1/65536.0f; break;
        case BIPOLAR_50mV: return 50.0f * vSens1/32768.0f; break;
    }

    return 0.0f;
}
uint16_t PAC1942::getCurrentCH1Raw(void){
    return i2c.readRegister16(deviceId, PAC1942_REG_VSENS_CH1_AVG);
}
   
float PAC1942::getCurrentCH2(void){
    int16_t vSens2 = i2c.readRegister16(deviceId, PAC1942_REG_VSENS_CH2_AVG);

    switch(configSenseCH2){
        case UNIPOLAR_100mV: return 100.0f * vSens2/65536.0f; break;
        case BIPOLAR_100mV: return 200.0f * vSens2/65536.0f; break;
        case BIPOLAR_50mV: return 50.0f * vSens2/32768.0f; break;
    }

    return 0.0f;
}
uint16_t PAC1942::getCurrentCH2Raw(void){
    return i2c.readRegister16(deviceId, PAC1942_REG_VSENS_CH2_AVG);
}


float PAC1942::getPowerCH1(void){
    uint8_t value[4];
    i2c.readNBytes(deviceId, PAC1942_REG_POWER_CH1, sizeof(value), value);

    int32_t power1 = (((value[0] & 0xFF) << 24) | ((value[1] & 0xFF) << 16) | ((value[2] & 0xFF) << 8) | (value[3] & 0xFF));
    power1 = power1 >> 2;

    float factor;
    if((configBusCH1 == BIPOLAR_9V && configSenseCH1 == BIPOLAR_100mV) || (configBusCH1 == BIPOLAR_9V && configSenseCH1 == UNIPOLAR_100mV) || (configBusCH1 == UNIPOLAR_9V && configSenseCH1 == BIPOLAR_100mV)){
        factor = 1.8;
    }else{
        factor = 0.9;
    }
    return factor * power1/1073741824.0;
}

float PAC1942::getPowerCH2(void){
    uint8_t value[4];
    i2c.readNBytes(deviceId, PAC1942_REG_POWER_CH2, sizeof(value), value);

    int32_t power2 = (((value[0] & 0xFF) << 24) | ((value[1] & 0xFF) << 16) | ((value[2] & 0xFF) << 8) | (value[3] & 0xFF));
    power2 = power2 >> 2;

    float factor;
    if((configBusCH2 == BIPOLAR_9V && configSenseCH2 == BIPOLAR_100mV) || (configBusCH2 == BIPOLAR_9V && configSenseCH2 == UNIPOLAR_100mV) || (configBusCH2 == UNIPOLAR_9V && configSenseCH2 == BIPOLAR_100mV)){
        factor = 1.8;
    }else{
        factor = 0.9;
    }
    return factor * power2/1073741824.0;
}

 float PAC1942::getAccCurrentCH1(void){
    uint8_t value[7];
    i2c.readNBytes(deviceId, PAC1942_REG_ACC_COUNT, 4, value);
    uint32_t accCounter = (((value[0] & 0xFF) << 24) | ((value[1] & 0xFF) << 16) | ((value[2] & 0xFF) << 8) | (value[3] & 0xFF));
    i2c.readNBytes(deviceId, PAC1942_REG_ACC_VACC1, 7, value);
    //int64_t accValue1 = (((uint64_t)(value[0] & 0xFF) << 48) | ((uint64_t)(value[1] & 0xFF) << 40) | ((uint64_t)(value[2] & 0xFF) << 32) | ((value[3] & 0xFF) << 24) | ((value[4] & 0xFF) << 16) | ((value[5] & 0xFF) << 8) | value[6] & 0xFF);
    int32_t accValue1 = (((uint64_t)(value[2] & 0xFF) << 32) | ((value[3] & 0xFF) << 24) | ((value[4] & 0xFF) << 16) | ((value[5] & 0xFF) << 8) | (value[6] & 0xFF));
    switch(configSenseCH1){
        case UNIPOLAR_100mV: return 100.0f * (accValue1/(float)accCounter)/65536.0f; break;
        case BIPOLAR_100mV: return 200.0f * ((int16_t)(accValue1/(float)accCounter))/65536.0f; break;
        case BIPOLAR_50mV: return 50.0f * ((int16_t)(accValue1/(float)accCounter))/32768.0f; break;
    }

    return 0.0f;
}

 float PAC1942::getAccCurrentCH2(void){
    uint8_t value[7];
    i2c.readNBytes(deviceId, PAC1942_REG_ACC_COUNT, 4, value);
    uint32_t accCounter = (((value[0] & 0xFF) << 24) | ((value[1] & 0xFF) << 16) | ((value[2] & 0xFF) << 8) | (value[3] & 0xFF));
    i2c.readNBytes(deviceId, PAC1942_REG_ACC_VACC1, 7, value);
    //int64_t accValue2 = (((uint64_t)(value[0] & 0xFF) << 48) | ((uint64_t)(value[1] & 0xFF) << 40) | ((uint64_t)(value[2] & 0xFF) << 32) | ((value[3] & 0xFF) << 24) | ((value[4] & 0xFF) << 16) | ((value[5] & 0xFF) << 8) | value[6] & 0xFF);
    int32_t accValue2 = (((uint64_t)(value[2] & 0xFF) << 32) | ((value[3] & 0xFF) << 24) | ((value[4] & 0xFF) << 16) | ((value[5] & 0xFF) << 8) | (value[6] & 0xFF));
    switch(configSenseCH2){
        case UNIPOLAR_100mV: return 100 * (accValue2/(float)accCounter)/65536.0f; break;
        case BIPOLAR_100mV: return 200 * ((int16_t)(accValue2/(float)accCounter))/65536.0f; break;
        case BIPOLAR_50mV: return 50 * ((int16_t)(accValue2/(float)accCounter))/32768.0f; break;
    }

    return 0.0f;
}

#endif