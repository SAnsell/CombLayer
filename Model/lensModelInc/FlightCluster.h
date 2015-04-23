/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   lensModelInc/FlightCluster.h
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
#ifndef lensModel_FlightCluster_h
#define lensModel_FlightCluster_h

class Simulation;

namespace lensSystem
{
/*!
  \class FlightCluster
  \version 1.0
  \author S. Ansell
  \date January 2010
  \brief Adds the ChipIR Guide
*/

class FlightCluster 
{
 private:

  const std::string keyName;
  /// Actual flight lines
  std::vector<moderatorSystem::FlightLine> FL;
  std::vector<int> InsertCells;     ///< Cells to be inserted

  void populate(const FuncDataBase&);
  void createLines(Simulation&,const attachSystem::FixedComp&,
		   const int);

 public:

  FlightCluster(const std::string&);
  FlightCluster(const FlightCluster&);
  FlightCluster& operator=(const FlightCluster&);
  ~FlightCluster();

  /// Clear cells
  void clearCells() { InsertCells.clear(); }
  void addInsertCell(const int);

  const attachSystem::FixedComp& getLine(const size_t) const;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const int);
};

}

#endif
 
