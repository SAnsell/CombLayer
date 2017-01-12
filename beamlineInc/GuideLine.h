/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   beamlineInc/GuideLine.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef beamlineSystem_GuideLine_h
#define beamlineSystem_GuideLine_h

class Simulation;


namespace beamlineSystem
{

class ShapeUnit;

/*!
  \class GuideLine
  \version 1.0
  \author S. Ansell
  \date April 2014
  \brief Basic beamline guide unit
*/

class GuideLine : public attachSystem::ContainedComp,
  public attachSystem::FixedGroup,
  public attachSystem::CellMap,
  public attachSystem::FrontBackCut
{
 private:

  const int SUItem;              ///< Item offest [200 def]
  const int SULayer;             ///< Item layer [20 def]

  const int guideIndex;         ///< Index of surface offset
  int cellIndex;                ///< Cell index

  double xStep;                 ///< Shielding X Step
  double yStep;                 ///< Shielding Y Step
  double zStep;                 ///< Shielding Z Step
  double xyAngle;               ///< Shielding Rotation angle
  double zAngle;                ///< Shielding Rotation angle

  double beamXStep;             ///< Shielding X Step
  double beamYStep;             ///< Shielding Y Step
  double beamZStep;             ///< Shielding Z Step
  double beamXYAngle;           ///< Shielding Rotation angle
  double beamZAngle;            ///< Shielding Z-Rotation angle

  
  // OUTER DIMENTIONS:
  double length;               ///< Full length
  double height;               ///< Centre to top height
  double depth;                ///< Centre to floor depth
  double leftWidth;            ///< Left width
  double rightWidth;           ///< Right width

  size_t nShapeLayers;              ///< Number of shapeLayers
  std::vector<double> layerThick;   ///< Thickness [inner->outer]
  std::vector<int> layerMat;        ///< Mat 


  bool beamFrontCut;           ///< Construct+Use beam cut as well
  bool beamEndCut;             ///< Construct+Use beam cut as well

  
  size_t nShapes;              ///< Number of shape segments
  /// Shape units
  std::vector<ShapeUnit*> shapeUnits;

  int activeShield;              ///< Outer layer active
  int feMat;                     ///< Layer shielding
 
  std::string shapeFrontSurf(const bool,const size_t) const;
  std::string shapeBackSurf(const bool,const size_t) const;
  
  void clear();
  void processShape(const FuncDataBase&);
  void addGuideUnit(const size_t,const Geometry::Vec3D&,
		    const double,const double,
		    const double,const double);

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int,
			const attachSystem::FixedComp&,const long int);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void createMainLinks();
  void createGuideLinks();

  void checkRectangle(const double,const double) const;
  Geometry::Vec3D calcActiveEndIntercept();
  
 public:

  GuideLine(const std::string&);
  GuideLine(const GuideLine&);
  GuideLine& operator=(const GuideLine&);
  virtual ~GuideLine();

  HeadRule getXSection(const size_t =0,const size_t= 0) const;
  HeadRule getXSectionOut(const size_t =0,const size_t= 0) const;
  
  virtual void createAll(Simulation&,
			 const attachSystem::FixedComp&,const long int,
			 const attachSystem::FixedComp&,const long int);


};

}

#endif
 
