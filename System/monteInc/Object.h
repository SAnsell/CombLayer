/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/Object.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef MonteCarlo_Object_h
#define MonteCarlo_Object_h

class Token;

namespace MonteCarlo
{
  class particle;
  class Material;

/*!
  \class Object
  \brief  Global object for object 
  \version 1.0
  \date July 2005
  \author S. Ansell

  An object is a collection of Rules and
  surface object
*/

class Object
{
 private:

  std::string FCUnit; ///< FixedComp name
  
  int ObjName;             ///< Number for the object
  int listNum;             ///< Creation number
  double Tmp;              ///< Starting temperature (if given)
  const Material* matPtr;  ///< Material Number 
  int trcl;          ///< transform number
  Importance imp;           ///< importance / 0 

  int populated;     ///< Full population

  bool activeMag;         ///< Magnetic field active
  double magMinStep;      ///< min step for mag field [fluka]
  double magMaxStep;      ///< max step for mag field [fluka]
  
  HeadRule HRule;    ///< Top rule

  Geometry::Vec3D COM;       ///< Centre of mass 
  
  /// Set of surfaces that are logically opposite in the rule.
  std::set<const Geometry::Surface*> logicOppSurf;
 
  int checkSurfaceValid(const Geometry::Vec3D&,const Geometry::Vec3D&) const;
  int checkExteriorValid(const Geometry::Vec3D&,const Geometry::Vec3D&) const;
  /// Calc in/out 
  int calcInOut(const int,const int) const;

 protected:
  
  int objSurfValid;                 ///< Object surface valid

  /// Full surfaces (make a map including complementary object ?)
  std::vector<const Geometry::Surface*> SurList;  
  std::set<int> SurSet;              ///< set of surfaces in cell [signed]

  const Geometry::Surface* getSurf(const int) const;
  
 public:

  Object();
  Object(const std::string&,const int,const int,
	 const double,const std::string&);
  Object(const std::string&,const int,const int,
	 const double,const HeadRule&);
  Object(const int,const int,const double,const std::string&);
  Object(const int,const int,const double,const HeadRule&);

  Object(const Object&);
  Object& operator=(const Object&);
  virtual Object* clone() const;
  virtual ~Object();

  /// Effective typeid
  virtual std::string className() const { return "Object"; }
  /// Visitor Acceptance
  virtual void acceptVisitor(Global::BaseVisit& A) const
    {  A.Accept(*this); }
  /// Accept visitor for input
  virtual void acceptVisitor(Global::BaseModVisit& A)
    { A.Accept(*this); }

  /// set the name
  void setFCUnit(const std::string& FC) { FCUnit=FC; }
  void setName(const int nx) { ObjName=nx; }           ///< Set Name 
  void setCreate(const int lx) { listNum=lx; }         ///< Set Creation point
  void setTemp(const double A) { Tmp=A; }              ///< Set temperature [Kelvin]

  /// access to importance
  void setImp(const double V) { imp.setImp(V); }
  void setImp(const std::string& particle,const double V)
  { imp.setImp(particle,V); }
  void setMagFlag() { activeMag=1; }  ///< implicit mag flag [no field]
  
  int setObject(std::string);
  int setObject(const int,const int,const std::vector<Token>&);
  int procString(const std::string&);
  int procHeadRule(const HeadRule&);

  void setMaterial(const int);  // to be written
  void setMagStep(const double,const double);
  
  int complementaryObject(const int,std::string&);
  int hasComplement() const;                           
  int isPopulated() const { return populated; }        ///< Is populated   

  /// accessor to FCName
  const std::string& getFCUnit() const { return FCUnit; }
  int getName() const  { return ObjName; }             ///< Get Name
  int getCreate() const  { return listNum; }           ///< Get Creation point
  int getMatID() const;
  /// Main accessor to material
  const Material* getMatPtr() const { return matPtr; }
  /// is material void
  bool isVoid() const;
  
  double getTemp() const { return Tmp; }               ///< Get Temperature [K]
  double getDensity() const;                           ///< to be written

  /// acess
  const Importance& getImportance() const { return imp; }
  /// access to importance
  std::tuple<bool,double> getImpPair() const
    { return imp.getAllPair(); }
  /// access to importance
  double getImp(const std::string& particle) const
    { return imp.getImp(particle); }
  double getImp(const int PNum) const
    { return imp.getImp(PNum); }
  double getAllImp() const { return imp.getAllImp(); }
  bool isZeroImp() const { return imp.isZeroImp(); }
  

