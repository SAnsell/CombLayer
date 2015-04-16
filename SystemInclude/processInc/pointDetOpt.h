/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   processInc/pointDetOpt.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef pointDetOpt_h
#define pointDetOpt_h

namespace ModelSupport
{

/*!
  \class pointDetOpt
  \version 1.0
  \author S. Ansell
  \brief Optimization for point detectors
  \date December 2010
*/
class pointDetOpt
{
 private:
  
  double energy;            ///< Energy of neutron to test:
  ObjectTrackAct OA;        ///< Object information 
  
 public:
  
  pointDetOpt(const Geometry::Vec3D&);
  pointDetOpt(const pointDetOpt&);
  pointDetOpt& operator=(const pointDetOpt&);
  ~pointDetOpt() {}  ///< Destructor

  void createObjAct(const Simulation&);
  void addTallyOpt(const int,Simulation&);

};
  
}

#endif
