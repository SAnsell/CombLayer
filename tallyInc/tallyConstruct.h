/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   tallyInc/tallyConstruct.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef tallySystem_tallyConstruct_h
#define tallySystem_tallyConstruct_h

namespace attachSystem
{
  class FixedComp;
}


class Simulation;

namespace tallySystem
{
  class fissionConstruct;

/*!
  \class tallyConstruct
  \version 1.0
  \author S. Ansell
  \date April 2012
  \brief Holds everthing for tallies

  Provides linkage to its outside on FixedComp[0]
*/
class tallyConstruct  : public basicConstruct
{
 private:

  /// Positions in the chipGrid  
  static std::map<std::string,int> chipGridPos; 

  pointConstruct* pointPtr;           ///< Point construct
  meshConstruct* meshPtr;             ///< Mesh point 
  fluxConstruct* fluxPtr;             ///< Flux [f4] 
  heatConstruct* heatPtr;             ///< Heat [f6]
  itemConstruct* itemPtr;             ///< Items : Beamline/named system
  surfaceConstruct* surfPtr;          ///< Surface [f1]
  fissionConstruct* fissionPtr;        ///< Fission f7

  void helpTallyType() const;

  // Grid stuff:
  void processGrid(Simulation&,const mainSystem::inputParam&,
		   const size_t) const;
  void processGridFree(Simulation&,const int,const int) const;
  void processGridObject(Simulation&,const std::string&,
			 const int,const int,const int) const;
  void processGridHelp() const;


  void applyMultiGrid(Simulation&,const int,
		      const int,const Geometry::Vec3D&,
		      const Geometry::Vec3D&,const Geometry::Vec3D&) const;

  void initStatic();
  
 public:

  tallyConstruct(const tallyConstructFactory&);
  tallyConstruct(const tallyConstruct&);
  tallyConstruct& operator=(const tallyConstruct&);
  virtual ~tallyConstruct();

  virtual void setPoint(pointConstruct*);
  virtual void setFission(fissionConstruct*);
  
  virtual int tallySelection(Simulation&,const mainSystem::inputParam&) const;
  virtual int tallyRenumber(Simulation&,const mainSystem::inputParam&) const;

};

}

#endif
 
