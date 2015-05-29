/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   scatMatInc/SQWmaterial.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef scatterSystem_SQWmaterial_h
#define scatterSystem_SQWmaterial_h

namespace scatterSystem
{
  
  class neutron;

  /*!
    \class SQWmaterial
    \brief Neutronic information on the material [ crystalline]
    \author S. Ansell
    \version 1.0
    \date May 2010
  */
  
class SQWmaterial : public neutMaterial
{
 private:

  double eFrac;                      ///< Fraction of eMaterial
  ENDF::ENDFmaterial HMat;           ///< Primary S(Q,w)
  neutMaterial* Extra;               ///< Extra material

 public:
  
  SQWmaterial();
  SQWmaterial(const std::string&,const double,const double,
	      const double,const double,const double,const double);
  SQWmaterial(const double,const double,const double,
	      const double,const double,const double);
  SQWmaterial(const SQWmaterial&);
  virtual SQWmaterial* clone() const;
  SQWmaterial& operator=(const SQWmaterial&);
  virtual ~SQWmaterial();
  
  /// Effective typeid
  virtual std::string className() const { return "SQWmaterial"; }
  /// Visitor acceptance
  virtual void acceptVisitor(Global::BaseVisit& A) const
    { A.Accept(*this); }
  /// Accept visitor for input
  virtual void acceptVisitor(Global::BaseModVisit& A)
    {  A.Accept(*this); }

  void setENDF7(const std::string&);
  void setExtra(const std::string&,const double frac,
		const double M,const double B,
		const double S,const double I,const double A);


  /// Access material
  ENDF::ENDFmaterial& getENDF() { return HMat; }
  /// Const access of material
  const ENDF::ENDFmaterial& getENDF() const { return HMat; } 
  
  virtual double ScatTotalRatio(const double) const;
  virtual double ScatCross(const double) const;
  virtual double TotalCross(const double) const;

  virtual double calcAtten(const double,const double) const;
  virtual void scatterNeutron(MonteCarlo::neutron&) const;

  virtual double dSdOdE(const MonteCarlo::neutron&,
			const MonteCarlo::neutron&) const;


};


} // NAMESPACE scatterSystem

#endif
