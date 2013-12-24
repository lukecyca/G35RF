#ifndef G35RF_h
#define G35RF_h

#define G35RF_POWER_ON  0x55
#define G35RF_POWER_OFF 0xaa
#define G35RF_FUNCTION  0x5a

void G35RF_enable();
int G35RF_get_command();

#endif
