#ifndef LOGIC_H
#define LOGIC_H

#include "acu_types.h"
#include "filter.h"

// ----------------------------------------
// 알고리즘 파라미터 (캘리브레이션 튜닝 값)
// ----------------------------------------

#define THRESHOLD_OUT_OF_RANGE  20.0    // (수정)센서 물리적 한계치 (20G 초과 시 에러) * 수정 전 16G -> 수정 후 20G
#define THRESHOLD_DELTA_V       -50.0   // X축 가속도 적분치 전개 기준 (음수 방향)
#define THRESHOLD_ARMING        -5.0    // 보조 축(X축 또는 Y축) 순간 가속도 전개 기준
#define STUCK_AT_TIME_LIMIT     50      // 500ms(10ms*50) 동안 값이 안 변하면 Stuck-at 에러

// ----------------------------------------
// 알고리즘 제어 구조체
// ----------------------------------------

typedef struct{
    double delta_v;         // 가속도 적분값 (속도 변화량)
    double last_accel_x;    // Stuck-at 검사용 이전 데이터
    int stuck_counter;      // 동일 값 유지 횟수 카운터
} DeployLogicState;

// ----------------------------------------
// 함수 프로토타입
// ----------------------------------------

void Logic_Init(DeployLogicState* logicState, AcuStatus* acuStatus);

// Fail-Safe 진단 (Out of Range 및 Stuck-at 검사)
bool Logic_CheckFailSafe(AcuStatus* acuStatus, DeployLogicState* logicState, const ImuData* rawData);

// 에어백 전개 조건 판별 (Delta-V 및 Arming 동시 만족 확인)
void Logic_EvaluateDeployment(AcuStatus* acuStatus, DeployLogicState* logicState, double filtered_x, double raw_y);

#endif