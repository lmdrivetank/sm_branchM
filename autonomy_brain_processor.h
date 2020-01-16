// Copyright 2019 Inceptio Technology. All Rights Reserved.
// Authors:
//    Ming Li (ming.li@inceptio.ai)

#ifndef _ATUONOMY_BRAIN_PROCESSOR_H_
#define _ATUONOMY_BRAIN_PROCESSOR_H_

#include <sys/time.h>
#include <time.h>
#include <memory>
#include <string>
#include "interfaces/vsp/AdSession/autonomy_brain_type_support.h"
#include "signal.h"
#include "ssm_task.h"

namespace vsp {
namespace AdSession {
using namespace interfaces::vsp::AdSession;
#define Max2(x, y) x > y ? x : y
#define Max3(x, y, z) Max2(Max2(x, y), z)
class autonomy_brain_processor : public vsp::ssm_task {
 public:
  autonomy_brain_processor() = default;
  ~autonomy_brain_processor() {}

  void InitInternal();

 public:
  ESessionState autonomy_brain_state_req_ = ESessionState_IDLE;
  ESessionState autonomy_brain_state_cur_ = ESessionState_IDLE;
  ESessionState vehicle_brain_state_req_ = ESessionState_READY;
  ESessionState vehicle_brain_state_cur_ = ESessionState_Count;
  ESessionState x86_state_req_ = ESessionState_IDLE;
  interfaces::onboard::EAutonomyState x86_state_cur_ =
      interfaces::onboard::STATE_UNKNOWN;

  DDS_Boolean out_odd_vehicle_ = false;
  DDS_Boolean out_odd_x86_ = false;
  DDS_Boolean out_odd_autonomy_ = false;
  short worst_sid_vehicle_ = 0;
  short worst_sid_x86_ = 0;
  short worst_sid_autonomy_ = 0;

  struct itimerval timer_;
  struct sigaction sa_;

 public:
  void autonomy_brain_sm_run();
  /// idle
  int EnterIdle();
  int MonitorInitRequest();
  /// init
  int EnterInit();
  int CallManagedModuleInit();
  int WaitManagedModuleReady();
  int InitProcess();
  /// ready
  int EnterReady();
  int MonitorAdPrepareRequest();
  int MonitorTerminateRequest();
  /// terminate
  int EnterTerminate();
  int CallManagedModuleTerminate();
  int WaitManagedModuleIdle();
  int TerminateProcess();
  /// ad prepare
  int EnterAdPrepare();
  int CallManagedModuleAdRrepare();
  int WaitManagedModuleAdReady();
  int AdPrepareProcess();
  int GetOddOk();  // ad ready
  int MonitorAdExitRequest();
  /// ad ready
  int EnterAdReady();
  int MonitorAdEngageRequest();  // ad engage
  // int MonitorAdExitRequest();
  /// ad exit
  int EnterAdExit();
  int CallManagedModuleAdExit();
  int AdExitProcess();  // ad ready
  /// ad engage
  int EnterAdEngage();
  int CallManagedModuleAdEngage();
  int WaitManagedModuleAdRun();
  int AdEngageProcess();
  /// ad run
  int EnterAdRun();
  int MonitorAdDisengageRequest();
  int MonitorAdFallbackRequest();
  int AdRunProcess();
  // ad disengage
  int EnterAdDisengage();
  int CallManagedModuleAdDisengage();
  int WaitManagedModuleAdDisengaged();
  int AdDisengageProcess();
  /// ad fallback
  int EnterFallback();
  int CallManagedModuleFallback();
  //  int MonitorAdPrepareRequest();
  int CallManagedModuleAdMrc();
  int WaitManagedModuleAdMrc();
  int MonitorAdMrc();
  // ad mrc
  int EnterAdMrc();
  //   int MonitorAdPrepareRequest();
};
}  // namespace AdSession
}  // namespace vsp
#endif  // ATUONOMY_BRAIN_H_
