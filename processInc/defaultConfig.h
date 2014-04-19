/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   processInc/defaultConfig.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef mainSystem_defaultConfig_h
#define mainSystem_defaultConfig_h

class FuncDataBase;

namespace mainSystem
{
 
  
/*!
  \class defaultConfig
  \version 1.0
  \author S. Ansell
  \date March 2014
  \brief 
*/

class defaultConfig 
{
 private:

  const std::string keyName;                ///< KeyName

  std::map<std::string,double> varVal;             ///< Var set
  std::map<std::string,std::string> varName;       ///< Var name [mat]
  std::map<std::string,Geometry::Vec3D> varVec;       ///< Var Vec [mat]
  std::map<std::string,std::string> flagName;      ///< Flag e.g. targetType

 public:

  defaultConfig(const std::string&);
  defaultConfig(const defaultConfig&);
  defaultConfig& operator=(const defaultConfig&);
  ~defaultConfig();

  void setVar(const std::string&,const double);
  void setVar(const std::string&,const std::string&);
  void setVar(const std::string&,const Geometry::Vec3D&);
  void setOption(const std::string&,const std::string&);

  void process(FuncDataBase&,mainSystem::inputParam&) const;
    
};

}

#endif
