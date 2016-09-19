/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sourceInc/SurfNormSource.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef SDef_SurfNormSource_h
#define SDef_SurfNormSource_h

namespace SDef
{
  class Source;
}

namespace SDef
{

/*!
  \class SurfNormSource
  \version 1.0
  \author S. Ansell
  \date September 2015
  \brief Creat a source on the +/- of a surface
*/

class SurfNormSource :
  public attachSystem::FixedComp
{
 private:
    
  int particleType;             ///< Particle Type
  double angleSpread;           ///< Angle from normal
  int surfNum;                  ///< Surfacte number
  double cutEnergy;             ///< Start energy
  double height;                ///< Height of source
  
  double weight;
  std::vector<double> Energy;   ///< Energies [MeV]
  std::vector<double> EWeight;  ///< Weights
  
  void populate(const FuncDataBase& Control);
  int populateEnergy(std::string,std::string);

  void setSurf(const attachSystem::FixedComp&,
			const long int);
  void createSource(SDef::Source&) const;

 public:

  SurfNormSource(const std::string&);
  SurfNormSource(const SurfNormSource&);
  SurfNormSource& operator=(const SurfNormSource&);
  ~SurfNormSource();

  /// Set cut energy
  void setCutEnergy(const double E) { cutEnergy=E; }
  void loadEnergy(const std::string&);
  
  void createAll(const FuncDataBase&,const attachSystem::FixedComp&,
		 const long int,SDef::Source&);
  
};

}

#endif
 
