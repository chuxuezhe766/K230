#ifndef __DSP_JY901S_H
#define __DSP_JY901S_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
extern uint8_t RxBuffer[5];
// 初始化函数
void JY901S_UART2_Init(void) ;

// 欧拉角获取接口
float JY901S_GetRoll(void);
float JY901S_GetPitch(void);
float JY901S_GetYaw(void);

#ifdef __cplusplus
}
#endif

#endif /* __DSP_JY901S_H */
