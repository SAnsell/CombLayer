/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/BeamPair.h
*
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef xraySystem_BeamPair_h
#define xraySystem_BeamPair_h

class Simulation;

namespace xraySystem
{

/*!
  \class BeamPair
  \author S. Ansell
  \version 1.0
  \date January 2018
  \brief Focasable mirror in mount
*/

class BeamPair :
  public attachSystem::ContainedGroup,
  public attachSystem::FixedOffsetGroup,
  public attachSystem::ExternalCut,
  public attachSystem::CellMap
{
 private:
  
  bool upFlag;             ///< Up/down

  double outLiftA;          ///< lift [when raised from beam cent]
  double outLiftB;          ///< lift [when raised from beam cent]
  double gapA;             ///< Gap from centre point (top +ve up)
  double gapB;             ///< Gap from centre point (base +ve down)
  
  double supportRadius;    ///< Radius of support  

  double xStepA;           ///< xstep of unit A
  double yStepA;           ///< yStep of unit A
  double xStepB;           ///< xStep of unit B
  double yStepB;           ///< yStep of unit B
  
  double blockXYAngle;     ///< Rotation angle about Z
  double height;           ///< height total 
  double width;            ///< width accross beam
  double length;           ///< Thickness in normal direction to reflection  


  int blockMat;            ///< block material    
  int supportMat;          ///< support material

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int,
			const attachSystem::FixedComp&,const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  std::vector<Geometry::Vec3D> calcEdgePoints() const;

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,const long int) {}
    
 public:

  BeamPair(const std::string&);
  BeamPair(const BeamPair&);
  BeamPair& operator=(const BeamPair&);
  virtual ~BeamPair();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,const long int,
		 const attachSystem::FixedComp&,const long int);
  
};

}

#endif
 
