# Twittering E-Solar-Bird with ATtiny13-Chip

Do you want to improve the world with electronics? Then this solar-powered E-Bird is for you. This bird alternative operates completely autonomously and is friendly in any climate. The bird starts chirping on its own as soon as there's a bit of light.

This is how the circuit board design looks like.
![SolraBird-PCB](2020Inkscape-SaveTheBirdsPCB/solarbird-kicad-x2-minmin-2.svg)

Freshly programmed, the bird also twitters. It adjusts its enthusiasm according to light conditions. The program is easily adjustable, which is why we've used this circuit for workshops, for example in schools.

In this project you find my chirping program and circuit board.

Some details for the old, still analog version of the solar bird you can find on [this page](http://mechatronicart.ch/diymakeaway/solarbird).

Protect the species!

This project has grown [within the community](https://mechatronicart.ch/). The basis comes from Christoph Haberer. Sounddesign and circuit board by [taube.xyz](/pick/kontakt), Spring 2011 and September 2020.

For some more pictures, visit [this homepage](https://taube.xyz/pick/solarbird).

## Some hints and advices building

- The twittering program works best on ATtiny13 or ATtiny13V chips. **Avoid the newer ATtiny13A chips.** The reason is, that these newer chips interrupt the programm, when the voltge drops a certain level. This is normally a good thing. But the behavior of the chips without the "A" is more interesting for the solar bird: the internal clock gets slower, and so the chirping gets slower and deeper in sound as well. This is perfect for manipulation of the twittering by making shadows on the solar cell. Also, you can program very fast twitterings and make them hearable when you put the bird into the shadow.
- The circuit board is designed so that you can use SMD-Chips, but as well the DIL-chips with the long legs. Simply bend the legs 90 degrees and solder them onto the surface. No need to drill holes.
- For programming the chip, there are 6 pins in a row on the backbones of the bird, from head to tail: GND, 5V, RESET, SCK, MISO, MOSI. It's easy there to solder a single row of pin-headers.
- If you want to program the chip without disconnecting the piezo speaker, you can add a resistor in between the speaker and the chip. There is a gap on the track in the lower part of the circuit board between pin 5 of the chip and the beak of the bird. Bridge it with a resistor of maybe 1K max.
- If you want to add sensors to the ADC of the chip, beware that the ADC doesn't work securely when the voltage drops very low. The working of the chip with very low voltages is a main feature of this solar bird. Test it well, if it works the way you like!