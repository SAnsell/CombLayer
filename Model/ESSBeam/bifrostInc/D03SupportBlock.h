/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/LineShield.h
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
#ifndef constructSystem_D03SupportBlock_h
#define constructSystem_D03SupportBlock_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class D03SupportBlock
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief D03SupportBlock unit  
*/

class D03SupportBlock :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap
{
 private:
  
  //  const int shieldIndex;        ///< Index of surface offset
  // int cellIndex;                ///< Cell index  

  bool activeFront;             ///< Flag for front active
  bool activeBack;              ///< Flag for back active

  HeadRule frontSurf;           ///< Front surfaces [if used]
  HeadRule frontCut;            ///< front divider surfaces [if used]
  HeadRule backSurf;            ///< Back surfaces [if used]
  HeadRule backCut;             ///< Back surfaces [if used]
  
  double l1edge,l1mid,gap,l2edge,l2mid;  ///< top level length before and after gap
  double length1low, length2low;   //length of voids
  double wid1F,wid1B, wid2, wid3;  // widths of geometry
  double hup, hlow; // heights of concrete blocks

  double RTube, TubeThick, TubeOffsetL, TubeOffsetV, RBend; // 
  int iBuildTube;
  
  int defMat;                    ///< Fe material layer
  int defMatF;                    ///< Fe material layer
  int defMatB;                    ///< Fe material layer
  //int voidMat;                    ///< Material in the void
  int gapMat;                    ///< material in the gap between blocks
  int iSideShield;
  
  int tubeMat;
    
  size_t nSeg;                   ///< number of segments
  // Layers
  size_t nLayers;               ///< number of layers left wall



  void removeFrontOverLap();
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  D03SupportBlock(const std::string&);
  D03SupportBlock(const D03SupportBlock&);
  D03SupportBlock& operator=(const D03SupportBlock&);
  virtual ~D03SupportBlock();


  HeadRule getXSectionIn() const;
  void setFront(const attachSystem::FixedComp&,const long int);
  void setBack(const attachSystem::FixedComp&,const long int);
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
