/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/anglePortItem.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef constructSystem_anglePortItem_h
#define constructSystem_anglePortItem_h

class Simulation;

namespace constructSystem
{

  /*!
    \class anglePortItem
    \brief Port with an intermediate bend
    \author S. Ansell
    \date January 2018
    \version 1.0

    This is a port with a bend in the main length
    effectively two pieces of pipe without a flange.
    It is good for 90 deg bends etc.
  */

class anglePortItem :
  public portItem
{

protected:

  Geometry::Vec3D bAxis;     ///< Secondary direction
  double lengthA;            ///< length to centre of turn
  double lengthB;            ///< length to cap from centre of turn
 
  void createSurfaces() override;
  virtual void createLinks();
  
 public:

  anglePortItem(const std::string&);
  anglePortItem(const std::string&,const std::string&);
  anglePortItem(const anglePortItem&);
  anglePortItem& operator=(const anglePortItem&);
  ~anglePortItem() override;

  // make public as accessor function:
  void populate(const FuncDataBase&) override;
      
  void constructObject(Simulation&,
		      const HeadRule&,const HeadRule&) override;
  
  virtual void addFlangeCut(MonteCarlo::Object*) const;
  
  using FixedComp::createAll;
  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int) override;
				       
};
  

}

#endif
 
