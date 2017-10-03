#include "../../../uLIB/devices/radio/ezradio.hh"
//#include "debug.h"

/* Commands */
enum {
  SI_CMD_NOP      = 0x00,
  SI_CMD_PART_INFO    = 0x01,
  SI_CMD_POWER_UP     = 0x02,
  SI_CMD_FUNC_INFO    = 0x10,
  SI_CMD_SET_PROPERTY   = 0x11,
  SI_CMD_GET_PROPERTY   = 0x12,
  SI_CMD_GPIO_PIN_CFG   = 0x13,
  SI_CMD_GET_ADC_READING = 0x14,
  SI_CMD_FIFO_INFO    = 0x15,
  SI_CMD_PACKET_INFO    = 0x16,
  SI_CMD_IRCAL    = 0x17,
  SI_CMD_PROTOCOL_CFG    = 0x18,
  SI_CMD_GET_INT_STATUS   = 0x20,
  SI_CMD_GET_PH_STATUS   = 0x21,
  SI_CMD_GET_MODEM_STATUS   = 0x22,
  SI_CMD_GET_CHIP_STATUS   = 0x23,
  SI_CMD_START_TX   = 0x31,
  SI_CMD_START_RX   = 0x32,
  SI_CMD_REQUEST_DEVICE_STATE = 0x33,
  SI_CMD_CHANGE_STATE   = 0x34,
  SI_CMD_RX_HOP   = 0x36,
  SI_CMD_READ_CMD_BUFF    = 0x44,
  SI_CMD_FRR_A_READ   = 0x50,
  SI_CMD_FRR_B_READ   = 0x51,
  SI_CMD_FRR_C_READ   = 0x53,
  SI_CMD_FRR_D_READ   = 0x57,
  SI_CMD_WRITE_TX_FIFO   = 0x66,
  SI_CMD_READ_RX_FIFO   = 0x77,
};


enum {
  RF_GLOBAL_CONFIG      = 0x0003,
  RF_SYNC_CONFIG        = 0x1100,
  RF_PKT_CONFIG1        = 0x1206,
  RF_PKT_FIELD_1_CONFIG = 0x120F,
  RF_MODEM_MOD_TYPE     = 0x2000,
  RF_MODEM_MAP_CONTROL  = 0x2001,
  RF_MODEM_DSM_CTRL     = 0x2002,
  RF_MODEM_DATA_RATE    = 0x2003,
  RF_MODEM_TX_NCO_MODE  = 0x2006,
  RF_MODEM_FREQ_DEV     = 0x200A,
  RF_MODEM_TX_RAMP_DELAY  = 0x2018,
  RF_MODEM_MDM_CTRL     = 0x2019,
  RF_MODEM_IF_CONTROL   = 0x201A,
  RF_MODEM_IF_FREQ      = 0x201B,
  RF_MODEM_DECIMATION_CFG = 0x201E,
  RF_MODEM_BCR_OSR      = 0x2022,
  RF_MODEM_BCR_NCO_OFFSET = 0x2024,
  RF_MODEM_BCR_GAIN     = 0x2027,
  RF_MODEM_BCR_GEAR     = 0x2029,
  RF_MODEM_BCR_MISC1    = 0x202A,
  RF_MODEM_AFC_GEAR     = 0x202C,
  RF_MODEM_AFC_WAIT     = 0x202D,
  RF_MODEM_AGC_CONTROL  = 0x2035,
  RF_MODEM_AGC_WINDOW_SIZE  = 0x2038,
  RF_MODEM_OOK_CNT1     = 0x2042,
  RF_MODEM_RSSI_CONTROL = 0x204C,
  RF_MODEM_RSSI_COMP    = 0x204E,
  RF_MODEM_RSSI_THRESHOLD   = 0x204A,
  RF_MODEM_CLKGEN_BAND  = 0x2008,
  RF_PA_MODE            = 0x2200,
  RF_PA_PWR_LVL         = 0x2201,
  RF_PA_BIAS_CLKDUTY    = 0x2202,
  RF_PA_TC              = 0x2203,
  RF_FREQ_CONTROL_INTE  = 0x4000
};

//static Debug dbg;


EZRadioPro::EZRadioPro(SPIDeviceInterface & spi, uint32_t xtalFrequency) 
  : _spi(spi), _xtalFrequency(xtalFrequency), _outDiv(4)
{
}


void EZRadioPro::shutdown()
{
  // NO SDN PIN 
}



//////////////////////////////////////////////////////////////////////////////////////////
// SPI communication methods
// 

