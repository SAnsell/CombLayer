/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/simpleItemInc/SimpleITEM.h
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
#ifndef essSystem_simpleITEM_h
#define essSystem_simpleITEM_h

namespace attachSystem
{
  class FixedComp;
  class FixedOffset;
  class TwinComp;
  class CellMap;
}

namespace insertSystem
{
  class insertPlate;
}

namespace essSystem
{  
  class GuideItem;

  /*!
    \class simpleITEM
    \version 1.0
    \author S. Ansell
    \date July 2014
    \brief simpleITEM beamline constructor for the ESS
  */
  
class simpleITEM : public attachSystem::CopiedComp
{
 private:

  /// Stop at [0:Complete / 1:Mono Wall / 2:Inner Bunker / 3:Outer Bunker ]
  int stopPoint;
  
  /// Main Beam Axis [for construction]
  std::shared_ptr<attachSystem::FixedOffset> simpleAxis;

  /// Simple plate
  std::shared_ptr<insertSystem::insertPlate> Plate;

  void setBeamAxis(const FuncDataBase&,const GuideItem&,const bool);
  
 public:
  
  simpleITEM(const std::string&);
  simpleITEM(const simpleITEM&);
  simpleITEM& operator=(const simpleITEM&);
  ~simpleITEM();
  
  void build(Simulation&,const GuideItem&,
	     const Bunker&,const int);

};

}

#endif
