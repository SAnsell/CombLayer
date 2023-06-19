/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/makeESSBL.h
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
#ifndef essSystem_makeESSBL_h
#define essSystem_makeESSBL_h

namespace beamlineSystem
{
  class GuideLine;
}

namespace essSystem
{
  class GuideBay;

  /*!
    \class makeESSBL
    \version 1.0
    \author S. Ansell
    \date April 2014
    \brief Simple beamline constructor for ESS
  */
  
class makeESSBL : 
  public  beamlineSystem::beamlineConstructor
{
 private:

  const std::string shutterName;              ///< Shutter name
  const std::string beamName;                 ///< Name of beamline
  
 public:

  static std::pair<int,int> getBeamNum(const std::string&);
  
  makeESSBL(std::string ,std::string );
  makeESSBL(const makeESSBL&);
  makeESSBL& operator=(const makeESSBL&);
  ~makeESSBL() override;  

  void build(Simulation&,const essSystem::Bunker&) override;

};

}

#endif
