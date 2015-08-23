/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chipInc/BeamStop.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef hutchSystem_BeamStop_h
#define hutchSystem_BeamStop_h

class Simulation;

namespace hutchSystem
{
  class ChipIRHutch;
  
/*!
  \class BeamStop
  \version 1.0
  \author S. Ansell
  \date May 2010
  \brief BeamStop [insert object]
*/

class BeamStop : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
  
  const int stopIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double xStep;             ///< Offset on XAxis of centre
  double yStep;             ///< origin forward step
  double zStep;             ///< Offset on ZAxis of centre
  
  double innerWidth;        ///< Inner width 
  double innerHeight;       ///< Inner height
  double innerLength;        ///< Inner Depth

  double steelWidth;        ///< Steel Width
  double steelHeight;       ///< SteelHeight
  double steelLength;        ///< Steel length [beam]
  double steelDepth;        ///< Steel Detpth

  double voidWidth;        ///< Void Width
  double voidHeight;       ///< VoidHeight
  double voidDepth;       ///< VoidHeight
  double voidLength;        ///< Steel Detpth

  double concWidth;        ///< Conc Width
  double concHeight;       ///< ConcHeight
  double concLength;        ///< concrete length
  double concDepth;         ///< Concreate depth

  int defInnerMat;         ///< Default inner material
  int steelMat;            ///< Steel material
  int concMat;             ///< concrete material

  std::vector<beamBlock> BBlock; ///< Data for beamstop layers

  int nLayers;                    ///< Number of layers
  std::vector<int> CDivideList;   ///< Cell divide List for 

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  
  void createSurfaces();
  void createDefSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  void layerProcess(Simulation&);

 public:

  BeamStop(const std::string&);
  BeamStop(const BeamStop&);
  BeamStop& operator=(const BeamStop&);
  ~BeamStop();

  int exitWindow(const double,std::vector<int>&,
		 Geometry::Vec3D&) const;

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
    
};


}  

#endif
 
