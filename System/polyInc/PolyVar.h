/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   polyInc/PolyVar.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef mathLevel_PolyVar_h
#define mathLevel_PolyVar_h

namespace mathLevel
{

  /*!
    \class PolyVar
    \version 1.0
    \author S. Ansell 
    \date December 2007
    \brief Holds a polynominal as a secondary type

    This holds a multi-variable polynomial of primary positive type.
    Assumes Euclidean division, hence a remainders.
    Internal solution of the polynomial is possible. Conversion to 
    other forms is not internally handled.

    Note that Index refers to the depth. It is only valid from 1 to N.
    PolyVar<1> has been specialised
  */

template<size_t VCount>
class PolyVar  : public PolyFunction
{
 private:

  size_t iDegree;                                ///< Degree of polynomial  
  std::vector<PolyVar<VCount-1> > PCoeff;     ///< Polynominals [0]=const 
  
 public:

  explicit PolyVar(const size_t =0);
  PolyVar(const size_t,const double);
  PolyVar(const PolyVar<VCount>&);                       
  template<size_t ICount> PolyVar(const PolyVar<ICount>&);
  PolyVar<VCount>& operator=(const PolyVar<VCount>&);
  template<size_t ICount> PolyVar<VCount>& operator=(const PolyVar<ICount>&);
  PolyVar<VCount>& operator=(const double&);
  virtual ~PolyVar();

    // member access
  void setDegree(const size_t);
  size_t getDegree() const;
  void zeroPoly();

  template<size_t ICount>
  void setComp(const size_t,const PolyVar<ICount>&);
  void setComp(const size_t,const double&);
  // get value
  double operator()(const std::vector<double>&) const;
  double operator()(const double*) const;
  double operator()(const double,const double) const;
  double operator()(const double,const double,const double) const;


  // ordering
  bool operator<(const PolyVar<VCount>&) const;
  // arithmetic updates
  PolyVar<VCount>& operator+=(const PolyVar<VCount>&);
  PolyVar<VCount>& operator-=(const PolyVar<VCount>&);
  PolyVar<VCount>& operator*=(const PolyVar<VCount>&);
 

  // arithmetic operation
  PolyVar<VCount> operator+(const PolyVar<VCount>&) const;
  PolyVar<VCount> operator-(const PolyVar<VCount>&) const;
  PolyVar<VCount> operator*(const PolyVar<VCount>&) const;

  PolyVar<VCount> operator+(const double) const;  // input is degree 0 poly
  PolyVar<VCount> operator-(const double) const;  // input is degree 0 poly
  PolyVar<VCount> operator*(const double) const;
  PolyVar<VCount> operator/(const double) const;
  PolyVar<VCount> operator-() const;


 // input is degree 0 poly
  PolyVar<VCount>& operator+=(const double); 
  PolyVar<VCount>& operator-=(const double); 
  PolyVar<VCount>& operator*=(const double);
  PolyVar<VCount>& operator/=(const double);

  int operator==(const PolyVar<VCount>&) const;
  int operator!=(const PolyVar<VCount>&) const;

    // derivation
  PolyVar getDerivative() const;
  PolyVar& derivative();

  // inversion ( invpoly[i] = poly[degree-i] for 0 <= i <= degree )
  PolyVar GetInversion() const;
  void compress(const double = -1.0);

  int isZero(const double) const;
  int isUnit(const double) const;
  int isUnitary(const double) const;
  size_t getCount(const double) const;
  size_t getVarFlag() const;
  size_t getVarCount() const;
  int isComplete() const;
  
  /// is a single double value (false)
  int isDouble() const { return 0; } 
  /// Get a double valued coefficient:
  double getDouble() const { return PCoeff[0].getDouble(); }  
  std::pair<PolyVar<VCount-1>,unsigned int> 
    eliminateVar(const PolyVar<VCount>&) const;
  PolyVar<VCount-1> reduce(const PolyVar<VCount>&) const;
  PolyVar<VCount-1> subVariable(const size_t,const double&) const;
  PolyVar<1> singleVar() const;
  int expand(const PolyVar<VCount>&);

  double substitute(const double&,const double&) const;
  double substitute(const double&,const double&,const double&) const;
  PolyVar<VCount-1> substitute(const double&) const;
  template<size_t ICount> PolyVar<ICount> substitute(const double&,const double&) const;

