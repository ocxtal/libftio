
/**
 * @file ftio.h
 *
 * @brief synchronous bit-bang utils for FTDI chips
 */
#ifndef _FTIO_H_INCLUDED
#define _FTIO_H_INCLUDED

#include <stdint.h>

/**
 * @struct ftio_device_s
 */
struct ftio_device_s {
	char *manu;
	char *desc;
};
typedef struct ftio_device_s ftio_device_t;

/**
 * @struct ftio_params_s
 */
struct ftio_params_s {
	ftio_device_t const *device;
	int64_t clock_freq;
	uint8_t io_mask;
};
typedef struct ftio_params_s ftio_params_t;

/**
 * @type ftio_t
 */
typedef struct ftio_s ftio_t;


/* basic functionalities */
/**
 * @fn ftio_get_device_list
 */
ftio_device_t *ftio_get_device_list();

/**
 * @fn ftio_free_device_list
 */
void ftio_free_device_list(
	ftio_device_t *device_list);

/**
 * @fn ftio_open
 */
ftio_t *ftio_open(
	ftio_params_t const *params);

/**
 * @fn ftio_close
 */
void ftio_close(
	ftio_t *ftio);

/**
 * @fn ftio_readwrite
 */
int ftio_readwrite(
	ftio_t *ftio,
	uint8_t const *out,
	uint8_t *in,
	uint64_t len);


/* high-level functionalities */
/**
 * @fn ftio_spi_readwrite
 */
int ftio_spi_readwrite(
	ftio_t *ftio,
	uint8_t const *out,
	uint8_t *in,
	uint64_t len);

/**
 * @fn ftio_i2c_readwrite
 */
int ftio_spi_readwrite(
	ftio_t *ftio,
	uint8_t const *out,
	uint8_t *in,
	uint64_t len);

/**
 * @fn ftio_595_write
 */
int ftio_595_write(
	ftio_t *ftio,
	uint8_t const *out,
	uint64_t len);

#endif
/**
 * end of ftio.h
 */