bool EZRadioPro::waitForReply(uint8_t *reply, uint8_t replyLength, uint16_t timeout)
{
  uint8_t cts;
  while (timeout > 0)
  {    
    _spi.select();
    _spi.write(SI_CMD_READ_CMD_BUFF);
    cts = _spi.read();
    if (cts == 0xFF) 
    {
      //dbg.print("< ");  
      for (; replyLength > 0; replyLength--) 
      {
        uint8_t x = _spi.read(); 
        *reply++ = x;
        //dbg.print(x); dbg.print(" ");
      }
      //dbg.print("\n");
      _spi.release();
      break;
    }
    _spi.release();
    
    delay_ms(1);
    timeout--;
  }

  if (timeout == 0) 
  {
    // TODO: possibly indicate error
    //dbg.print("CTS TIMEOUT\n");
    return false;
  }

  //if (cts == 0xFF) 
  //{
  //  _ctsHigh = true;
  //}

  //dbg.print("CTS: "); dbg.print(cts); dbg.print("\n");
  return (cts == 0xFF);
}


bool EZRadioPro::waitForCTS(uint16_t timeout)
{ 
  return waitForReply(0, 0, timeout);
}


bool EZRadioPro::sendCommand(uint8_t cmd, const uint8_t *data, uint8_t dataLength, uint8_t *reply, uint8_t replyLength, bool pollCTS)
{
  if (pollCTS) {
    if (!waitForCTS()) {
      //dbg.print("No CTS received\n");
      _isError = true;
      return false;
    }
  }
  
  _spi.select();
  //dbg.print("> ");
  _spi.write(cmd);
  //dbg.print(cmd); dbg.print(" ");
  for(; dataLength; dataLength--) {
    uint8_t x = *(data++);
    _spi.write(x);
    //dbg.print(x); dbg.print(" ");
  }
  delay_us(1); /* Select hold time min 50 ns */
  //dbg.print("\n");
  _spi.release();

  if (replyLength == 0) {
    _isError = false;
    return true;
  }
  else {
    //return waitForReply(reply, replyLength);
    _isError = !waitForReply(reply, replyLength);
    return !_isError;
  }
}

bool EZRadioPro::isError() {
  return _isError;
}

bool EZRadioPro::sendImmediate(uint8_t cmd, uint8_t *reply, uint8_t replyLength, bool pollCTS)
{
  if (pollCTS) {
    if (!waitForCTS())
      return false;
  }
  
  _spi.select();
  //dbg.print("> ");  
  _spi.write(cmd);
  //dbg.print(cmd); dbg.print(" < ");
  for(; replyLength; replyLength--) {
    uint8_t x = _spi.read();
    _spi.write(x);
    *(reply++) = x;
  }
  delay_us(1); /* Select hold time min 50 ns */
  //dbg.print("\n");
  _spi.release();

  return true;
}



bool EZRadioPro::configure(uint8_t *params)
{
  while (*params != 0) {
    uint8_t length = *params;
    params++;
   
    if (!sendCommand(*params, params + 1, length - 1)) {
      return false;
    }
    if (!waitForReply(0, 0)) {
      return false;
    }
    
    params += length;
  }
  return true;
}

void EZRadioPro::powerUpXTAL(uint8_t bootOptions) 
{
  uint8_t xtalOptions = 0x00;

  uint8_t data[6] = {
    bootOptions, xtalOptions,
    (uint8_t)(_xtalFrequency >> 24),
    (uint8_t)(_xtalFrequency >> 16),
    (uint8_t)(_xtalFrequency >> 8),
    (uint8_t)(_xtalFrequency)
  };

  sendCommand(SI_CMD_POWER_UP, data, sizeof(data));
}

void EZRadioPro::powerUpTCXO(uint8_t bootOptions) 
{
  uint8_t xtalOptions = 0x01;

  uint8_t data[6] = {
    bootOptions, xtalOptions,
    (uint8_t)(_xtalFrequency >> 24),
    (uint8_t)(_xtalFrequency >> 16),
    (uint8_t)(_xtalFrequency >> 8),
    (uint8_t)(_xtalFrequency)
  };

  sendCommand(SI_CMD_POWER_UP, data, sizeof(data));
}

void EZRadioPro::setXOTune(uint8_t xoTune)
{
  uint8_t data[] = { 
    0x00, 0x02, 0x00, xoTune, 0x00
  };
  sendCommand(SI_CMD_SET_PROPERTY, data, sizeof(data));
}


