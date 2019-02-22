/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/MonoShutterGenerator.h
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
#ifndef setVariable_MonoShutterGenerator_h
#define setVariable_MonoShutterGenerator_h

class FuncDataBase;

namespace setVariable
{
  class PortTubeGenerator;
  class PortItemGenerator;
  class ShutterUnitGenerator;

/*!
  \class MonoShutterGenerator
  \version 1.0
  \author S. Ansell
  \date May 2018
  \brief MonoShutterGenerator for variables
*/

class MonoShutterGenerator
{
 private:

  std::unique_ptr<PortTubeGenerator> PTubeGen;   ///< Main port tube
  std::unique_ptr<PortItemGenerator> PItemGen;   ///< port items
  
  /// Shutter units [both the same]
  std::unique_ptr<ShutterUnitGenerator> SUnitGen;  

 public:

  MonoShutterGenerator();
  MonoShutterGenerator(const MonoShutterGenerator&);
  MonoShutterGenerator& operator=(const MonoShutterGenerator&);
  virtual ~MonoShutterGenerator();

  template<typename CF> void setCF();
  template<typename CF> void setOutCF();
  template<typename CF> void setTopCF();
  
  void setMat(const std::string&,const double);
  
  void generateShutter(FuncDataBase&,const std::string&,
		       const bool,const bool) const;
  
};

}

#endif
 
