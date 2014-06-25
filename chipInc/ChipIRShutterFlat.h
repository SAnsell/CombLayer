/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   chipInc/ChipIRShutterFlat.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef shutterSystem_ChipShutterFlat_h
#define shutterSystem_ChipShutterFlat_h

class Simulation;

namespace shutterSystem
{
  class GeneralShutter;

/*!
  \class ChipIRShutterFlat
  \author S. Ansell
  \version 1.0
  \date October 2011
  \brief Specialized for for the ChipIRShutterFlat
*/

class ChipIRShutterFlat : public GeneralShutter
{
 private:
  
  std::string chipKey;                   ///< Key name 
  
  std::vector<insertBaseInfo> CInfo; ///< The components to put in pipe

  double forwardStep;           ///< Distance to advance down pipe 
  double midStep;               ///< Distance of 2nd component [lead]

  boost::array<double,4> linerStartXZ;   ///< Linear (lead) start +/-x +/-z
  boost::array<double,4> linerEndXZ;     ///< Linear (lead) start +/-x +/-z

  double insertStep;            ///< Insert step at end of shine path
  int shineMat;                 ///< Material for shine system
  
  double backScrapThick;        ///< Backscraper thickness
  double backScrapExtra;        ///< Backscraper outer
  int backScrapMat;             ///< Backscapper mat

  //--------------
  // FUNCTIONS:
  //--------------
  void populate(const Simulation&);
  void createCInfoTable(Simulation&);
  void createShutterInsert(Simulation&);  
  void createShinePipe(Simulation&);
  void createUnitVector();

 public:

  ChipIRShutterFlat(const size_t,const std::string&,
		const std::string&);
  ChipIRShutterFlat(const ChipIRShutterFlat&);
  ChipIRShutterFlat& operator=(const ChipIRShutterFlat&);
  virtual ~ChipIRShutterFlat();

  virtual void createAll(Simulation&,const double,
			 const attachSystem::FixedComp*);
  const insertBaseInfo& getLastItem() const;
  
};

}

#endif
 
