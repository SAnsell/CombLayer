/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/Object.h
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
namespace Geometry
{
  class Plane;
}
 

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

  std::string FCUnit;      ///< FixedComp name
  
  int ObjName;             ///< Number for the object
  int listNum;             ///< Creation number
  double Tmp;              ///< Starting temperature (if given)
  const Material* matPtr;  ///< Material Number 
  int trcl;                ///< transform number [deprecated]
  Importance imp;          ///< importance / 0 

  int populated;           ///< Full population

  int activeMag;           ///< Mag active [0:None:1:Basic:2:Sync+Mag]
  double magMinStep;       ///< min step for mag field [fluka]
  double magMaxStep;       ///< max step for mag field [fluka]

  bool activeElec;         ///< Electric field active
  double elecMinStep;       ///< min step for mag field [fluka]
  double elecMaxStep;       ///< max step for mag field [fluka]
  
  HeadRule HRule;           ///< Top rule

  Geometry::Vec3D COM;      ///< Centre of mass 
   
  int checkSurfaceValid(const Geometry::Vec3D&,const Geometry::Vec3D&) const;
  /// Calc in/out 
  int calcInOut(const int,const int) const;
  void clearValid();
  
 protected:
  
  int objSurfValid;                 ///< Object surface valid
  /// Full surfaces 
  std::set<const Geometry::Surface*> surfSet;  
  std::set<int> surNameSet;              ///< set of surfaces in cell [signed]

  const Geometry::Surface* getSurf(const int) const;
  
 public:

  Object();
  Object(std::string ,const int,const int,
	 const double,const std::string&);
  Object(std::string ,const int,const int,
	 const double,HeadRule );
  Object(const int,const int,const double,const std::string&);
  Object(const int,const int,const double,HeadRule );

  Object(const Object&);
  Object& operator=(const Object&);
  virtual Object* clone() const;
  virtual ~Object();

  /// set the name
  void setFCUnit(const std::string& FC) { FCUnit=FC; }
  void setName(const int nx) { ObjName=nx; }           ///< Set Name 
  void setCreate(const int lx) { listNum=lx; }         ///< Set Creation point
  void setTemp(const double A) { Tmp=A; }              ///< Set temperature [Kelvin]

  /// access to importance
  void setImp(const double V) { imp.setImp(V); }
  void setImp(const std::string& particle,const double V)
  { imp.setImp(particle,V); }
  void setMagFlag(const bool syncFlag)
    { activeMag=(syncFlag) ? 2 : 1; }    ///< implicit mag flag [no field]
  void setElecFlag() { activeElec=1; }  ///< implicit elec flag [no field]
  
  int setObject(std::string);
  int setObject(const int,const int,const std::vector<Token>&);
  int procString(const std::string&);
  int procHeadRule(const HeadRule&);

  void setMaterial(const int);
  void setMaterial(const std::string&); 
  void setMagStep(const double,const double);
  void setElecStep(const double,const double);
  
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
  bool hasElecField() const { return activeElec; }  ///< Active elec field
  /// Steps
  std::pair<double,double> getMagStep() const
    { return std::pair<double,double>(magMinStep,magMaxStep); }  
  std::pair<double,double> getElecStep() const
    { return std::pair<double,double>(elecMinStep,elecMaxStep); }  

  int isValid(const Geometry::Vec3D&) const;            
  int isValid(const Geometry::Vec3D&,const int) const;
  int isSideValid(const Geometry::Vec3D&,const int) const;            
  int isValid(const std::map<int,int>&) const;
  int isValid(const Geometry::Vec3D&,const std::map<int,int>&) const; 

  
  int isAnyValid(const Geometry::Vec3D&,const std::set<int>&) const;
  
  std::set<int> surfValid(const Geometry::Vec3D&) const;
  std::map<int,int> mapValid(const Geometry::Vec3D&) const;

  
  int isOnSurface(const Geometry::Vec3D&) const;
  std::set<int> isOnSide(const Geometry::Vec3D&) const;

  int surfSign(const int) const;
  /// Access surface set
  const std::set<int>& getSurfSet() const { return surNameSet; }

  std::vector<int> getSurfaceIndex() const;
  /// Access the surface list [of pointers]
  const std::set<const Geometry::Surface*>& getSurfPtrSet() const
    { return surfSet; }

  std::vector<std::pair<int,int>> getImplicatePairs(const int) const;
  std::vector<std::pair<int,int>> getImplicatePairs() const;

  
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

  int trackDirection(const int,
		     const Geometry::Vec3D&,
		     const Geometry::Vec3D&) const;
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
