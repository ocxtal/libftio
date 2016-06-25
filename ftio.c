
/**
 * @file ftio.c
 *
 * @brief synchronous bit-bang utils for FTDI chips
 */

#include <stdint.h>
#include "kvec.h"
#include "ftio.h"
#include "libftdi1/ftdi.h"

/**
 * @struct ftio_s
 */
struct ftio_s {
	struct ftdi_context *ctx;
	struct ftio_params_s params;
};

/**
 * @fn ftio_get_device_list
 */
ftio_device_t *ftio_get_device_list(
	void)
{
	struct ftdi_context ctx;
	if(ftdi_init(&ctx) != 0) {
		return(NULL);
	}

	struct ftdi_device_list *dev_list;
	int dev_cnt = ftdi_usb_find_all(&ctx, &dev_list, 0, 0);
	if(dev_cnt < 0) {
		return(NULL);
	}

	kvec_t(struct ftio_device_s) v;
	kv_init(v);
	for(struct ftdi_device_list *p = dev_list; p != NULL; p = p->next) {
		int const buf_size = 1024;
		char manu[buf_size];
		char desc[buf_size];
		char serial[buf_size];

		if(ftdi_usb_get_strings(&ctx, p->dev,
			manu, buf_size, desc, buf_size, serial, buf_size) != 0) {
			continue;
		}

		struct ftio_device_s d = (struct ftio_device_s){
			.manu = strdup(manu),
			.desc = strdup(desc)
		};
		kv_push(v, d);
	}
	kv_push(v, ((struct ftio_device_s){
		.manu = NULL,
		.desc = NULL
	}));

	ftdi_list_free(&dev_list);
	ftdi_deinit(&ctx);
	return(kv_ptr(v));
}

/**
 * @fn ftio_free_device_list
 */
void ftio_free_device_list(
	ftio_device_t *dev_list)
{
	if(dev_list == NULL) { return; }

	for(struct ftio_device_s *p = dev_list; p->manu != NULL; p++) {
		free(p->manu); p->manu = NULL;
		free(p->desc); p->desc = NULL;
	}
	free(dev_list);
	return;
}


/**
 * @fn ftio_close
 */
void ftio_close(
	ftio_t *ftio)
{
	if(ftio == NULL) { return; }
	
	if(ftio->ctx != NULL) {
		ftdi_free(ftio->ctx); ftio->ctx = NULL;
	}
	free(ftio);
	return;
}

/**
 * @fn ftio_open_device
 */
static inline
int ftio_open_device(
	struct ftio_s *ftio,
	struct ftio_device_s const *device)
{
	/* if device is not specified, open the first found device */
	if(device == NULL) {
		struct ftio_device_s *dev_list = ftio_get_device_list();
		if(dev_list == NULL) { return(-100); }

		int ret = ftdi_usb_open_string(ftio->ctx, dev_list->desc);

		ftio_free_device_list(dev_list);
		return(ret);
	} else {
		return(ftdi_usb_open_string(ftio->ctx, device->desc));
	}
}

/**
 * @fn ftio_open
 */
ftio_t *ftio_open(
	ftio_params_t const *params)
{
	struct ftio_params_s p = (params != NULL)
		? *params
		:((struct ftio_params_s){
			.device = NULL,
			.clock_freq = 10000,		/* 10kHz */
			.io_mask = 0
		});

	struct ftio_s *ftio = (struct ftio_s *)malloc(sizeof(struct ftio_s));
	if(ftio == NULL) {
		return(NULL);
	}
	memset(ftio, 0, sizeof(struct ftio_s));

	struct ftio_device_s *dev_list = NULL;
	if(p.device == NULL) {
		dev_list = ftio_get_device_list();
		if(dev_list == NULL) { return(NULL); }
		p.device = dev_list;
	}

	/* create context */
	if((ftio->ctx = ftdi_new()) == NULL) {
		goto _ftio_open_error_handler;
	}

	/* open device */
	if(ftio_open_device(ftio, p.device) != 0) {
		goto _ftio_open_error_handler;
	}

	/* set clock freq */
	p.clock_freq = (p.clock_freq == 0) ? 10000 : p.clock_freq;
	if(ftdi_set_baudrate(ftio->ctx, p.clock_freq) != 0) {
		goto _ftio_open_error_handler;
	}

	/* set output mask */
	if(ftdi_set_bitmode(ftio->ctx, p.io_mask, BITMODE_SYNCBB) != 0) {
		goto _ftio_open_error_handler;
	}

	ftio->params = p;
	ftio_free_device_list(dev_list);
	return(ftio);

_ftio_open_error_handler:;
	ftio_free_device_list(dev_list);
	ftio_close(ftio);
	return(NULL);
}

/**
 * end of ftio.c
 */
