load("//irs_common:irs_bzl/irs_common.bzl", "cc_irs_node_binary")

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "ssm_task",
    srcs = ["ssm_task.cc"],
    hdrs = ["ssm_task.h"],    
    deps = [
        "//utils/base:utils_base",
    ],
)
cc_library(
    name = "driver_brain_processor",
    srcs = ["driver_brain_processor.cc"],
    hdrs = ["driver_brain_processor.h","ssm_task.h"],    
    deps = [
        "//utils/base:utils_base",
        "//vsp/AdSession:ssm_task",
        "//interfaces/vsp/AdSession:AdSession_idl",
    ],
)
cc_library(
    name = "autonomy_brain_processor",
    srcs = ["autonomy_brain_processor.cc"],
    hdrs = ["autonomy_brain_processor.h","ssm_task.h"],    
    deps = [
        "//utils/base:utils_base",
        "//vsp/AdSession:ssm_task",
        "//interfaces/vsp/AdSession:autonomy_brain_idl",
        "//interfaces/vsp/AdSession:AdSession_idl",
    ],
)
cc_library(
    name = "vehicle_brain_processor",
    srcs = ["vehicle_brain_processor.cc"],
    hdrs = ["vehicle_brain_processor.h","ssm_task.h"],    
    deps = [
        "//utils/base:utils_base",
        "//vsp/AdSession:ssm_task",
        "//interfaces/vsp/AdSession:AdSession_idl",
    ],
)
cc_irs_node_binary(
    name = "vehicle_brain",
    srcs = ["vehicle_brain.cc"],
    hdrs = ["vehicle_brain.h","vehicle_brain_processor.h"],
    deps = [
	"//utils/base:utils_base",	
	"//vsp/AdSession:vehicle_brain_processor",
	"//irs_common/topics:irs_topics",
	"//interfaces/vsp/AdSession:AdSession_idl",
	"//interfaces/vsp/datalink/vci:vci_state_idl", 
	"//interfaces/vsp/AdSession:vehicle_manager_idl",    	       
    ],
)
cc_irs_node_binary(
    name = "autonomy_brain",
    srcs = ["autonomy_brain.cc"],
    hdrs = ["autonomy_brain.h","autonomy_brain_processor.h"],
    deps = [
        "//utils/base:utils_base",	
	    "//vsp/AdSession:autonomy_brain_processor",
	    "//irs_common/topics:irs_topics",
	    "//interfaces/vsp/AdSession:AdSession_idl",
        "//interfaces/vsp/AdSession:autonomy_brain_idl",
        "//interfaces/vsp/AdSession:vehicle_manager_idl",
        "//interfaces/vsp/datalink/vsp_proxy:vsp_report_idl",        
    ],
)
cc_irs_node_binary(
    name = "driver_brain",
    srcs = ["driver_brain.cc"],
    hdrs = ["driver_brain.h","driver_brain_processor.h"],
    deps = [
        "//utils/base:utils_base",	
	    "//vsp/AdSession:driver_brain_processor",
	    "//irs_common/topics:irs_topics",
	    "//interfaces/vsp/AdSession:AdSession_idl",
	    "//interfaces/vsp/AdSession:driver_proxy_idl",
        "//interfaces/vsp/AdSession:autonomy_brain_idl",
	    "//interfaces/vsp/AdSession:vehicle_manager_idl",   
        "//interfaces/vsp/datalink/hmi_proxy:hmi_proxy_state_idl",
        "//interfaces/vsp/datalink/hmi_proxy:human_report_idl",
    ],
)
