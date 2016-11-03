#include "SpiDev.h"

int main(int argc, char const *argv[]) {
  /**
   * Example usage
   */

  spiConnect spi_conn;

  spi_conn.init("/dev/spidev1.0",
		SPI_CPOL | !SPI_CPHA,
		8,
		0,
	        1500,
		"Test Spi");

  uint8_t data[30] = "HELLOMYNAMEISBOB";
  spi_conn.transfer(data, 30);

  return 0;
}
