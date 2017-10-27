/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sourceInc/SourceBase.h
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
#ifndef SDef_SourceBase_h
#define SDef_SourceBase_h

namespace SDef
{
  class Source;
}

namespace SDef
{

/*!
  \class SourceBase
  \version 1.0
  \author S. Ansell
  \date November 2014
  \brief Adds gamma ray circular divergent source
*/

class SourceBase 
{
 protected:

  int particleType;             ///< Particle Type
  double cutEnergy;             ///< Energy cut point
  
  std::vector<double> Energy;   ///< Energies [MeV]
  std::vector<double> EWeight;  ///< Weights

  double weight;                ///< Start particle weight
  
  int populateEnergy(std::string,std::string);
  int populateEFile(const std::string&,const int,const int);

 public:

  SourceBase();
  SourceBase(const SourceBase&);
  SourceBase& operator=(const SourceBase&);
  virtual SourceBase* clone() const =0;
  virtual ~SourceBase() {}        ///< Destructor

  void populate(const std::string&,const FuncDataBase&);

  /// Set particle type
  void setParticle(const int T) { particleType=T; }
  /// Set cut energy
  void setCutEnergy(const double E) { cutEnergy=E; }
  void setEnergy(const double);
  void createEnergySource(SDef::Source&) const;

  virtual void createSource(SDef::Source&) const =0;
  virtual void writePHITS(std::ostream&) const =0;
  virtual void write(std::ostream&) const =0;
};

}

#endif
 
