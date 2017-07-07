/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderatorInc/Bucket.h
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
#ifndef moderatorSystem_Bucket_h
#define moderatorSystem_Bucket_h

class Simulation;

namespace moderatorSystem
{

/*!
  \class Bucket
  \version 1.0
  \author S. Ansell
  \date August 2011
  \brief Bucket [insert object]
*/

class Bucket : 
  public attachSystem::ContainedComp,
    public attachSystem::FixedOffset
{
 private:
  
  const int cdIndex;           ///< Index of surface offset
  int cellIndex;               ///< Cell index
 
  double radius;               ///< radius of hole
  double thickness;            ///< Thickness
  double openZ;                ///< Open Z height
  double topZ;                 ///< Top height
  
  int matN;                    ///< Material Number [Cd]
  
  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);

 public:

  Bucket(const std::string&);
  Bucket(const Bucket&);
  Bucket& operator=(const Bucket&);
  ~Bucket();


  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);

};

}

#endif
 
