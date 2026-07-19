#include "cmsis_os2.h"
#include "main.h"

int main(){
	HAL_Init();
	osKernelInitialize();

	MX_FREERTOS_Init();
	osKernelStart();

	while(1){

}
}