int16_t EZRadioPro::getTemperature()
{
  uint8_t reply[8];

  uint8_t data[] = { 
    0x10
  };

  sendCommand(SI_CMD_GET_ADC_READING, data, sizeof(data), reply, 8);

  /* Calculate the temperature in C * 10 */
  int32_t temp;
  temp  = (reply[4] << 8) | reply[5];
  temp *= 568;
  temp /= 256;
  temp -= 2970;

  return temp;
}


void EZRadioPro::setModulation(ModulationType modType, ModulationSource modSource, uint8_t txDirectModeGPIO, uint8_t txDirectModeType)
{
  uint8_t mode = 
    ((txDirectModeType & 1) << 7) |
    ((txDirectModeGPIO & 3) << 5) |
    ((modSource & 3)        << 3) |
    ((modType & 7)          << 0);
    
  uint8_t data[] = { 
    0x20, 0x03, 0x00, 
    mode, 0x00, 0x07
  };
  sendCommand(SI_CMD_SET_PROPERTY, data, sizeof(data));
}


void EZRadioPro::changeState(EZRadioPro::State state)
{
  uint8_t data[] = { 
    (uint8_t)state
  };
  sendCommand(SI_CMD_CHANGE_STATE, data, 1);
}


void EZRadioPro::enableTX(void)
{
  changeState(kStateTX);
}


void EZRadioPro::disableRadio(void)
{
  changeState(kStateReady);
}

void EZRadioPro::setFrequency(uint32_t freq)
{
  // Set the output divider according to recommended ranges given in EZRadioPro datasheet  
  _outDiv = 4;
  uint8_t band = 0;

  if (freq < 705000000UL) { _outDiv = 6;  band = 1;};
  if (freq < 525000000UL) { _outDiv = 8;  band = 2;};
  if (freq < 353000000UL) { _outDiv = 12; band = 3;};
  if (freq < 239000000UL) { _outDiv = 16; band = 4;};
  if (freq < 177000000UL) { _outDiv = 24; band = 5;};
  
  uint32_t _pfdFrequency = 2 * _xtalFrequency / _outDiv;
  
  uint8_t n = ((unsigned int)(freq / _pfdFrequency)) - 1;
  
  float ratio = (float)freq / (float)_pfdFrequency;
  float rest  = ratio - (float)n;
  
  uint32_t m = (unsigned long)(rest * 524288UL); 

  band |= 0x08; // High power setting

  /* set up CLKGEN */
  {
    uint8_t data[] = { 
      0x20, 0x01, 0x51, band };
    sendCommand(SI_CMD_SET_PROPERTY, data, sizeof(data));
  }
    
  // Set the step size
  unsigned char size_1 = 0x00;
  unsigned char size_0 = 0x02;

  /* Set the pll parameters */
  {
    uint8_t data[] = {
      0x40, 0x04, 0x00, 
      n, 
      (uint8_t)(m >> 16), 
      (uint8_t)(m >> 8), 
      (uint8_t)(m), 
      //size_1, size_0
    };
    sendCommand(SI_CMD_SET_PROPERTY, data, sizeof(data));
  }
  
  //changeState(kStateTXTune);
}

void EZRadioPro::startTX(uint8_t channel, uint16_t pktLength, State txCompleteState)
{
  uint8_t data[] = { 
    channel, 
    (uint8_t)(txCompleteState << 4),
    (uint8_t)(pktLength >> 8), 
    (uint8_t)(pktLength >> 0),
    0,
    0
  };

  /* Change to ready state */
  //changeState(kStateReady);

  /* Send START_TX command */
  sendCommand(SI_CMD_START_TX, data, sizeof(data), 0, 0, false);
}

void EZRadioPro::startRX(uint8_t channel, uint16_t pktLength, State preambleTimeoutState, State validPacketState, State invalidPacketState)
{
  uint8_t data[] = { 
    channel, 
    0,
    (uint8_t)(pktLength >> 8), 
    (uint8_t)(pktLength >> 0),
    preambleTimeoutState,
    validPacketState,
    invalidPacketState
  };

  /* Send START_TX command */
  sendCommand(SI_CMD_START_RX, data, sizeof(data), 0, 0, false);
}

void EZRadioPro::writeTX(const uint8_t *data, uint8_t length)
{
  //dbg.print("TX: ");
  for (uint8_t idx = 0; idx < length; idx++) {
    //dbg.print(data[idx]); dbg.print(" ");
  }
  //dbg.print("\n");
  sendCommand(SI_CMD_WRITE_TX_FIFO, data, length, 0, 0, false);
}

void EZRadioPro::readRX(uint8_t *data, uint8_t length)
{
  sendImmediate(SI_CMD_READ_RX_FIFO, data, length, false);
}

