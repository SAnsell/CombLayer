/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chipInc/ChipIRInsert.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef ShutterSystem_ChipIRInsert_h
#define ShutterSystem_ChipIRInsert_h

class Simulation;

namespace shutterSystem
{
  class shutterInfo;
  class GeneralShutter;
  class CylinderColl;
  class LeadPlate;

/*!
  \class ChipIRInsert
  \author S. Ansell
  \version 1.0
  \date October 2009
  \brief Specialized for for the ChipIRInsert
*/

class ChipIRInsert : public BulkInsert
{
 private:
  
  const std::string compName;  ///< local keyName

  Geometry::Vec3D voidOrigin; ///< Void origin
  Geometry::Vec3D Axis;       ///< Centre Cylinder axis

  double zModAngle;           ///< Z angle rotation
  double xyModAngle;          ///< XY angle rotation
  double fStep;               ///< Forward step
  double bStep;               ///< Back step
  double radius;              ///< Radius of hole
  double lowCut;              ///< Low cut distance
  double rXDisp;              ///< X dispacement of hole
  double rZDisp;              ///< Z dispacement of hole

  double clearTopGap;         ///< Top clear gap
  double clearSideGap;        ///< Side clear gap
  double clearBaseGap;        ///< Base clear gap
  double clearTopOff;         ///< Top clear dist
  double clearSideOff;        ///< Side clear dist
  double clearBaseOff;        ///< Base clear dist


  int frontMat;               ///< Front plate material
  int backMat;                ///< Back plate material
  int defMat;                 ///< Default material

  int chipInnerVoid;              ///< Inner void cell 
  int chipOuterVoid;              ///< Outer void cell 

  std::shared_ptr<CylinderColl> CCol;     ///< Cylinder interlayer
  std::shared_ptr<LeadPlate> PbA;     ///< Lead Plate 1
  std::shared_ptr<LeadPlate> PbB;     ///< Lead Plate 2

  size_t nLayers;           ///< Number of layers
  std::vector<double> cFrac;      ///< Plate Layer thicknesss (fractions)
  std::vector<int> cMat;          ///< Plate Layer materials
  std::vector<int> CDivideList;   ///< Cell divide List for 
    
  // Function:

  void populate(const Simulation&);
  void createUnitVector();

  void createSurfaces();
  void createObjects(Simulation&);
  void createLeadPlate(Simulation&);
  void layerProcess(Simulation&);

  void createDatumPoints() const;

 public:

  ChipIRInsert(const size_t,const std::string&,const std::string&);
  ChipIRInsert(const ChipIRInsert&);
  ChipIRInsert& operator=(const ChipIRInsert&);
  virtual ~ChipIRInsert();

  int exitWindow(const double,std::vector<int>&,
		 Geometry::Vec3D&) const;

  void createAll(Simulation&,const shutterSystem::GeneralShutter&);
  
};

}

#endif
 
