/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/MicoRoof.h
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
#ifndef constructSystem_MicoRoof_h
#define constructSystem_MicoRoof_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class MicoRoof
  \version 1.0
  \author S. Ansell
  \date July 2015
  \brief MicoRoof unit  
*/

class MicoRoof :
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
  
  double l1edge,l1mid,gap,gapV, l2edge,l2mid;  ///< top level length before and after gap
  double length1low, length2low;   //length of voids
  double wid1f,wid1b, wid2, wid3f,wid3b;  // widths of geometry
  double hup, hlow; // heights of concrete blocks
  double InnerB4CThick;
  
  int defMatf, defMatb, B4CMat, B4CMatF, B4CMatB;                    ///< Fe material layer
  //int voidMat;                    ///< Material in the void
  int gapMat;                    ///< material in the gap between blocks

    
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

  MicoRoof(const std::string&);
  MicoRoof(const MicoRoof&);
  MicoRoof& operator=(const MicoRoof&);
  virtual ~MicoRoof();


  HeadRule getXSectionIn() const;
  void setFront(const attachSystem::FixedComp&,const long int);
  void setBack(const attachSystem::FixedComp&,const long int);
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
