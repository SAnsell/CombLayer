/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   attachCompInc/FixedComp.h
 *
 * Copyright (c) 2004-2026 by Stuart Ansell
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
#ifndef attachSystem_FixedComp_h
#define attachSystem_FixedComp_h

class Simulation;
class localRotate;
class HeadRule;
/*!
  \namespace attachSystem
  \version 1.0
  \author S. Ansell
  \date April 2013
  \brief General linking/contained holders
*/


namespace attachSystem
{
  class FixedGroup;

/*!
  \class FixedComp
  \version 1.0
  \author S. Ansell
  \date March 2010
  \brief Component that is at a fixed position
*/

class FixedComp
{
 private:

  HeadRule getUSLink(const size_t) const;
  HeadRule getUSLinkComplement(const size_t) const;
  int getUSLinkSurf(const size_t) const;

  size_t getOrCreateLinkIndex(const std::string&);

 protected:

  const std::string keyName;       ///< Key Name
  ModelSupport::surfRegister SMap; ///< Surface register
  const int buildIndex;            ///< Index of surface offset
  int cellIndex;                   ///< Cell index

  std::map<std::string,size_t> keyMap; ///< Keynames to linkPt index

  Geometry::Vec3D X;            ///< X-coordinate
  Geometry::Vec3D Y;            ///< Y-coordinate
  Geometry::Vec3D Z;            ///< Z-coordinate
  Geometry::Vec3D Origin;       ///< Origin

  Geometry::Vec3D orientateAxis; ///< Axis for reorientation
  long int primeAxis;            ///< X/Y/Z Axis for reorientation

  std::vector<LinkUnit> LU;     ///< Linked unit items

  void makeOrthogonal();

  const HeadRule& getUSMainRule(const size_t) const;
  const HeadRule& getUSCommonRule(const size_t) const;

  void setUSLinkComplement(const size_t,const FixedComp&,const size_t);
  void setUSLinkCopy(const size_t,const FixedComp&,const size_t);

  //  virtual std::string getLinkString(const long int) const;

  // Legacy numeric-index link-point API. Retained only so that
  // name-keyed wrappers (and code not yet migrated) can still reach
  // the underlying LU slot by position; new/migrated code must use
  // the name-keyed overloads below instead.
  void setConnect(const size_t,const Geometry::Vec3D&,const Geometry::Vec3D&);
  void setLineConnect(const size_t,const Geometry::Vec3D&,
		      const Geometry::Vec3D&);

  void setLinkSurf(const size_t,const int);
  void setLinkSurf(const size_t,const std::string&);
  void setLinkSurf(const size_t,const HeadRule&);
  void setLinkSurf(const size_t,const HeadRule&,const bool,
		   const HeadRule&,const bool);

  void setLinkComp(const size_t,const int);
  void setLinkComp(const size_t,const std::string&);
  void setLinkComp(const size_t,const HeadRule&);

  void addLinkSurf(const size_t,const int);
  void addLinkSurf(const size_t,const std::string&);
  void addLinkSurf(const size_t,const HeadRule&);

  void addLinkComp(const size_t,const int);
  void addLinkComp(const size_t,const std::string&);
  void addLinkComp(const size_t,const HeadRule&);

  void setBridgeSurf(const size_t,const int);
  void setBridgeSurf(const size_t,const HeadRule&);
  void addBridgeSurf(const size_t,const int);
  void addBridgeSurf(const size_t,const std::string&);

  void setLinkCopy(const size_t,const FixedComp&,const std::string&);
  void setLinkCopy(const size_t,const FixedComp&,const long int);

  void nameSideIndex(const size_t,const std::string&);
  void nameSideIndex(const std::map<std::string,size_t>&);
  void setNConnect(const size_t);

 public:

  /// Tag type selecting the non-registered constructors (no objectRegister entry)
  struct unregistered_t {};
  /// Tag value selecting the non-registered constructors
  static constexpr unregistered_t unregistered{};

  static void computeZOffPlane(const Geometry::Vec3D&,
			       const Geometry::Vec3D&,
			       Geometry::Vec3D&);

  /// Tag type selecting the explicit-reserved-cell-range constructor
  struct resSize_t {};
  /// Tag value selecting the explicit-reserved-cell-range constructor
  static constexpr resSize_t withResSize{};

