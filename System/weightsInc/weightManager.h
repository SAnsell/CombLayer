/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightsInc/weightManager.h
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
#ifndef WeightSystem_weightManager_h
#define WeightSystem_weightManager_h

class Simulation;

namespace WeightSystem
{
  class WForm;
  class WeightMesh;
  class WWG;
/*!
  \class weightManager
  \version 1.0
  \author S. Ansell
  \date April 2010
  \brief Controls the addition of weights to the system
*/

class weightManager 
{
 public:

  typedef std::map<std::string,WForm*> CtrlTYPE;  ///< WControl map  
  CtrlTYPE WMap;                           ///< Map of weight systems
  WWG* WWGPtr;                             ///< Weight mesh
    
 private:  

  weightManager();
  ////\cond SINGLETON
  weightManager(const weightManager&);
  weightManager& operator=(const weightManager&);
  ////\endcond SINGLETON
  
 public:

  static weightManager& Instance();
  ~weightManager();
  
  bool hasParticle(const std::string&) const;
  WForm* getParticle(const std::string&) const;
  WWG& getWWG();
  const WWG& getWWG() const;
  template<typename T> void addParticle(const std::string&);
  
  void renumberCell(const int,const int);  
  void maskCell(const int);
  bool isMasked(const int) const;

  void writePHITS(std::ostream&) const;
  void write(std::ostream&) const;

};

}

#endif
 
