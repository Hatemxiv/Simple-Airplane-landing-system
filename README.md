# Simple-Airplane-landing-system
The project aims to design an embedded system that controls the airplane landing sequence using a Tiva C microcontroller, FreeRTOS as the real-time operating system the airplane's status.
The system also includes an 7 segment display that provides real-time information about the airplane's status during the landing sequence, such as task number
The display is updated in real-time using FreeRTOS, which ensures that critical tasks running there are three different running tasks (landing task, take off task, and stable task) at different priorities.
The three tasks , landing has highest priority, take off has medium priority and stable has lowest priority. Lading task, when switch 2 on tiva is pressed this task will run and Red led is on and display number 1 on 7 segment take off when switch 1 on tiva is pressed this task will run and green led is on. number 2 on 7 segment
stable task with low priority and periodic task will run after take off or landing finished and display on 7 segment number 3.
When system will be on idle state the yellow, pink and sky blue leds will run using round robin scheduling technique.
