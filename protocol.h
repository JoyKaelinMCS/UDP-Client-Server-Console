#pragma once

#include <stdint.h>

#pragma pack(push, 1)

struct MyMessage {
	uint8_t type;
	uint32_t seq;
	char payload[256];
};

#pragma pack(pop)