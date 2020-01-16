// Copyright 2019 Inceptio Technology. All Rights Reserved.
// Authors:
//    Ming Li (ming.li@inceptio.ai)

#include "vehicle_brain.h"
#include <memory>
#include <utility>
#include "irs_common/base/irs_message.h"
#include "irs_common/irs_node.h"
#include "irs_common/topics/irs_topics.h"
#include "utils/base/time.h"

namespace vsp {
namespace AdSession {
vehicle_brain::vehicle_brain(const std::string& node_name,
                             const irscpp::NodeOptions& options)
    : IRSNode(node_name, options), vehicle_brain_processor() {
  vehicle_brain_status_ = CREATE_MSG_UNIQUE(vehicle_brain_cur_status);
  vci_req_status_ = CREATE_MSG_UNIQUE(vci_req_status);
}

void vehicle_brain::InitInternal(const irscpp::NodeOptions& /*options*/) {
  vehicle_brain_req_status_sub_ = CreateSubscription<vehicle_brain_req_status>(
      irs_common::topics::kVehicleBrainReq,
      std::bind(&vehicle_brain::RequestCallback, this, std::placeholders::_1),
      KEEP_LATEST_MSG);

  vci_cur_status_sub_ = CreateSubscription<vci_cur_status>(
      irs_common::topics::kVciCurrentState,
      std::bind(&vehicle_brain::MonitorCallback, this, std::placeholders::_1),
      KEEP_LATEST_MSG);

  vehicle_brain_cur_status_pub_ = CreatePublisher<vehicle_brain_cur_status>(
      irs_common::topics::kVehicleBrainStatus, KEEP_LATEST_MSG);

  vci_req_status_pub_ = CreatePublisher<vci_req_status>(
      irs_common::topics::kVciRequestState, KEEP_LATEST_MSG);

  vehicle_brain_processor::InitInternal();

  IRS_INFO("vehicle brain init succeed!");
  AddPeriodicExecutive(1000, [this]() {
    this->vehicle_brain_sm_run();
    this->PublishMsg();
  });
}  // namespace AdSession

void vehicle_brain::RequestCallback(
    std::shared_ptr<vehicle_brain_req_status> msg) {
  int64_t now = 0;
  if (!utils::base::get_current_system_time_nano(&now)) {
    IRS_ERROR("Failed to get current steady time.");
    return;
  }

  if (vehicle_brain_state_req_ != msg->vehicle_manager_request_state) {
    IRS_INFO("vehicle recv request state %d.",
             msg->vehicle_manager_request_state);
    vehicle_brain_state_req_ = msg->vehicle_manager_request_state;
  }
}

void vehicle_brain::MonitorCallback(std::shared_ptr<vci_cur_status> msg) {
  int64_t now = 0;
  if (!utils::base::get_current_system_time_nano(&now)) {
    IRS_ERROR("Failed to get current steady time.");
    return;
  }

  if ((vci_state_cur_ != msg->current_state) ||
      (out_odd_vci_ != msg->out_of_odd) || (worst_sid_vci_ != msg->worst_sid)) {
    IRS_INFO("vehicle brain monitor vci_state %d", msg->current_state);
    vci_state_cur_ = msg->current_state;
    out_odd_vci_ = msg->out_of_odd;
    worst_sid_vci_ = msg->worst_sid;
  }
}

void vehicle_brain::PublishMsg() {
  int64_t now = 0;
  const auto succeed = utils::base::get_current_steady_time_nano(&now);
  if (succeed) {
    vehicle_brain_status_->current_state = vehicle_brain_state_cur_;
    vehicle_brain_status_->request_state = vehicle_brain_state_req_;
    vehicle_brain_status_->out_of_odd = out_odd_vehicle_;
    vehicle_brain_status_->worst_sid = worst_sid_vehicle_;
    vehicle_brain_cur_status_pub_->Publish(vehicle_brain_status_);

    if (vci_req_status_->request_state != vci_state_req_) {
      vci_req_status_->request_state = vci_state_req_;
      vci_req_status_pub_->Publish(vci_req_status_);
    }

    IRS_INFO("vehicle brain: current state %d; vci request state %d!",
             vehicle_brain_state_cur_, vci_state_req_);
  }
}
}  // namespace AdSession
}  // namespace vsp
REGISTER_IRS_NODE(vsp::AdSession::vehicle_brain)
