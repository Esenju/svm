#pragma once
#ifndef CONFIG_H
#define CONFIG_H

// CHANGE MODE HERE ONLY (0 = Part A, 1 = Part B, 2 = 2 Consumers)
#define MODE 2

// Memory map for Router (used in MODE 2)
#include <cstdint>
static const uint64_t BASE1 = 0x00000000;
static const uint64_t END1  = 0x001D4C00;
static const uint64_t BASE2 = 0x00200000;
static const uint64_t END2  = 0x003D4C00;

#endif