/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuildInc/BeRefInnerStructure.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef essSystem_BeRefInnerStructure_h
#define essSystem_BeRefInnerStructure_h

class Simulation;

namespace essSystem
{
/*!
  \class BeRefInnerStructure
  \author Konstantin Batkov
  \version 2.0
  \date Jan 2016
  \brief Inner structure of Be reflector (engineering details)
*/

class BeRefInnerStructure : public attachSystem::ContainedComp,
    public attachSystem::FixedComp, public attachSystem::CellMap
{
 private:
  
  const int insIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  size_t nLayers;
  std::vector<double> baseFrac;
  std::vector<int> mat;  // materials

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces(const attachSystem::FixedComp&);
  void createObjects(Simulation&, const attachSystem::FixedComp&);
  void createLinks();
  void layerProcess(Simulation&, const attachSystem::FixedComp&, const std::string&, const int&, const int&);

 public:

  BeRefInnerStructure(const std::string&);
  BeRefInnerStructure(const BeRefInnerStructure&);
  BeRefInnerStructure& operator=(const BeRefInnerStructure&);
  virtual BeRefInnerStructure* clone() const;
  virtual ~BeRefInnerStructure();

  void createAll(Simulation&,const attachSystem::FixedComp&);

};

}

#endif
 
