GNU_PREFIX := xc32

ADD_FEATURE_VECTOR?=y
ADD_SEGMENT_DATA?=n
BUILD_TENSORFLOW?=n


# Toolchain commands
CC              := '$(GNU_PREFIX)-gcc'
CXX             := '$(GNU_PREFIX)-g++'
AS              := '$(GNU_PREFIX)-as'
AR              := '$(GNU_PREFIX)-ar' -r
LD              := '$(GNU_PREFIX)-ld'
NM              := '$(GNU_PREFIX)-nm'
OBJDUMP         := '$(GNU_PREFIX)-objdump'
OBJCOPY         := '$(GNU_PREFIX)-objcopy'
SIZE            := '$(GNU_PREFIX)-size'

THIS_DIR          := $(shell dirname $(abspath $(lastword $(MAKEFILE_LIST))))
OUT               := $(abspath $(THIS_DIR))/_build

#DEBUG_FLAGS = -O0 -g3
CFLAGS += $(SML_FLAGS)
CFLAGS += -std=c11 -fno-builtin -nostdlib -nodefaultlibs -ffreestanding -nostartfiles -O2



LDFLAGS += -L/include -L./
IFLAGS += -I/include -I./
LDLIBS += -lm

IFLAGS += -I./ \
 -I/build/CMSIS_5/CMSIS/Core/Include/ \
 -I/build/CMSIS-NN/Include/ \
 -I/build/CMSIS-DSP/Include/



SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,$(OUT)/%.o,$(SRCS))

MAIN = $(OUT)/libmplabml.a

$(OUT)/%.o:$(THIS_DIR)/%.c
		@echo $(ANSI_RED)"[cPP]"$(ANSI_OFF) $@
		$(AT)$(CC) $(CFLAGS) $(IFLAGS) $(LDFLAGS) -c -o $@ $< $(LDLIBS)


$(MAIN): $(OBJS)
		@echo $(LDFLAGS)
	        $(AR) -cs $(MAIN) $(OBJS)

clean:
		@echo $(ANSI_RED)"[CLEAN]"$(ANSI_OFF) $@
		rm -f $(OBJS) $(LIB)

main:  $(MAIN)

all:	main


.PHONY:	main all clean
