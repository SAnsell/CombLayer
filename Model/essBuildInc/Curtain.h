/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/Curtain.h
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
#ifndef essSystem_Curtain_h
#define essSystem_Curtain_h

class Simulation;

namespace essSystem
{

/*!
  \class Curtain
  \version 1.0
  \author S. Ansell
  \date April 2015
  \brief Bulk around Reflector
*/

class Curtain : public attachSystem::ContainedGroup,
  public attachSystem::FixedGroup,
  public attachSystem::CellMap,
  public attachSystem::SurfMap
{
 private:
   
  const int curIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index  
  

  double wallRadius;             ///< inner radius [calculated]
  
  double leftPhase;              ///< Sector phase left
  double rightPhase;             ///< Sector phase right
    
  double innerStep;              ///< inner radius [calculated]
  double wallThick;              ///< Wall radius
  double baseGap;                ///< Base gap
  double outerGap;               ///< Gap outer of wall
  double topRaise;               ///< Raise of top step
  double depth;                  ///< Floor depth
  double height;                 ///< Roof height

  size_t nTopLayers;             ///< Number of top layers
  size_t nMidLayers;             ///< Number of top layers
  size_t nBaseLayers;             ///< Number of top layers
  std::vector<double> topFrac;    ///< Layer divider
  std::vector<double> midFrac;    ///< Layer divider
  std::vector<double> baseFrac;   ///< Layer divider

  std::vector<int> topMat;    ///< Top layer materials
  std::vector<int> midMat;    ///< Mid layer materials
  std::vector<int> baseMat;   ///< Low layer materials
  
  int wallMat;                   ///< wall material  
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int,const long int);


  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&,const attachSystem::FixedComp&,
		     const long int,const long int);

  void layerProcess(Simulation&,const attachSystem::FixedComp&,
		    const long int);

  
 public:

  Curtain(const std::string&);
  Curtain(const Curtain&);
  Curtain& operator=(const Curtain&);
  virtual ~Curtain();

  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,const long int);


};

}

#endif
 

