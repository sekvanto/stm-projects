I'm not sure why this doesn't work yet, but it seems that the task with FatFS usage in FreeRTOS requires a 4KB+ stack, which is too much. I'll return to this later to debug or test with using another mcu. `https://www.freertos.org/FreeRTOS_Support_Forum_Archive/December_2014/freertos_FreeRTOS_FatFs_Works_only_with_taskENTER_CRITICAL_5dc853ffj.html`

The mmcbb.c is rewritten for the FreeRTOS usage already though, as well as FatFS is configured for reentrancy.
