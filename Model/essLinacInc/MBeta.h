/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/MBeta.h
 *
 * Copyright (c) 2018-2021 by Konstantin Batkov
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
#ifndef essSystem_MBeta_h
#define essSystem_MBeta_h

class Simulation;

namespace constructSystem
{
  class VacuumPipe;
}

namespace essSystem
{
  class SpokeCavity;
  class QXQuad6;
  class MagRoundVacuumPipe;
  class MBCVT;
/*!
  \class MBeta
  \version 1.0
  \author Douglas Di Julio
  \date 15 Mar 2018
  \brief spoke cavity section of the ESS linac
*/

class MBeta :
    public attachSystem::CopiedComp,
    public attachSystem::ContainedComp,
    public attachSystem::FixedOffset
{
 private:
  const std::string baseName;    ///< Base name
  /// array of vacuum pipes
  std::vector<std::shared_ptr<constructSystem::VacuumPipe>> vacPipe;
  /// array of spoke cavities
  std::vector<std::shared_ptr<SpokeCavity>> spkcvt;
  /// array of spoke cavities
  std::vector<std::shared_ptr<QXQuad6>> qxquad;
  /// array of spoke cavities
  std::vector<std::shared_ptr<MagRoundVacuumPipe>> magRoundVacuumPipe;
  /// array of mbcvt
  std::vector<std::shared_ptr<MBCVT>> mbcvt;
  
  void populate(const FuncDataBase&);
  void createLinks();

 public:

  MBeta(const std::string&,const std::string&);
  MBeta(const MBeta&);
  MBeta& operator=(const MBeta&);
  virtual MBeta* clone() const;
  virtual ~MBeta();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			 const long int);

};

}

#endif
 

