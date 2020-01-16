// Copyright 2019 Inceptio Technology. All Rights Reserved.
// Authors:
//    Ming Li (ming.li@inceptio.ai)

#ifndef _VEHICLE_BRAIN_PROCESSOR_H_
#define _VEHICLE_BRAIN_PROCESSOR_H_

#include <sys/time.h>
#include <time.h>
#include <memory>
#include <string>
#include "interfaces/vsp/AdSession/AdSession_type_support.h"
#include "signal.h"
#include "ssm_task.h"

namespace vsp {
namespace AdSession {

using namespace interfaces::vsp::AdSession;
#define Max2(x, y) x > y ? x : y
#define Max3(x, y, z) Max2(Max2(x, y), z)

class vehicle_brain_processor : public vsp::ssm_task {
 public:
  vehicle_brain_processor() = default;
  ~vehicle_brain_processor() = default;

  void InitInternal();

 public:
  ESessionState vehicle_brain_state_req_ = ESessionState_IDLE;
  ESessionState vehicle_brain_state_cur_ = ESessionState_IDLE;
  ESessionState vci_state_cur_ = ESessionState_Count;
  ESessionState vci_state_req_ = ESessionState_IDLE;

  DDS_Boolean out_odd_vehicle_ = false;
  DDS_Boolean out_odd_vci_ = false;
  short worst_sid_vehicle_ = 0;
  short worst_sid_vci_ = 0;

  struct itimerval timer_;
  struct sigaction sa_;

 public:
  void vehicle_brain_sm_run();
  /// idle
  int EnterIdle();
  int MonitorInitRequest();
  /// init
  int EnterInit();
  int CallManagedModuleInit();
  int WaitManagedModuleReady();
  int InitProcess();
  /// terminate
  int EnterTerminate();
  int CallManagedModuleTerminate();
  int WaitManagedModuleIdle();
  int TerminateProcess();
  /// ready
  int EnterReady();
  int MonitorAdPrepareRequest();
  int MonitorTerminateRequest();
  /// ad prepare
  int EnterAdPrepare();
  int CallManagedModuleAdRrepare();
  int WaitManagedModuleAdReady();
  int AdPrepareProcess();
  int GetOddOk();  // ad ready
  int MonitorAdExitRequest();
  /// ad exit
  int EnterAdExit();
  int CallManagedModuleAdExit();
  int AdExitProcess();  // ad ready
  /// ad ready
  int EnterAdReady();
  int MonitorAdEngageRequest();  // ad engage
  // int MonitorAdExitRequest();
  /// ad engage
  int EnterAdEngage();
  int CallManagedModuleAdEngage();
  int WaitManagedModuleAdRun();
  int AdEngageProcess();
  // ad disengage
  int EnterAdDisengage();
  int CallManagedModuleAdDisengage();
  int WaitManagedModuleAdDisengaged();
  int AdDisengageProcess();
  /// ad run
  int EnterAdRun();
  int MonitorAdDisengageRequest();
  int MonitorAdFallbackRequest();
  int AdRunProcess();
  /// ad fallback
  int EnterFallback();
  int CallManagedModuleFallback();
  //  int MonitorAdPrepareRequest();
  int MonitorAdMrc();
  // ad mrc
  int EnterAdMrc();
  int CallManagedModuleAdMrc();
  //   int MonitorAdPrepareRequest();
};
}  // namespace AdSession
}  // namespace vsp
#endif
