#include "tasksheetmanager.h"
#include <stddef.h>
#include <assert.h>
#include <config.h>
#include <headsmp.h>

TTask * m_tsm_tasks[CPU_MAX_NUM][MAX_TASKS * MAX_SHEET_SIZE_PER_TASK];
SHEET * m_tsm_sheets[CPU_MAX_NUM][MAX_TASKS * MAX_SHEET_SIZE_PER_TASK];

void tsm_add_task(TTask * pTask)
{
	int cpu = get_cpuid();
	assert(cpu <= CPU_MAX_NUM);

	for (unsigned i = 0; i < MAX_SHEET_SIZE_PER_TASK; i ++)
	{
		if (0 != pTask->m_apSheets[i])
		{
			for (unsigned j = 0; j < MAX_TASKS * MAX_SHEET_SIZE_PER_TASK; j ++)
			{
				if (0 == m_tsm_sheets[cpu][j])
				{
					m_tsm_tasks[cpu][j] = pTask;
					m_tsm_sheets[cpu][j] = pTask->m_apSheets[i];

					break;
				}
			}
		}
	}
}

void tsm_remove_task(TTask * pTask)
{
	int cpu = get_cpuid();
	assert(cpu <= CPU_MAX_NUM);

	for (unsigned i = 0; i < MAX_SHEET_SIZE_PER_TASK; i ++)
	{
		if (0 != pTask->m_apSheets[i])
		{
			for (unsigned j = 0; j < MAX_TASKS * MAX_SHEET_SIZE_PER_TASK; j ++)
			{
				if (pTask->m_apSheets[i] == m_tsm_sheets[cpu][j])
				{
					m_tsm_tasks[cpu][j] = 0;
					m_tsm_sheets[cpu][j] = 0;;
					break;
				}
			}
		}
	}
}

void tsm_add_sheet(TTask * pTask, SHEET * pSheet)
{
	int cpu = get_cpuid();
	assert(cpu <= CPU_MAX_NUM);

	for (unsigned i = 0; i < MAX_TASKS * MAX_SHEET_SIZE_PER_TASK; i ++)
	{
		if (m_tsm_sheets[cpu][i] == pSheet)
		{
			// already added
			return ;
		}
	}

	for (unsigned j = 0; j < MAX_TASKS * MAX_SHEET_SIZE_PER_TASK; j ++)
	{
		if (0 == m_tsm_sheets[cpu][j])
		{
			m_tsm_tasks[cpu][j] = pTask;
			m_tsm_sheets[cpu][j] = pSheet;
			break;
		}
	}
}

void tsm_remove_sheet(TTask * pTask, SHEET * pSheet)
{
    UNUSED(pTask);

	int cpu = get_cpuid();
	assert(cpu <= CPU_MAX_NUM);

	for (unsigned j = 0; j < MAX_TASKS * MAX_SHEET_SIZE_PER_TASK; j ++)
	{
		if (pSheet == m_tsm_sheets[cpu][j])
		{
			m_tsm_tasks[cpu][j] = 0;
			m_tsm_sheets[cpu][j] = 0;
			break;
		}
	}
}

TTask * tsm_get_task_by_sheet(SHEET * pSheet)
{
	int cpu = get_cpuid();
	assert(cpu <= CPU_MAX_NUM);

	for (unsigned i = 0; i < MAX_TASKS * MAX_SHEET_SIZE_PER_TASK; i ++)
	{
		if (m_tsm_sheets[cpu][i] == pSheet)
		{
			return m_tsm_tasks[cpu][i];
		}
	}

	return 0;
}

boolean tsm_task_have_sheet(TTask * pTask)
{
	int cpu = get_cpuid();
	assert(cpu <= CPU_MAX_NUM);

	for (unsigned j = 0; j < MAX_TASKS * MAX_SHEET_SIZE_PER_TASK; j ++)
	{
		if (pTask == m_tsm_tasks[cpu][j])
		{
			return TRUE;
		}
	}

	return FALSE;
}
