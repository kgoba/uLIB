INCLUDE         += -I$(OPENCM3_DIR)/include/
CFLAGS          += -Os -ggdb3 $(INCLUDE) -D$(FAMILY) $(ARCH) -ffunction-sections -fdata-sections
CXXFLAGS        += -Os -ggdb3 $(INCLUDE) -D$(FAMILY) $(ARCH) -fno-rtti -fno-exceptions -ffunction-sections -fdata-sections
CPPFLAGS        += -MD
LDFLAGS         += -static -nostartfiles $(ARCH)
LDFLAGS         += -Wl,--start-group -Wl,--end-group -Wl,--gc-sections
LDFLAGS         += -lgcc -lc --specs=nosys.specs
LDLIBS          += -L$(OPENCM3_DIR)/lib
LDLIBS          += -l$(OPENCM3_LIB)

TARGET ?= firmware

#include $(OPENCM3_DIR)/mk/genlink-config.mk
include $(OPENCM3_DIR)/mk/gcc-config.mk

.PHONY: clean all

all: $(TARGET).elf $(TARGET).bin $(TARGET).lst
	$(SIZE) $(TARGET).elf 

$(TARGET).lst: $(TARGET).elf
	$(OBJDUMP) -S $(TARGET).elf >$(TARGET).lst

clean:
	$(Q)$(RM) -rf $(TARGET).* *.o

flash:
	st-flash --reset write $(TARGET).bin 0x8000000

#include $(OPENCM3_DIR)/mk/genlink-rules.mk
include $(OPENCM3_DIR)/mk/gcc-rules.mk

