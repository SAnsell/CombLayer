/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   moderatorInc/makeTS2.h
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
#ifndef t1System_makeT1Real_h
#define t1System_makeT1Real_h

namespace hutchSystem
{
  class ChipIRHutch;
}

namespace constructSystem
{
  class TargetBase;
}

namespace TMRSystem
{
  class TS2target;
}  


namespace moderatorSystem
{
  class FlightLine;
  class Reflector;
}

namespace shutterSystem
{
  class t1BulkShield;
  class t1CylVessel;
  class t1Vessel;
  class MonoPlug;
}

namespace moderatorSystem
{

  /*!
    \class makeTS2
    \version 1.0
    \author S. Ansell
    \date July 2012
    \brief General constructor for the TS1 system
  */

class makeTS2
{
 private:

//  boost::shared_ptr<t1PlateTarget> TarObj;            ///< Pressure vessel
//  boost::shared_ptr<constructSystem::TargetBase> TarObj;  
  

//  std::string buildTarget(Simulation&,const std::string&,const int);
  
 public:
  
  makeTS2();
  makeTS2(const makeTS2&);
  makeTS2& operator=(const makeTS2&);
  ~makeTS2();
  
  void build(Simulation*,const mainSystem::inputParam&);

};

}

#endif
