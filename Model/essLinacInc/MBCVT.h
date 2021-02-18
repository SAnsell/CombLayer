/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essLinacInc/MBCVT.h
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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
#ifndef essSystem_MBCVT_h
#define essSystem_MBCVT_h

class Simulation;

namespace essSystem
{

/*!
  \class MBCVT
  \version 1.0
  \author Konstantin Batkov
  \date 4 Mar 2017
  \brief Permanent Quadrupole Magnet
*/

class MBCVT :
    public attachSystem::ContainedComp,
    public attachSystem::FixedOffset,
    public attachSystem::CellMap
{
 private:

  const std::string baseName; ///< Base name (e.g. Linac)
  const std::string extraName; ///< extra name (e.g. MBCVT)

  int engActive;                ///< Engineering active flag

  double length;                ///< Total length including void
  double pipeRad;               ///< Total length including void

  int voidMat;                  ///< air material
  int pipeMat;                  ///< material in pipe
  int innerMat;                 ///< 
  int outerMat;                 ///< 
  int thermalMat;               ///< 
  int heliumMat;                ///< Why different (from voidMta)?

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:

  MBCVT(const std::string&,const std::string&,const size_t);
  MBCVT(const MBCVT&);
  MBCVT& operator=(const MBCVT&);
  virtual MBCVT* clone() const;
  virtual ~MBCVT();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif


