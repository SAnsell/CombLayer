/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/F5Collimator.h
 *
 * Copyright (c) 2015 Konstantin Batkov
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
#ifndef essSystem_F5Collimator_h
#define essSystem_F5Collimator_h

class Simulation;

namespace essSystem
{
  class F5Collimator : public attachSystem::ContainedComp, public attachSystem::FixedComp, tallySystem::F5Calc {
      private:
	const int colIndex;             ///< Index of surface offset
	int cellIndex;                  ///< Cell index

	double xStep;                   ///< X step (point detector x coordinate)
	double yStep;                   ///< Y step (point detector y coordinate)
	double zStep;                   ///< Z step (point detector z coordinate)
	double xyAngle;                 ///< XY Angle
	double delta;                   ///< angle used to manually fine-tune the calculated xyAngle. Zero by default. Used only with -f5-collimators.
	double zAngle;                  ///< Z Angle

	double height;                  ///< Height along z
	double length;                   ///< Width  along x
	double width;                   ///< Width  along y
	double wall;                    ///< Wall thickness
	double viewWidth;               ///< View width in horizontal direction (distance between points B and C)

	int LinkPoint;              ///< Link point number (currently number-6 defines the quadrant as in butterfly.svgz). Not used if <0. Calculated if setTheta is used.
	double radius;              ///< Radius for cylindrical coordinates. Defined via 'F5Radius' in essVariables
	double theta;               ///< Theta as defined on page 183 of TDR. If theta is set vis setTheta, [xyz]Step are calculated.

	std::vector<Geometry::Vec3D> vecFP; ///< vector of focal points
	std::string range; ///< Wavelengh range {cold, thermal} - to define which focal points to use
	
	// Functions:

	void populate(FuncDataBase&);
	void populateWithTheta(FuncDataBase&);
	void createUnitVector(const attachSystem::FixedComp&);

	void createSurfaces();
	void createObjects(Simulation&);
	void createLinks();


      public:
	F5Collimator(const std::string&);
	F5Collimator(const F5Collimator&);
	F5Collimator& operator=(const F5Collimator&);
	virtual F5Collimator* clone() const;
	virtual ~F5Collimator();

	void setTheta(double t);
	inline void setFocalPoints(std::vector<Geometry::Vec3D> vec) { vecFP = vec; }
	inline void setRange(std::string val) { range = val; }
	int getMainCell() const { return colIndex+1; }
	virtual void addToInsertChain(attachSystem::ContainedComp&) const; 
	void createAll(Simulation&,const attachSystem::FixedComp&);

	//	inline const Geometry::Vec3D &getLinkPt(const size_t Index) { return attachSystem::FixedComp::getLinkPt(Index);}
	//	inline size_t NConnect() { return attachSystem::FixedComp::NConnect();}
  };
}

#endif
