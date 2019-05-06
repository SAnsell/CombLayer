/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/Spoke.h
 *
 * Copyright (c) 2018 by Konstantin Batkov
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
#ifndef essSystem_Spoke_h
#define essSystem_Spoke_h

class Simulation;

namespace constructSystem
{
  class VacuumPipe;
}

namespace essSystem
{
  class SpokeCavity;
  class QXQuad;
  class MagRoundVacuumPipe;
/*!
  \class Spoke
  \version 1.0
  \author Douglas Di Julio
  \date 15 Mar 2018
  \brief spoke cavity section of the ESS linac
*/

class Spoke : public attachSystem::CopiedComp,
  public attachSystem::ContainedComp,
  public attachSystem::FixedOffset
{
 private:
  const std::string baseName;    ///< Base name
  std::vector<std::shared_ptr<constructSystem::VacuumPipe> > vacPipe; ///< array of vacuum pipes
  std::vector<std::shared_ptr<SpokeCavity> > spkcvt; ///< array of spoke cavities
  std::vector<std::shared_ptr<QXQuad> > qxquad; ///< array of spoke cavities
  std::vector<std::shared_ptr<MagRoundVacuumPipe> > magRoundVacuumPipe; ///< array of spoke cavities
  
  void populate(const FuncDataBase&);
  void createLinks();
 

 public:

  Spoke(const std::string&,const std::string&);
  Spoke(const Spoke&);
  Spoke& operator=(const Spoke&);
  virtual Spoke* clone() const;
  virtual ~Spoke();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
 

