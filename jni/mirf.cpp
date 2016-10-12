
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "nRF24L01.h"
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include "mirf.h"

#ifdef ANDROID
#include <android/log.h>
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOG_TAG "mirf-android"
#define printf	LOGI
#endif

/****************************************************************************/

mirf::mirf(uint8_t _cepin, uint32_t _freq, uint8_t _spi_channel, uint8_t _payload_size, uint8_t _mirf_CH): ce_pin(_cepin), freq(_freq), spi_channel(_spi_channel), payload_size(_payload_size), mirf_CH(_mirf_CH)
{
   /* sets up the wiringPi library */
   printf("RF24::begin: sets up the wiringPi library)");
   if (wiringPiSetup () < 0) {
      printf ("Unable to setup wiringPi: %s\n", strerror (errno));
   }
   printf("RF24::begin: sets up the wiringPi library SPI)");
   printf("RF24::begin: SPI speed: %u)", freq);
   int dev = wiringPiSPISetup(spi_channel, freq);
   //printf ("dev = %d\n", spi_channel);
   if (dev < 0) {
      printf ("Unable to open spi device: %s\n", strerror (errno));
   }

   printf ("RF24::ce_pin: %d\n\r", ce_pin);
   pinMode(ce_pin, OUTPUT);
   printf ("RF24::digitalWrite:\n\r");
   digitalWrite(ce_pin, 0);

}

/****************************************************************************/

// Sets the important registers in the MiRF module and powers the module
// in transmitting mode
void mirf::config(void) {
	// Set RF channel
	config_register(RF_CH, mirf_CH);
	
	// Set length of incoming payload 
	config_register(RX_PW_P0, payload_size);
	config_register(RX_PW_P1, payload_size);
	
	// Set auto retransmit delay to 500uS and leave auto retransmit count to 3
	config_register(SETUP_RETR, (1<<4) | 3);
	
	// Set RADDR and TADDR
	write_register(RX_ADDR_P0, addr[0], 5);
	write_register(RX_ADDR_P1, addr[1], 5);
    write_register(RX_ADDR_P2, addr[2], 5);
    write_register(RX_ADDR_P3, addr[3], 5);
    write_register(RX_ADDR_P4, addr[4], 5);
    write_register(RX_ADDR_P5, addr[5], 5);
	write_register(TX_ADDR, addr[0], 5);
	
	// Enable RX_ADDR_P0 and RX_ADDR_P1 address matching since we also enable auto acknowledgement
	config_register(EN_RXADDR, (1<<ERX_P0 | 1<<ERX_P1 | 1<<ERX_P2 | 1<<ERX_P3 | 1<<ERX_P3 | 1<<ERX_P5));
	
	// Power up in transmitter mode
	power_up_tx();
}

// Reconfigs the MIRF to receiving mode
void mirf::reconfig_rx(void) {

	// Set RADDR and TADDR
	write_register(RX_ADDR_P0, addr[0], 5);
	write_register(RX_ADDR_P1, addr[1], 5);
    write_register(RX_ADDR_P2, addr[2], 5);
    write_register(RX_ADDR_P3, addr[3], 5);
    write_register(RX_ADDR_P4, addr[4], 5);
    write_register(RX_ADDR_P5, addr[5], 5);
	write_register(TX_ADDR, addr[0], 5);
	
	// Enable RX_ADDR_P0 and RX_ADDR_P1 address matching since we also enable auto acknowledgement
	config_register(EN_RXADDR, (1<<ERX_P0 | 1<<ERX_P1 | 1<<ERX_P2 | 1<<ERX_P3 | 1<<ERX_P3 | 1<<ERX_P5));
	
	// Power up in transmitter mode
	power_up_tx();
}

