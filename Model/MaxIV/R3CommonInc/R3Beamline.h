/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R3CommonInc/R3Beamline.h
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
#ifndef xraySystem_R3Beamline_h
#define xraySystem_R3Beamline_h

namespace xraySystem
{

  class R3Ring;
  /*!
    \class R3Beamline
    \version 1.0
    \author S. Ansell
    \date January 2018
    \brief General constructor for the xray system
  */

class R3Beamline :
  public attachSystem::CopiedComp
{
 protected:

  /// ring component  [taken from main setup]
  std::shared_ptr<R3Ring> r3Ring;
  std::string startPoint;       ///< Start point
  std::string stopPoint;        ///< End point

 public:
  
  R3Beamline(const std::string&,const std::string&);
  R3Beamline(const R3Beamline&);
  R3Beamline& operator=(const R3Beamline&);
  virtual ~R3Beamline();

  /// set ring
  void setRing(std::shared_ptr<R3Ring> R)
    { r3Ring=R; }

  /// Accessor to stop point
  void setStopPoint(const std::string& SP)  { stopPoint=SP; }

  virtual void build(Simulation&,const attachSystem::FixedComp&,
		     const long int) =0;

};

}

#endif