void EZRadioPro::configureGPIO(uint8_t gpio0, uint8_t gpio1, uint8_t gpio2, uint8_t gpio3, uint8_t nirq, uint8_t sdo, uint8_t genConfig)
{
  uint8_t data[] = { gpio0, gpio1, gpio2, gpio3, nirq, sdo, genConfig };

  uint8_t reply[7];

  sendCommand(SI_CMD_GPIO_PIN_CFG, data, 7, reply, 7);
}


void EZRadioPro::setPreambleLength(uint8_t length)
{
  uint8_t data[] = { 
    0x10, 0x01, 0x00,
    length
  };

  sendCommand(SI_CMD_SET_PROPERTY, data, sizeof(data));
}

void EZRadioPro::setPreambleConfig(uint8_t config)
{
  uint8_t data[] = { 
    0x10, 0x01, 0x04,
    config
  };

  sendCommand(SI_CMD_SET_PROPERTY, data, sizeof(data));
}


void EZRadioPro::setSync(uint8_t config, uint16_t syncWord)
{
  uint8_t data[] = { 
    0x11, 0x03, 0x00,
    config,
    (uint8_t)(syncWord >> 8),
    (uint8_t)(syncWord)
  };

  sendCommand(SI_CMD_SET_PROPERTY, data, sizeof(data));
}


void EZRadioPro::setPowerLevel(uint8_t level)
{
  uint8_t data[] = { 
    0x22, 0x01, 0x01,
    level
  };

  sendCommand(SI_CMD_SET_PROPERTY, data, sizeof(data));  
}


void EZRadioPro::setNCOModulo(NCOModulo osr, uint32_t ncoFreq) {
  //uint32_t ncoFreq;
  //switch (osr) {
  //  case kModulo10: ncoFreq = _xtalFrequency / 10; break;
  //  case kModulo20: ncoFreq = _xtalFrequency / 20; break;
  //  case kModulo40: ncoFreq = _xtalFrequency / 40; break;
  //}
  
  uint8_t data[] = { 
    0x20, 0x04, 0x06,
    (uint8_t)(ncoFreq >> 24) & 0x03 | (uint8_t)(osr << 2),
    (uint8_t)(ncoFreq >> 16),
    (uint8_t)(ncoFreq >> 8),
    (uint8_t)(ncoFreq),
  };
  
  sendCommand(SI_CMD_SET_PROPERTY, data, sizeof(data));
}


void EZRadioPro::setDataRate(uint32_t dataRate)
{
  uint8_t data[] = { 
    0x20, 0x03, 0x03,
    (uint8_t)(dataRate >> 16),
    (uint8_t)(dataRate >> 8),
    (uint8_t)(dataRate),
  };
  sendCommand(SI_CMD_SET_PROPERTY, data, sizeof(data));
}


void EZRadioPro::setDeviation(uint32_t deviation)
{
  uint32_t x = ((uint64_t)(1ul << 18) * _outDiv * deviation)/ _xtalFrequency;

  uint8_t data[] = { 
    0x20, 0x03, 0x0A,
    (uint8_t)(x >> 16),
    (uint8_t)(x >> 8),
    (uint8_t)(x)
  };
  sendCommand(SI_CMD_SET_PROPERTY, data, 6);
}


void EZRadioPro::flushTX()
{
  uint8_t data[] = { 0x01 };
  sendCommand(SI_CMD_FIFO_INFO, data, sizeof(data));
}

void EZRadioPro::flushRX()
{
  uint8_t data[] = { 0x02 };
  sendCommand(SI_CMD_FIFO_INFO, data, sizeof(data));
}

uint8_t EZRadioPro::getAvailableRX()
{
  uint8_t data[] = { 0x00 };
  uint8_t reply[2];
  sendCommand(SI_CMD_FIFO_INFO, data, sizeof(data), reply, 2);
  return reply[0];
}

void EZRadioPro::getIntStatus(void)
{
  uint8_t data[] = { 0x00, 0x00, 0x00 };
  //uint8_t data[] = { 0xFF, 0xFF, 0xFF };
  sendCommand(SI_CMD_GET_INT_STATUS, data, sizeof(data));
}

void EZRadioPro::getIntStatus(IRQStatus &status)
{
  uint8_t data[] = { 0x00, 0x00, 0x00 };
  sendCommand(SI_CMD_GET_INT_STATUS, data, sizeof(data), status.rawData, 8);
}

