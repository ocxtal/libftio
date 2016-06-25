
/**
 * @file test.c
 */
#include <stdio.h>
#include <stdint.h>
#include "ftio.h"

int main(void)
{
	ftio_device_t *dev_list = ftio_get_device_list();
	if(dev_list == NULL) {
		printf("Failed to enumerate devices.\n");
		return(1);
	}

	for(ftio_device_t *d = dev_list; d->manu != NULL; d++) {
		printf("Manufacturer: %s, Description: %s\n", d->manu, d->desc);
	}

	ftio_free_device_list(dev_list);
	return(0);
}

/**
 * end of test.c
 */
