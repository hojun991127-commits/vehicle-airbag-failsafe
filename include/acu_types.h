#ifndef ACU_TYPES_H
#define ACU_TYPES_H

#include <stdint.h>
#include <stdbool.h>

// 시스템 동작 상태 (System State)
typedef enum{
    STATE_NORMAL = 0,   // 정상 주행 중
    STATE_DEPLOYED,     // 에어백 전개 완료
    STATE_FAULT         // 고장 감지 (Fail-Safe 작동)
} SystemState;

// 진단 에러 코드 (Diagnostic Trouble Code, DTC)
typedef enum{
    ERR_NONE = 0,
    ERR_OUT_OF_RANGE,
    ERR_STUCK_AT
} ErrorCode;

// IMU 센서 원시 데이터 구조체
typedef struct{
    double time_ms; 
    double accel_x; // 주 충돌 감지 축
    double accel_y; // 보조(Arming) 감지 축
    double accel_z; // 참고 축
} ImuData;

// ACU 전체 상태를 관리하는 구조체
typedef struct{
    SystemState currentState;
    ErrorCode currentError;
    bool isTriggered;           // 에어백 전개 하드웨어 핀 상태(가상)
} AcuStatus;

#endif