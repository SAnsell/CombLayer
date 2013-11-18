/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   chipInc/ChipSample.h
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
#ifndef hutchSystem_ChipSample_h
#define hutchSystem_ChipSample_h

class Simulation;

namespace hutchSystem
{
/*!
  \class ChipSample
  \version 1.0
  \author S. Ansell
  \date January 2011
  \brief Plate inserted in object 
*/

class ChipSample : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
 
  const int ID;                  ///< ID index 
  const int csIndex;             ///< Index of surface offset
  int cellIndex;                 ///< Cell index
  int populated;                 ///< 1:var

  int tableNum;             ///< Table number
  double zAngle;            ///< Z angle rotation
  double xyAngle;           ///< XY angle rotation
  double XStep;             ///< Step across beamline
  double YStep;             ///< Step down beamline
  double ZLift;             ///< Step up

  double width;             ///< Full Width
  double height;            ///< Full Height
  double length;            ///< Full length

  int defMat;               ///< Material
  
  void populate(const Simulation&);
  void createUnitVector(const FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&);

 public:

  ChipSample(const std::string&,const int);
  ChipSample(const ChipSample&);
  ChipSample& operator=(const ChipSample&);
  ~ChipSample();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const attachSystem::FixedComp&);


};

}

#endif
 
