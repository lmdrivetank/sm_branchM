// Copyright 2019 Inceptio Technology. All Rights Reserved.
// Authors:
//    Ming Li (ming.li@inceptio.ai)

#ifndef _ATUONOMY_BRAIN_H_
#define _ATUONOMY_BRAIN_H_

#include <sys/time.h>
#include <time.h>
#include <memory>
#include <string>

#include "autonomy_brain_processor.h"
#include "interfaces/vsp/AdSession/autonomy_brain_type_support.h"
#include "interfaces/vsp/AdSession/vehicle_manager_type_support.h"
#include "interfaces/vsp/datalink/vsp_proxy/vsp_report_type_support.h"
#include "irs_common/irs_node.h"
#include "utils/base/time.h"

namespace vsp {
namespace AdSession {
using namespace interfaces::vsp::AdSession;
class autonomy_brain : public irs_common::IRSNode,
                       public autonomy_brain_processor {
 public:
  autonomy_brain(const std::string& node_name,
                 const irscpp::NodeOptions& options = irscpp::NodeOptions());
  ~autonomy_brain() {}

  void InitInternal(const irscpp::NodeOptions& /*options*/) override;

 private:
  /// publish
#define autonomy_brain_cur_status \
  interfaces::vsp::AdSession::autonomy_brain::autonomy_brain_status
#define vehicle_brain_req_status \
  interfaces::vsp::AdSession::vehicle_manager::vehicle_manager_state_request
#define autonomy_x86_req_status interfaces::vsp::vspProxy::AutonomyStatus
// subscript
#define autonomy_brain_req_status \
  interfaces::vsp::AdSession::autonomy_brain::autonomy_brain_state_request
#define vehicle_brain_cur_status \
  interfaces::vsp::AdSession::vehicle_manager::vehicle_manager_status
#define autonomy_x86_cur_status interfaces::vsp::vspProxy::AutonomyStatus

 private:
  void RequestCallback_DB(std::shared_ptr<autonomy_brain_req_status> msg);
  void MonitorCallback_VB(std::shared_ptr<vehicle_brain_cur_status> msg);
  void MonitorCallback_VP(std::shared_ptr<autonomy_x86_cur_status> msg);

  void PublishMsg();

 private:
  std::unique_ptr<autonomy_brain_cur_status> autonomy_brain_cur_status_;
  std::unique_ptr<vehicle_brain_req_status> vehicle_brain_req_status_;
  std::unique_ptr<autonomy_x86_req_status> autonomy_x86_req_status_;

 private:
  irs_common::IRSPublisher<autonomy_brain_cur_status>::SharedPtr
      autonomy_brain_cur_status_pub_;
  irs_common::IRSPublisher<vehicle_brain_req_status>::SharedPtr
      vehicle_brain_req_status_pub_;
  irs_common::IRSPublisher<autonomy_x86_req_status>::SharedPtr
      autonomy_x86_req_status_pub_;

  irs_common::IRSSubscription<autonomy_x86_cur_status>::SharedPtr
      autonomy_x86_cur_status_sub_;
  irs_common::IRSSubscription<vehicle_brain_cur_status>::SharedPtr
      vehicle_brain_cur_status_sub_;
  irs_common::IRSSubscription<autonomy_brain_req_status>::SharedPtr
      autonomy_brain_req_status_sub_;

  PROHIBIT_COPY_AND_ASSIGN(autonomy_brain);
};
}  // namespace AdSession
}  // namespace vsp
#endif  // ATUONOMY_BRAIN_H_
