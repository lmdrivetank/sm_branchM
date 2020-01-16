// Copyright 2019 Inceptio Technology. All Rights Reserved.
// Authors:
//    Ming Li (ming.li@inceptio.ai)

#ifndef _VEHICLE_BRAIN_H_
#define _VEHICLE_BRAIN_H_

#include <sys/time.h>
#include <time.h>
#include <memory>
#include <string>
#include "interfaces/vsp/AdSession/vehicle_manager_type_support.h"
#include "interfaces/vsp/datalink/vci/vci_state_type_support.h"
#include "irs_common/irs_node.h"
#include "utils/base/time.h"
#include "vehicle_brain_processor.h"
namespace vsp {
namespace AdSession {
class vehicle_brain : public irs_common::IRSNode,
                      public vehicle_brain_processor {
 public:
  vehicle_brain(const std::string& node_name,
                const irscpp::NodeOptions& options = irscpp::NodeOptions());
  ~vehicle_brain() {}

  void InitInternal(const irscpp::NodeOptions& /*options*/) override;

 private:
#define vehicle_brain_cur_status vehicle_manager::vehicle_manager_status
#define vehicle_brain_req_status vehicle_manager::vehicle_manager_state_request
#define vci_cur_status interfaces::vsp::vci::VciCurrentState
#define vci_req_status interfaces::vsp::vci::VciRequestState
 private:
  void RequestCallback(std::shared_ptr<vehicle_brain_req_status> msg);
  void MonitorCallback(std::shared_ptr<vci_cur_status> msg);

  void PublishMsg();

 private:
  std::unique_ptr<vehicle_brain_cur_status> vehicle_brain_status_;
  std::unique_ptr<vci_req_status> vci_req_status_;

 private:
  irs_common::IRSPublisher<vehicle_brain_cur_status>::SharedPtr
      vehicle_brain_cur_status_pub_;
  irs_common::IRSPublisher<vci_req_status>::SharedPtr vci_req_status_pub_;

  irs_common::IRSSubscription<vehicle_brain_req_status>::SharedPtr
      vehicle_brain_req_status_sub_;
  irs_common::IRSSubscription<vci_cur_status>::SharedPtr vci_cur_status_sub_;

  PROHIBIT_COPY_AND_ASSIGN(vehicle_brain);
};
}  // namespace AdSession
}  // namespace vsp
#endif
