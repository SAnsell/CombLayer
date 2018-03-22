/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/PipeLine.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef ModelSupport_PipeLine_h
#define ModelSupport_PipeLine_h

namespace ModelSupport
{
 
class surfRegister;
  
/*!
  \class PipeLine
  \version 1.0
  \author S. Ansell
  \date July 2011
  \brief A single unit of a pipe
*/

class PipeLine 
{
 private:

  const std::string keyName;       ///< KeyName
  
  size_t nCylinder;                ///< Number of pre-cylinders
  std::vector<cylValues> CV;       ///< Cylinder Values [ one for each radius]

  std::vector<Geometry::Vec3D> Pts;      ///< Points in pipe

  std::map<size_t,HeadRule> layerSurf;   ///< Outgoing surface
  std::map<size_t,HeadRule> commonSurf;  ///< common surface

  std::map<size_t,std::set<int> > segExtra;  ///< Extra components to add
  std::vector<size_t> activeFlags;   ///< Activity flags : one for each PUnit

  size_t nAngle;                     ///< NAngle
  std::vector<pipeUnit*> PUnits;     ///< pipeUnits (1 less than Pts)
  std::string startSurf;             ///< Start surface
  
  void forcedInsertCells(const size_t);
  
  void clearPUnits();

  int createUnits(Simulation&);

 public:

  PipeLine(const std::string&);
  PipeLine(const PipeLine&);
  PipeLine& operator=(const PipeLine&);
  ~PipeLine();

  /// Number of point in pipe
  size_t nPoints() const { return Pts.size(); }
  /// Debug accessor
  const std::vector<Geometry::Vec3D>& getPt() const
    { return Pts; }
  
  void setPoints(const std::vector<Geometry::Vec3D>&);  
  void addPoint(const Geometry::Vec3D&);
  void addSurfPoint(const Geometry::Vec3D&,const std::string&);
  void addSurfPoint(const Geometry::Vec3D&,const std::string&,
		    const std::string&);
  void addRadius(const double,const int,const double);
  void setActive(const size_t,const size_t);
  void setNAngle(const size_t);

  void setStartSurf(const std::string&);
  void addInsertCell(const size_t,const int);
  const Geometry::Vec3D& getAxis(const size_t) const;
  const HeadRule& getCap(const size_t,const int) const;
  const pipeUnit& first() const;
  const pipeUnit& last() const; 
  
  void createAll(Simulation&);
    
};

}

#endif
