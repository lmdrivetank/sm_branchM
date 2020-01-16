// Copyright 2019 Inceptio Technology. All Rights Reserved.
// Authors:
//    Ming Li (ming.li@inceptio.ai)

#ifndef _DRIVER_BRAIN_H_
#define _DRIVER_BRAIN_H_

#include <sys/time.h>
#include <time.h>
#include <memory>
#include <string>

#include "driver_brain_processor.h"
#include "interfaces/vsp/AdSession/autonomy_brain_type_support.h"
#include "interfaces/vsp/AdSession/driver_proxy_type_support.h"
#include "interfaces/vsp/datalink/hmi_proxy/hmi_proxy_state_type_support.h"
#include "interfaces/vsp/datalink/hmi_proxy/human_report_type_support.h"
#include "irs_common/irs_node.h"
#include "utils/base/time.h"

namespace vsp {
namespace AdSession {
class driver_proxy : public irs_common::IRSNode, public driver_brain_processor {
 public:
  driver_proxy(const std::string& node_name,
               const irscpp::NodeOptions& options = irscpp::NodeOptions());
  ~driver_proxy() {}

  void InitInternal(const irscpp::NodeOptions& /*options*/) override;

 private:
/// publish
#define driver_proxy_cur_status \
  interfaces::vsp::AdSession::driver_proxy::driver_proxy_status
#define autonomy_brain_req_status \
  interfaces::vsp::AdSession::autonomy_brain::autonomy_brain_state_request
#define hmi_proxy_req_status interfaces::vsp::hmiProxy::HmiProxyRequestState
  /// subscript
#define autonomy_brain_cur_status \
  interfaces::vsp::AdSession::autonomy_brain::autonomy_brain_status
#define hmi_proxy_cur_status interfaces::vsp::hmiProxy::HmiProxyCurrentState
#define hmi_input_cur_status interfaces::vsp::hmiProxy::HmiInput

 private:
  void MonitorCallback_AB(std::shared_ptr<autonomy_brain_cur_status> msg);
  void MonitorCallback_HP(std::shared_ptr<hmi_proxy_cur_status> msg);
  void MonitorCallback_HI(std::shared_ptr<hmi_input_cur_status> msg);

  void PublishMsg();

  // private:
 public:
  std::unique_ptr<driver_proxy_cur_status> driver_proxy_cur_status_;
  std::unique_ptr<autonomy_brain_req_status> autonomy_brain_req_status_;
  std::unique_ptr<hmi_proxy_req_status> hmi_proxy_req_status_;

 private:
  irs_common::IRSPublisher<driver_proxy_cur_status>::SharedPtr
      driver_proxy_cur_status_pub_;
  irs_common::IRSPublisher<autonomy_brain_req_status>::SharedPtr
      autonomy_brain_req_status_pub_;
  irs_common::IRSPublisher<hmi_proxy_req_status>::SharedPtr
      hmi_proxy_req_status_pub_;

  irs_common::IRSSubscription<autonomy_brain_cur_status>::SharedPtr
      autonomy_brain_cur_status_sub_;
  irs_common::IRSSubscription<hmi_proxy_cur_status>::SharedPtr
      hmi_proxy_cur_status_sub_;
  irs_common::IRSSubscription<hmi_input_cur_status>::SharedPtr
      hmi_input_cur_status_sub_;

  PROHIBIT_COPY_AND_ASSIGN(driver_proxy);
};
}  // namespace AdSession
}  // namespace vsp
#endif  // DRIVER_PROXY_H_
