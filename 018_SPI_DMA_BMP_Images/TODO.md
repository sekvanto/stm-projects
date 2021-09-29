This is supposed to work with DMA, but i didn't add it yet.
I wrote a `spi_dma` module and it works correctly with LCD display itself, but doesn't work with SPI SD card.
My guess it's probably because LCD display doesn't require MISO (reading info) unlike SD card.
Either way, the module works, but there is a small bug somewhere yet to be found.
I'll leave the code of the module in `corrupted_spi_dma_module` directory, it's for the future TODO.
