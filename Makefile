# Standard SystemC Makefile
# You can override SYSTEMC_HOME when calling make, e.g., make SYSTEMC_HOME=/opt/systemc

SYSTEMC_HOME ?= /usr/local/systemc-3.0.0

# Arch varies depending on local system (lib-linux64, lib-cygwin, lib-macosx, lib-linuxaarch64)
TARGET_ARCH ?= lib-linux64

CXX = g++
CXXFLAGS = -I$(SYSTEMC_HOME)/include -Wall -Wextra -std=c++17 -O2
LDFLAGS = -L$(SYSTEMC_HOME)/$(TARGET_ARCH) -lsystemc -Wl,-rpath=$(SYSTEMC_HOME)/$(TARGET_ARCH)

TARGET = soc_sim

SRCS = sc_main.cpp
HDRS = producer.h consumer.h router.h config.h

all: $(TARGET)

$(TARGET): $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

clean:
	rm -f $(TARGET) *.o output_*.ppm

.PHONY: all clean
