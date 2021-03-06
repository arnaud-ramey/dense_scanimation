                  +----------------------+
                  |  dense_scanimation   |
                  +----------------------+

[![Build Status](https://travis-ci.org/arnaud-ramey/dense_scanimation.svg)](https://travis-ci.org/arnaud-ramey/dense_scanimation)

A tool for generating dense Scanimation images.

License :                  see the LICENSE file.
Authors :                  see the AUTHORS file.
How to build the program:  see the INSTALL file.

Scanimation is a state-of-the-art six-phase animation process that combines
the "persistence of vision" principle with a striped acetate overlay to give
the illusion of movement.
It originally referes to the process of
placing an acetate overlay containing vertical stripes
over the image and slowly moved from right to left.
These are sold in sets under the brand name Scanimation.
It was invented by Rufus Butler Seder,
and patented in 2006 under US Patent #7151541.[6]
The original books are sold here:
  http://scanimationbooks.com/

However, the original design of the overlay made of vertical bars
suffers from a poor density of the transparent zones:
for an image made of ten images, the transparent bars only represents 10% of
the total area, so that it can be hard to see well the underlying drawing.

We then developped a technique called "Dense Scanimation".
The idea is to change the pattern:
instead of using vertical transparent bars, we can actually use transparent zones
that are better distributed on the image:
most notably, the transparents zones are shifted from one line to another,
so that they are not aligned vertically.
Even though the global ratio of transparent zones is the same,
their visual density is more homogenous, and the animation is easier to see.

For instance, for an animation with four images,
the classical Scanimation overlay would look like:
###.###.###.###.###
###.###.###.###.###
###.###.###.###.###
###.###.###.###.###
(where '#' represents a black dot and '.' a transparent one)

The Scanimation overlay would look like:
###.###.###.###.###
.###.###.###.###.##
#.###.###.###.###.#
##.###.###.###.###.
###.###.###.###.###

The program both generates the overlay and the collage image with these patterns
using any input images.
It also ships a GUI to visualize the technique.
________________________________________________________________________________

How to use the program
________________________________________________________________________________
To display the help, just launch the program in a terminal.
It will display the help of the program.

== Synopsis ==

$ dense_scanimation_maker FRAMEIMAGES

where FRAMEIMAGES is the list of frames composing the animation.

The following output files are generated:
* "big_image.png" : the color collage image made by mixing the different input images
* "overlay.png" :   the monochrome overlay

The size of these two output images is equal to the size of the biggest input image.
In the collage image, the smaller input images are centered.

== Keyboard shortcuts ==
* 'b'                 change background color
* 'o'                 change overlay function
* 'c'                 use vertically constrained mode
* 'q', Esc            quit

________________________________________________________________________________

Samples
________________________________________________________________________________

Using the sample images given with the library:
$ dense_scanimation_maker samples/donjon??.*
