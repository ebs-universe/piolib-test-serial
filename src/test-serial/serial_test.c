

#include <application.h>
#include <hal/uc/uart.h>

#ifdef APP_ENABLE_BLINK
#include <blink.h>
#endif

uint8_t test_serial_message[] = "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ"; 
uint8_t test_serial_appl = 0;
uint8_t test_serial_read_buf[64];


#if TEST_SERIAL_TYPE == 0
  
static inline HAL_BASE_t testif_population_rxb(void) {
  return uart_population_rxb(TEST_SERIAL_INTFNUM);
}

static inline HAL_BASE_t testif_read(uint8_t *buffer, HAL_BASE_t len) {
  return uart_read(TEST_SERIAL_INTFNUM, buffer, len);
}

static inline HAL_BASE_t testif_putc(uint8_t byte, uint8_t token, HAL_BASE_t handlelock){
  return uart_putc(TEST_SERIAL_INTFNUM, byte, token, handlelock);
}
 
static inline HAL_BASE_t testif_reqlock(HAL_BASE_t len, uint8_t token){
  return uart_reqlock(TEST_SERIAL_INTFNUM, len, token);
}

static inline HAL_BASE_t testif_write(uint8_t *buffer, HAL_BASE_t len, uint8_t token){
  return uart_write(TEST_SERIAL_INTFNUM, buffer, len, token);
}

static inline void testif_send_trigger(void){
  return uart_send_trigger(TEST_SERIAL_INTFNUM);
}

#elif TEST_SERIAL_TYPE == 1

static inline HAL_BASE_t testif_population_rxb(void) {
  return usbcdc_population_rxb(TEST_SERIAL_INTFNUM);
}

static inline HAL_BASE_t testif_read(uint8_t *buffer, HAL_BASE_t len) {
  return usbcdc_read(TEST_SERIAL_INTFNUM, buffer, len);
}

static inline HAL_BASE_t testif_putc(uint8_t byte, uint8_t token, HAL_BASE_t handlelock){
  return usbcdc_putc(TEST_SERIAL_INTFNUM, byte, token, handlelock);
}
 
static inline HAL_BASE_t testif_reqlock(HAL_BASE_t len, uint8_t token){
  return usbcdc_reqlock(TEST_SERIAL_INTFNUM, len, token);
}

static inline HAL_BASE_t testif_write(uint8_t *buffer, HAL_BASE_t len, uint8_t token){
  return usbcdc_write(TEST_SERIAL_INTFNUM, buffer, len, token);
}

static inline void testif_send_trigger(void){
  return usbcdc_send_trigger(TEST_SERIAL_INTFNUM);
}

#else
  #error "Interface for the Serial Test has an unrecognized type" 
#endif


static void test_serial_read_task(void)
{
  if ( test_serial_appl ) {
    return;
  }
  if ( testif_population_rxb() )
  {
    testif_read(&test_serial_read_buf[0], 1);
    test_serial_appl = test_serial_read_buf[0];
    if (test_serial_appl) {
        #ifdef APP_ENABLE_BLINK
        set_blink_period(100);
        #endif
        testif_putc(test_serial_appl, 0x00, 0x00);
    }
    return;
  }
}

static void test_serial_write_task(void) {
    if (test_serial_appl == 'a') {
        if (testif_reqlock(sizeof(test_serial_message) - 1, 0x00)){
            testif_write(test_serial_message, sizeof(test_serial_message) - 1, 0x00);
        } else {
            testif_send_trigger();
        }
    }
}

void test_serial_loophandler(void){
    test_serial_read_task();
    test_serial_write_task();
}

