/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/TargetBase.h
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
#ifndef constructSystem_TargetBase_h
#define constructSystem_TargetBase_h

class Simulation;

namespace ts1System
{
  class BeamWindow;
  class ProtonVoid;
}

namespace constructSystem
{

/*!
  \class TargetBase
  \version 1.0
  \author S. Ansell
  \date January 2013
  \brief TargetBase [insert object]
*/

class TargetBase :
  public attachSystem::ContainedComp,
    public attachSystem::FixedOffset
{
 protected:

  std::shared_ptr<ts1System::BeamWindow> BWPtr;    ///< Beam window object
  std::shared_ptr<ts1System::ProtonVoid> PLine;    ///< Proton line  

  virtual void createBeamWindow(Simulation&,const long int);
  
 public:

  TargetBase(const std::string&,const size_t);
  TargetBase(const TargetBase&);
  TargetBase& operator=(const TargetBase&);
  virtual ~TargetBase() {}   ///< Destructor
  virtual TargetBase* clone() const =0;

  virtual void addProtonLine(Simulation&,const attachSystem::FixedComp&,
		     const long int) =0;
  virtual void addProtonLineInsertCell(const int);
  virtual void addProtonLineInsertCell(const std::vector<int>&);

  /// Null op function [for overload]
  virtual void setRefPlates(const int,const int) {}
  virtual void createAll(Simulation&,const attachSystem::FixedComp&) =0;
  virtual std::vector<int> getInnerCells() const;
  /// Main cell body
  virtual int getMainBody() const  { return 0; }
  /// Ta cell body
  virtual int getSkinBody() const { return 0; }


  /// Proton line    
  std::shared_ptr<ts1System::ProtonVoid> getProton() const { return PLine; }
};

}

#endif
 
