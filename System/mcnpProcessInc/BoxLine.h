/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/BoxLine.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef ModelSupport_BoxLine_h
#define ModelSupport_BoxLine_h

namespace ModelSupport
{
 
class surfRegister;


  
/*!
  \class BoxLine
  \version 1.0
  \author S. Ansell
  \date October 2011
  \brief A section of pipe
*/

class BoxLine 
{
 private:

  const std::string keyName;         ///< KeyName
  
  Geometry::Vec3D ZAxis;              ///< Box Z axis 

  std::vector<boxValues> CV;          ///< Section Values [ one for layer]
  std::vector<Geometry::Vec3D> Pts;   ///< Points in pipe

  std::map<size_t,HeadRule> layerSurf;   ///< Outgoing surface
  std::map<size_t,HeadRule> commonSurf;  ///< common surface

  std::map<size_t,std::set<int> > segExtra;  ///< Extra components to add
  std::vector<size_t> activeFlags;    ///< Activity flags : one for each PUnit

  std::vector<boxUnit*> PUnits;       ///< pipeUnits (1 less than Pts)
  std::string startSurf;             ///< Start surface
  

  void clearPUnits();
  void copyPUnits(const BoxLine&);

  void forcedInsertCells(const size_t);

  int createUnits(Simulation&);
  void insertPipe(Simulation&);


 public:

  BoxLine(const std::string&);
  BoxLine(const BoxLine&);
  BoxLine& operator=(const BoxLine&);
  ~BoxLine();

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

  void addSection(const double,const double,
		  const int,const double);
  void setActive(const size_t,const size_t);
  void setInitZAxis(const Geometry::Vec3D&);
  void setStartSurf(const std::string&);

  void addInsertCell(const size_t,const int);

  void insertPipeToCell(Simulation&,const int);


  const HeadRule& getCap(const size_t,const int) const;
  const boxUnit& first() const;
  const boxUnit& last() const; 

  
  void build(Simulation&);
    
};

}

#endif
