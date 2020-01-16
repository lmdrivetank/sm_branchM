// Copyright 2019 Inceptio Technology. All Rights Reserved.
// Authors:
//    Ming Li (ming.li@inceptio.ai)

//#include "vsp/AutoDriveSession/autonomy_brain.h"
#include "autonomy_brain.h"
#include <memory>
#include <utility>
#include "irs_common/base/irs_message.h"
#include "irs_common/irs_node.h"
#include "irs_common/topics/irs_topics.h"
#include "utils/base/time.h"

namespace vsp {
namespace AdSession {
autonomy_brain::autonomy_brain(const std::string& node_name,
                               const irscpp::NodeOptions& options)
    : IRSNode(node_name, options), autonomy_brain_processor() {
  autonomy_brain_cur_status_ = CREATE_MSG_UNIQUE(autonomy_brain_cur_status);
  vehicle_brain_req_status_ = CREATE_MSG_UNIQUE(vehicle_brain_req_status);
  autonomy_x86_req_status_ = CREATE_MSG_UNIQUE(autonomy_x86_req_status);
}

void autonomy_brain::InitInternal(const irscpp::NodeOptions& /*options*/) {
  autonomy_x86_cur_status_sub_ = CreateSubscription<autonomy_x86_cur_status>(
      irs_common::topics::kVspAutonomyStatus,
      std::bind(&autonomy_brain::MonitorCallback_VP, this,
                std::placeholders::_1),
      KEEP_LATEST_MSG);

  vehicle_brain_cur_status_sub_ = CreateSubscription<vehicle_brain_cur_status>(
      irs_common::topics::kVehicleBrainStatus,
      std::bind(&autonomy_brain::MonitorCallback_VB, this,
                std::placeholders::_1),
      KEEP_LATEST_MSG);

  autonomy_brain_req_status_sub_ =
      CreateSubscription<autonomy_brain_req_status>(
          irs_common::topics::kAutonomyBrainReq,
          std::bind(&autonomy_brain::RequestCallback_DB, this,
                    std::placeholders::_1),
          KEEP_LATEST_MSG);

  autonomy_brain_cur_status_pub_ = CreatePublisher<autonomy_brain_cur_status>(
      irs_common::topics::kAutonomyBrainStatus, KEEP_LATEST_MSG);

  vehicle_brain_req_status_pub_ = CreatePublisher<vehicle_brain_req_status>(
      irs_common::topics::kVehicleBrainReq, KEEP_LATEST_MSG);

  autonomy_x86_req_status_pub_ = CreatePublisher<autonomy_x86_req_status>(
      irs_common::topics::kAutonomyStateReq_x86, KEEP_LATEST_MSG);

  autonomy_brain_processor::InitInternal();

  IRS_INFO("autonomy brain init succeed!");

  AddPeriodicExecutive(1000, [this]() {
    this->autonomy_brain_sm_run();
    this->PublishMsg();
  });
}
void autonomy_brain::MonitorCallback_VB(
    std::shared_ptr<vehicle_brain_cur_status> msg) {
  int64_t now = 0;
  if (!utils::base::get_current_system_time_nano(&now)) {
    IRS_ERROR("Failed to get current steady time.");
    return;
  }
  if ((vehicle_brain_state_cur_ != msg->current_state) ||
      (out_odd_vehicle_ != msg->out_of_odd) ||
      (worst_sid_vehicle_ != msg->worst_sid)) {
    IRS_INFO("autonomu brain monitor vehicle brain state %d",
             msg->current_state);
    out_odd_vehicle_ = msg->out_of_odd;
    worst_sid_vehicle_ = msg->worst_sid;
    vehicle_brain_state_cur_ = msg->current_state;
  }
}

void autonomy_brain::RequestCallback_DB(
    std::shared_ptr<autonomy_brain_req_status> msg) {
  int64_t now = 0;
  if (!utils::base::get_current_system_time_nano(&now)) {
    IRS_ERROR("Failed to get current steady time.");
    return;
  }
  if (autonomy_brain_state_req_ != msg->autonomy_brain_request_state) {
    IRS_INFO("autonomu brain recv driver brain request_state %d",
             msg->autonomy_brain_request_state);
    autonomy_brain_state_req_ = msg->autonomy_brain_request_state;
  }
}

void autonomy_brain::MonitorCallback_VP(
    std::shared_ptr<autonomy_x86_cur_status> msg) {
  int64_t now = 0;
  if (!utils::base::get_current_system_time_nano(&now)) {
    IRS_ERROR("Failed to get current steady time.");
    return;
  }
  if ((x86_state_cur_ != msg->current_state) ||
      (out_odd_x86_ != msg->out_of_odd) ||
      (worst_sid_x86_ != msg->worst_severity)) {
    out_odd_x86_ = msg->out_of_odd;
    worst_sid_x86_ = msg->worst_severity;
    x86_state_cur_ = msg->current_state;
  }
}

void autonomy_brain::PublishMsg() {
  int64_t now = 0;
  const auto succeed = utils::base::get_current_steady_time_nano(&now);
  if (succeed) {
    autonomy_brain_cur_status_->current_state = autonomy_brain_state_cur_;
    autonomy_brain_cur_status_->request_state = autonomy_brain_state_req_;
    autonomy_brain_cur_status_->out_of_odd = out_odd_autonomy_;
    autonomy_brain_cur_status_->worst_sid = worst_sid_autonomy_;
    autonomy_brain_cur_status_pub_->Publish(autonomy_brain_cur_status_);
    IRS_INFO("autonomy brain: current state %d; request state %d!",
             autonomy_brain_state_cur_, autonomy_brain_state_req_);

    vehicle_brain_req_status_->vehicle_manager_request_state =
        vehicle_brain_state_req_;
    vehicle_brain_req_status_pub_->Publish(vehicle_brain_req_status_);
    IRS_INFO("autonomy brain: vehicle request state %d!",
             vehicle_brain_state_req_);

    if (x86_state_req_ == ESessionState_AD_ENGAGE ||
        x86_state_req_ == ESessionState_AD_RUN ||
        x86_state_req_ == ESessionState_AD_DISENGAGE ||
        x86_state_req_ == ESessionState_AD_FALLBACK ||
        x86_state_req_ == ESessionState_AD_MRC) {
      autonomy_x86_req_status_->should_in_autonomy = true;
    } else {
      autonomy_x86_req_status_->should_in_autonomy = false;
    }
    if (out_odd_autonomy_ == true) {
      autonomy_x86_req_status_->fault_exist = true;
    }
    autonomy_x86_req_status_pub_->Publish(autonomy_x86_req_status_);
  }
}
}  // namespace AdSession
}  // namespace vsp
REGISTER_IRS_NODE(vsp::AdSession::autonomy_brain)
