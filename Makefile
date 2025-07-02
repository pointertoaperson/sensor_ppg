
# ==============================================================================
#  Project   : connectHeart
#  File      : Makefile
#  Author    : Umesh Puri
#  Created   : 2025-06-02
#  Version   : 1.0
#  Purpose   : Build system for STM32F103-based connectHeart project
#
#  Description:
#    This Makefile builds the firmware for the connectHeart project using
#    arm-none-eabi toolchain. It supports compiling, linking, cleaning,
#    and flashing to the STM32F103 target.
#	@license MIT License
#  Copyright (c) 2025 Umesh.
# ==============================================================================




PROJECT = connectHeart_V1
DEVICE = STM32F103xB
# Project Structure
SRCDIR = src src/drivers common common/cmsis common/src/dev dsp_lib/

BINDIR = bin
OBJDIR = obj
INCDIR = include 

COM_DIR =src/ src/drivers  common common/include common/cmsis  common/cmsis/Driver/Include CMSIS/Lib dsp_lib/
COMDIR= $(foreach dir, $(COM_DIR),  -I$(dir))

#  target cpu
CPU = cortex-m3

# Sources
SRC = $(foreach dir, $(SRCDIR),$(wildcard $(dir)/*.c))  

objs_c = $(patsubst %.c,%.o,$(wildcard $(SRC)/*.c))
objs_comp = $(foreach i, $(objs_c), $(OBJDIR)/$(notdir $i))
objs_comp += $(OBJDIR)/all_asm.o

ASM = $(wildcard src/*.s) 

#defines
DEFINE = -DSTM32F103xB 
DEFINE += -DSTM32F1
DEFINE+= -D__STATIC_FORCEINLINE='static inline __attribute__((always_inline))'


# Include directories
INCLUDE  = -I$(INCDIR) $(COMDIR)

# Linker
LSCRIPT = STM32F103XD_FLASH.ld 

# C/C++ Flags
CCOMMONFLAGS = -Wall -Os -fno-common -mthumb -mcpu=$(CPU) --specs=nosys.specs --specs=nano.specs -g

# C Flags
GCFLAGS  = -std=c11 -DARM_MATH_CM3  -Wa,-ahlms=$(addprefix $(OBJDIR)/,$(notdir $(<:.c=.lst)))
GCFLAGS += $(CCOMMONFLAGS) $(INCLUDE) $(DEFINE) 
LDFLAGS +=  -Ldsp_lib/ -larm_cortexM3l_math  -T$(LSCRIPT) -mthumb -mcpu=$(CPU) --specs=nosys.specs --specs=nano.specs -Wl,-Map,$(BINDIR)/$(PROJECT).map -Wl,--gc-sections

ASFLAGS += -mcpu=$(CPU)

# Tools
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
AR = arm-none-eabi-ar
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size --format=SysV -x
OBJDUMP = arm-none-eabi-objdump

RM = rm -rf

## Build process

OBJ := $(patsubst %.c,%.o,$(wildcard  $(SRC)*.c))
OBJ += $(addprefix $(OBJDIR)/,$(notdir $(ASM:.s=.o)))


all: $(BINDIR)/$(PROJECT).bin $(BINDIR)/$(PROJECT).hex

Build: $(BINDIR)/$(PROJECT).bin

macros:
	$(CC) $(GCFLAGS) -dM -E - < /dev/null

cleanBuild: clean

clean:
	$(RM) $(BINDIR)
	$(RM) $(OBJDIR)

size:
	$(SIZE) $(BINDIR)/$(PROJECT).elf

# Compilation

$(BINDIR)/$(PROJECT).hex: $(BINDIR)/$(PROJECT).elf
	$(OBJCOPY) -O ihex $(BINDIR)/$(PROJECT).elf $(BINDIR)/$(PROJECT).hex

$(BINDIR)/$(PROJECT).bin: $(BINDIR)/$(PROJECT).elf
	$(OBJCOPY) -O binary $(BINDIR)/$(PROJECT).elf $(BINDIR)/$(PROJECT).bin

$(BINDIR)/$(PROJECT).elf: $(OBJDIR)/all_asm.o $(objs_c) $(LSCRIPT)
	echo "\n===linking===\n"
	@mkdir -p $(dir $@); \
	$(CC) $(objs_comp) $(LDFLAGS) -o $(BINDIR)/$(PROJECT).elf; 
	$(OBJDUMP) -D $(BINDIR)/$(PROJECT).elf > $(BINDIR)/$(PROJECT).lst; 
	$(SIZE) $(BINDIR)/$(PROJECT).elf

%.o: %.c
	@echo "\n=== compiling $< ===\n"
	@mkdir -p $(dir $@)
	$(CC) $(GCFLAGS) -c $< -o $(OBJDIR)/$(notdir $@)

$(OBJDIR)/all_asm.o: $(ASM)
	@echo "\n=== compiling $< ===\n"
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ -c $^



prints:
	@for f in $(SRC); do  echo $$f; done;
	@for f in $(objs_c); do  echo $$f; done;
	@for f in $(objs_comp); do  echo $$f; done;
