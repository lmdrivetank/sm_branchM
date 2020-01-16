// Copyright 2019 Inceptio Technology. All Rights Reserved.
// Authors:
//    Ming Li (ming.li@inceptio.ai)

#ifndef _DRIVER_BRAIN_PROCESSOR_H_
#define _DRIVER_BRAIN_PROCESSOR_H_

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
#define TimeOut_DriverTakeOver_S 10
class driver_brain_processor : public vsp::ssm_task {
 public:
  driver_brain_processor() = default;
  ~driver_brain_processor() = default;

  void InitInternal();

 public:
  ESessionState driver_brain_state_req_ = ESessionState_IDLE;
  ESessionState driver_brain_state_cur_ = ESessionState_IDLE;
  ESessionState autonomy_brain_state_req_ = ESessionState_IDLE;
  ESessionState autonomy_brain_state_cur_ = ESessionState_Count;
  ESessionState hmi_proxy_state_req_ = ESessionState_IDLE;
  ESessionState hmi_proxy_state_cur_ = ESessionState_Count;
  bool request_engage_autonomy_ = false;
  bool current_engage_autonomy_ = false;

  DDS_Boolean out_odd_total_ = false;
  DDS_Boolean out_odd_driver_ = false;
  DDS_Boolean out_odd_autonomy_ = false;
  short worst_sid_total_ = 0;
  short worst_sid_driver_ = 0;
  short worst_sid_autonomy_ = 0;

  struct itimerval timer_;
  struct sigaction sa_;

 public:
  void driver_brain_sm_run();
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
  int AdRunProcess();
  /// ad disengage
  int EnterAdDisengage();
  int CallManagedModuleAdDisengage();
  int WaitManagedModuleAdDisengaged();
  int AdDisengageProcess();
  /// ad fallback
  int EnterFallback();
  int CallManagedModuleFallback();
  int ReportFallback2Driver();
  int StartFallBackTimer();
  int GetDriverTakeOver();   // ad prepare
  int GetFallbackTimeOut();  // ad mrc
  int CallManagedModuleAdMrc();
  int WaitManagedModuleAdMrc();
  // ad mrc
  int EnterAdMrc();
  // int GetDriverTakeOver();//ad prepare
};
}  // namespace AdSession
}  // namespace vsp
#endif