  explicit FixedComp(unregistered_t,std::string ="Null");
  explicit FixedComp(const std::string&);
  // For the rare case (e.g. RoofPillars) that needs a larger cell-index
  // reservation than the default 10000 -- everything else should use
  // the plain single-string constructor above.
  FixedComp(resSize_t,const std::string&,const size_t);
  FixedComp(const std::string&,Geometry::Vec3D );
  FixedComp(const std::string&,
	    Geometry::Vec3D ,const Geometry::Vec3D&,
	    const Geometry::Vec3D&,const Geometry::Vec3D&);
  FixedComp(const FixedComp&);
  FixedComp(const FixedComp&&);
  FixedComp& operator=(const FixedComp&);
  virtual ~FixedComp() {}     ///< Destructor

  const LinkUnit& operator[](const size_t) const;

  /// have cells been built
  bool hasActiveCells() const
  { return (buildIndex+1)!=cellIndex; }

  void reOrientate();
  void reOrientate(const size_t,const Geometry::Vec3D&);

  // Operator Set:
  virtual void createUnitVector(const FixedComp&);
  virtual void createUnitVector(const FixedComp&,const Geometry::Vec3D&);
  virtual void createUnitVector(const FixedComp&,const long int);
  virtual void createUnitVector(const FixedComp&,const long int,const long int);
  virtual void createUnitVector(const FixedComp&,const long int,
				const FixedComp&,const long int);
  virtual void createUnitVector(const Geometry::Vec3D&,
				const Geometry::Vec3D&);

  virtual void createUnitVector(const Geometry::Vec3D&,
				const Geometry::Vec3D&,
				const Geometry::Vec3D&);

  virtual void createUnitVector(const Geometry::Vec3D&,
				const Geometry::Vec3D&,
				const Geometry::Vec3D&,
				const Geometry::Vec3D&);

  void createPairVector(const FixedComp&,const long int,
			const FixedComp&,const long int);

  void setCentre(const Geometry::Vec3D&);
  void applyShift(const double,const double,const double);

  void applyAngleRotate(const double,const double);
  void applyAngleRotate(const double,const double,const double);
  void applyFullRotate(const double,const double,
		       const Geometry::Vec3D&);
  void applyFullRotate(const double,const double,const double,
		       const Geometry::Vec3D&);

  void linkAngleRotate(const size_t,const double,const double);
  void linkAngleRotate(const size_t,const double,const double,const double);
  void linkShift(const size_t,const double,const double,const double);

  void reverseX();
  void reverseZ();

  // Name-keyed link-point API: the name is obligatory and the first
  // use of a given name allocates its LU slot on demand -- the
  // numeric index is purely an internal detail from here on.
  void setConnect(const std::string&,const Geometry::Vec3D&,
		  const Geometry::Vec3D&);
  void setLineConnect(const std::string&,const Geometry::Vec3D&,
		      const Geometry::Vec3D&);
  void setBasicExtent(const double,const double,const double);

  void setLinkSurf(const std::string&,const int);
  void setLinkSurf(const std::string&,const std::string&);
  void setLinkSurf(const std::string&,const HeadRule&);
  void setLinkSurf(const std::string&,const HeadRule&,const bool,
		   const HeadRule&,const bool);

  void setLinkComp(const std::string&,const int);
  void setLinkComp(const std::string&,const std::string&);
  void setLinkComp(const std::string&,const HeadRule&);

  void addLinkSurf(const std::string&,const int);
  void addLinkSurf(const std::string&,const std::string&);
  void addLinkSurf(const std::string&,const HeadRule&);

  void addLinkComp(const std::string&,const int);
  void addLinkComp(const std::string&,const std::string&);
  void addLinkComp(const std::string&,const HeadRule&);

  void setBridgeSurf(const std::string&,const int);
  void setBridgeSurf(const std::string&,const HeadRule&);
  void addBridgeSurf(const std::string&,const int);
  void addBridgeSurf(const std::string&,const std::string&);

  void setLinkCopy(const std::string&,const FixedComp&,const long int);
  void setLinkCopy(const std::string&,const FixedComp&,const std::string&);

