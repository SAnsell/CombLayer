/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/PolynomialRoots.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef mathLevel_PolynomialRoots_h
#define mathLevel_PolynomialRoots_h

namespace mathLevel
{

  /*!
    \class PolynomialRoot
    \version 1.0
    \author S. Ansell and D. Eberly
    \date December 2007
    \brief Finds real root for high-order polynominals 

    Methods are
    
    A: algebraic using closed-form expressions (fast, typically not robust)
    B: bisection (after root bounding, slow and robust)
    N: Newton's/bisection hybrid (after root bounding, medium and robust)
    E: eigenvalues of a companion matrix (fast and robust)
    
    Root bounds:
    
    For a monic polynomial
    x^n + a[n-1]*x^{n-1} + ... + a[1]*x + a[0]
    the Cauchy bound is
    M = 1 + max{|a[0]|,...,|a[n-1]|}.
    All real-value roots must lie in the interval [-M,M].  

    For a non-monic polynomial:
    b[n]*x^n + b[n-1]*x^{n-1} + ... + b[1]*x + b[0],

    if b[n] is not zero, divide through by it and calculate Cauchy's
    bound:

  */


class PolynomialRoots
{
 private:

  const double epsilon;          ///< Tolerance
  const int iMaxIterations;      ///< Max number of iterations
  int iCount;                    ///< Total count

  std::vector<double> Root;      ///< Roots

  void BalanceCompanion3 (Geometry::Matrix<double>&) const;
  bool isBalancedCompanion3(const double,const double,const double,
			    const double,const double) const;
  bool QRIteration3(Geometry::Matrix<double>&);
  double getColNorm(const int,const Geometry::Matrix<double>&) const;
  double getRowNorm(const int,const Geometry::Matrix<double>&) const;
  void scaleRow(const int,const double, Geometry::Matrix<double>&) const;
  void scaleCol(const int,const double, Geometry::Matrix<double>&) const;
  bool isBalanced3(const Geometry::Matrix<double>&) const;
  void Balance3(Geometry::Matrix<double>&) const;
  void FrancisQRStep (Geometry::Matrix<double>&,std::vector<double>&);
  

  /*  // support for FindB
  bool Bisection (const PolyBase&, double fXMin, double fXMax,
		  int iDigitsAccuracy, double& rfRoot);

    // support for FindE
  void GetHouseholderVector (int iSize, const Vector3<double>& rkU,
			     Vector3<double>& rkV);

  void PremultiplyHouseholder (GMatrix<double>& rkMat, GVector<double>& rkW,
			       int iRMin, int iRMax, int iCMin, int iCMax, int iVSize,
			       const Vector3<double>& rkV);

  void PostmultiplyHouseholder (GMatrix<double>& rkMat, GVector<double>& rkW,
				int iRMin, int iRMax, int iCMin, int iCMax, int iVSize,
				const Vector3<double>& rkV);

  

  
  void BalanceCompanion4 (GMatrix<double>& rkMat);
  bool IsBalancedCompanion4 (double fA10, double fA21, double fA32, double fA03,
			     double fA13, double fA23, double fA33);
  bool QRIteration4 (GMatrix<double>& rkMat);
  
  // support for testing if all roots have negative real parts
  bool AlldoublePartsNegative (int iDegree, double* afCoeff);
  */

 public:

  PolynomialRoots(const double);
  ~PolynomialRoots();
  
  /// member access
  int getCount () const { return iCount; }
  const std::vector<double>& getRoots() const { return Root; }  ///< Access roots

  // linear equations:  c1*x+c0 = 0
  int findA (const double,const double);
  double getBound(const double,const double) const;

  // quadratic equations: c2*x^2+c1*x+c0 = 0
  int  findA (const double,const double,const double);
  double getBound(const double,const double,const double);

  // cubic equations: c3*x^3+c2*x^2+c1*x+c0 = 0
  int findA (double,double,double,double);
  int findE (double,double,double,double,bool);
  double getBound(const double,const double,const double,const double);

  // Solve A*r^3 + B*r = C where A > 0 and B > 0.  This equation always has
  // exactly one root.
  double SpecialCubic (const double,const double,const double) const;

  // quartic equations: c4*x^4+c3*x^3+c2*x^2+c1*x+c0 = 0
  int findA (double,double,double,double,double);
  int findE (double,double,double,double,double,bool);
  double getBound(const double,const double,const double,const double,const double);

  // general equations: sum_{i=0}^{degree} c(i)*x^i = 0
  bool findB (const PolyBase&,const int iDigits =8);
  //  bool FindN (const PolyBase&,const int iDigits =8);
  bool findE (const PolyBase&,bool);
  double getBound(const PolyBase&);
  // find roots on specified intervals
  //  bool FindB (const PolyBase&, double fXMin, double fXMax,
  //	      int iDigits);
  
  //  bool FindN (const PolyBase&, double fXMin, double fXMax,
  //	      int iDigits =8);
  
  bool AlldoublePartsNegative (const PolyBase&);
  bool AlldoublePartsPositive (const PolyBase&);
  
  // Count the number of roots on [t0,t1].  Uses Sturm sequences to do the
  // counting.  It is allowed to pass in t0 = -Math<double>::MAX_REAL or
  // t1 = Math<double>::MAX_REAL.  The value of m_fEpsilon is used as a
  // threshold on the value of a Sturm polynomial at an end point.  If
  // smaller, that value is assumed to be zero.  The return value is the
  // number of roots.  If there are infinitely many, -1 is returned.
  int GetRootCount (const PolyBase&, double fT0, double fT1);

};

}  // NAMESPACE 

#endif
