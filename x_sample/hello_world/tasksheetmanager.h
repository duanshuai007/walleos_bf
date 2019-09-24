#ifndef _X_SAMPLE_HELLO_WORLD_TASK_SHEET_MANAGER_H_
#define _X_SAMPLE_HELLO_WORLD_TASK_SHEET_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <task.h>
#include <graphic/graphic.h>

// add task and all it's sheets
void tsm_add_task(TTask * pTask);

// add task and one pecific sheet
void tsm_add_sheet(TTask * pTask, SHEET * pSheet);

TTask * tsm_get_task_by_sheet(SHEET * pSheet);

void tsm_remove_sheet(TTask * pTask, SHEET * pSheet);

void tsm_remove_task(TTask * pTask);

boolean tsm_task_have_sheet(TTask * pTask);

#ifdef __cplusplus
}
#endif

#endif
