# GCode Image Generators

Two programs which convert specifically formatted text documents into GCode for a pencil drawing machine powered by an arduino with a CNC shield.

## How to Run

Download the file and / or run the code on your IDE of choice (Codeblocks or DevC++ should both work). For both programs, type the name of the text file which is being inspected followed by ".txt". If there are no issues in the formatting of the document, the program will eventually finish and output a new text file with the same name and a suffix, "GCode". It is important that the example documents are not tampered with to prevent unwanted effects as there are no implemented safety measures for the program to recognize incorrect syntax.

## How to Use

### "GCode Pixel Generator"

The first program is simpler and essentially makes pixel art by drawing identical square shapes adjacent to one another. The text document should resemble a rectangular grid of numbers including 0, 1, and 2. 0 represents an empty pixel (white), 1 represents a lightly filled pixel (an outlined square), and 2 represents a dark pixel (a square with a diagonal line going across). If these options are viewed as different colours, then the text documents conceptually resemble a bitmap. Existing documents can easily be edited one pixel at a time to create new images or they can be created from scratch; so long as the grid is perfectly rectangular, the program automatically counts the size of the image. When given a file name, the program will display the image it finds, giving the user a chance to change mistakes or spot issues with the program. If the image is valid, the program outputs a text file with all of the GCode instructions for a CNC-powered machine to be able to draw the image (see attached examples).

### "GCode Line Generator"

The second program uses a more complicated but versatile way of describing an image. Put briefly, the image is created from a set of enclosed polygons which are either empty or shaded in with regular, parallel lines.  
  
The syntax that this program recognizes is as follows:  
  
The text file begins by defining a set of fundamental parameters with numerical values (floats and decimals are allowed). It is important that the names of these parameters are not changed and no additional characters are added after the given numbers as the names are merely cosmetic and the program is told to ignore series of characters of a certain length. Following these definitions, the syntax becomes slightly more open.   
  
Shapes are created by connecting points on a Cartesian plane in a certain order; as such, these points must be defined before a shape can be created using them. Putting and open bracket - ( - signifies to the program that a point is being defined and the syntax for this point is identical to that of an ordered pair in mathematics --> (X.x,Y.y) , no spaces. The program assigns numbers to points starting at 1 in the order that it finds them in (numbers found before points in the examples are cosmetic and ignored by the program). 
  
Once all the points needed to make a shape are defined, One line of text defines a shape. The beginning of a shape definition is signified by the capital letter "S". Following the "S" can either be a "P", "F", or "A", standing for "Perimeter", "Fill", or "All". This dictates whether the program draws the outline of the shape, its filled area, or both. After one additional space, the numbers of the points used to draw the shape are listed in their desired order (imagine a game of "connect the dots"). One more space after that, the mathematical slope (rise over run) and the spacing of the lines used to fill the shape are defined, separated by a space. 

This process can be repeated to make multiple shapes and points can be repeatedly used for different shapes or the same shape if need be. This process is by no means intuitive so refer to the example code for clarifications.

### Practical Success

This program was proven to work after being used to create instructions for a real machine that reproduced the desired images successfully.
