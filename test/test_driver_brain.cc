// Copyright 2019 Inceptio Technology. All Rights Reserved.
// Authors:Ming Li (ming.li@inceptio.ai)

#include "gtest/gtest.h"
#include "vsp/AdSession/driver_brain_processor.h"

namespace vsp {
namespace AdSession {

TEST(DriverBrain, WorkFlow) {
  driver_brain_processor db = driver_brain_processor();
  db.InitInternal();
  while (1) {
    printf("****************************************\n");
    printf("driver brain current state %d\n", db.driver_brain_state_cur_);
    if (db.driver_brain_state_cur_ == ESessionState_READY) {
      db.request_engage_autonomy_ = true;
    } else if (db.driver_brain_state_cur_ == ESessionState_AD_RUN) {
      db.request_engage_autonomy_ = false;
    }
    printf("request_engage_autonomy_ %d\n",
           db.request_engage_autonomy_ == false ? 0 : 1);
    db.driver_brain_sm_run();
    printf("driver brain current state %d\n", db.driver_brain_state_cur_);
    printf("autonomy_brain_state_cur_ %d\n", db.autonomy_brain_state_cur_);
    printf("autonomy_brain_state_req_ %d\n", db.autonomy_brain_state_req_);

    if (db.autonomy_brain_state_req_ == ESessionState_INIT) {
      db.autonomy_brain_state_cur_ = ESessionState_READY;
    } else if (db.autonomy_brain_state_req_ == ESessionState_AD_PREPARE) {
      db.autonomy_brain_state_cur_ = ESessionState_AD_READY;
    } else if (db.autonomy_brain_state_req_ == ESessionState_AD_ENGAGE) {
      db.autonomy_brain_state_cur_ = ESessionState_AD_RUN;
    } else if (db.autonomy_brain_state_req_ == ESessionState_AD_DISENGAGE) {
      db.autonomy_brain_state_cur_ = ESessionState_AD_PREPARE;
    }
    printf("****************************************\n");
    printf("autonomy_brain_state_cur_ %d\n", db.autonomy_brain_state_cur_);
  }
  printf("~~~~~~~~~~ all is end ~~~~~~~~~~\n");
}
}  // namespace AdSession
}  // namespace vsp