void EZRadioPro::getModemStatus(ModemStatus &status)
{
  sendCommand(SI_CMD_GET_MODEM_STATUS, 0, 0, status.rawData, 8);
}

void EZRadioPro::getChipStatus(ChipStatus &status)
{
  sendCommand(SI_CMD_GET_CHIP_STATUS, 0, 0, status.rawData, 3);
}

void EZRadioPro::getPartInfo(PartInfo &info)
{
  sendCommand(SI_CMD_PART_INFO, 0, 0, info.rawData, 8);
}

void EZRadioPro::setIntControl(bool enableChipInt, bool enableModemInt, bool enablePHInt)
{
  uint8_t x = 0;
  if (enableChipInt) x |= 0x04;
  if (enableModemInt) x |= 0x02;
  if (enablePHInt) x |= 0x01;
  
  uint8_t data[] = { 
    0x01, 0x01, 0x00,
    x
  };
  sendCommand(SI_CMD_SET_PROPERTY, data, sizeof(data));
}

void EZRadioPro::setPHInterrupts(uint8_t mask)
{
  uint8_t data[] = { 
    0x01, 0x01, 0x01,
    mask
  };
  sendCommand(SI_CMD_SET_PROPERTY, data, sizeof(data));
}

void EZRadioPro::setGlobalConfig(uint8_t globalConfig)
{
  setParameter(RF_GLOBAL_CONFIG, globalConfig);
}

void EZRadioPro::setPacketConfig(uint8_t config)
{
  setParameter(RF_PKT_CONFIG1, config);
}

void EZRadioPro::setField1Config(uint8_t config)
{
  setParameter(RF_PKT_FIELD_1_CONFIG, config);
}

void EZRadioPro::setModemParams(uint8_t modemControl, uint8_t ifControl, uint32_t ifFreq, uint8_t cfg1, uint8_t cfg2)
{  
  uint8_t data[] = { 
    0x20, 0x07, 0x19,
    modemControl,
    ifControl,
    (uint8_t)(ifFreq >> 16),
    (uint8_t)(ifFreq >> 8),
    (uint8_t)(ifFreq),
    cfg1, 
    cfg2
  };
  sendCommand(SI_CMD_SET_PROPERTY, data, sizeof(data));
}

void EZRadioPro::setBCRParams(uint16_t osr, uint32_t ncoOffset, uint16_t gain, uint8_t gear, uint8_t misc1)
{  
  uint8_t data[] = { 
    0x20, 0x09, 0x22,
    (uint8_t)(osr >> 8),
    (uint8_t)(osr >> 0),
    (uint8_t)(ncoOffset >> 16),
    (uint8_t)(ncoOffset >>  8),
    (uint8_t)(ncoOffset >>  0),
    (uint8_t)(gain >> 8),
    (uint8_t)(gain >> 0),
    gear,
    misc1
  };
  sendCommand(SI_CMD_SET_PROPERTY, data, sizeof(data));
}

void EZRadioPro::setPAConfig(uint8_t mode, uint8_t level, uint8_t duty, uint8_t tc)
{
  uint8_t data[] = { 
    0x22, 0x04, 0x00,
    mode,
    level,
    duty,
    tc
  };
  sendCommand(SI_CMD_SET_PROPERTY, data, sizeof(data));
}

void EZRadioPro::setParameter(uint16_t id, uint8_t value)
{
  uint8_t data[] = { 
    (uint8_t)(id >> 8),
    0x01, 
    (uint8_t)id,
    value
  };
  sendCommand(SI_CMD_SET_PROPERTY, data, sizeof(data));
}

void EZRadioPro::setParameter16(uint16_t id, uint16_t value)
{
  uint8_t data[] = { 
    (uint8_t)(id >> 8),
    0x02, 
    (uint8_t)id,
    (uint8_t)(value >> 8),
    (uint8_t)(value)
  };
  sendCommand(SI_CMD_SET_PROPERTY, data, sizeof(data));
}

uint8_t EZRadioPro::getState()
{ 
  uint8_t reply[2]; 
  sendCommand(SI_CMD_REQUEST_DEVICE_STATE, 0, 0, reply, 2);
  return reply[0] & 0x0F;
}

void EZRadioPro::setRSSIMode(uint8_t mode)
{
  setParameter(RF_MODEM_RSSI_CONTROL, mode);
}

void EZRadioPro::setRSSIComp(uint8_t comp)
{
  setParameter(RF_MODEM_RSSI_COMP, comp);
}

void EZRadioPro::setRSSIThreshold(uint8_t threshold)
{
  setParameter(RF_MODEM_RSSI_THRESHOLD, threshold);
}

