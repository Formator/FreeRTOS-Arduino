#define USE_FREERTOS

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

extern char _end;
extern "C" char *sbrk(int i);
char *ramstart = (char *)0x20070000;
char *ramend = (char *)0x20088000;

#ifdef USE_FREERTOS
#include <FreeRTOS_ARM.h>
static void TaskPrintMem( void *pvParameters );
#endif // USE_FREERTOS

static void showMem();

void setup()
{
  Serial.begin(9600);
  showMem();
  delay(5000);

#ifdef USE_FREERTOS
  xTaskCreate(TaskPrintMem, NULL, configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
  vTaskStartScheduler();
#endif
}

void loop()
{
#ifndef USE_FREERTOS
  showMem();
  delay(5000);
#endif
}

#ifdef USE_FREERTOS
static void TaskPrintMem(void *pvParameters)
{
  for (;;)
  {
    showMem();
    vTaskDelay((5000L * configTICK_RATE_HZ) / 1000L);
  }
}
#endif

static void showMem()
{
  struct mallinfo mi = mallinfo();
  char *heapend = sbrk(0);
  register char * stack_ptr asm("sp");
  printf("    arena=%d\n", mi.arena);
  printf("  ordblks=%d\n", mi.ordblks);
  printf(" uordblks=%d\n", mi.uordblks);
  printf(" fordblks=%d\n", mi.fordblks);
  printf(" keepcost=%d\n", mi.keepcost);

  printf("RAM Start %lx\n", (unsigned long)ramstart);
  printf("Data/Bss end %lx\n", (unsigned long)&_end);
  printf("Heap End %lx\n", (unsigned long)heapend);
  printf("Stack Ptr %lx\n", (unsigned long)stack_ptr);
  printf("RAM End %lx\n", (unsigned long)ramend);

  printf("Heap RAM Used: %d\n", mi.uordblks);
  printf("Program RAM Used %d\n", &_end - ramstart);
  printf("Estimated Free RAM: %d\n\n", stack_ptr - (heapend + mi.fordblks));

  Serial.flush();
}
