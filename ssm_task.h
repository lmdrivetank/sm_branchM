// Copyright 2019 Inceptio Technology. All Rights Reserved.
// Authors:
//    Ming Li (ming.li@inceptio.ai)

#ifndef _SSM_TASK_H_
#define _SSM_TASK_H_

#include <functional>
#include <list>
#include <string>
#include <vector>

namespace vsp {
#define workCondition_OK (0)
#define workCondition_Not (-1)
#define workResult_Succ (0)
#define workResult_Fail (-1)
#define workState_Begin (0)
#define workState_End (-1)
/////////////////////
typedef int Ti_TaskBranch;
#define TaskBranch_Switch (0x10)
#define TaskBranch_ParallelWith (0x20)
#define TaskBranch_ParallelOr (0x40)
//////////////////
#define TaskBranch_Begin (0x01)
#define TaskBranch_End (0x02)
#define TaskBranch_Firsts (0x04)
//////////////////
typedef int Ti_TaskBranchNode;
#define TaskBranchNode_Null (0x0)
#define TaskBranch_SwitchBegin (0x11)
#define TaskBranch_SwitchEnd (0x12)
#define TaskBranch_SwitchFirsts (0x14)
#define TaskBranch_ParallelWithBegin (0x21)
#define TaskBranch_ParallelWithEnd (0x22)
#define TaskBranch_ParallelWithFirsts (0x24)
#define TaskBranch_ParallelOrBegin (0x41)
#define TaskBranch_ParallelOrEnd (0x42)
#define TaskBranch_ParallelOrFirsts (0x44)
#define GetBranchBeginNode(branch) (branch + TaskBranch_Begin)
#define GetBranchEndNode(branch) (branch + TaskBranch_End)
#define GetBranchFirstsNode(branch) (branch + TaskBranch_Firsts)
/////////////////////
typedef struct Ts_StateTask;
typedef struct Ts_TaskBranchMgr {
  int branch_id = 0;
  int branch_count_limit = 0;
  int branch_count_started = 0;
  int branch_count_arrived = 0;
  std::list<Ts_StateTask *> lpTasks = std::list<Ts_StateTask *>();
  bool operator==(const Ts_TaskBranchMgr task) const {
    return this->branch_id == task.branch_id;
  };
};

typedef std::function<int()> Tpf_work;

struct Ts_StateTask {
  int task_id;
  bool is_first_task;
  Tpf_work workCondition;
  Tpf_work workPerformance;
  Ti_TaskBranchNode tbn = TaskBranchNode_Null;
  Ts_TaskBranchMgr *pTaskBranchMgr = nullptr;
  std::list<Ts_StateTask *> next_ptasks = std::list<Ts_StateTask *>();
};

typedef struct {
  int check_result = workResult_Fail;
  Tpf_work failCheck;
  Tpf_work fallBack;
} Ts_StateDepend;

typedef std::list<Ts_StateTask> Tl_StateTask;
typedef std::list<Ts_StateDepend> Tl_StateDepend;
typedef std::list<Ts_TaskBranchMgr> Tl_TaskBranchMgr;
// state branch manager
typedef struct {
  int state;
  std::string state_name;
  int task_count;
  bool is_hold;
  std::list<Ts_StateTask *> lpFirstTasks = std::list<Ts_StateTask *>();
  std::list<Ts_StateTask *> lpCurrTasks = std::list<Ts_StateTask *>();
  int branchs_count;
  int branchs_count_curr;
  Tl_TaskBranchMgr lTaskBranchMgrs;
  Tl_StateTask lStateTasks;
  Tl_StateDepend lStateDepends;
} Ts_StateManager;

// state machine manager
typedef std::list<Ts_StateManager> Tl_StateManager;
typedef struct {
  int state_count;
  int curr_state;
  int prev_state;
  int next_state;
  Ts_StateManager *pStateManager;  // current state

  Tl_StateManager lStateManager;  // state branch manager vector
} Ts_smm;

class ssm_task {
 public:
  Ts_smm smm;  // state machine manager
  bool sm_continue = false;

 public:
  ssm_task();
  ~ssm_task();

 public:
  // request state
  void init(int state);
  void SetState(int state);
  int GetState();

 public:
  // should add function for check state transition available
  void EnterNextState();
  void FinishCurrState();

  void HoldCurrState();
  void ResumePrevState();

 public:
  void AddState(int state, std::string state_name = "");
  Ts_StateManager *GetState(const int state);

  void AddStateDepend(const int state, Tpf_work fail_check, Tpf_work fallback,
                      int check_result = workResult_Fail);
  // int GetStateDepends(const int state, Tl_StateDepend *pvStateDepend);

  void AddStateTask(const int state, int task_id, Tpf_work condition,
                    Tpf_work performance);
  Ts_StateTask *GetStateTask1(Ts_StateManager *pStateManager,
                              const int task_id);
  Ts_StateTask *GetStateTask2(const int state, const int task_id);

  void FinishChainTask();

  int branch_start() { return workResult_Succ; }
  int branch_first() { return workResult_Succ; }
  int branch_finish() { return workResult_Succ; }
  void ChainStateTask(const int state, std::vector<int> task_buff);
  void SetTaskBranch(const int state, Ti_TaskBranch tb, int start_id,
                     int end_id, int br_limit = 0);

  void sm_schedule();
  void sm_go();
};
}  // namespace vsp
#endif