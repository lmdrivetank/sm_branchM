// Copyright 2019 Inceptio Technology. All Rights Reserved.
// Authors:Ming Li (ming.li@inceptio.ai)

#include "gtest/gtest.h"
//#include "vsp/AdSession/autonomy_brain.h"
#include "vsp/AdSession/autonomy_brain_processor.h"
//#include "vsp/AdSession/driver_brain.h"
#include "vsp/AdSession/driver_brain_processor.h"
//#include "vsp/AdSession/vehicle_brain.h"
#include "vsp/AdSession/vehicle_brain_processor.h"

namespace vsp {
namespace AdSession {
char* enum_name[] = {"ESessionState_IDLE",         "ESessionState_INIT",
                     "ESessionState_TERMINATE",    "ESessionState_READY",
                     "ESessionState_AD_PREPARE",   "ESessionState_AD_EXIT",
                     "ESessionState_AD_READY",     "ESessionState_AD_ENGAGE",
                     "ESessionState_AD_DISENGAGE", "ESessionState_AD_RUN",
                     "ESessionState_AD_FALLBACK",  "ESessionState_AD_MRC"};
char* enum_name_x86[] = {
    "STATE_UNKNOWN",   "NOT_INITIALIZED",     "NOT_READY",
    "READY_TO_ENGAGE", "REQUESTING_AUTONOMY", "IN_AUTONOMY",
    "IN_FALLBACK",     "SWITCHING_TO_MRC",    "IN_MRC"};
#if 0
TEST(BrainSession, MessageFlow) {
// publish
#define hmi_input_cur_status interfaces::vsp::hmiProxy::HmiInput
#define autonomy_x86_cur_status interfaces::vsp::vspProxy::AutonomyStatus
#define vci_cur_status interfaces::vsp::vci::VciCurrentState
  // subscribe
#define hmi_proxy_req_status interfaces::vsp::hmiProxy::HmiProxyRequestState
#define autonomy_x86_req_status interfaces::vsp::vspProxy::AutonomyStatus
#define vci_req_status interfaces::vsp::vci::VciRequestState

  std::unique_ptr<hmi_input_cur_status> hmi_input_cur_status_;
  std::unique_ptr<autonomy_x86_cur_status> autonomy_x86_cur_status_;
  std::unique_ptr<vci_cur_status> vci_cur_status_;

  hmi_input_cur_status_ = CREATE_MSG_UNIQUE(hmi_input_cur_status);
  autonomy_x86_cur_status_ = CREATE_MSG_UNIQUE(autonomy_x86_cur_status);
  vci_cur_status_ = CREATE_MSG_UNIQUE(vci_cur_status);

  irs_common::IRSPublisher<hmi_input_cur_status>::SharedPtr
      hmi_input_cur_status_pub_;
  irs_common::IRSPublisher<autonomy_x86_cur_status>::SharedPtr
      autonomy_x86_cur_status_pub_;
  irs_common::IRSPublisher<vci_cur_status>::SharedPtr vci_cur_status_pub_;
  irs_common::IRSSubscription<hmi_proxy_req_status>::SharedPtr
      hmi_proxy_req_status_sub_;
  irs_common::IRSSubscription<autonomy_x86_req_status>::SharedPtr
      autonomy_x86_req_status_sub_;
  irs_common::IRSSubscription<vci_req_status>::SharedPtr vci_req_status_sub_;
}
#endif
TEST(BrainSession, WorkFlow) {
  int d2a, a2v = ESessionState_READY, a2x86, v2vci, flag;

  driver_brain_processor db = driver_brain_processor();
  autonomy_brain_processor ab = autonomy_brain_processor();
  vehicle_brain_processor vb = vehicle_brain_processor();
  db.InitInternal();
  ab.InitInternal();
  vb.InitInternal();
  while (1) {
    printf(
        "\n\n\n~~~~~~~~~~~~~~~~~~~driver brain "
        "start****************************************\n");
    printf("driver_brain_state_cur_ %s\n",
           enum_name[db.driver_brain_state_cur_]);
    if (db.driver_brain_state_cur_ == ESessionState_AD_READY) {
      db.request_engage_autonomy_ = true;
    } /* else if (db.driver_brain_state_cur_ == ESessionState_AD_MRC) {
       db.request_engage_autonomy_ = false;
     }*/
    else if (db.driver_brain_state_cur_ == ESessionState_AD_MRC) {
      db.request_engage_autonomy_ = false;
    } else if (db.driver_brain_state_cur_ == ESessionState_AD_RUN) {
      ab.out_odd_x86_ = true;
    } else if (db.driver_brain_state_cur_ == ESessionState_AD_PREPARE) {
      ab.out_odd_x86_ = false;
    }

    printf("###subsript request_engage_autonomy_ %d\n",
           db.request_engage_autonomy_ == false ? 0 : 1);
    printf("###subscript autonomy_brain_state_cur_ %s\n",
           enum_name[ab.autonomy_brain_state_cur_]);
    printf("###subscript vehicle_brain_state_cur_ %s\n",
           enum_name[vb.vehicle_brain_state_cur_]);
    printf("~~~driver_brain_sm_run~~~\n");
    db.out_odd_autonomy_ = ab.out_odd_autonomy_;
    db.driver_brain_sm_run();
    printf("&&&action driver_brain_state_cur_ %s\n",
           enum_name[db.driver_brain_state_cur_]);

    if (d2a != db.autonomy_brain_state_req_) {
      d2a = db.autonomy_brain_state_req_;
      printf("###publish autonomy_brain_state_req_ %s\n",
             enum_name[db.autonomy_brain_state_req_]);
      ab.autonomy_brain_state_req_ = db.autonomy_brain_state_req_;
    } else {
      printf("~~~~no instruction to request~~~~\n");
    }

    printf(
        "\n~~~~~~~~~~~~~~~~~~~autonomy brain "
        "start****************************************\n");
    printf("autonomy_brain_state_cur_ %s\n",
           enum_name[ab.autonomy_brain_state_cur_]);
    printf("###subscript autonomy_brain_state_req_ %s\n",
           enum_name[ab.autonomy_brain_state_req_]);
    printf("###subscript x86_state_cur_ %s\n",
           enum_name_x86[ab.x86_state_cur_]);
    printf("###subscript vehicle_brain_state_cur_ %s\n",
           enum_name[vb.vehicle_brain_state_cur_]);
    printf("~~~autonomy_brain_sm_run~~~\n");
    ab.autonomy_brain_sm_run();
    printf("&&&action autonomy_brain_state_cur_ %s\n",
           enum_name[ab.autonomy_brain_state_cur_]);
    db.autonomy_brain_state_cur_ = ab.autonomy_brain_state_cur_;

    flag = 0;
    if (a2v != ab.vehicle_brain_state_req_) {
      a2v = ab.vehicle_brain_state_req_;
      flag = 1;
      printf("###publish vehicle_brain_state_req_ %s\n",
             enum_name[ab.vehicle_brain_state_req_]);
      vb.vehicle_brain_state_req_ = ab.vehicle_brain_state_req_;
    }
    if (a2x86 != ab.x86_state_req_) {
      a2x86 = ab.x86_state_req_;
      flag = 1;
      printf("###publish x86_state_req_ %s\n", enum_name[ab.x86_state_req_]);
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
        case ESessionState_AD_MRC:
          ab.x86_state_cur_ = interfaces::onboard::IN_MRC;
          break;
        default:
          break;
      }
    }
    if (flag == 0) {
      printf("~~~~no instruction to request~~~~\n");
    }

    printf("\nvehicle brain start****************************************\n");
    printf("vehicle_brain_state_cur_ %s\n",
           enum_name[vb.vehicle_brain_state_cur_]);
    printf("###subscript vehicle_brain_state_req_ %s\n",
           enum_name[vb.vehicle_brain_state_req_]);
    printf("###subscript vci_state_cur_ %s\n", enum_name[vb.vci_state_cur_]);
    printf("~~~vehicle_brain_sm_run~~~\n");
    vb.vehicle_brain_sm_run();
    printf("&&&action vehicle_brain_state_cur_ %s\n",
           enum_name[vb.vehicle_brain_state_cur_]);
    ab.vehicle_brain_state_cur_ = vb.vehicle_brain_state_cur_;

    if (v2vci != vb.vci_state_req_) {
      v2vci = vb.vci_state_req_;
      printf("###publish vci_state_req_ %s\n", enum_name[vb.vci_state_req_]);
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
    } else {
      printf("~~~~no instruction to request~~~~\n");
    }

    printf("****************************************\n");
    sleep(2);
    printf("~~~~~~~~~~ all is end ~~~~~~~~~~\n");
  }
}
}  // namespace AdSession
}  // namespace vsp