  //  PolyVar<1> substitute(const std::vector<double>&) const;
  PolyVar<VCount>& makeTriplet(const std::vector<double>&);
  virtual int read(const std::string&);
  virtual int write(std::ostream&,const int=0) const;
};

template<size_t VCount> 
std::ostream& operator<<(std::ostream&,const PolyVar<VCount>&);

/*!
  \class PolyVar< 1 >
  \version 1.0
  \author S. Ansell
  \date December 2007
  \brief Specialization of PolyVar(Var)
*/

template<> 
class PolyVar<1> : public PolyFunction
{
 private:

  size_t iDegree;                    ///< Degree of polynomial [0 == constant]
  std::vector<double> PCoeff;     ///< Coefficients [low->high]

  int checkSmallPoly(std::vector<std::complex<double> >&) const;

 public:

  explicit PolyVar<1>(const size_t =0);
  PolyVar<1>(const size_t,const double);
  PolyVar<1>(const std::vector<double>&);
  //  PolyVar<1>(const double);
  PolyVar<1>(const size_t,const double,const double);
  PolyVar<1>(const size_t,const double,const double,const double);
  PolyVar<1>(const PolyVar<1>&);
  PolyVar<1>& operator=(const PolyVar<1>&);
  PolyVar<1>& operator=(const double&);
  virtual ~PolyVar<1>();

    // member access
  void setDegree(const size_t);
  size_t getDegree() const;
  void setComp(const size_t,const double&);
  void zeroPoly();

  operator const std::vector<double>& () const;
  operator std::vector<double>& ();
  double operator[](const size_t) const;
  double& operator[](const size_t);

  // evaluation
  double operator()(const double) const;
  double operator()(const std::vector<double>&) const;
  double operator()(const double*) const;

  std::complex<double> evalPoly(const std::complex<double>&) const;
  
  // ordering
  bool operator<(const PolyVar<1>&) const;
  // arithmetic updates
  PolyVar<1>& operator+=(const PolyVar<1>&);
  PolyVar<1>& operator-=(const PolyVar<1>&);
  PolyVar<1>& operator*=(const PolyVar<1>&);

  // arithmetic operations
  PolyVar<1> operator+(const PolyVar<1>&) const;
  PolyVar<1> operator-(const PolyVar<1>&) const;
  PolyVar<1> operator*(const PolyVar<1>&) const;

  PolyVar<1> operator+(const double) const;  // input is degree 0 poly
  PolyVar<1> operator-(const double) const;  // input is degree 0 poly
  PolyVar<1> operator*(const double) const;
  PolyVar<1> operator/(const double) const;
  PolyVar<1> operator-() const;

 // input is degree 0 poly
  PolyVar<1>& operator+=(const double); 
  PolyVar<1>& operator-=(const double); 
  PolyVar<1>& operator*=(const double);
  PolyVar<1>& operator/=(const double);

  int operator==(const PolyVar<1>&) const;
  int operator!=(const PolyVar<1>&) const;
  int operator==(const double&) const;
  int operator!=(const double&) const;

    // derivation
  PolyVar<1> getDerivative() const;
  PolyVar<1>& derivative();

  // inversion ( invpoly[i] = poly[degree-i] for 0 <= i <= degree )
  PolyVar<1> GetInversion() const;

  /// Access single value
  PolyVar<1> singleVar() const { return *this; }
  double substitute(const double&) const;
  double subVariable(const size_t,const double&) const;
  void compress(const double = -1.0); 
  void unitPrimary(const double = -1.0);
  void normalize();

  void divide(const PolyVar<1>&,PolyVar<1>&,PolyVar<1>&,
	      const double =-1.0) const;

  std::vector<double> realRoots(const double= -1.0);
  std::vector<std::complex<double> > calcDurandKernerRoots(const double= -1.0);
  std::vector<std::complex<double> > calcRoots(const double= -1.0);

  int isZero(const double) const;
  int isUnit(const double) const;
  int isUnitary(const double) const;
  size_t getCount(const double) const;
  size_t getVarFlag() const;
  size_t getVarCount() const;
  int isComplete() const;

  /// is a single value ???
  int isDouble() const { return 1; }
  /// Get a value of the last coefficient
  double getDouble() const { return PCoeff[0]; }

  int expand(const PolyVar<1>&);

  virtual int read(const std::string&);
  virtual int write(std::ostream&,const int =0) const;
};

}  // NAMESPACE mathlevel

#endif
