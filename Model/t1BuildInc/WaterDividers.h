/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1BuildInc/WaterDividers.h
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
#ifndef moderatorSystem_WaterDividers_h
#define moderatorSystem_WaterDividers_h

class Simulation;

namespace ts1System
{

/*!
  \class WaterDividers
  \version 1.0
  \author S. Ansell
  \date November 2011
  \brief W/Ta plates 
*/

class WaterDividers : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int wIndex;             ///< Index of surface offset
  int cellIndex;                ///< Cell index
  
  double conHeight;             ///< Height of the connect

  size_t fblkConnect;           ///< Connect index
  double fblkSize;              ///< Connection block size
  double fblkSndStep;           ///< Distance back of join block [mid:mid]
  double fblkSndOut;            ///< Distance Out for the snd blcok
  double fblkSndWidth;          ///< Width (X) of join block
  double fblkSndLen;            ///< Length (Y) of snd block
  double fblkWallThick;         ///< wall thickness of divider
  double fblkEndLen;            ///< Connection block length
  double fblkEndThick;          ///< Connection block size

  size_t mblkConnect;           ///< Connect index [mid layer]
  double mblkSize;              ///< Connection block size
  double mblkOutRad;            ///< Outer radius on first block
  double mblkInRad;             ///< Inner radius on first block
  double mblkExtLen;            ///< Distance back of join block [mid:mid]
  double mblkExtWidth;          ///< Width of join block
  double mblkWallThick;         ///< wall thickness of divider
  double mblkEndLen;            ///< Connection block length
  double mblkEndThick;          ///< Connection block size

  size_t bblkConnect;           ///< Connect index [back layer]
  double bblkSize;              ///< Connection block size
  double bblkOutRad;            ///< Outer radius on first block
  double bblkInRad;             ///< Inner radius on first block
  double bblkExtLen;            ///< Distance back of join block [mid:mid]
  double bblkExtWidth;          ///< Width of join block
  double bblkWallThick;         ///< wall thickness of divider
  double bblkEndLen;            ///< Connection block length
  double bblkEndThick;          ///< Connection block size

  double insOutThick;
  double insInThick;
  double insWidth;
  double insHeight;
  double insRad;

  int dMat;                     ///< divide material
  int insMat;                   ///<  Water [D2O]

  double cornerThick;
  double cornerWidth;
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);
  void createSurfaces(const PlateTarget&,
		      const attachSystem::FixedComp&);
  void createLinks();
  void createObjects(Simulation&);

 public:

  WaterDividers(const std::string&);
  WaterDividers(const WaterDividers&);
  WaterDividers& operator=(const WaterDividers&);
  ~WaterDividers();
  
  void createAll(Simulation&,const PlateTarget&,
		 const attachSystem::FixedComp&);

};

}

#endif
 
