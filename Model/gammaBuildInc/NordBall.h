/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   gammaBuildInc/NordBall.h
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
#ifndef gammaSystem_NordBall_h
#define gammaSystem_NordBall_h

class Simulation;

namespace gammaSystem
{

/*!
  \class NordBall
  \author S. Ansell
  \version 1.0
  \date November 2014
  \brief Specialized NordBall scintalator detector
*/

class NordBall : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset
{
 private:

  const size_t detNumber;         ///< Detector number
  const std::string baseName;     ///< Base name
  const int detIndex;             ///< Index
  int cellIndex;                  ///< Cell index

  size_t nFace;                   ///< Number of layers
  double faceWidth;               ///< Face short distance
  double backWidth;               ///< Back short distance
  double frontLength;             ///< Total length
  double backLength;              ///< Back section
  double wallThick;               ///< Wall Thickness

  double plateThick;              ///< Plate Thickness
  double plateRadius;             ///< Plate Radius
  double supportThick;            ///< Support if present 
  double elecRadius;              ///< Radius inside plate
  double elecThick;               ///< Finale electronics

  int mat;                        ///< Material
  int wallMat;                    ///< Wall material [detector surf]
  int plateMat;                   ///< Support Material
  int supportMat;                 ///< Support Material
  int elecMat;                    ///< Electronics Material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  NordBall(const size_t,const std::string&);
  NordBall(const NordBall&);
  NordBall& operator=(const NordBall&);
  virtual ~NordBall();
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
