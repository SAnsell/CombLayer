/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bibBuildInc/GuideShield.h
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
#ifndef bibSystem_GuideShield_h
#define bibSystem_GuideShield_h

class Simulation;


namespace bibSystem
{

/*!
  \class GuideShield
  \version 1.0
  \author S. Ansell
  \date August 2013
  \brief Shied object round the guide
*/

class GuideShield : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:

  const std::string baseName;   ///< Base keyname
  const int shieldIndex;        ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double innerWidth;            ///< Inner width
  double innerHeight;           ///< Inner height
 
  size_t nLayers;                ///< Number of layers
  std::vector<double> Height;    ///< Height of layers  (total)
  std::vector<double> Width;     ///< Width of layer (total)
  std::vector<int> Mat;          ///< Materials for each layer

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);
  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&,const attachSystem::ContainedComp*,
		     const attachSystem::FixedComp&,const long int,
		     const attachSystem::FixedComp&,const long int);
  void calcInnerDimensions(const attachSystem::FixedComp&);

 public:

  GuideShield(const std::string&,const size_t);
  GuideShield(const GuideShield&);
  GuideShield& operator=(const GuideShield&);
  virtual ~GuideShield();

  std::vector<int> getCells() const;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const attachSystem::FixedComp&,const long int,
		 const attachSystem::FixedComp&,const long int);
  
};

}

#endif
 
