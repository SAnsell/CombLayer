/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/CrossPipe.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef constructSystem_CrossPipe_h
#define constructSystem_CrossPipe_h

class Simulation;

namespace constructSystem
{
  
/*!
  \class CrossPipe
  \version 1.0
  \author S. Ansell
  \date January 2018
  \brief CrossPipe unit  
*/

class CrossPipe :
  public attachSystem::FixedOffset,
  public attachSystem::ContainedComp,
  public attachSystem::CellMap,
  public attachSystem::SurfMap,
  public attachSystem::FrontBackCut
{
 private:
  
  const int vacIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index  

  double horrRadius;            ///< horrizontal radius [inner]
  double vertRadius;            ///< vertical radius [inner]
  double height;                ///< void height
  double depth;                 ///< void depth
  double frontLength;           ///< void length [from centre]
  double backLength;            ///< void length [from centre]

  double feThick;               ///< pipe thickness
  double topPlate;              ///< Top plate thickness
  double basePlate;             ///< base plate thickness

  double flangeRadius;          ///< Joining Flange radius [-ve for rect]
  double flangeLength;          ///< Joining Flange length
    
  int voidMat;                  ///< Void material
  int feMat;                    ///< Pipe material
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  CrossPipe(const std::string&);
  CrossPipe(const CrossPipe&);
  CrossPipe& operator=(const CrossPipe&);
  virtual ~CrossPipe();

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
