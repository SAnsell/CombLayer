/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/PipeCollimator.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef constructSystem_RingFlange_h
#define constructSystem_RingFlange_h

class Simulation;

namespace constructSystem
{
  /*!
    \class RingFlange
    \version 1.0
    \author S. Ansell
    \date June 2015
    \brief Variable detemine hole type
  */
  
class RingFlange : public RingSeal
{
 private:


  size_t nBolts;           ///< Number of bolts
  double rotAngleOffset;   ///< Rotation angle offset
  double boltRadius;       ///< Radius of bolt
  double boltCentDist;     ///< Distance of bolt to centre point
  int boltMat;             ///< bolt material

  int windowFlag;          ///< Window flag
  double windowThick;      ///< thickness
  double windowStep;       ///< Step from centre
  int windowMat;           ///< Material for window
  
  void insertBolt(Simulation&,const double,const double,
		  const std::string&) const;
  
  void addWindow(Simulation&);
  void addBolts(Simulation&);
  
 public:
  
  RingFlange(const std::string&);
  RingFlange(const RingFlange&);
  RingFlange& operator=(const RingFlange&);
  virtual ~RingFlange() {}  ///< Destructor

  void populate(const FuncDataBase&);

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