  /// Return the top rule
  const Rule* topRule() const { return HRule.getTopRule(); }
  /// get head rule 
  const HeadRule& getHeadRule() const { return HRule; }
  
  void populate();
  void rePopulate();
  int createSurfaceList();
  void createLogicOpp();
  int isObjSurfValid() const { return objSurfValid; }  ///< Check validity needed
  void setObjSurfValid()  { objSurfValid=1; }          ///< set as valid
  int addSurfString(const std::string&);
  void addUnion(const HeadRule&);
  void addIntersection(const HeadRule&);
  int removeSurface(const int);        
  int substituteSurf(const int,const int,Geometry::Surface*);  
  void makeComplement();

  bool hasSurface(const int) const;
  bool hasMagField() const { return activeMag; }  ///< Active mag
  /// Steps
  std::pair<double,double> getMagStep() const
    { return std::pair<double,double>(magMinStep,magMaxStep); }  
  int isValid(const Geometry::Vec3D&) const;            
  int isValid(const Geometry::Vec3D&,const int) const;            
  int isDirectionValid(const Geometry::Vec3D&,const int) const;
  int isDirectionValid(const Geometry::Vec3D&,const std::set<int>&,
		       const int) const;            
  int isValid(const Geometry::Vec3D&,const std::set<int>&) const;            
  int pairValid(const int,const Geometry::Vec3D&) const;   
  int isValid(const std::map<int,int>&) const; 
  std::set<int> surfValid(const Geometry::Vec3D&) const;
  std::map<int,int> mapValid(const Geometry::Vec3D&) const;

  
  int isOnSurface(const Geometry::Vec3D&) const;
  int isOnSide(const Geometry::Vec3D&) const;

  int surfSign(const int) const;
  /// Access surface index
  const std::set<int>& getSurfSet() const { return SurSet; }

  std::vector<int> getSurfaceIndex() const;
  /// Access the surface list [of pointers]
  const std::vector<const Geometry::Surface*>& getSurList() const
    { return SurList; }

  std::vector<std::pair<int,int>> getImplicatePairs(const int) const;
  std::vector<std::pair<int,int>> getImplicatePairs() const;
  std::set<int> getSelfPairs() const;
  
  ///\cond ABSTRACT
  virtual void displace(const Geometry::Vec3D&) {}
  virtual void rotate(const Geometry::Matrix<double>&) {}
  virtual void mirror(const Geometry::Plane&) {}
  ///\endcond ABSTRACT

  // INTERSECTION
  int hasIntercept(const Geometry::Vec3D&,const Geometry::Vec3D&) const;

  std::tuple<int,const Geometry::Surface*,Geometry::Vec3D,double>
  trackSurfIntersect(const Geometry::Vec3D&,const Geometry::Vec3D&) const;

  Geometry::Vec3D trackPoint(const Geometry::Vec3D&,
			     const Geometry::Vec3D&) const;
  
  Geometry::Vec3D trackClosestPoint(const Geometry::Vec3D&,
				    const Geometry::Vec3D&,
				    const Geometry::Vec3D&) const;

  int trackSurf(const Geometry::Vec3D&,const Geometry::Vec3D&) const;
  
  int trackCell(const MonteCarlo::particle&,double&,
		const Geometry::Surface*&,
		const int) const;

  int trackDirection(const Geometry::Vec3D&,
		     const Geometry::Vec3D&) const;


  /// acessor to forward 
  Geometry::Vec3D getCofM() const { return COM; }
  // OUTPUT
  std::string cellCompStr() const;
  std::vector<Token> cellVec() const;
  std::string headStr() const;
  std::string str() const;
  std::string pointStr(const Geometry::Vec3D&) const;
  std::string cellStr(const std::map<int,Object*>&) const;
  
  void write(std::ostream&) const;         
  void writePHITS(std::ostream&) const;    
  void writeFLUKA(std::ostream&) const;    
  void writeFLUKAmat(std::ostream&) const;
  void writePOVRay(std::ostream&) const;    

  void checkPointers() const;

};

 std::ostream& operator<<(std::ostream&,const Object&);

}

#endif
