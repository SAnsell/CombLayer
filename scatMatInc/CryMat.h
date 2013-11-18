/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   scatMatInc/CryMat.h
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
#ifndef scatterSystem_CryMat_h
#define scatterSystem_CryMat_h

namespace scatterSystem
{
  
  class neutron;

  /*!
    \class CryMat
    \brief Neutronic information on the material [ crystalline]
    \author S. Ansell
    \version 1.0
    \date May 2010
    
    Based on 
  */
  
class CryMat : public neutMaterial
{
 private:

  double debyeTemp;        ///< Debye temperature

  double C1;               ///< C1 constant
  double C2;               ///< approx : 4.27exp(A/61)
  double Rsum;             ///< single phonon R value
  
  double B0plusBT;         ///< multiphonon constant
  
  Crystal::CifStore XStruct;        ///< Crystal structure

  double Rvalue(const double) const;
  double Bvalue(const double) const;
  double sigmaSph(const double) const;
  double sigmaMph(const double) const;

 public:
  
  CryMat();
  CryMat(const std::string&,const double,const double,
	   const double,const double,const double,const double);
  CryMat(const double,const double,const double,
	 const double,const double,const double);
  CryMat(const CryMat&);
  virtual CryMat* clone() const;
  CryMat& operator=(const CryMat&);
  virtual ~CryMat();
  
  /// Effective typeid
  virtual std::string className() const { return "CryMat"; }
  /// Visitor acceptance
  virtual void acceptVisitor(Global::BaseVisit& A) const
    { A.Accept(*this); }
  /// Accept visitor for input
  virtual void acceptVisitor(Global::BaseModVisit& A)
    {  A.Accept(*this); }

  void setTemperatures(const double,const double);
  void setMass(const double);
  void setCif(const std::string&);

  Crystal::CifStore& getCIF() { return XStruct; }
  const ::Crystal::CifStore& getCIF() const { return XStruct; }
  
  virtual double ScatTotalRatio(const double) const;
  virtual double ScatCross(const double) const;
  virtual double TotalCross(const double) const;

  virtual double calcAtten(const double,const double) const;

};


} // NAMESPACE scatterSystem

#endif
