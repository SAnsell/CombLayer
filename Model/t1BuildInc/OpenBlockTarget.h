/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1BuildInc/OpenBlockTarget.h
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
#ifndef ts1System_OpenBlockTarget_h
#define ts1System_OpenBlockTarget_h

class Simulation;

namespace ts1System
{

/*!
  \class OpenBlockTarget
  \version 1.0
  \author S. Ansell
  \date November 2011
  \brief W/Ta plates 
*/

class OpenBlockTarget : public constructSystem::TargetBase
{
 private:
  
  const int ptIndex;            ///< Index of surface offset
  int cellIndex;                ///< Cell index

  int frontPlate;               ///< Front Plate
  int backPlate;                ///< Back Plate
  
  double height;                ///< Height inner pressure tank
  double width;                 ///< Width inner presure tank  

  size_t nBlock;                ///< nBlock
  std::vector<double> tBlock;   ///< Vector of block thickness
  std::vector<double> tVoid;    ///< Vector of block inner void thickness

  double WWidth;                ///< W Width 
  double WHeight;               ///< W Height 
  double WVoidHeight;           ///< W inner void Height 
  double WVoidWidth;            ///< W inner void Width 

  double taThick;               ///< Ta clad thickness
  double waterThick;            ///< water gap thickness

  double pressThick;            ///< Pressure thickness

  int taMat;                    ///< Ta material
  int wMat;                     ///< W material [def]
  int waterMat;                 ///< Water material
  int pressMat;                 ///< Pressure mat

  void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&);

 public:

  OpenBlockTarget(const std::string&);
  OpenBlockTarget(const OpenBlockTarget&);
  OpenBlockTarget& operator=(const OpenBlockTarget&);
  virtual OpenBlockTarget* clone() const;
  virtual ~OpenBlockTarget();

  Geometry::Vec3D plateEdge(const size_t,double&,double&) const;
  void populate(const FuncDataBase&);  
  double getTargetLength() const;
  void createAll(Simulation&,const attachSystem::FixedComp&);

  virtual void addProtonLine(Simulation&,const attachSystem::FixedComp&,
			     const long int); 
  
};

}

#endif
 
