#ifndef vepl1_threshold_h_
#define vepl1_threshold_h_
//:
// \file
// \brief set pixel to given value if above/below certain threshold
//
//   This class actually implements two threshold operations, namely the
//   `classical' single threshold mapping where pixel values not larger than
//   T are mapped to L, values larger than T to H; and the `clipping'
//   operation where only pixel values below T are mapped to L, the others
//   left unchanged.
//
//   For the first version, the constructor must be given three parameters:
//   T, L and H.  For the second version, two parameters: T and L.
//   In the second situation, there clearly must be an implicit conversion
//   from DataIn to DataOut.  Note that this is not necessary in the
//   first situation, because all output pixels will be either L or H.
//
//   Note that the input image data type must support "operator<=()"; thus
//   thresholding of colour images makes no sense (unless you define a
//   sensible "<=" for RGB triples).
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   28 April 2001

#include <vil1/vil1_image.h>

//: set pixel to given value if above/below certain threshold
vil1_image vepl1_threshold(vil1_image const& , double threshold=128.0, double below=1.0, double above=0.0);

#endif // vepl1_threshold_h_
