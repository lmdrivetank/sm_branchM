// Copyright 2019 Inceptio Technology. All Rights Reserved.
// Authors:
//    Ming Li (ming.li@inceptio.ai)
#include "ssm_task.h"
#include <string.h>
#include <algorithm>

namespace vsp {

ssm_task::ssm_task() {
  smm.state_count = 0;
  smm.prev_state = workState_Begin;
  smm.curr_state = workState_Begin;
  smm.next_state = workState_Begin;
  smm.lStateManager.clear();
}

ssm_task::~ssm_task() {}

void ssm_task::AddState(int state, std::string state_name) {
  Ts_StateManager _StateManager;
  _StateManager.state = state;
  _StateManager.task_count = 0;
  _StateManager.lpFirstTasks.clear();
  _StateManager.lpCurrTasks.clear();
  _StateManager.lStateDepends.clear();
  _StateManager.lStateTasks.clear();
  _StateManager.branchs_count = 0;
  _StateManager.state_name = state_name;
  _StateManager.branchs_count_curr = 0;

  smm.lStateManager.push_back(_StateManager);
  smm.state_count++;
}

Ts_StateManager* ssm_task::GetState(const int state) {
  for (auto iter = smm.lStateManager.begin(); iter != smm.lStateManager.end();
       iter++) {
    if (iter->state == state) {
      return &(*iter);
    }
  }
  return nullptr;
}

void ssm_task::init(int state) {
  smm.curr_state = state;
  EnterNextState();
}
void ssm_task::SetState(int state) { smm.next_state = state; }

int ssm_task::GetState() { return smm.curr_state; }

void ssm_task::EnterNextState() {
  if (smm.curr_state != smm.next_state) {
    smm.curr_state = smm.next_state;
    sm_continue = true;
  }
  smm.pStateManager = GetState(smm.curr_state);
  smm.pStateManager->is_hold = false;
  smm.pStateManager->lpCurrTasks.clear();
  std::copy(smm.pStateManager->lpFirstTasks.begin(),
            smm.pStateManager->lpFirstTasks.end(),
            std::back_inserter(smm.pStateManager->lpCurrTasks));
}

void ssm_task::FinishCurrState() {
  smm.pStateManager->lpCurrTasks.clear();
  smm.pStateManager->is_hold = false;
  std::copy(smm.pStateManager->lpFirstTasks.begin(),
            smm.pStateManager->lpFirstTasks.end(),
            std::back_inserter(smm.pStateManager->lpCurrTasks));
}

void ssm_task::HoldCurrState() {
  GetState(smm.curr_state)->is_hold = true;
  smm.prev_state = smm.curr_state;
}

void ssm_task::ResumePrevState() {
  GetState(smm.curr_state)->is_hold = false;
  smm.curr_state = smm.prev_state;
}

void ssm_task::AddStateDepend(const int state, Tpf_work fail_check,
                              Tpf_work fall_back, int check_result) {
  Ts_StateDepend _StateDependPair;
  for (auto& StateManager : smm.lStateManager) {
    if (StateManager.state == state) {
      _StateDependPair.check_result = check_result;
      _StateDependPair.failCheck = fail_check;
      _StateDependPair.fallBack = fall_back;
      StateManager.lStateDepends.push_back(_StateDependPair);
      break;
    }
  }
}

void ssm_task::AddStateTask(const int state, int task_id, Tpf_work condition,
                            Tpf_work performance) {
  Ts_StateTask state_task;
  for (auto& StateManager : smm.lStateManager) {
    if (StateManager.state == state) {
      state_task.task_id = task_id;
      state_task.workCondition = condition;
      state_task.workPerformance = performance;
      state_task.is_first_task = true;
      state_task.tbn = TaskBranchNode_Null;
      StateManager.lStateTasks.push_back(state_task);
      break;
    }
  }
}

Ts_StateTask* ssm_task::GetStateTask1(Ts_StateManager* pStateManager,
                                      const int task_id) {
  for (auto iter = pStateManager->lStateTasks.begin();
       iter != pStateManager->lStateTasks.end(); iter++) {
    if (iter->task_id == task_id) {
      return &(*iter);
    }
  }
  return nullptr;
}

Ts_StateTask* ssm_task::GetStateTask2(const int state, const int task_id) {
  Ts_StateManager* pStateManager = GetState(state);

  return GetStateTask1(pStateManager, task_id);
}

void ssm_task::ChainStateTask(const int state, std::vector<int> task_buff) {
  Ts_StateManager* pStateManager = GetState(state);
  Ts_StateTask* prev_ptask = nullptr;
  Ts_StateTask* curr_ptask = nullptr;
  for (auto task_id : task_buff) {
    curr_ptask = GetStateTask1(pStateManager, task_id);
    if (prev_ptask != nullptr) {
      curr_ptask->is_first_task = false;
      if (prev_ptask->next_ptasks.end() == find(prev_ptask->next_ptasks.begin(),
                                                prev_ptask->next_ptasks.end(),
                                                curr_ptask)) {
        prev_ptask->next_ptasks.push_back(curr_ptask);
      }
    }
    prev_ptask = curr_ptask;
  }
}

void ssm_task::FinishChainTask() {
  for (auto iter_StateManager = smm.lStateManager.begin();
       iter_StateManager != smm.lStateManager.end(); iter_StateManager++) {
    for (auto iter_task = iter_StateManager->lStateTasks.begin();
         iter_task != iter_StateManager->lStateTasks.end(); iter_task++) {
      if (iter_task->is_first_task == true) {
        iter_StateManager->lpFirstTasks.push_back(&(*iter_task));
      }
    }
  }
}
void ssm_task::SetTaskBranch(const int state, Ti_TaskBranch tb, int start_id,
                             int end_id, int br_limit) {
  Ts_StateManager* pStateManager = GetState(state);
  Tl_TaskBranchMgr* plTaskBranchMgrs = &(pStateManager->lTaskBranchMgrs);
  Ts_TaskBranchMgr ts_TaskBranchMgr;
  Ts_TaskBranchMgr* pTaskBranchMgr = nullptr;
  Ts_StateTask* ptask = nullptr;

  ts_TaskBranchMgr.branch_id = pStateManager->branchs_count;
  pStateManager->branchs_count++;
  if (tb == TaskBranch_Switch) {
    ts_TaskBranchMgr.branch_count_limit = 1;
  } else if (tb == TaskBranch_ParallelWith) {
    ts_TaskBranchMgr.branch_count_limit = ptask->next_ptasks.size();
  } else if (tb == TaskBranch_ParallelOr) {
    ts_TaskBranchMgr.branch_count_limit = br_limit;
  }
  plTaskBranchMgrs->push_back(ts_TaskBranchMgr);
  pTaskBranchMgr = &(*find(plTaskBranchMgrs->begin(), plTaskBranchMgrs->end(),
                           ts_TaskBranchMgr));

  ptask = GetStateTask1(pStateManager, start_id);
  ptask->tbn = GetBranchBeginNode(tb);
  ptask->pTaskBranchMgr = pTaskBranchMgr;

  for (auto& ptask_next : ptask->next_ptasks) {
    ptask_next->tbn = GetBranchFirstsNode(tb);
    ptask_next->pTaskBranchMgr = pTaskBranchMgr;
  }

  ptask = GetStateTask1(pStateManager, end_id);
  ptask->tbn = GetBranchEndNode(tb);
  ptask->pTaskBranchMgr = pTaskBranchMgr;
}
void ssm_task::sm_go() {
  do {
    sm_continue = false;
    sm_schedule();
  } while (sm_continue);
}
void ssm_task::sm_schedule() {
  int curr_state = smm.curr_state;
  bool condition_succeed = false;
  Ts_StateManager* pStateManager = GetState(curr_state);
  std::list<Ts_StateTask*>& curr_ptasks = pStateManager->lpCurrTasks;
  Ts_TaskBranchMgr* ptask_branch_mgr = nullptr;
  Ts_StateTask* state_ptask = nullptr;

  for (auto iter = curr_ptasks.begin(); iter != curr_ptasks.end();) {
    state_ptask = *iter;

    for (auto& state_depend : pStateManager->lStateDepends) {
      if (state_depend.failCheck != nullptr &&
          state_depend.failCheck() == state_depend.check_result) {
        state_depend.fallBack();
        // accord fail level ,fallback maybe not finish and just hold
        FinishCurrState();
        EnterNextState();
        return;
      }
    }
    ptask_branch_mgr = state_ptask->pTaskBranchMgr;
    if (((state_ptask->tbn & TaskBranch_End) == TaskBranch_End) &&
        (ptask_branch_mgr->branch_count_arrived !=
         ptask_branch_mgr->branch_count_limit)) {
      HoldCurrState();
      iter++;
      continue;
    }
    condition_succeed = true;
    if (state_ptask->workCondition != nullptr) {
      if (state_ptask->workCondition() != workResult_Succ) {
        condition_succeed = false;
        iter++;
        if (state_ptask->tbn == TaskBranch_SwitchFirsts) {
          curr_ptasks.remove(state_ptask);
          ptask_branch_mgr->lpTasks.remove(state_ptask);
        } else if ((state_ptask->tbn == TaskBranchNode_Null) ||
                   ((state_ptask->tbn & TaskBranch_End) == TaskBranch_End) ||
                   ((state_ptask->tbn & TaskBranch_Begin) ==
                    TaskBranch_Begin) ||
                   (state_ptask->tbn == TaskBranch_ParallelWithFirsts)) {
          HoldCurrState();
        }
        continue;
      }
    }
    if (condition_succeed == true) {
      state_ptask->workPerformance();
      for (auto ptask_next : state_ptask->next_ptasks) {
        if ((ptask_next->tbn & TaskBranch_End) == TaskBranch_End) {
          if (ptask_next->pTaskBranchMgr->branch_count_arrived == 0) {
            curr_ptasks.push_back(ptask_next);
          }
          ptask_next->pTaskBranchMgr->branch_count_arrived++;
        } else {
          curr_ptasks.push_back(ptask_next);
        }
      }

      if ((state_ptask->tbn & TaskBranch_Begin) == TaskBranch_Begin) {
        pStateManager->branchs_count_curr++;
        ptask_branch_mgr->branch_count_arrived = 0;
        ptask_branch_mgr->lpTasks.clear();
        for (auto ptask_next : state_ptask->next_ptasks) {
          ptask_branch_mgr->lpTasks.push_back(ptask_next);
        }
      } else if ((state_ptask->tbn & TaskBranch_End) == TaskBranch_End) {
        pStateManager->branchs_count_curr--;
        ptask_branch_mgr->branch_count_arrived = 0;
        ptask_branch_mgr->branch_count_started = 0;
      } else if ((state_ptask->tbn & TaskBranch_Firsts) == TaskBranch_Firsts) {
        ptask_branch_mgr->branch_count_started++;
        ptask_branch_mgr->lpTasks.remove(state_ptask);

        if (ptask_branch_mgr->branch_count_started ==
            ptask_branch_mgr->branch_count_limit) {
          for (auto ptask : ptask_branch_mgr->lpTasks) {
            curr_ptasks.remove(ptask);
          }
          ptask_branch_mgr->lpTasks.clear();
        }
      }
    }
    iter++;
    curr_ptasks.remove(state_ptask);
  }
  if (pStateManager->is_hold == false &&
      pStateManager->branchs_count_curr == 0) {
    FinishCurrState();
    EnterNextState();
  }
  pStateManager->is_hold = false;
}
}  // namespace vsp