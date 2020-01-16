// Copyright 2019 Inceptio Technology. All Rights Reserved.
// Authors:
//    Ming Li (ming.li@inceptio.ai)

#include "driver_brain_processor.h"
#include <memory>
#include <utility>

namespace vsp {
namespace AdSession {
void driver_brain_processor::InitInternal() {
  /////idle
  AddState(ESessionState::ESessionState_IDLE);
  AddStateTask(ESessionState::ESessionState_IDLE, 0,
               std::bind(&driver_brain_processor::MonitorInitRequest, this),
               std::bind(&driver_brain_processor::EnterInit, this));
  //////init
  AddState(ESessionState::ESessionState_INIT);
  AddStateTask(ESessionState::ESessionState_INIT, 0,
               std::bind(&driver_brain_processor::WaitManagedModuleIdle, this),
               std::bind(&driver_brain_processor::CallManagedModuleInit, this));
  AddStateTask(ESessionState::ESessionState_INIT, 1,
               std::bind(&driver_brain_processor::WaitManagedModuleReady, this),
               std::bind(&driver_brain_processor::InitProcess, this));
  AddStateTask(ESessionState::ESessionState_INIT, 2, nullptr,
               std::bind(&driver_brain_processor::EnterReady, this));
  std::vector<int> chain_init = {0, 1, 2};
  ChainStateTask(ESessionState::ESessionState_INIT, chain_init);
  //////ready
  AddState(ESessionState::ESessionState_READY);
  AddStateTask(ESessionState::ESessionState_READY, 0, nullptr,
               std::bind(&driver_brain_processor::branch_start, this));
  AddStateTask(
      ESessionState::ESessionState_READY, 1,
      std::bind(&driver_brain_processor::MonitorAdPrepareRequest, this),
      std::bind(&driver_brain_processor::EnterAdPrepare, this));
  AddStateTask(
      ESessionState::ESessionState_READY, 2,
      std::bind(&driver_brain_processor::MonitorTerminateRequest, this),
      std::bind(&driver_brain_processor::EnterTerminate, this));
  AddStateTask(ESessionState::ESessionState_READY, 3, nullptr,
               std::bind(&driver_brain_processor::branch_finish, this));
  std::vector<int> chain_ready1 = {0, 1, 3};
  std::vector<int> chain_ready2 = {0, 2, 3};
  ChainStateTask(ESessionState::ESessionState_READY, chain_ready1);
  ChainStateTask(ESessionState::ESessionState_READY, chain_ready2);
  SetTaskBranch(ESessionState::ESessionState_READY, TaskBranch_ParallelOr, 0, 3,
                1);
  //////terminate
  AddState(ESessionState::ESessionState_TERMINATE);
  AddStateTask(
      ESessionState::ESessionState_TERMINATE, 0, nullptr,
      std::bind(&driver_brain_processor::CallManagedModuleTerminate, this));
  AddStateTask(ESessionState::ESessionState_TERMINATE, 1,
               std::bind(&driver_brain_processor::WaitManagedModuleIdle, this),
               std::bind(&driver_brain_processor::TerminateProcess, this));
  AddStateTask(ESessionState::ESessionState_TERMINATE, 2, nullptr,
               std::bind(&driver_brain_processor::EnterIdle, this));
  std::vector<int> chain_terminate = {0, 1, 2};
  ChainStateTask(ESessionState::ESessionState_TERMINATE, chain_terminate);
  //////ad prepare
  AddState(ESessionState::ESessionState_AD_PREPARE);
  AddStateDepend(ESessionState::ESessionState_AD_PREPARE,
                 std::bind(&driver_brain_processor::MonitorAdExitRequest, this),
                 std::bind(&driver_brain_processor::EnterAdExit, this),
                 workResult_Succ);
  AddStateTask(
      ESessionState::ESessionState_AD_PREPARE, 0, nullptr,
      std::bind(&driver_brain_processor::CallManagedModuleAdRrepare, this));
  AddStateTask(
      ESessionState::ESessionState_AD_PREPARE, 1,
      std::bind(&driver_brain_processor::WaitManagedModuleAdReady, this),
      std::bind(&driver_brain_processor::EnterAdReady, this));

  std::vector<int> chain_adprepare1 = {0, 1};
  ChainStateTask(ESessionState::ESessionState_AD_PREPARE, chain_adprepare1);
  //////ad ready
  AddState(ESessionState::ESessionState_AD_READY);
  AddStateDepend(ESessionState::ESessionState_AD_READY,
                 std::bind(&driver_brain_processor::GetOddOk, this),
                 std::bind(&driver_brain_processor::EnterAdPrepare, this));
  AddStateTask(ESessionState::ESessionState_AD_READY, 0, nullptr,
               std::bind(&driver_brain_processor::branch_start, this));
  AddStateTask(ESessionState::ESessionState_AD_READY, 1,
               std::bind(&driver_brain_processor::MonitorAdEngageRequest, this),
               std::bind(&driver_brain_processor::EnterAdEngage, this));
  AddStateTask(ESessionState::ESessionState_AD_READY, 2,
               std::bind(&driver_brain_processor::MonitorAdExitRequest, this),
               std::bind(&driver_brain_processor::EnterAdExit, this));
  AddStateTask(ESessionState::ESessionState_AD_READY, 3, nullptr,
               std::bind(&driver_brain_processor::branch_finish, this));
  std::vector<int> chain_adready1 = {0, 1, 3};
  std::vector<int> chain_adready2 = {0, 2, 3};
  ChainStateTask(ESessionState::ESessionState_AD_READY, chain_adready1);
  ChainStateTask(ESessionState::ESessionState_AD_READY, chain_adready2);
  SetTaskBranch(ESessionState::ESessionState_AD_READY, TaskBranch_ParallelOr, 0,
                3, 1);
  //////ad exit
  AddState(ESessionState::ESessionState_AD_EXIT);
  AddStateTask(
      ESessionState::ESessionState_AD_EXIT, 0, nullptr,
      std::bind(&driver_brain_processor::CallManagedModuleAdExit, this));
  AddStateTask(ESessionState::ESessionState_AD_EXIT, 1,
               std::bind(&driver_brain_processor::WaitManagedModuleReady, this),
               std::bind(&driver_brain_processor::EnterReady, this));
  std::vector<int> chain_ad_exit = {0, 1};
  ChainStateTask(ESessionState::ESessionState_AD_EXIT, chain_ad_exit);
  //////ad engage
  AddState(ESessionState::ESessionState_AD_ENGAGE);
  AddStateDepend(ESessionState::ESessionState_AD_ENGAGE,
                 std::bind(&driver_brain_processor::GetOddOk, this),
                 std::bind(&driver_brain_processor::EnterFallback, this));
  AddStateTask(
      ESessionState::ESessionState_AD_ENGAGE, 0, nullptr,
      std::bind(&driver_brain_processor::CallManagedModuleAdEngage, this));
  AddStateTask(ESessionState::ESessionState_AD_ENGAGE, 1,
               std::bind(&driver_brain_processor::WaitManagedModuleAdRun, this),
               std::bind(&driver_brain_processor::EnterAdRun, this));
  std::vector<int> chain_engage = {0, 1};
  ChainStateTask(ESessionState::ESessionState_AD_ENGAGE, chain_engage);
  //////ad run
  AddState(ESessionState::ESessionState_AD_RUN);
  AddStateDepend(ESessionState::ESessionState_AD_RUN,
                 std::bind(&driver_brain_processor::GetOddOk, this),
                 std::bind(&driver_brain_processor::EnterFallback, this));
  AddStateTask(ESessionState::ESessionState_AD_RUN, 0, nullptr,
               std::bind(&driver_brain_processor::branch_start, this));
  AddStateTask(
      ESessionState::ESessionState_AD_RUN, 1,
      std::bind(&driver_brain_processor::MonitorAdDisengageRequest, this),
      std::bind(&driver_brain_processor::EnterAdDisengage, this));
  AddStateTask(ESessionState::ESessionState_AD_RUN, 2, nullptr,
               std::bind(&driver_brain_processor::AdRunProcess, this));
  AddStateTask(ESessionState::ESessionState_AD_RUN, 3, nullptr,
               std::bind(&driver_brain_processor::branch_finish, this));
  std::vector<int> chain_adrun1 = {0, 1, 3};
  std::vector<int> chain_adrun2 = {0, 2, 3};
  ChainStateTask(ESessionState::ESessionState_AD_RUN, chain_adrun1);
  ChainStateTask(ESessionState::ESessionState_AD_RUN, chain_adrun2);
  SetTaskBranch(ESessionState::ESessionState_AD_RUN, TaskBranch_Switch, 0, 3,
                1);
  //////ad disengage -> ad prepare
  AddState(ESessionState::ESessionState_AD_DISENGAGE);
  AddStateDepend(ESessionState::ESessionState_AD_DISENGAGE,
                 std::bind(&driver_brain_processor::GetOddOk, this),
                 std::bind(&driver_brain_processor::EnterFallback, this));
  AddStateTask(
      ESessionState::ESessionState_AD_DISENGAGE, 0, nullptr,
      std::bind(&driver_brain_processor::CallManagedModuleAdDisengage, this));
  AddStateTask(
      ESessionState::ESessionState_AD_DISENGAGE, 1,
      std::bind(&driver_brain_processor::WaitManagedModuleAdDisengaged, this),
      std::bind(&driver_brain_processor::AdDisengageProcess, this));
  AddStateTask(ESessionState::ESessionState_AD_DISENGAGE, 2, nullptr,
               std::bind(&driver_brain_processor::EnterAdPrepare, this));
  std::vector<int> chain_disengage = {0, 1, 2};
  ChainStateTask(ESessionState::ESessionState_AD_DISENGAGE, chain_disengage);
  ////// ad fallback
  AddState(ESessionState::ESessionState_AD_FALLBACK);
  AddStateTask(
      ESessionState::ESessionState_AD_FALLBACK, 0, nullptr,
      std::bind(&driver_brain_processor::CallManagedModuleFallback, this));
  AddStateTask(ESessionState::ESessionState_AD_FALLBACK, 1, nullptr,
               std::bind(&driver_brain_processor::ReportFallback2Driver, this));
  AddStateTask(ESessionState::ESessionState_AD_FALLBACK, 2, nullptr,
               std::bind(&driver_brain_processor::StartFallBackTimer, this));
  AddStateTask(ESessionState::ESessionState_AD_FALLBACK, 3,
               std::bind(&driver_brain_processor::GetDriverTakeOver, this),
               std::bind(&driver_brain_processor::EnterAdPrepare, this));
  AddStateTask(
      ESessionState::ESessionState_AD_FALLBACK, 4,
      std::bind(&driver_brain_processor::GetFallbackTimeOut, this),
      std::bind(&driver_brain_processor::CallManagedModuleAdMrc, this));
  AddStateTask(ESessionState::ESessionState_AD_FALLBACK, 5,
               std::bind(&driver_brain_processor::WaitManagedModuleAdMrc, this),
               std::bind(&driver_brain_processor::EnterAdMrc, this));
  AddStateTask(ESessionState::ESessionState_AD_FALLBACK, 6, nullptr,
               std::bind(&driver_brain_processor::branch_finish, this));
  std::vector<int> chain_fallback1 = {0, 1, 2, 3, 6};
  std::vector<int> chain_fallback2 = {0, 1, 2, 4, 5, 6};
  ChainStateTask(ESessionState::ESessionState_AD_FALLBACK, chain_fallback1);
  ChainStateTask(ESessionState::ESessionState_AD_FALLBACK, chain_fallback2);
  SetTaskBranch(ESessionState::ESessionState_AD_FALLBACK, TaskBranch_ParallelOr,
                2, 6, 1);
  ////// ad mrc
  AddState(ESessionState::ESessionState_AD_MRC);
  AddStateTask(ESessionState::ESessionState_AD_MRC, 0,
               std::bind(&driver_brain_processor::GetDriverTakeOver, this),
               std::bind(&driver_brain_processor::EnterAdPrepare, this));
  ////
  FinishChainTask();

  init(ESessionState::ESessionState_IDLE);
  driver_brain_sm_run();
}
int driver_brain_processor::EnterIdle() {
  smm.next_state = ESessionState_IDLE;
  hmi_proxy_state_req_ = ESessionState_IDLE;
  return workResult_Succ;
}
int driver_brain_processor::EnterInit() {
  smm.next_state = ESessionState_INIT;
  hmi_proxy_state_req_ = ESessionState_INIT;
  return workResult_Succ;
}
int driver_brain_processor::EnterReady() {
  smm.next_state = ESessionState_READY;
  hmi_proxy_state_req_ = ESessionState_READY;
  return workResult_Succ;
}
int driver_brain_processor::EnterTerminate() {
  smm.next_state = ESessionState_TERMINATE;
  hmi_proxy_state_req_ = ESessionState_TERMINATE;
  return workResult_Succ;
}
int driver_brain_processor::EnterAdPrepare() {
  smm.next_state = ESessionState_AD_PREPARE;
  hmi_proxy_state_req_ = ESessionState_AD_PREPARE;
  return workResult_Succ;
}
int driver_brain_processor::EnterAdReady() {
  smm.next_state = ESessionState_AD_READY;
  hmi_proxy_state_req_ = ESessionState_AD_READY;
  return workResult_Succ;
}
int driver_brain_processor::EnterAdExit() {
  smm.next_state = ESessionState_AD_EXIT;
  hmi_proxy_state_req_ = ESessionState_AD_EXIT;
  return workResult_Succ;
}
int driver_brain_processor::EnterAdEngage() {
  smm.next_state = ESessionState_AD_ENGAGE;
  hmi_proxy_state_req_ = ESessionState_AD_ENGAGE;
  return workResult_Succ;
}
int driver_brain_processor::EnterAdRun() {
  smm.next_state = ESessionState_AD_RUN;
  hmi_proxy_state_req_ = ESessionState_AD_RUN;
  return workResult_Succ;
}
int driver_brain_processor::EnterAdDisengage() {
  smm.next_state = ESessionState_AD_DISENGAGE;
  hmi_proxy_state_req_ = ESessionState_AD_DISENGAGE;
  return workResult_Succ;
}
int driver_brain_processor::EnterFallback() {
  smm.next_state = ESessionState_AD_FALLBACK;
  hmi_proxy_state_req_ = ESessionState_AD_FALLBACK;
  return workResult_Succ;
}
int driver_brain_processor::EnterAdMrc() {
  smm.next_state = ESessionState_AD_MRC;
  hmi_proxy_state_req_ = ESessionState_AD_MRC;
  return workResult_Succ;
}

int driver_brain_processor::MonitorInitRequest() {
  int result = workResult_Succ;
  return result;
}
int driver_brain_processor::CallManagedModuleInit() {
  int result = workResult_Succ;
  autonomy_brain_state_req_ = ESessionState_INIT;
  return result;
}
int driver_brain_processor::WaitManagedModuleReady() {
  int result = workResult_Fail;
  if (autonomy_brain_state_cur_ == ESessionState_READY) {
    result = workResult_Succ;
  }
  return result;
}
int driver_brain_processor::InitProcess() {
  int result = workResult_Succ;
  return result;
}
int driver_brain_processor::MonitorAdPrepareRequest() {
  int result = workResult_Fail;
  if (driver_brain_state_req_ == ESessionState_AD_PREPARE) {
    result = workResult_Succ;
  }
  return result;
}
int driver_brain_processor::MonitorTerminateRequest() {
  int result = workResult_Fail;
  if (driver_brain_state_req_ == ESessionState_TERMINATE) {
    result = workResult_Succ;
  }
  return result;
}
int driver_brain_processor::CallManagedModuleTerminate() {
  int result = workResult_Succ;
  hmi_proxy_state_req_ = ESessionState_TERMINATE;
  return result;
}
int driver_brain_processor::WaitManagedModuleIdle() {
  int result = workResult_Fail;
  if (autonomy_brain_state_cur_ == ESessionState_IDLE) {
    result = workResult_Succ;
  }
  return result;
}
int driver_brain_processor::TerminateProcess() {
  int result = workResult_Succ;
  return result;
}
int driver_brain_processor::GetOddOk() {
  int result = workResult_Succ;
  if (true == out_odd_autonomy_ || true == out_odd_driver_) {
    out_odd_total_ = true;
    result = workResult_Fail;
  } else {
    out_odd_total_ = false;
  }
  return result;
}
int driver_brain_processor::CallManagedModuleAdRrepare() {
  int result = workResult_Succ;
  autonomy_brain_state_req_ = ESessionState_AD_PREPARE;
  return result;
}
int driver_brain_processor::WaitManagedModuleAdReady() {
  int result = workResult_Fail;
  if (autonomy_brain_state_cur_ == ESessionState_AD_READY) {
    result = workResult_Succ;
  }
  return result;
}
int driver_brain_processor::AdPrepareProcess() {
  int result = workResult_Succ;
  return result;
}
int driver_brain_processor::MonitorAdExitRequest() {
  int result = workResult_Fail;
  if (driver_brain_state_req_ == ESessionState_AD_EXIT) {
    result = workResult_Succ;
  }
  return result;
}
int driver_brain_processor::MonitorAdEngageRequest() {
  int result = workResult_Fail;
  if (driver_brain_state_req_ == ESessionState_AD_ENGAGE) {
    result = workResult_Succ;
  }
  return result;
}
////////////////ad exit
int driver_brain_processor::CallManagedModuleAdExit() {
  int result = workResult_Succ;
  autonomy_brain_state_req_ = ESessionState_AD_EXIT;
  return result;
}
int driver_brain_processor::AdExitProcess() {
  int result = workResult_Succ;
  return result;
}
///////////ad engage
int driver_brain_processor::CallManagedModuleAdEngage() {
  int result = workResult_Succ;
  autonomy_brain_state_req_ = ESessionState_AD_ENGAGE;
  return result;
}
int driver_brain_processor::WaitManagedModuleAdRun() {
  int result = workResult_Fail;
  if (autonomy_brain_state_cur_ == ESessionState_AD_RUN) {
    result = workResult_Succ;
  }
  return result;
}
int driver_brain_processor::AdEngageProcess() {
  int result = workResult_Fail;
  return result;
}
////////ad run
int driver_brain_processor::MonitorAdDisengageRequest() {
  int result = workResult_Fail;
  if (driver_brain_state_req_ == ESessionState_AD_DISENGAGE) {
    result = workResult_Succ;
  }
  return result;
}
int driver_brain_processor::AdRunProcess() {
  int result = workResult_Succ;
  return result;
}
////////ad disengage
int driver_brain_processor::CallManagedModuleAdDisengage() {
  int result = workResult_Succ;
  autonomy_brain_state_req_ = ESessionState_AD_DISENGAGE;
  return result;
}
int driver_brain_processor::WaitManagedModuleAdDisengaged() {
  int result = workResult_Fail;
  if (autonomy_brain_state_cur_ == ESessionState_AD_READY ||
      autonomy_brain_state_cur_ == ESessionState_AD_PREPARE) {
    result = workResult_Succ;
  }
  return result;
}
int driver_brain_processor::WaitManagedModuleAdMrc() {
  int result = workResult_Fail;
  if (autonomy_brain_state_cur_ == ESessionState_AD_MRC) {
    result = workResult_Succ;
  }
  return result;
}
int driver_brain_processor::AdDisengageProcess() {
  int result = workResult_Succ;
  return result;
}
/////////ad fallback
int driver_brain_processor::CallManagedModuleFallback() {
  int result = workResult_Succ;
  autonomy_brain_state_req_ = ESessionState_AD_FALLBACK;
  return result;
}
int driver_brain_processor::ReportFallback2Driver() {
  int result = workResult_Succ;
  return result;
}
static bool is_time_out;
static void TimeOutCallBack() { is_time_out = true; }
int driver_brain_processor::StartFallBackTimer() {
  int result = workResult_Succ;
  is_time_out = false;

  sa_.sa_handler = __sighandler_t(&TimeOutCallBack);
  sigaction(SIGALRM, &sa_, nullptr);

  timer_.it_value.tv_sec = TimeOut_DriverTakeOver_S;
  timer_.it_value.tv_usec = 0;
  timer_.it_interval.tv_sec = 0;
  timer_.it_interval.tv_usec = 0;
  setitimer(ITIMER_REAL, &timer_, nullptr);
  return result;
}
int driver_brain_processor::GetDriverTakeOver() {
  int result = workResult_Succ;
  if (request_engage_autonomy_ == false) {
    sa_.sa_handler = nullptr;
    sigaction(SIGALRM, &sa_, nullptr);

    timer_.it_value.tv_sec = 0;
    timer_.it_value.tv_usec = 0;
    timer_.it_interval.tv_sec = 0;
    timer_.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer_, nullptr);
  } else {
    result = workResult_Fail;
  }
  return result;
}
int driver_brain_processor::GetFallbackTimeOut() {
  int result = workResult_Succ;
  if (is_time_out == true && request_engage_autonomy_ == true) {
    sa_.sa_handler = nullptr;
    sigaction(SIGALRM, &sa_, nullptr);

    timer_.it_value.tv_sec = 0;
    timer_.it_value.tv_usec = 0;
    timer_.it_interval.tv_sec = 0;
    timer_.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer_, nullptr);
  } else {
    result = workResult_Fail;
  }
  return result;
}
int driver_brain_processor::CallManagedModuleAdMrc() {
  int result = workResult_Succ;
  autonomy_brain_state_req_ = ESessionState_AD_MRC;
  return result;
}

void driver_brain_processor::driver_brain_sm_run() {
  if (request_engage_autonomy_ == true) {
    switch (driver_brain_state_cur_) {
      case ESessionState_READY:
        driver_brain_state_req_ = ESessionState_AD_PREPARE;
        break;
      case ESessionState_AD_READY:
        driver_brain_state_req_ = ESessionState_AD_ENGAGE;
        break;
      default:
        break;
    }
  } else {
    switch (driver_brain_state_cur_) {
      case ESessionState_READY:
        driver_brain_state_req_ = ESessionState_AD_PREPARE;
        break;
      case ESessionState_AD_RUN:
      case ESessionState_AD_FALLBACK:
      case ESessionState_AD_MRC:
        driver_brain_state_req_ = ESessionState_AD_DISENGAGE;
        break;
      default:
        break;
    }
  }

  sm_go();

  driver_brain_state_cur_ =
      static_cast<interfaces::vsp::AdSession::ESessionState>(smm.curr_state);
}
}  // namespace AdSession
}  // namespace vsp
