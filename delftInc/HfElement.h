/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delftInc/HfElement.h
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
#ifndef delftSystem_HfElement_h
#define delftSystem_HfElement_h

class FuncDataBase;

namespace delftSystem
{

/*!
  \class HfElement
  \version 1.0
  \author S. Ansell
  \date June 2012
  \brief Modifies a fuel element to take a control rod
*/

class HfElement : public FuelElement,
    public attachSystem::ContainedGroup
{
 private:
  
  const std::string cntlKey;          ///< Control keyname
  const int controlIndex;             ///< Local control number 

  size_t cutSize;               /// Cut edge

  int absMat;                   ///< control material
  int bladeMat;                 ///< Blade material

  double absThick;              ///< Hf thickness
  double absWidth;              ///< Hf thickness
  double bladeThick;            ///< blade separate thickness
  double coolThick;             ///< coolant thickness

  double absHeight;             ///< Hf bar height
  double lift;                  ///< Lift on the hf blades.

  void populate(const FuncDataBase&);
  
  void createSurfaces(const attachSystem::FixedComp&);
  void createObjects(Simulation&);
  void createLinks();

 public:

  HfElement(const size_t,const size_t,
		 const std::string&,const std::string&);
  HfElement(const HfElement&);
  HfElement& operator=(const HfElement&);
  virtual ~HfElement() {}   ///< Destructor

  virtual void createAll(Simulation&,const FixedComp&,
			 const Geometry::Vec3D&,const FuelLoad&);

};

}  

#endif
