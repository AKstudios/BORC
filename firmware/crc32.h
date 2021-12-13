#pragma once

// Computes the CRC32 of a buffer
uint32_t crc32(void* buf, uint32_t len, uint32_t partial_crc = 0);