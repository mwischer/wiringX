/*
	Copyright (c) 2016 CurlyMo <curlymoo1@gmail.com>

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <signal.h>   
	
#include "../../soc/soc.h"
#include "../../wiringX.h"	
#include "../platform.h"	
#include "hummingboard_base_pro_sdl.h"			

struct platform_t *hummingboardBaseProSDL = NULL;

/*
|3v|5v|
| 0|5v|
| 1|0v|
| 2| 3|
|0v| 4|
| 5| 6|
| 7|0v|
| 8| 9|
|3v|10|
|11|0v|
|12|13|
|14|15|
|0v|16|

|-----|
|SPDIF|
|-----|
*/

/*
 * Not all GPIO where usable through sysfs
 * from the kernel used.
 */
static int irq[] = {
	 -1,  -1,   1,
	 -1,  -1,  73,
	 72,  71,  70,
	194, 195,  -1,
	 -1,  67,  -1,
	 -1,  -1
};

static int map[] = {
	/*	GPIO3_IO18,	GPIO3_IO17,	GPIO1_IO01 */
			 82,				 81,				  1,
	/*	GPIO5_IO28,	GPIO5_IO29,	GPIO3_IO09 */
			149,				150,				 73,
	/*	GPIO3_IO08,	GPIO3_IO07,	GPIO3_IO06 */
			 72,				 71,				 70,
	/*	GPIO7_IO02,	GPIO7_IO03,	GPIO2_IO24 */
			185,				186,				 56,
	/*	GPIO2_IO25,	GPIO3_IO03,	GPIO2_IO23 */
			 57,				 67,				 55,
	/*	GPIO2_IO26,	GPIO2_IO27 */
			 58,				 59
};

static int hummingboardBaseProSDLValidGPIO(int pin) {
	if(pin >= 0 && pin < (sizeof(map)/sizeof(map[0]))) {
		return 0;
	} else {
		return -1;
	}
}

static int hummingboardBaseProSDLISR(int i, enum isr_mode_t mode) {
	if(irq[i] == -1) {
		wiringXLog(LOG_ERR, "The %s gpio %d cannot be used as interrupt", hummingboardBaseProSDL->name, i);
		return -1;
	}
	return hummingboardBaseProSDL->soc->isr(i, mode);
}

void hummingboardBaseProSDLInit(void) {
	platform_register(&hummingboardBaseProSDL, "hummingboard_base_sdl");
	platform_add_alias(&hummingboardBaseProSDL, "hummingboard_pro_sdl");

	hummingboardBaseProSDL->soc = soc_get("NXP", "IMX6SDLRM");
	hummingboardBaseProSDL->soc->setMap(map);
	hummingboardBaseProSDL->soc->setIRQ(irq);

	hummingboardBaseProSDL->digitalRead = hummingboardBaseProSDL->soc->digitalRead;
	hummingboardBaseProSDL->digitalWrite = hummingboardBaseProSDL->soc->digitalWrite;
	hummingboardBaseProSDL->pinMode = hummingboardBaseProSDL->soc->pinMode;
	hummingboardBaseProSDL->setup = hummingboardBaseProSDL->soc->setup;

	hummingboardBaseProSDL->isr = &hummingboardBaseProSDLISR;
	hummingboardBaseProSDL->waitForInterrupt = hummingboardBaseProSDL->soc->waitForInterrupt;

	hummingboardBaseProSDL->selectableFd = hummingboardBaseProSDL->soc->selectableFd;
	hummingboardBaseProSDL->gc = hummingboardBaseProSDL->soc->gc;

	hummingboardBaseProSDL->validGPIO = &hummingboardBaseProSDLValidGPIO;

}
