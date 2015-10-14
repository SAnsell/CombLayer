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
	double zAngle;                  ///< Z Angle

	double height;                  ///< Height along z
	double length;                   ///< Width  along x
	double width;                   ///< Width  along y
	double wall;                    ///< Wall thickness

	int GluePoint;              ///< Glue point number (currently number defines quadrant as in butterfly.svgz). Not used if <0
	
	// Functions:

	void populate(FuncDataBase&);
	void createUnitVector(const attachSystem::FixedComp&);

	void createSurfaces();
	void createObjects(Simulation&);
	void createLinks();


      public:
	F5Collimator(const std::string&);
	//F5Collimator(const F5Collimator&);
	F5Collimator& operator=(const F5Collimator&);
	virtual ~F5Collimator();

	int getMainCell() const { return colIndex+1; }
	virtual void addToInsertChain(attachSystem::ContainedComp&) const; 
	void createAll(Simulation&,const attachSystem::FixedComp&);

	//	inline const Geometry::Vec3D &getLinkPt(const size_t Index) { return attachSystem::FixedComp::getLinkPt(Index);}
	//	inline size_t NConnect() { return attachSystem::FixedComp::NConnect();}
  };
}

#endif
