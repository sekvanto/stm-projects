This is supposed to work with DMA, but i didn't add it yet.
I wrote a `spi_dma` module and it works correctly with readonly stuff like LCD display, but doesn't work with SPI SD card.
My guess it's probably because LCD display doesn't require MISO (reading info) unlike SD card.
Either way, the module works, but there is a small bug somewhere yet to be found.

So essentially the program is done, albeit the reading still happens much slower than it would be acceptable for playing the wave, unfortunately.

What i tried: The SPI interface by itself is slow, so for speeding it up one might use DMA buffer transfers. The issue here is i wrote a module for that, but it works only with readonly SPI interactions like if you take a LCD display. However, SD card is bidirectional, and the transfer fails to be done, as in gets stuck in an endless loop. This is basically the same issue as with BMP files display implementation, however that one i actually finished fully because you don't really need the speed of DMA there.

The good news is: this exercise was supposed to be meant for learning about DMA driven DAC, which i implemented fully correctly. The waves are being played in a right way. The issue is related to _getting the wave data_ from SD-card where reading speed is slower than needed. This might be fixed either by using SPI DMA, or by switching to another Fs/interaction interface.

Update: I added DMA to the SPI communication module, and changed the speed to fast. Hovewer, the reading speed is still extremely slow and the audio is not being produced correctly. Later i'll try to find another solution to this issue.
