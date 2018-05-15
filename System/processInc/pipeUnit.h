/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   processInc/pipeUnit.h
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
#ifndef ModelSupport_pipeUnit_h
#define ModelSupport_pipeUnit_h

namespace ModelSupport
{
 
class surfRegister;

  /*!
    \struct cylValues 
    \version 1.0
    \author S. Ansell
    \date August 2011
    \brief Holds simple things about the cylinder
  */

struct cylValues
{
  double CRadius;   ///< Radius 
  int MatN;         ///< Material number
  double Temp;      ///< Radius 

  /// Basic constructor
  cylValues(const double& R,const int& M,const double& T) :
    CRadius(R),MatN(M),Temp(T) {}  
};

/*!
  \class pipeUnit
  \version 1.0
  \author S. Ansell
  \date July 2011
  \brief A single unit of a pipe
*/

class pipeUnit : public attachSystem::FixedComp,
  public attachSystem::ContainedComp
{
 private:

  const int surfIndex;      ///< surface number 
  int cellIndex;            ///< Cell index
  size_t nAngle;            ///< Angles to approximate for edge finding

  pipeUnit* prev;           ///< Previous pipe unit
  pipeUnit* next;           ///< Next pipe unit

  Geometry::Vec3D APt;      ///< Start point
  Geometry::Vec3D BPt;      ///< End Point
  
  Geometry::Vec3D Axis;     ///< Main axis
  Geometry::Vec3D ANorm;    ///< A-Pt Normal [outer]
  Geometry::Vec3D BNorm;    ///< B-Pt Normal [outer]

  HeadRule ASurf;           ///< Start point rule if used [inward facing]
  HeadRule BSurf;           ///< End point rule if used [inward facing]

  size_t activeFlag;                  ///< Flag for active layers
  std::vector<cylValues> cylVar;      ///< Cylinder variables
  std::set<int> cellCut;              ///< Cells the pipe 
  
  void calcNorm(const int,const Geometry::Vec3D&,
		const Geometry::Vec3D&);
  void checkForward();
  std::string createCaps() const;

  void populate(const size_t,const std::vector<cylValues>&);
  void createSurfaces();
  void createOuterObject();
  void createObjects(Simulation&);
  double getOuterRadius() const;
  size_t getOuterIndex() const;
  void insertObjects(Simulation&);

 public:
  
  pipeUnit(const std::string&,const size_t);
  pipeUnit(const pipeUnit&);
  /// Virtual constructor
  virtual pipeUnit* clone() const { return new pipeUnit(*this); }  
  pipeUnit& operator=(const pipeUnit&);
  virtual ~pipeUnit();

  /// Set Number of angles
  void setNAngle(const size_t A) { nAngle=(A) ? A : 6; }
  /// Access Axis
  const Geometry::Vec3D& getAxis() const { return Axis; }
  const Geometry::Vec3D& getPt(const int) const;
  const HeadRule& getCap(const int) const;
  
  void clearInsertSet();
  void addInsertSet(const std::set<int>&);
  
  /// Set surface number
  void setPoints(const Geometry::Vec3D&,const Geometry::Vec3D&);
  void setASurf(const HeadRule&);
  void setBSurf(const HeadRule&);
  void connectFrom(pipeUnit*);
  void connectTo(pipeUnit*);

  void createAll(Simulation&,const size_t,const std::vector<cylValues>&);
    
};

}

#endif