  /// Get keyname
  const std::string& getKeyName() const { return keyName; }
  /// Access X direction
  const Geometry::Vec3D& getX() const { return X; }
  /// Access Y direction
  const Geometry::Vec3D& getY() const { return Y; }
  /// Access Z direction
  const Geometry::Vec3D& getZ() const { return Z; }
  /// Access centre
  virtual const Geometry::Vec3D& getCentre() const  { return Origin; }
  virtual int getExitWindow(const long int,std::vector<int>&) const;
  virtual const Geometry::Vec3D& getExit() const;

  void copyLinkObjects(const FixedComp&);
  /// How many connections
  size_t NConnect() const { return LU.size(); }

  const LinkUnit& getSignedRefLU(const long int)  const;
  const LinkUnit& getLU(const size_t)  const;
  LinkUnit& getLU(const size_t);

  LinkUnit getSignedLU(const long int) const;
  bool hasSideIndex(const std::string&) const;
  long int getSideIndex(const std::string&) const;
  std::string getSideName(const long int) const;

  std::vector<Geometry::Vec3D> getAllLinkPts() const;

  bool hasLinkPt(const long int) const;
  bool hasLinkPt(const std::string&) const;

  bool hasLinkSurf(const long int) const;
  bool hasLinkSurf(const std::string&) const;

  Geometry::Vec3D getLinkPt(const std::string&) const;
  Geometry::Vec3D getLinkAxis(const std::string&) const;
  int getLinkSurf(const std::string&) const;

  Geometry::Vec3D getLinkPt(const long int) const;
  Geometry::Vec3D getLinkAxis(const long int) const;
  Geometry::Vec3D getLinkZAxis(const long int) const;


  double getLinkDistance(const std::string&,const std::string&) const;
  double getLinkDistance(const long int,const long int) const;
  double getLinkDistance(const long int,const FixedComp&,
			 const long int) const;
  virtual int getLinkSurf(const long int) const;

  HeadRule getFullRule(const std::string&) const;
  HeadRule getFullRule(const long int) const;

  HeadRule getMainRule(const long int) const;
  HeadRule getMainRule(const std::string&) const;

  HeadRule getCommonRule(const std::string&) const;
  HeadRule getCommonRule(const long int) const;

  long int findLinkAxis(const Geometry::Vec3D&) const;

  /// access next cell if need
  int nextCell() { return cellIndex++; }
  /// access next cell as needed
  int getNextCell() const { return cellIndex; }


  void calcLinkAxis(const long int,Geometry::Vec3D&,
		    Geometry::Vec3D&,Geometry::Vec3D&) const;

  /// remove secondary control on axis
  void clearAxisControl() { primeAxis=0; }
  void setAxisControl(const long int,const Geometry::Vec3D&);
  virtual void applyRotation(const localRotate&);
  virtual void applyRotation(const Geometry::Vec3D&,const double);

  void setExit(const int,const Geometry::Vec3D&,const Geometry::Vec3D&);
  void setExit(const Geometry::Vec3D&,const Geometry::Vec3D&);


  std::vector<int> splitObject(Simulation&,const int,const int);
  std::vector<int> splitObject(Simulation&,const std::string&,const int);

  std::vector<int> splitObject(Simulation&,const int,const int,
			       const Geometry::Vec3D&,const Geometry::Vec3D&);
  std::vector<int> splitObject(Simulation&,const int,const int,
			       const std::vector<Geometry::Vec3D>&,
			       const std::vector<Geometry::Vec3D>&);

  std::vector<int> splitObjectAbsolute
  (Simulation&,const int,const std::string&,
     const Geometry::Vec3D&,const Geometry::Vec3D&);

  std::vector<int> splitObjectAbsolute
    (Simulation&,const int,const int,
     const Geometry::Vec3D&,const Geometry::Vec3D&);

  std::vector<int> splitObjectAbsolute
    (Simulation&,const int,const int, const std::vector<Geometry::Vec3D>&,
     const std::vector<Geometry::Vec3D>&);

  virtual void createAll(Simulation&,const FixedComp&,const long int);
  virtual void createAll(Simulation&,const FixedComp&,const long int,
			 const FixedComp&,const long int);
  virtual void createAll(Simulation&,const FixedComp&,const std::string&);
  virtual void createAll(Simulation&,const FixedComp&,const std::string&,
			 const FixedComp&,const std::string&);
  virtual void createAll(Simulation&,const FixedGroup&,const std::string&,
			 const std::string&);
  virtual void createAll(Simulation&,const FixedGroup&,const std::string&,
			 const long int);

};

}

#endif
