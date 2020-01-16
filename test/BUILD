package(
    default_visibility = ["//visibility:public"],
)
cc_test(
    name = "test_driver_brain",
    srcs = ["test_driver_brain.cc"],
    deps = [
        "//utils/base:utils_base",
        "//vsp/AdSession:ssm_task",
	    "//irs_common/topics:irs_topics",
	    "//interfaces/vsp/AdSession:AdSession_idl",
	    "//interfaces/vsp/AdSession:driver_proxy_idl",
        "//interfaces/vsp/AdSession:autonomy_brain_idl",
	    "//interfaces/vsp/AdSession:vehicle_manager_idl",   
        "//interfaces/vsp/datalink/hmi_proxy:hmi_proxy_state_idl",
        "//vsp/AdSession:driver_brain_processor",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)
cc_test(
    name = "test_autonomy_brain",
    srcs = ["test_autonomy_brain.cc"],
    deps = [
        "//utils/base:utils_base",
        "//vsp/AdSession:ssm_task",
	    "//irs_common/topics:irs_topics",
	    "//interfaces/vsp/AdSession:AdSession_idl",
        "//interfaces/vsp/AdSession:autonomy_brain_idl",
        "//interfaces/vsp/AdSession:vehicle_manager_idl",
        "//interfaces/vsp/datalink/vsp_proxy:vsp_report_idl",      
        "//vsp/AdSession:autonomy_brain_processor",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)
cc_test(
    name = "test_vehicle_brain",
    srcs = ["test_vehicle_brain.cc"],
    deps = [
        "//utils/base:utils_base",
        "//vsp/AdSession:ssm_task",
	    "//irs_common/topics:irs_topics",
	    "//interfaces/vsp/AdSession:AdSession_idl",
	    "//interfaces/vsp/datalink/vci:vci_state_idl", 
	    "//interfaces/vsp/AdSession:vehicle_manager_idl", 
        "//vsp/AdSession:vehicle_brain_processor",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)
cc_test(
    name = "test_brain_session",
    srcs = ["test_brain_session.cc"],
    deps = [
        "//utils/base:utils_base",
        "//vsp/AdSession:ssm_task",
	    "//irs_common/topics:irs_topics",
	    "//interfaces/vsp/AdSession:AdSession_idl",
	    "//interfaces/vsp/datalink/vci:vci_state_idl", 
	    "//interfaces/vsp/AdSession:vehicle_manager_idl", 
                "//interfaces/vsp/AdSession:autonomy_brain_idl",
        "//interfaces/vsp/datalink/vsp_proxy:vsp_report_idl",    
        	    "//interfaces/vsp/AdSession:driver_proxy_idl",
        "//interfaces/vsp/datalink/hmi_proxy:hmi_proxy_state_idl",
        "//vsp/AdSession:driver_brain_processor",  
        "//vsp/AdSession:autonomy_brain_processor",
        "//vsp/AdSession:vehicle_brain_processor",
        "@gtest",
        "@gtest//:gtest_main",
    ],
)