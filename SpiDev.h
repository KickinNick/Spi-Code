/*
 * Basic Spi Bus Class Header
 */
#include <iostream>
#include <string>
#include <linux/spi/spidev.h>
#include <stdint.h>
using namespace std;

class spiConnect
{
  private:
    struct spi_ioc_transfer spi; //Structure containing base settings for the bus
    int spiEnable; //Used to confirm spi path is enabled when open() is given
    int frequency; //Speed in Hz
    string deviceName; //File path
    uint8_t bits; //Bits per word

    int spiPathTest(int path);
    
    /* Error Messages for SPI bus configuration */

    const int ERROR_OPEN_DEVICE        = -1;
    const int ERROR_SPI_MODE_SETUP     = -2;
    const int ERROR_SPI_CHECK_MODE     = -3;
    const int ERROR_SPI_SET_WORDLENGTH = -4;
    const int ERROR_SPI_GET_WORDLENGTH = -5;
    const int ERROR_SPI_SET_DATARATE   = -6;
    const int ERROR_SPI_GET_DATARATE   = -7;
    const int ERROR_FD                 = -8;
 public:
    spiConnect(); //Default Constructor
    virtual ~spiConnect(); //Default Destructor

    int init(string   device,
	     uint8_t  mode,
	     uint8_t  bitsParam,
	     uint8_t  LSB,
	     uint32_t speed,
	     string nameParam);

    int transfer(uint8_t *dataPtr,
		 int      length);
};
