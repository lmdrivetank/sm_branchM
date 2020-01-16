// Copyright 2019 Inceptio Technology. All Rights Reserved.
// Authors:Ming Li (ming.li@inceptio.ai)

#include <string>
#include "gtest/gtest.h"
#include "vsp/AdSession/vehicle_brain_processor.h"

namespace vsp {
namespace AdSession {
char* enum_name[] = {"ESessionState_IDLE",         "ESessionState_INIT",
                     "ESessionState_TERMINATE",    "ESessionState_READY",
                     "ESessionState_AD_PREPARE",   "ESessionState_AD_EXIT",
                     "ESessionState_AD_READY",     "ESessionState_AD_ENGAGE",
                     "ESessionState_AD_DISENGAGE", "ESessionState_AD_RUN",
                     "ESessionState_AD_FALLBACK",  "ESessionState_AD_MRC"};
TEST(VehicleBrain, WorkFlow) {
  vehicle_brain_processor vb = vehicle_brain_processor();
  vb.InitInternal();
  while (1) {
    printf("****************************************\n");
    printf("vehicle_brain_state_cur_ %s\n",
           enum_name[vb.vehicle_brain_state_cur_]);
    switch (vb.vehicle_brain_state_cur_) {
      case ESessionState_IDLE: {
        vb.vehicle_brain_state_req_ = ESessionState_INIT;
      } break;
      case ESessionState_READY:
        vb.vehicle_brain_state_req_ = ESessionState_AD_PREPARE;
        break;
      case ESessionState_AD_READY:
        vb.vehicle_brain_state_req_ = ESessionState_AD_ENGAGE;
        break;
      case ESessionState_AD_RUN:
        vb.vehicle_brain_state_req_ = ESessionState_AD_DISENGAGE;
        break;
      default:
        break;
    }
    printf("vehicle_brain_state_req_ %s\n",
           enum_name[vb.vehicle_brain_state_req_]);
    printf("***sm run***\n");
    vb.vehicle_brain_sm_run();
    printf("vehicle_brain_state_cur_ %s\n",
           enum_name[vb.vehicle_brain_state_cur_]);
    printf("vci_state_cur_ %s\n", enum_name[vb.vci_state_cur_]);
    printf("vci_state_req_ %s\n", enum_name[vb.vci_state_req_]);
    switch (vb.vci_state_req_) {
      case ESessionState_INIT:
        vb.vci_state_cur_ = ESessionState_READY;
        break;
      case ESessionState_AD_PREPARE:
        vb.vci_state_cur_ = ESessionState_AD_READY;
        break;
      case ESessionState_AD_ENGAGE:
        vb.vci_state_cur_ = ESessionState_AD_RUN;
        break;
      case ESessionState_AD_DISENGAGE:
        vb.vci_state_cur_ = ESessionState_AD_PREPARE;
        break;
      default:
        break;
    }
    printf("****************************************\n");
    printf("vci_state_cur_ %s\n", enum_name[vb.vci_state_cur_]);
  }
}
}  // namespace AdSession
}  // namespace vsp
