
CXXFLAGS += -g -mmcu=$(MCU) -Os 
CXXFLAGS += -DF_CPU=$(F_CPU)
CXXFLAGS += -fdata-sections -ffunction-sections 
CXXFLAGS += -funsigned-char -funsigned-bitfields -fshort-enums -fno-inline-small-functions 
CXXFLAGS += -fpack-struct
INCLUDES += -I../../include
LFLAGS   += -g -mmcu=$(MCU) -Wl,--relax,--gc-sections
LIBS 	 += 

# SRC = $(wildcard *.cc) $(EXTSRC)
OBJ = $(SRC:.cc=.o)

.PHONY: all clean fuse program

all: $(TARGET).hex $(TARGET).lst

clean:
	@echo "  Cleaning all"
	@rm -f $(TARGET).hex $(TARGET).lst $(TARGET).elf $(OBJ)

$(TARGET).hex: $(TARGET).elf
	@$(OBJCOPY) -j .text -j .data -O ihex $(TARGET).elf $(TARGET).hex

$(TARGET).lst: $(TARGET).elf
	@$(OBJDUMP) -h -S $(TARGET).elf >$(TARGET).lst

$(TARGET).elf: $(OBJ) $(LIBS)
	@echo "  Linking..."
	@$(LD) -o $@ $(OBJ) $(LFLAGS) $(LIBS)
	@$(SIZE) $@

.cc.o:
	@echo "  Compiling $<..."
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

fuse:
	avrdude -p $(MCU) -c $(PROGRAMMER) $(PROG_OPTS) -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(XFUSE):m

program: $(TARGET).hex
	avrdude -p $(MCU) -c $(PROGRAMMER) $(PROG_OPTS) -U flash:w:$(TARGET).hex

