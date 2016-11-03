#include "SpiDev.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>
#include <linux/spi/spidev.h>
#include <cstring>
#include <iostream>

using namespace std;

spiConnect::spiConnect() {}               //Default Constructor;
spiConnect::~spiConnect()
{
  if (spiEnable > 0)
  {
    close(spiEnable); //Deconstructor which will close the bus
  }                   //connection gracefully
}

int spiConnect::spiPathTest(int path)
{
  return fcntl(path, F_GETFD) != -1 || errno != EBADF;
}

int spiConnect::init(const string   device,
		     const uint8_t  mode,
		     const uint8_t  bitsParam,
		     const uint8_t  LSB,
		     const uint32_t speed,
		     const string   nameParam)
{
  deviceName = nameParam;
  frequency = speed;
  bits = bitsParam;
  int ret; 
  
  if ((spiEnable = open(device.c_str(), O_RDWR)) < 0)
  {
    perror("Error Open Device");
    return ERROR_OPEN_DEVICE;
  }
  ////////////////
  if (spiPathTest(spiEnable) == -1)
  {
    perror("Error file descriptor is invalid");
    return ERROR_FD;
  }
  else
  {
    cout << "Spi Path is good" << endl;
  }
  ////////////////
  // set mode
  ret = ioctl(spiEnable, SPI_IOC_WR_MODE, &mode);

  if (ret < 0)
  {
    perror("Error Set SPI-Mode");
    return ERROR_SPI_MODE_SETUP;
  }

  // check mode
  ret = ioctl(spiEnable, SPI_IOC_RD_MODE, &mode);

  if (ret < 0)
  {
    perror("Error Get SPI-Mode");
    return ERROR_SPI_CHECK_MODE;
  }

  // set Least Significant Bit (LSB) MODE
  ret = ioctl(spiEnable, SPI_IOC_WR_LSB_FIRST, &LSB);

  if (ret < 0)
  {
    perror("Error Set LSB-Mode");
    return ERROR_SPI_MODE_SETUP;
  }

  // SPI word length
  ret = ioctl(spiEnable, SPI_IOC_WR_BITS_PER_WORD, &bits);

  if (ret < 0)
  {
    perror("Error Set Wordlength");
    return ERROR_SPI_SET_WORDLENGTH;
  }

  // SPI get length
  ret = ioctl(spiEnable, SPI_IOC_RD_BITS_PER_WORD, &bits);

  if (ret < 0)
  {
    perror("error Get Wordlength");
    return ERROR_SPI_GET_WORDLENGTH;
  }
  
  // Set Data Rate
  ret = ioctl(spiEnable, SPI_IOC_WR_MAX_SPEED_HZ, &frequency);

  if (ret < 0)
  {
    perror("Error Set Speed");
    return ERROR_SPI_SET_DATARATE;
  }
  
  // check datarate
  ret = ioctl(spiEnable, SPI_IOC_RD_MAX_SPEED_HZ, &frequency);

  if (ret < 0)
  {
    perror("Error Get Speed");
    return ERROR_SPI_GET_DATARATE;
  }

  // Print all spi messages

  cout << "SPI Conection - " << deviceName << endl;
  cout << "SPI Device    - " << device << endl;
  cout << "SPI MODE      - " << mode << endl;
  cout << "Wordlength    - " << bits << endl;
  cout << "Speed         - " << (frequency/1000) << "kHz" << endl;
  cout << "Filehandle    - " << spiEnable << endl;

  return 0;
}

int spiConnect::transfer(uint8_t *dataPtr,
			 int     length)
{
  int ret;
  uint8_t rx[length];

  memset(rx, 0, sizeof(uint8_t) * length);
  memset(&spi, 0, sizeof(spi));

  spi.tx_buf        = (unsigned long)dataPtr;
  spi.rx_buf        = (unsigned long)rx;
  spi.len           = length;
  spi.delay_usecs   = 0;
  spi.speed_hz      = frequency;
  spi.bits_per_word = bits;
  spi.cs_change     = 0;

  ret = ioctl(spiEnable, SPI_IOC_MESSAGE(1), &spi);

  if (ret < 0)
  {
    switch (errno)
    {
      case EBADF:
        cout << "ERROR: fd is not a valid file descriptor." << endl;
        break;
      case EFAULT:
        cout << "ERROR: argp references an inaccessible memory area" << endl;
        break;
      case EINVAL:
        cout << "ERROR: request or argp is not valid" << endl;
        break;
      case ENOTTY:
        cout << "ERROR: fd is not associated with a character special device" << endl;
        break;
      default: // Perhaps this should be default:
        cout << "ERROR: The specified request does not apply to the kind of object that the file descriptor fd references." << endl;
    }
    perror("Error on I/O - ioctl");
    return -1;
  }

  memcpy(dataPtr, rx, sizeof(uint8_t) * length);

  return 0;
}
