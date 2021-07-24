/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"

#include "IsoCommonDef.h"
#include "IsoCommonApi.h"
#include "settings.h"
extern void isobus_main(void);
void app_main(void)
{




	hw_DebugPrint("app_main \n");
	/* Initialize application */
	Settings_init();










	isobus_main();

}
