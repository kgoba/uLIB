#include <stdint.h>

#include "os.hh"

class Si446xBase {
public:
  enum ModulationType {
    kModCW    = 0,
    kModOOK   = 1,
    kMod2FSK  = 2,
    kMod2GFSK = 3,
    kMod4FSK  = 4,
    kMod4GFSK = 5
  };
  
  enum ModulationSource {
    kSourceFIFO    = 0,
    kSourceDirect  = 1,
    kSourcePN9     = 2
  };  

  enum NCOModulo {
    kModulo10  = 0,
    kModulo20  = 2,
    kModulo40  = 1
  };  

  enum State {
    kStateNoChange  = 0,
    kStateSleep     = 1,
    kStateSPIActive = 2,
    kStateReady     = 3,
    kStateTXTune    = 5,
    kStateRXTune    = 6,
    kStateTX        = 7,
    kStateRX        = 8
  };
};

class SPIDeviceInterface {
public:
    virtual void select() = 0;
    virtual void release() = 0;

    virtual void write(uint8_t x) = 0;
    virtual uint8_t read() = 0;
};

//template<class SPIDevice>
class EZRadioPro : public Si446xBase {
public:
  struct PartInfo {
    uint16_t getPartID() {
      return ((rawData[1] << 8) | rawData[2]);
    }
  
    uint8_t getRevision() {
      return rawData[0];
    }
    
    uint8_t   rawData[8];
  };

  struct IRQStatus {
    bool isPacketSentPending() {
      return rawData[2] & (1 << 5);
    }
    
    bool isPacketRXPending() {
      return rawData[2] & (1 << 4);
    }

    bool isCRCErrorPending() {
      return rawData[2] & (1 << 3);
    }

    bool isPacketSent() {
      return rawData[3] & (1 << 5);
    }

    bool isPacketRX() {
      return rawData[3] & (1 << 4);
    }
    
    uint8_t   rawData[8];
  };  

  struct ModemStatus {
    uint8_t getCurrentRSSI() {
      return rawData[2];
    }

    uint8_t getLatchedRSSI() {
      return rawData[3];
    }

    bool isSyncDetect() {
      return rawData[1] & (1 << 0);
    }
    
    bool isPreambleDetect() {
      return rawData[1] & (1 << 1);
    }

    bool isInvalidSync() {
      return rawData[1] & (1 << 5);
    }

    bool isInvalidPreamble() {
      return rawData[1] & (1 << 2);
    }
        
    bool isRSSI() {
      return rawData[1] & (1 << 3);
    }

    bool isSyncDetectPending() {
      return rawData[0] & (1 << 0);
    }
    
    bool isPreambleDetectPending() {
      return rawData[0] & (1 << 1);
    }

    bool isInvalidSyncPending() {
      return rawData[0] & (1 << 5);
    }

    bool isInvalidPreamblePending() {
      return rawData[0] & (1 << 2);
    }
        
    bool isRSSIPending() {
      return rawData[0] & (1 << 3);
    }
    
    uint8_t   rawData[8];
  };     

  struct ChipStatus {
    uint8_t   rawData[3];
  };  
  
  EZRadioPro(SPIDeviceInterface &spi, uint32_t xtalFrequency);

  bool configure(uint8_t *params);

  void getPartInfo(PartInfo &info);
  uint8_t getState();
  
  void shutdown();
  void powerUpTCXO(uint8_t bootOptions = 0x01);
  void powerUpXTAL(uint8_t bootOptions = 0x01);

  void setXOTune(uint8_t xoTune);
  void setGlobalConfig(uint8_t globalConfig);
  
  void setPowerLevel(uint8_t level);
  void setPAConfig(uint8_t mode, uint8_t level, uint8_t duty, uint8_t tc);
  void configureGPIO(uint8_t gpio0 = 0, uint8_t gpio1 = 0, uint8_t gpio2 = 0, uint8_t gpio3 = 0, uint8_t nirq = 0, uint8_t sdo = 0, uint8_t genConfig = 3);

  void enableTX();
  void disableRadio();

  void setFrequency(uint32_t freq);
  void startTX(uint8_t channel, uint16_t pktLength = 0, State txCompleteState = kStateNoChange);
  void startRX(uint8_t channel, uint16_t pktLength = 0, State preambleTimeoutState = kStateNoChange, State validPacketState = kStateReady, State invalidPacketState = kStateReady);

  void setModulation(ModulationType modType, ModulationSource modSource = kSourceFIFO, uint8_t txDirectModeGPIO = 0, uint8_t txDirectModeType = 0);
  void setNCOModulo(NCOModulo osr, uint32_t ncoFreq);
  void setDataRate(uint32_t dataRate);
  void setDeviation(uint32_t deviation);
  void setModemParams(uint8_t modemControl, uint8_t ifControl, uint32_t ifFreq, uint8_t cfg1, uint8_t cfg2);
  void setBCRParams(uint16_t osr, uint32_t ncoOffset, uint16_t gain, uint8_t gear, uint8_t misc1);
  void setAFCParams(uint8_t gear, uint8_t wait, uint16_t gain, uint16_t limiter, uint8_t misc);
  void setAGCControl(uint8_t mode);
  void setAGCParams(uint8_t windowSize, uint8_t rfpdDecay, uint8_t ifpdDecay, uint16_t fsk4Gain, uint16_t fsk4Threshold, uint8_t fsk4Map, uint8_t ookPDTC);
  void setRSSIMode(uint8_t mode);
  void setRSSIComp(uint8_t comp);
  void setRSSIThreshold(uint8_t threshold);
  
  void setPreambleLength(uint8_t length);
  void setPreambleConfig(uint8_t config);
  void setSync(uint8_t config, uint16_t syncWord = 0);
  void setPacketConfig(uint8_t config);
  void setField1Config(uint8_t config);

  void writeTX(const uint8_t *data, uint8_t length);
  void flushTX();

  uint8_t getAvailableRX();
  void readRX(uint8_t *data, uint8_t length);
  void flushRX();

  void setIntControl(bool enableChipInt, bool enableModemInt, bool enablePHInt);
  void setPHInterrupts(uint8_t mask);

  //void check(uint8_t param1, uint8_t param2);

  void getIntStatus();
  void getIntStatus(IRQStatus &status);
  void getPHStatus();
  void getModemStatus(ModemStatus &status);  
  void getChipStatus(ChipStatus &status);
  
  int16_t getTemperature(); 

  void changeState(State state);
  
  bool isError();
  
private: 
  bool waitForCTS(uint16_t timeout = 200);
  bool waitForReply(uint8_t *reply, uint8_t replyLength, uint16_t timeout = 200);
  
  bool sendCommand(uint8_t cmd, const uint8_t *data, uint8_t dataLength, uint8_t *reply = 0, uint8_t replyLength = 0, bool pollCTS = true);
  bool sendImmediate(uint8_t cmd, uint8_t *reply, uint8_t replyLength, bool pollCTS = true);

  void setParameter(uint16_t id, uint8_t value);
  void setParameter16(uint16_t id, uint16_t value);

  SPIDeviceInterface & _spi;
  
  uint32_t    _xtalFrequency;
  uint8_t     _outDiv;
  
  bool        _isError;
};

