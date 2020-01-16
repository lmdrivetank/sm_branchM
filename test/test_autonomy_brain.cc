// Copyright 2019 Inceptio Technology. All Rights Reserved.
// Authors:Ming Li (ming.li@inceptio.ai)

#include "gtest/gtest.h"
#include "vsp/AdSession/autonomy_brain_processor.h"

namespace vsp {
namespace AdSession {
TEST(AutonoyBrain, WorkFlow) {
  autonomy_brain_processor ab = autonomy_brain_processor();
  ab.InitInternal();
  while (1) {
    printf("****************************************\n");
    printf("autonomy brain current state %d\n", ab.autonomy_brain_state_cur_);
    switch (ab.autonomy_brain_state_cur_) {
      case ESessionState_IDLE: {
        ab.autonomy_brain_state_req_ = ESessionState_INIT;
        ab.vehicle_brain_state_cur_ = ESessionState_READY;
      } break;
      case ESessionState_READY:
        ab.autonomy_brain_state_req_ = ESessionState_AD_PREPARE;
        break;
      case ESessionState_AD_READY:
        ab.autonomy_brain_state_req_ = ESessionState_AD_ENGAGE;
        break;
      case ESessionState_AD_RUN:
        ab.autonomy_brain_state_req_ = ESessionState_AD_DISENGAGE;
        break;
      default:
        break;
    }
    printf("autonomy_brain_state_req_ %d\n", ab.autonomy_brain_state_req_);
    printf("***sm run***\n");
    ab.autonomy_brain_sm_run();
    printf("autonomy brain current state %d\n", ab.autonomy_brain_state_cur_);
    printf("vehicle_brain_state_cur_ %d\n", ab.vehicle_brain_state_cur_);
    printf("vehicle_brain_state_req_ %d\n", ab.vehicle_brain_state_req_);
    printf("x86_state_cur_ %d\n", ab.x86_state_cur_);
    printf("x86_state_req_ %d\n", ab.x86_state_req_);
    switch (ab.vehicle_brain_state_req_) {
      case ESessionState_AD_PREPARE:
        ab.vehicle_brain_state_cur_ = ESessionState_AD_READY;
        break;
      case ESessionState_AD_ENGAGE:
        ab.vehicle_brain_state_cur_ = ESessionState_AD_RUN;
        break;
      case ESessionState_AD_DISENGAGE:
        ab.vehicle_brain_state_cur_ = ESessionState_AD_PREPARE;
        break;
      default:
        break;
    }
    switch (ab.x86_state_req_) {
      case ESessionState_INIT:
        ab.x86_state_cur_ = interfaces::onboard::NOT_READY;
        break;
      case ESessionState_AD_PREPARE:
        ab.x86_state_cur_ = interfaces::onboard::READY_TO_ENGAGE;
        break;
      case ESessionState_AD_ENGAGE:
        ab.x86_state_cur_ = interfaces::onboard::IN_AUTONOMY;
        break;
      case ESessionState_AD_DISENGAGE:
        ab.x86_state_cur_ = interfaces::onboard::NOT_READY;
        break;
      default:
        break;
    }
    printf("****************************************\n");
    printf("vehicle_brain_state_cur_ %d\n", ab.vehicle_brain_state_cur_);
    printf("x86_state_cur_ %d\n", ab.x86_state_cur_);
  }
  printf("~~~~~~~~~~ all is end ~~~~~~~~~~\n");
}
}  // namespace AdSession
}  // namespace vsp
