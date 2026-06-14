# IMU 센서 기반 에어백 전개 판단 및 Fail-Safe 로직 구현

## 프로젝트 개요
본 프로젝트는 차량용 안전제어기(Airbag Control Unit)의 핵심 요구사항인 실시간성(Real-time)과 기능 안전(ISO 26262 Fail-Safe)을 고려하여 설계된 C/C++ 기반의 에어백 전개 판단 시스템입니다. 

IMU 센서(가속도계) 데이터를 실시간으로 처리하여 충돌 여부를 판단하며, 센서 고장(Stuck-at, Out of Range) 발생 시 오작동을 방지하는 방어 로직을 구현했습니다.

## 주요 기술 및 특징
- **Language:** C/C++
- **Signal Processing:** 정적 링 버퍼 기반 이동 평균 필터(Moving Average Filter) 설계 (동적 할당 배제)
- **Deployment Logic:** 가속도 적분 기반 속도 변화량($\Delta V$) 산출 및 Arming 이중 안전장치 교차 검증
- **Fail-Safe Diagnostics:** 물리적 한계 초과 노이즈 및 고착 고장(Stuck-at) 감지, 에어백 트리거 강제 차단(Symptom Lock)

## 디렉토리 구조
- `/src`, `/inc`: 핵심 알고리즘 및 시스템 동작 소스/헤더 파일
- `/data`: 주행 시나리오별 가상 IMU 센서 데이터 (Case A, B, C)
- `/docs`: 아키텍처 설계도 및 시뮬레이션 결과 리포트

## 시뮬레이션 시나리오
1. **Case A (정상 주행):** 노이즈가 포함된 일상 주행 (전개 안 됨)
2. **Case B (정면 충돌):** Delta-V 및 Arming 조건 동시 충족 (정상 전개)
3. **Case C (센서 고장):** 값 고착 및 범위 초과 오류 발생 시 즉각 진단 및 전개 차단 (Fail-Safe 작동)