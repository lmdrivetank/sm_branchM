// Copyright 2019 Inceptio Technology. All Rights Reserved.
// Authors:
//    Ming Li (ming.li@inceptio.ai)

#include "driver_brain.h"
#include <memory>
#include <utility>
#include "irs_common/base/irs_message.h"
#include "irs_common/irs_node.h"
#include "irs_common/topics/irs_topics.h"
#include "utils/base/time.h"

namespace vsp {
namespace AdSession {
driver_proxy::driver_proxy(const std::string& node_name,
                           const irscpp::NodeOptions& options)
    : IRSNode(node_name, options), driver_brain_processor() {
  driver_proxy_cur_status_ = CREATE_MSG_UNIQUE(driver_proxy_cur_status);
  autonomy_brain_req_status_ = CREATE_MSG_UNIQUE(autonomy_brain_req_status);
  hmi_proxy_req_status_ = CREATE_MSG_UNIQUE(hmi_proxy_req_status);
}

void driver_proxy::InitInternal(const irscpp::NodeOptions& /*options*/) {
  autonomy_brain_cur_status_sub_ =
      CreateSubscription<autonomy_brain_cur_status>(
          irs_common::topics::kAutonomyBrainStatus,
          std::bind(&driver_proxy::MonitorCallback_AB, this,
                    std::placeholders::_1),
          KEEP_LATEST_MSG);

  hmi_proxy_cur_status_sub_ = CreateSubscription<hmi_proxy_cur_status>(
      irs_common::topics::kHmiProxyCurrentState,
      std::bind(&driver_proxy::MonitorCallback_HP, this, std::placeholders::_1),
      KEEP_LATEST_MSG);

  hmi_input_cur_status_sub_ = CreateSubscription<hmi_input_cur_status>(
      irs_common::topics::kVspHmiInput,
      std::bind(&driver_proxy::MonitorCallback_HI, this, std::placeholders::_1),
      KEEP_LATEST_MSG);

  driver_proxy_cur_status_pub_ = CreatePublisher<driver_proxy_cur_status>(
      irs_common::topics::kDriverBrainStatus, KEEP_LATEST_MSG);

  autonomy_brain_req_status_pub_ = CreatePublisher<autonomy_brain_req_status>(
      irs_common::topics::kAutonomyBrainReq, KEEP_LATEST_MSG);

  hmi_proxy_req_status_pub_ = CreatePublisher<hmi_proxy_req_status>(
      irs_common::topics::kHmiProxyRequestState, KEEP_LATEST_MSG);

  driver_brain_processor::InitInternal();

  IRS_INFO("driver brain init succeed!");

  ////
  AddPeriodicExecutive(1000, [this]() {
    this->driver_brain_sm_run();
    this->PublishMsg();
  });
}

void driver_proxy::MonitorCallback_AB(
    std::shared_ptr<autonomy_brain_cur_status> msg) {
  int64_t now = 0;
  if (!utils::base::get_current_system_time_nano(&now)) {
    IRS_ERROR("Failed to get current steady time.");
    return;
  }
  if ((autonomy_brain_state_cur_ != msg->current_state) ||
      (out_odd_autonomy_ != msg->out_of_odd) ||
      (worst_sid_autonomy_ != msg->worst_sid)) {
    IRS_INFO("driver brain monitor autonomy brain state %d",
             msg->current_state);
    autonomy_brain_state_cur_ = msg->current_state;
    out_odd_autonomy_ = msg->out_of_odd;
    worst_sid_autonomy_ = msg->worst_sid;
  }
}

void driver_proxy::MonitorCallback_HP(
    std::shared_ptr<hmi_proxy_cur_status> msg) {
  int64_t now = 0;
  if (!utils::base::get_current_system_time_nano(&now)) {
    IRS_ERROR("Failed to get current steady time.");
    return;
  }
  if ((out_odd_driver_ != msg->out_of_odd) ||
      (worst_sid_driver_ != msg->worst_sid) ||
      (hmi_proxy_state_cur_ != msg->current_state)) {
    IRS_INFO("driver brain monitor hmi proxy state %d", msg->current_state);
    out_odd_driver_ = msg->out_of_odd;
    worst_sid_driver_ = msg->worst_sid;
    hmi_proxy_state_cur_ = msg->current_state;
  }
}

void driver_proxy::MonitorCallback_HI(
    std::shared_ptr<hmi_input_cur_status> msg) {
  int64_t now = 0;
  if (!utils::base::get_current_system_time_nano(&now)) {
    IRS_ERROR("Failed to get current steady time.");
    return;
  }

  if (request_engage_autonomy_ != msg->request_engaging_autonomy) {
    IRS_INFO("driver brain monitor hmi input request_engaging_autonomy %d",
             msg->request_engaging_autonomy);
    request_engage_autonomy_ = msg->request_engaging_autonomy;
  }
}

void driver_proxy::PublishMsg() {
  int64_t now = 0;
  const auto succeed = utils::base::get_current_steady_time_nano(&now);

  if (succeed) {
    driver_proxy_cur_status_->current_state = driver_brain_state_cur_;
    driver_proxy_cur_status_->request_state = driver_brain_state_req_;
    driver_proxy_cur_status_->out_of_odd = out_odd_total_;
    driver_proxy_cur_status_->worst_sid = worst_sid_total_;
    driver_proxy_cur_status_pub_->Publish(driver_proxy_cur_status_);
    IRS_INFO("driver brain: current state %d!", driver_brain_state_cur_);

    hmi_proxy_req_status_->request_state = hmi_proxy_state_req_;
    hmi_proxy_req_status_pub_->Publish(hmi_proxy_req_status_);
    IRS_INFO("driver brain: hmi proxy request state %d!", hmi_proxy_state_req_);

    autonomy_brain_req_status_->autonomy_brain_request_state =
        autonomy_brain_state_req_;
    autonomy_brain_req_status_pub_->Publish(autonomy_brain_req_status_);
    IRS_INFO("driver brain: autonomy  brain request state %d !",
             autonomy_brain_state_req_);
  }
}

}  // namespace AdSession
}  // namespace vsp
REGISTER_IRS_NODE(vsp::AdSession::driver_proxy)
