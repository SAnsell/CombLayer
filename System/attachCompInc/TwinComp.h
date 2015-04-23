/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   attachCompInc/TwinComp.h
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
#ifndef attachSystem_TwinComp_h
#define attachSystem_TwinComp_h

namespace attachSystem
{
/*!
  \class TwinComp
  \version 1.0
  \author S. Ansell
  \date  May 2012
  \brief Both a beamline and a 
*/

class TwinComp  : public FixedComp,
    public SecondTrack
{
 private:
 

 public:
  
  TwinComp(const std::string&,const size_t NL);
  TwinComp(const TwinComp&);
  TwinComp& operator=(const TwinComp&);
  virtual ~TwinComp() {}     ///< Destructor
  
  void createUnitVector(const TwinComp&);
  void createUnitVector(const FixedComp&,const SecondTrack&);
  void setBeamExit(const int,const Geometry::Vec3D&,
		   const Geometry::Vec3D&);

		   
  virtual void applyRotation(const Geometry::Vec3D&,const double);
  void report() const;

};

}

#endif
 
