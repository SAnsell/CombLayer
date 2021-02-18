/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essLinacInc/DTL.h
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
#ifndef essSystem_DTL_h
#define essSystem_DTL_h

class Simulation;

namespace essSystem
{
  class PMQ;

/*!
  \class DTL
  \version 1.0
  \author Konstantin Batkov
  \date 28 Mar 2017
  \brief Drift Tube Linac section
*/

class DTL : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset,
  public attachSystem::CellMap
{
 private:

  const std::string baseName; ///< Base name (e.g. Linac)
  const std::string extraName; ///< extra name (e.g. DTL)

  int engActive;                ///< Engineering active flag

  double itLength; ///< intertank length
  double itRadius; ///< intertank pipe radius
  double itWallThick; ///< intertank pipe wall thick
  size_t nLayers;                   ///< number of layers
  std::vector<double> radius;   ///< Radii of each layer
  std::vector<int> mat; ///< materials
  int airMat; ///< air material
  size_t nPMQ; ///< number of PMQs
  std::vector<std::shared_ptr<PMQ> > pmq; ///< array of permanent quadrupole magnets

  void createPMQ(Simulation&, const long int);

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  DTL(const std::string&,const std::string&,const size_t);
  DTL(const DTL&);
  DTL& operator=(const DTL&);
  virtual DTL* clone() const;
  virtual ~DTL();

  using FixedComp::createAll;
  virtual void createAll(Simulation&,
			 const attachSystem::FixedComp&,
			 const long int);

};

}

#endif


