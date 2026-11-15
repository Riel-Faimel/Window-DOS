#ifndef __kernel_drv_ps2_ps2_c_h
#define __kernel_drv_ps2_ps2_c_h

#ifdef __cplusplus
extern "C" {
#endif

extern const char code2ascii[128];
extern const char shift_code2ascii[128];

typedef struct ps2_seq {
    const char *data;
    unsigned char len;
} ps2_seq;

extern const ps2_seq ps2_ext_seq[128];

#ifdef __cplusplus
}
#endif

#endif