#pragma once

#ifdef _M_X64
typedef  uint64_t address_size;
#else
typedef  uint32_t address_size;
#endif