// Reconfigs the MIRF to transmitting mode
void mirf::reconfig_tx(void) {

	// Set RADDR and TADDR
	write_register(RX_ADDR_P0, addr[0], 5);
	write_register(RX_ADDR_P1, addr[1], 5);
	write_register(TX_ADDR, addr[0], 5);
	
	// Enable RX_ADDR_P0 and RX_ADDR_P1 address matching since we also enable auto acknowledgement
	config_register(EN_RXADDR, (1<<ERX_P0 | 1<<ERX_P1));
	
	// Power up in transmitter mode
	power_up_tx();
}

/****************************************************************************/
// set rx and tx addressess (0 = tx and rx[0], 1 .. 5 = rx[x])
// need to reconfig after setting
void mirf::set_address(uint8_t pos, uint8_t* address) {
    if (pos > 5) return;
    printf ("set_address.pos: %d .address: %s", pos, address);
    addr[pos]=address;
}

/****************************************************************************/

// Receive data
uint8_t mirf::receive_data(void* buf) {

	power_up_rx(); // Power up to receiver mode
	digitalWrite(ce_pin, 1);

	uint8_t waitcount = 0;
	while (!(status() & (1<<RX_DR))) { // Wait until data is ready to be read
		delayMicroseconds(500);
		waitcount++;
		if (waitcount >= 100) { // If we haven't heard back in 50ms, exit
			digitalWrite(ce_pin, 0); // Stop listening
			return 0;
		}
	}
	digitalWrite(ce_pin, 0);
	get_data(buf);
	config_register(STATUS,(1<<RX_DR)); // Reset status register

	return 1;
}


// Send data and wait for an ACK
uint8_t mirf::transmit_data(void* buf) {

   printf ("transmit_data.node: %x", ((char*)buf)[0]);

	power_up_tx();  // Power up to transmitter mode
	flush_tx(); // Write cmd to flush tx fifo

	send_data(buf); // Write payload

	digitalWrite(ce_pin, 1); // Start transmission
	delayMicroseconds(15);
	digitalWrite(ce_pin, 0);
	
	int waitcount = 0;
	while (!(status() & (1<<TX_DS))) { // Wait until we receive an ACK back
		delayMicroseconds(500); // Wait the auto retransmit time
		if (status() & (1<<MAX_RT)) { // If maximum retries reached
			config_register(STATUS, 1<<MAX_RT); // Reset maximum retries to be able to transmit again next time
			digitalWrite(ce_pin, 1);
			delayMicroseconds(15);
			digitalWrite(ce_pin, 0);
		}
		waitcount++;
		if (waitcount >= 10) { // If we haven't heard back in 5ms, exit
			config_register(STATUS, 1<<MAX_RT); // Reset maximum retries to be able to transmit again next time
			config_register(STATUS,(1<<TX_DS)); // Reset status register
			printf("Transmit ACK timeout\n\r");			
			return 0;
		}
	}
	
	config_register(STATUS,(1<<TX_DS)); // Reset status register
	
	return 1;
}

/****************************************************************************/

// Read the status register
uint8_t mirf::status(void) {
  uint8_t status = read_register(STATUS);
  return status;
}

/****************************************************************************/

// Checks if MAX_RT has been reached
uint8_t mirf::max_rt_reached(void) {
  uint8_t status = read_register(STATUS);
  return status & (1<<MAX_RT);
}

/****************************************************************************/

// Checks if data is available for reading
uint8_t mirf::data_ready(void) {
  uint8_t status = read_register(STATUS);
  return status & (1<<RX_DR);
}

/****************************************************************************/

uint8_t mirf::read_register(uint8_t reg, uint8_t* buf, uint8_t len)
{
  int ret;
  uint8_t status;
 
  uint8_t cnt = len + 1;
  uint8_t outbuffer[18];
  outbuffer[0] = R_REGISTER | ( REGISTER_MASK & reg );
  ret = wiringPiSPIDataRW (spi_channel, outbuffer, cnt);
  memcpy(buf, &outbuffer[1], sizeof(uint8_t) * len);
  status = outbuffer[0];  

  return status;
}

/****************************************************************************/

uint8_t mirf::read_register(uint8_t reg)
{

    uint8_t buffer[1];
    buffer[0] = R_REGISTER | ( REGISTER_MASK & reg );
    read_register(reg,buffer,sizeof buffer);
    uint8_t result = buffer[0];

  return result;
}

