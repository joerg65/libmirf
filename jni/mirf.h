/*
	Mirf library ported to Android
 
	Copyright (C) <2016>  <Jörg Wolff>

	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
    Copyright (c) 2007 Stefan Engelke <mbox@stefanengelke.de>

    Permission is hereby granted, free of charge, to any person 
    obtaining a copy of this software and associated documentation 
    files (the "Software"), to deal in the Software without 
    restriction, including without limitation the rights to use, copy, 
    modify, merge, publish, distribute, sublicense, and/or sell copies 
    of the Software, and to permit persons to whom the Software is 
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be 
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.

    $Id$
*/

/*
    Copyright (c) 2007 Stefan Engelke <mbox@stefanengelke.de>

    Permission is hereby granted, free of charge, to any person 
    obtaining a copy of this software and associated documentation 
    files (the "Software"), to deal in the Software without 
    restriction, including without limitation the rights to use, copy, 
    modify, merge, publish, distribute, sublicense, and/or sell copies 
    of the Software, and to permit persons to whom the Software is 
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be 
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.

    $Id$

	Ported and extended to Android by J.Wolff 09/2016.
*/


#ifndef __mirf_H__
#define __mirf_H__

/*
#define RADDR				(uint8_t *)"mirf2"
#define TADDR				(uint8_t *)"mirf1"
#define TADDR2				(uint8_t *)"mirf3"*/

#include <stdint.h>

#define mirf_CONFIG		( (1<<EN_CRC) | (0<<CRCO) )

class mirf
{

public:
  mirf(uint8_t _cepin, uint32_t _freq, uint8_t _spi_channel, uint8_t _payload_size, uint8_t _mirf_CH);
  void config(void);
  void reconfig_rx(void);
  void reconfig_tx(void);
  void set_address(uint8_t pos, uint8_t* address);
  uint8_t receive_data(void* buf);
  uint8_t transmit_data(void* buf);
  uint8_t status(void);
  uint8_t max_rt_reached(void);
  uint8_t data_ready(void);
  uint8_t read_register(uint8_t reg, uint8_t* buf, uint8_t len);
  uint8_t read_register(uint8_t reg);
  uint8_t write_register(uint8_t reg, const uint8_t* buf, uint8_t len);
  uint8_t write_register(uint8_t reg, uint8_t value);
  void config_register(uint8_t reg, uint8_t value);
  uint8_t get_data(void* buf);
  uint8_t send_data(void* buf);
  void power_up_rx(void);
  void power_up_tx(void);
  void power_down(void);
  uint8_t flush_rx(void);
  uint8_t flush_tx(void);
  void start_listening(void);
  void stop_listening(void);
  void delay_us(unsigned int howLong);

private:
  uint8_t ce_pin;
  uint32_t freq;
  uint8_t spi_channel;
  uint8_t payload_size;
  uint8_t mirf_CH;

  uint8_t *addr[6] = { (uint8_t *)"mirf1", (uint8_t *)"mirf2", (uint8_t *)"mirf3", (uint8_t *)"mirf4", (uint8_t *)"mirf5", (uint8_t *)"mirf6" };
};
#endif
