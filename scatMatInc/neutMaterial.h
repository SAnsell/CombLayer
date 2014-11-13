/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   scatMatInc/neutMaterial.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef scatterSystem_neutMaterial_h
#define scatterSystem_neutMaterial_h

namespace MonteCarlo
{
  class neutron;
}

namespace scatterSystem
{
  /*!
    \class neutMaterial
    \brief Neutronic information on the material
    \author S. Ansell
    \version 1.0
    \date July 2007
    
    This can be extended so that more sophisticated material
    components can be used.
    \todo This class needs to have a base class.
  */
  
class neutMaterial
{
 protected:
  
  std::string Name;      ///< Name 
  int mcnpxNum;          ///< MCNPXnumber

  double Amass;          ///< Mean Atomic Mass
  double density;        ///< number density [atom/A^3]
  double realTemp;       ///< Temperature [K]
  double bcoh;           ///< b [fm]
  double scoh;           ///< scattering cross section 
  double sinc;           ///< incoherrrent cross section 
  double sabs;           ///< Absorption cross section
  double bTotal;         ///< Total scattering cross section
  
 public:
  
  neutMaterial();
  neutMaterial(const std::string&,const double,const double,
	   const double,const double,const double,const double);
  neutMaterial(const double,const double,const double,
	   const double,const double,const double);
  neutMaterial(const neutMaterial&);
  virtual neutMaterial* clone() const;
  neutMaterial& operator=(const neutMaterial&);
  virtual ~neutMaterial();
  
  /// Effective typeid
  virtual std::string className() const { return "neutMaterial"; }
  /// Visitor acceptance
  virtual void acceptVisitor(Global::BaseVisit& A) const
    { A.Accept(*this); }
  /// Accept visitor for input
  virtual void acceptVisitor(Global::BaseModVisit& A)
    {  A.Accept(*this); }

  /// Name accessor
  const std::string& getName() const { return Name; }  
  /// Number accessor
  int getNumber() const { return mcnpxNum; }  

  void setName(const std::string& N) { Name=N; }  ///< Set Name
  void setNumber(const int N) { mcnpxNum=N; }  ///< Set Number
  void setDensity(const double);
  virtual void setMass(const double M) { Amass=M; }  ///< Set Mass
  void setTmp(const double T) { realTemp=T; }  ///< Set Temperature [Kelvin]
  void setScat(const double,const double,const double);

  double getAtomDensity() const { return density; }   ///< Density accessor
  double getScat() const { return scoh+sinc; }  ///< Scattering cross-section
  double getBCoh() const { return bcoh; }       ///< Coherrent scattering length
  double getCoh() const { return scoh; }        ///< Coherrent x-sec
  double getInc() const { return sinc; }        ///< Incoherrent x-sec
  double getAbs() const { return sabs; }        ///< Absorption x-section
  double getScatFrac(const double) const;    

  // get Scattering prob
  virtual double ScatTotalRatio(const double) const;
  virtual double ScatCross(const double) const;
  virtual double ElasticTotalRatio(const double) const;
  virtual double TotalCross(const double) const;

  virtual double calcRefIndex(const double) const;
  virtual double calcAtten(const double,const double) const;
  
  virtual void scatterNeutron(MonteCarlo::neutron&) const;
  
  virtual double dSdOdE(const MonteCarlo::neutron&,
			const MonteCarlo::neutron&) const;

  virtual void write(std::ostream&) const;

};


} // NAMESPACE MonteCarlo

#endif
