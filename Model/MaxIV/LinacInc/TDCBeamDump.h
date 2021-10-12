/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/TDCBeamDump.h
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
#ifndef tdcSystem_TDCBeamDump_h
#define tdcSystem_TDCBeamDump_h

class Simulation;

namespace tdcSystem
{

/*!
  \class TDCBeamDump
  \version 1.0
  \author Konstantin Batkov
  \date October 2021
  \brief TDC beam dump
*/

class TDCBeamDump : public attachSystem::ContainedComp,
                    public attachSystem::FixedRotate,
                    public attachSystem::CellMap,
                    public attachSystem::SurfMap,
                    public attachSystem::ExternalCut
{
 private:

  double length;                ///< Total length including void
  double bulkWidthLeft;         ///< Left-side bulk width
  double bulkWidthRight;        ///< Right-side bulk width
  double bulkHeight;            ///< Bulk height
  double bulkDepth;             ///< Bulk depth
  double bulkThickBack;         ///< Bulk thickness behind the core
  double coreRadius;            ///< Core radius
  double coreLength;            ///< Core length
  double preCoreLength;         ///< Length of void before core
  double skinThick;             ///< Skin thickness

  int coreMat;                   ///< Core material
  int bulkMat;                  ///< Bulk material
  int skinMat;                  ///< Skin material

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  TDCBeamDump(const std::string&);
  TDCBeamDump(const TDCBeamDump&);
  TDCBeamDump& operator=(const TDCBeamDump&);
  virtual TDCBeamDump* clone() const;
  virtual ~TDCBeamDump();

  using attachSystem::FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif
