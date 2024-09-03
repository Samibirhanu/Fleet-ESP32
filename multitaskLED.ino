// macro for selecting the core of stensaw processor of esp32  which is two core processors
#if CONFIG_FREERTOS_UNICORE
// The number one core processor
#define ARDUINO_RUNNING_CORE 0
#else
// The number two core processor
#define ARDUINO_RINNING_CORE 1
#endif

// Create task functions

void taskFunction1(void * pvParameters){

  (void) pvParameters;
  
  for(;;){
    //code excuited by the task
    //This could be reading a sensor, controlling an output, ..etc
    digitalWrite(2, HIGH);
    delay(100);
    digitalWrite(2, LOW);
    vTaskDelay(1000/ portTICK_PERIOD_MS);  // delay for 1 second

  // the time is calculated in terms of ticks per milliseconds
  
  }
}

void taskFunction2(void * pvParameters){

  (void) pvParameters;
  
  for(;;){
    //code excuited by the task
    //This could be reading a sensor, controlling an output, ..etc
    digitalWrite(14, HIGH);
    delay(100);
    digitalWrite(14, LOW);
    vTaskDelay(1000/ portTICK_PERIOD_MS); // delay for 1 second

  // the time is calculated in terms of ticks per milliseconds
  
  }
}


void setup(){
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  pinMode(14, OUTPUT);
  
  // create the task
  // taskName, descriptive name, stack depth, Pv parameter, priority(low number indicates low priority ), pointer to a task_T(interacting tasks from other tasks), arduino_running_core

  xTaskCreatePinnedToCore(taskFunction1, "Task1", 1024, NULL, 1, NULL,ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(taskFunction2, "Task2", 1024, NULL, 2, NULL,ARDUINO_RUNNING_CORE);


  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.

}

void loop(){
// Nothing goes here, all the work is done in the tasks
}
