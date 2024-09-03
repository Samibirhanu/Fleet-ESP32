// macro for selecting the core of stensaw processor of esp32  which is two core processors
#if CONFIG_FREERTOS_UNICORE
// The number one core processor
#define ARDUINO_RUNNING_CORE 0
#else
// The number two core processor
#define ARDUINO_RINNING_CORE 1
#endif

// Create task functions

void taskFunction(void * pvParameters){

  (void) pvParameters;
  
  for(;;){
    //code excuited by the task
    //This could be reading a sensor, controlling an output, ..etc
    Serial.print("hello from task!");
    vTaskDelay(1000/ portTICK_PERIOD_MS); // delay for 1 second

  // the time is calculated in terms of ticks per milliseconds

  }
}

void setup(){
  Serial.begin(115200);
  
  // create the task
  // taskName, descriptive name, stack depth, Pv parameter, priority, pointer to a task_T, arduino_running_core

  xTaskCreatePinnedToCore(taskFunction, "Task", 1024, NULL, 1, NULL,ARDUINO_RUNNING_CORE);

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
  
}

void loop(){

  // Nothing goes here, all the work is done in the tasks
}