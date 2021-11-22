/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/RoundShutterGenerator.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef setVariable_RoundShutterGenerator_h
#define setVariable_RoundShutterGenerator_h

class FuncDataBase;

namespace setVariable
{
  class SixPortGenerator;
  class PortTubeGenerator;
  class PortItemGenerator;
  class ShutterUnitGenerator;

/*!
  \class RoundShutterGenerator
  \version 1.0
  \author S. Ansell
  \date May 2018
  \brief RoundShutterGenerator for variables
*/

class RoundShutterGenerator
{
 private:

  std::unique_ptr<SixPortGenerator> ITubeGen;   ///< Main port tube
  std::unique_ptr<PortItemGenerator> PItemGen;   ///< port items
  
  /// Shutter units [both the same]
  std::unique_ptr<ShutterUnitGenerator> SUnitGen;  

 public:

  RoundShutterGenerator();
  RoundShutterGenerator(const RoundShutterGenerator&);
  RoundShutterGenerator& operator=(const RoundShutterGenerator&);
  virtual ~RoundShutterGenerator();

  template<typename CF> void setCF();
  template<typename CF> void setOutCF();
  template<typename CF> void setTopCF();
  
  void setMat(const std::string&,const double);
  void setLift(const double,const double);
  
  void generateShutter(FuncDataBase&,const std::string&,
		       const bool,const bool) const;
  
};

}

#endif
 