/****************************************************************************/

uint8_t mirf::write_register(uint8_t reg, const uint8_t* buf, uint8_t len)
{
  uint8_t status;

  int ret;
  uint8_t cnt = len + 1;
  uint8_t outbuffer[18];
  outbuffer[0] = W_REGISTER | ( REGISTER_MASK & reg );
  
  memcpy(&outbuffer[1], buf, sizeof(uint8_t) * len);
  ret = wiringPiSPIDataRW (spi_channel, outbuffer, cnt);
  status = outbuffer[0];

  return status;
}

/****************************************************************************/

uint8_t mirf::write_register(uint8_t reg, uint8_t value)
{
  uint8_t status;

    uint8_t buffer[1];
    buffer[0] = value;
    status = write_register(reg,buffer,sizeof buffer);

  return status;
}

/****************************************************************************/

// Write one byte into the MiRF register
void mirf::config_register(uint8_t reg, uint8_t value) 
{
  write_register(reg, value);
}

/****************************************************************************/

uint8_t mirf::get_data(void* buf)
{
  uint8_t status;
  uint8_t* current = reinterpret_cast<uint8_t*>(buf);

  int ret;
  uint8_t cnt = payload_size + 1;
  uint8_t outbuffer[cnt];
  outbuffer[0] = R_RX_PAYLOAD;
  ret = wiringPiSPIDataRW (spi_channel, outbuffer, cnt);
  memcpy(current, &outbuffer[1], sizeof(uint8_t) * payload_size);
  status = outbuffer[0]; 

  return status;
}

/****************************************************************************/

uint8_t mirf::send_data(void* buf)
{
  uint8_t status;
  uint8_t* current = reinterpret_cast<uint8_t*>(buf);

  int ret;
  uint8_t outbuffer[payload_size + 1];
  outbuffer[0] = W_TX_PAYLOAD;
  memcpy(&outbuffer[1], current, payload_size);
  printf("outbuf: 0x%x 0x%x 0x%x cnt: %d\n\r", outbuffer[0], outbuffer[1], outbuffer[2], payload_size + 1);
  ret = wiringPiSPIDataRW (spi_channel, outbuffer, payload_size + 1);
  status = outbuffer[0]; 

  return status;
}

/****************************************************************************/

void mirf::power_up_rx(void)
{
  config_register(CONFIG, mirf_CONFIG | ( (1<<PWR_UP) | (1<<PRIM_RX) ));
}

/****************************************************************************/

void mirf::power_up_tx(void)
{
  config_register(CONFIG, mirf_CONFIG | ( (1<<PWR_UP) | (0<<PRIM_RX) ));
}

/****************************************************************************/

void mirf::power_down(void)
{
  config_register(CONFIG, mirf_CONFIG | (0<<PWR_UP) );
}

/****************************************************************************/

uint8_t mirf::flush_rx(void)
{
  uint8_t status;

  int ret;
  uint8_t cnt = 1;
  uint8_t outbuffer[18];
  outbuffer[0] = FLUSH_RX;
  ret = wiringPiSPIDataRW (spi_channel, outbuffer, cnt);
  status = outbuffer[0];

  return status;
}

/****************************************************************************/

uint8_t mirf::flush_tx(void)
{
  uint8_t status;

  int ret;
  uint8_t cnt = 1;
  uint8_t outbuffer[18];
  outbuffer[0] = FLUSH_TX;
  ret = wiringPiSPIDataRW (spi_channel, outbuffer, cnt);
  status = outbuffer[0];

  return status;
}

/****************************************************************************/

void mirf::start_listening(void)
{
  digitalWrite(ce_pin, 1); // Start listening
}

/****************************************************************************/

void mirf::stop_listening(void)
{
  digitalWrite(ce_pin, 0); // Start listening
}

void mirf::delay_us(unsigned int howLong)
{
  delayMicroseconds(howLong);
}
