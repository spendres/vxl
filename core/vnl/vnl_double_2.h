// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
//-*- c++ -*-------------------------------------------------------------------
#ifndef vnl_double_2_h_
#define vnl_double_2_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : vnl_double_2
//
// .SECTION Description
//    A vnl_vector of 2 doubles.
//
// .NAME        vnl_double_2 - vnl_vector of 2 doubles. 
// .LIBRARY     vnl
// .HEADER	Numerics package
// .INCLUDE     vnl/vnl_double_2.h
// .FILE        vnl/vnl_double_2.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 04 Aug 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_vector_fixed.h>

class vnl_double_2 : public vnl_vector_fixed<double,2> {
public:
  vnl_double_2() {}
  vnl_double_2(const vnl_vector<double>& rhs): vnl_vector_fixed<double,2>(rhs) {}
  vnl_double_2(const vnl_vector_fixed<double,2>& rhs): vnl_vector_fixed<double,2>(rhs) {}
  vnl_double_2(double px, double py) {
    data[0] = px;
    data[1] = py;
  }
};

#ifdef VCL_GCC_27
inline bool operator==(const vnl_double_2& a, const vnl_double_2& b)
{
  return *(vnl_vector<double> const*)&a == *(vnl_vector<double> const*)&b;
}

ostream& operator<< (ostream& s, vnl_double_2 const& a);
#endif

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_double_2.
