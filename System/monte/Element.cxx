/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/Element.cxx
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Triple.h"
#include "support.h"
#include "Zaid.h"
#include "IsoTable.h"
#include "Element.h"

namespace MonteCarlo
{
  
//----------------------------------------
//    ABUNDANCE
//----------------------------------------

Abundance::Abundance() : 
  Z(0)
  /*!
    Default Constructor
  */
{}

Abundance::Abundance(const size_t Az) :
  Z(Az)
  /*!
    Constructor
    \param Az :: Sets the Element Z number
  */
{}

Abundance::Abundance(const Abundance& A) :
  Z(A.Z),WList(A.WList),Frac(A.Frac)
  /*!
    Copy Constructor
    \param A :: Object to copy
  */
{}

Abundance&
Abundance::operator=(const Abundance& A)
  /*!
    Copy Constructor
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Z=A.Z;
      WList=A.WList;
      Frac=A.Frac;
    }
  return *this;
}

Abundance::~Abundance()
  /*!
    Standard Destructor
  */
{}

void
Abundance::addIso(const size_t I,const double F)
  /*!
    Adds an isotope to the list
    \param I :: Isotope number
    \param F :: Fraction
  */
{
  WList.push_back(I);
  Frac.push_back(F);
  return;
}

double
Abundance::meanMass() const
  /*!
    Calculate the true, mean mass.
    \return mean atomic mass
  */
{
  double mass=0.0;
  for(size_t i=0;i<WList.size();i++)
    mass+=Frac[i]*IsoTable::Instance().getMass(Z,WList[i]);
  return mass;
}

//----------------------------------------
//    ELEMENT
//----------------------------------------

Element::Element() :
  Nelem(94)
  /*!
    Standard Contructor
  */
{
  populate();
  populateEdge();
  populateIso();
}

Element::~Element()
  /*!
    Destructor
  */
{}

const Element&
Element::Instance()
  /*!
    Singleton constructor
    \return *this;
   */
{
  static Element EI;
  return EI;
}

size_t
Element::elmIon(const std::string& Astr) const
  /*! 
    Calculate the Z and the ion from a string
    of type :: Em2+ 
    \param Astr :: String 
    \retval Z of the element (if recognised)
   */
{

  // check and find length of real element part
  unsigned int i(0);
  for(;i<3 && i<Astr.length() && 
	isalpha(Astr[i]);i++) ;
  
  return this->elm(Astr.substr(0,i));
}

std::pair<size_t,int>
Element::elmIonPair(const std::string& Astr) const
  /*! 
    Calculate the Z and the ion from a string
    of type :: Em2+ 
    \param Astr :: String 
    \retval Z of the element (recognised) : charge
   */
{

  // check and find length of real element part
  std::pair<size_t,int> Out(0,0);
  unsigned int i(0);
  for(;i<3 && i<Astr.length() && 
	isalpha(Astr[i]);i++) ;
  if (!i || i==3)  // failed 
    return Out;

  Out.first=this->elm(Astr.substr(0,i));
  int digit=0;
  int sign(0);
  for(;i<Astr.length();i++)
    {
      if (isdigit(Astr[i]))
	digit=static_cast<int>(Astr[i]-'0');
      else if (Astr[i]=='-')
	sign=-1;
      else if (Astr[i]=='+')
	sign=1;
    }
  if (!digit && sign)
    digit=sign;
  digit*=sign;
  Out.second=digit;

  return Out;
}

size_t
Element::elm(const std::string& Astr) const
  /*! 
    Find element number from a string
    \param Astr :: Element name
    \retval Z of the element (recognised)

   */
{
  ELog::RegMethod RegA("Element","elm");
  std::string Ts;
  size_t i(0);
  for(i=0;i<Astr.length() && !isalpha(Astr[i]);i++) ;
  if (i!=Astr.length())
    {
      Ts=static_cast<char>(toupper(Astr[i]));
      if ((i+1)!=Astr.length() && isalpha(Astr[i+1]))
	Ts+=static_cast<char>(tolower(Astr[i+1]));
      std::map<std::string,size_t>::const_iterator xv;
      xv=Nmap.find(Ts);
      if (xv!=Nmap.end())
	return xv->second;
    }
  
  throw ColErr::InContainerError<std::string>(Astr,"Element not known");
}
  
const std::string&
Element::elmSym(const size_t Z) const
  /*! 
    Determines the element from the Z number
    \param Z :: Atomic number
    \returns The element symbol as a string
  */
{
  if (Z<1 || Z>Nelem)
    throw ColErr::InContainerError<size_t>(Z,"Element number not known");

  return Sym[Z-1];
}

double
Element::Kedge(const size_t Z) const
  /*!
    Returns the K-edge at Z
    \param Z :: Atomic Number
    \returns K-Edge [keV] (or 0.0 on failure)
  */
{
  if (Z<1 || Z>Nelem)
    return 0.0;
  return KEdge[static_cast<size_t>(Z)-1];
}

double
Element::Kedge(const std::string& Symb) const
  /*!
    Returns the K-edge at Element
    \param Symb :: Atomic Symbol
    \returns K-Edge [keV] (or 0.0 on failure)
  */
{
  const size_t Z=elm(Symb);
  if (Z<1 || Z>Nelem)
    return 0.0;
  return KEdge[Z-1];
}

void
Element::addZaid(const Zaid& ZItem,
		 std::vector<size_t>& cZaid,
		 std::vector<double>& cFrac) const
  /*!
    Given a full string zaid, convert to a cinder type
    \param ZItem :: Zaid
    \param cZaid :: Array of current cinder Zaids 
    \param cFrac :: Vector of current cinder 
  */
{
  ELog::RegMethod RegA("Element","addZaid");

  const size_t Z=ZItem.getZ();
  if (!Z) return;    // void Zaid. 
  const size_t ZN=Z-1;
  const double Frac=ZItem.getDensity();
  if (ZItem.getIso()==0)
    {
      for(size_t i=0;i<Isotopes[ZN].WList.size();i++)
        {
	  if (Isotopes[ZN].Frac[i]>0.0)
	    {
	      cZaid.push_back(Z*1000+Isotopes[ZN].WList[i]);
	      cFrac.push_back(Frac*Isotopes[ZN].Frac[i]);
	    }
	}
    }
  else
    {
      // Specific isotope
      cZaid.push_back(ZItem.getZaidNum());
      cFrac.push_back(Frac);
    }
  return;
}        

void
Element::populate()
  /*!
    Initialiser function to help
    create the object's data.
  */
{
  std::string NameData[]= {
    "H" , "He", "Li", "Be", "B" ,"C",
    "N" ,"O" ,"F" ,"Ne", "Na", "Mg",
    "Al", "Si", "P" ,"S" ,"Cl", "Ar",
    "K" ,"Ca", "Sc", "Ti", "V" ,"Cr",
    "Mn", "Fe", "Co", "Ni", "Cu", "Zn",
    "Ga", "Ge", "As", "Se", "Br", "Kr",
    "Rb", "Sr", "Y" ,"Zr", "Nb", "Mo",
    "Tc", "Ru", "Rh", "Pd", "Ag", "Cd",
    "In", "Sn", "Sb", "Te", "I" ,"Xe",
    "Cs", "Ba", "La", "Ce", "Pr", "Nd",
    "Pm", "Sm", "Eu", "Gd", "Tb", "Dy",
    "Ho", "Er", "Tm", "Yb", "Lu", "Hf",
    "Ta", "W" ,"Re", "Os", "Ir", "Pt",
    "Au", "Hg", "Tl", "Pb", "Bi", "Po",
    "At", "Rn", "Fr", "Ra", "Ac", "Th",
    "Pa", "U" ,"Np", "Pu", "Am", "Cm",
    "Bk", "Cf", "Es", "Fm", "Md", "No",
    "Lr", "Rf", "Ha", "Sg", "Bh", "Hs",
    "Mt" };


  for(size_t i=0;i<94;i++)
    {
      Nmap[NameData[i]]=i+1;
      Sym.push_back(NameData[i]);
    }
  
  return;
}

void
Element::populateIso()
  /*!
    Function to populate all the abundancies
    and the isotopes availiable.
  */
{
  // Number of isotopes for each element
  const size_t NIso[]={
    3, 2, 2, 2, 2, 
    3, 3, 4, 2, 3, 
    1, 5, 3, 5, 1, 
    7, 5, 8, 9, 11, 
    5, 7, 8, 9, 8, 
    9, 8, 9, 5, 5, 
    2, 5, 4, 6, 2, 
    6, 2, 4, 8, 10, 
    9, 12, 3, 8, 2, 
    7, 5, 8, 2, 10, 
    2, 8, 2, 10, 3, 
    7, 2, 4, 1, 8, 
    4, 8, 5, 9, 1, 
    7, 4, 6, 8, 7, 
    5, 9, 7, 10, 5, 
    7, 8, 9, 8, 8, 
    17, 17, 15, 12, 5, 
    5, 2, 8, 6, 11, 
    11, 13, 9, 13, 9, 
    9, 7, 6, 6, 5 }; 
  
  const size_t IsoAM[]={
    1, 2, 3,                                     // Hydrogen   
    3, 4,                                        // Helium     
    6, 7,                                        // Lithium    
    7, 9,                                        // Be         
    10, 11,                                      // Boron      
    12, 13, 14,                                  // Carbon     
    14, 15, 16,                                  // Nitrogen   
    16, 17, 18, 19,                              // Oxygen     
    18, 19,                                      // F          
    20, 21, 22,                                  // Ne (10)    
    23,                                          // Na         
    23, 24, 25, 26, 27,                          // Mg (12)        
    26, 27, 28,                                  // Al         
    27, 28, 29, 30, 31,                          // Si         
    31,                                          // P          
    31, 32, 33, 34, 35, 36, 37,                  // S          
    34, 35, 36, 37, 38,                          // Cl         
    36, 37, 38, 39, 40, 41, 42, 43,              // Ar         
    38, 39, 40, 41, 42, 43, 44, 45, 46,          // K          
    39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,  // Ca (20)    
    44, 45, 46, 47, 48,                          // Sc         
    45, 46, 47, 48, 49, 50, 51,                  // Ti (22)      
    47, 48, 49, 50, 51, 52, 53, 54,              // V          
    49, 50, 51, 52, 53, 54, 55, 56, 57,          // Cr         
    51, 52, 53, 54, 55, 56, 57, 58,              // Mn
    53, 54, 55, 56, 57, 58, 59, 60, 61,          // Fe (26)
    57, 58, 59, 60, 61, 62, 63, 64,              // Co
    57, 58, 59, 60, 61, 62, 63, 64, 65,          // Ni (28)
    62, 63, 64, 65, 66,                          // Cu
    64, 66, 67, 68, 70,                          // Zn
    69, 71,                                      // Ga
    70, 72, 73, 74, 76,                          // Ge
    72, 73, 74, 75, 
    74, 76, 77, 78, 80, 82, 
    79, 81, 
    78, 80, 82, 83, 84, 86, 
    85, 87, 
    84, 86, 87, 88, 
    86, 87, 88, 89, 90, 91, 92, 93, 
    88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 
    91, 92, 93, 94, 95, 96, 97, 98, 100, 
    90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 
    97, 98, 99, 
    96, 98, 99, 100, 101, 102, 103, 104, 
    103, 105, 
    102, 104, 105, 106, 107, 108, 110, 
    106, 107, 108, 109, 110, 
    106, 108, 110, 111, 112, 113, 114, 116, 
    113, 115, 
    112, 114, 115, 116, 117, 118, 119, 120, 122, 124, 
    121, 123, 
    120, 122, 123, 124, 125, 126, 128, 130, 
    127, 135, 
    124, 126, 128, 129, 130, 131, 132, 134, 135, 136, 
    133, 134, 135, 
    130, 132, 134, 135, 136, 137, 138, 
    138, 139, 
    136, 138, 140, 142, 
    141, 
    142, 143, 144, 145, 146, 147, 148, 150, 
    145, 147, 148, 149, 
    144, 147, 148, 149, 150, 151, 152, 154, 
    151, 152, 153, 154, 155, 
    150, 151, 152, 154, 155, 156, 157, 158, 160, 
    159, 
    156, 158, 160, 161, 162, 163, 164, 
    163, 164, 165, 166, 
    162, 164, 166, 167, 168, 170, 
    166, 167, 168, 169, 170, 171, 172, 173, 
    168, 170, 171, 172, 173, 174, 176, 
    173, 174, 175, 176, 177, 
    174, 175, 176, 177, 178, 179, 180, 181, 183, 
    179, 180, 181, 182, 183, 184, 186, 
    179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 
    184, 185, 186, 187, 188, 
    184, 186, 187, 188, 189, 190, 192, 
    188, 189, 190, 191, 192, 193, 194, 195, 
    190, 192, 193, 194, 195, 196, 197, 198, 199, 
    193, 194, 195, 196, 197, 198, 199, 200, 
    196, 198, 199, 200, 201, 202, 203, 204, 
    194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 
    197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 214, 
    200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 
    203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 214, 215, 
    206, 208, 209, 210, 215, 
    211, 218, 219, 220, 222, 
    221, 223, 
    220, 221, 222, 223, 224, 225, 226, 227, 
    223, 224, 225, 226, 227, 228, 
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 
    227, 228, 229, 230, 231, 232, 233, 234, 236, 237, 238, 
    228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 
    232, 233, 234, 235, 236, 237, 238, 239, 240, 
    233, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 
    238, 239, 240, 241, 242, 243, 244, 245, 246, 
    241, 242, 243, 244, 245, 246, 247, 248, 249, 
    243, 245, 246, 247, 248, 249, 250, 
    246, 247, 249, 250, 251, 252, 
    249, 250, 251, 252, 253, 254}; 

  double FracV[]={

    99.985000, 0.015000, 0.000000,                                 // Hydrogen   
                                                        
    0.000140, 99.999860,                                           // Helium     

    7.500000, 92.500000,                                           // Lithium                                          
                                                                                  
    0.000000, 100.000000,                                          // Berillium               
                                                                                  
    19.900000, 80.100000,                                          // Boron               
                                                                                  
    98.900000, 1.100000, 0.000000,                                 // Carbon (6)                
                                                                                  
    99.630000, 0.370000, 0.000000,                                 // Nitrogen               
                                                                                  
    99.760000, 0.040000, 0.200000, 0.000000,                       // Oxygen               
                                                                                  
    0.000000, 100.000000,                                          // Florine               
                                                                                  
    90.480000, 0.270000, 9.250000,                                 // Neon  (10)             
                                                                                  
    100.000000,                                                    // Na (11)
                                                                                  
    0.000000, 78.990000, 10.000000, 11.010000, 0.000000,           // Mg (12)
                                                                        
    0.000000, 100.000000, 0.000000,                                // Al (13)
    
    0.000000, 92.230000, 4.670000, 3.100000, 0.000000,             // Si (14)
 
    100.000000,                                                                         // P

    0.000000, 95.020000, 0.750000, 4.210000, 0.000000, 0.020000, 0.000000,              // S

    0.000000, 75.770000, 0.000000, 24.230000, 0.000000,                                 // Cl

    0.337000, 0.000000, 0.063000, 0.000000, 99.600000, 0.000000, 0.000000, 0.000000,    // Ar
    
    0.000000, 93.258100, 0.011700, 6.730200, 0.000000, 0.000000, 0.000000, 0.000000,    // K
    0.000000, 

    0.000000, 96.941000, 0.000000, 0.647000, 0.135000, 2.086000, 0.000000, 0.004000,    // Ca (20)
    0.000000, 0.187000, 0.000000, 
    
    0.000000, 100.000000, 0.000000, 0.000000, 0.000000,                                 // Sc
    
    0.000000, 8.000000, 7.300000, 73.800000, 5.500000, 5.400000, 0.000000,              // Ti (22)
    
    0.000000, 0.000000, 0.000000, 0.250000, 99.750000, 0.000000, 0.000000, 0.000000,    // V
    
    0.000000, 4.350000, 0.000000, 83.790000, 9.500000, 2.360000, 0.000000, 0.000000,    // Cr
    0.000000, 
    
    0.000000, 0.000000, 0.000000, 0.000000, 100.000000, 0.000000, 0.000000, 0.000000,   // Mn
    
    0.000000, 5.820000, 0.000000, 91.800000, 2.100000, 0.280000, 0.000000, 0.000000,    // Fe
    0.000000, 

    0.000000, 0.000000, 100.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000,   // Co
    
    0.000000, 68.270000, 0.000000, 26.100000, 1.130000, 3.590000, 0.000000, 0.910000,   // Ni
    0.000000, 
    
    0.000000, 69.170000, 0.000000, 30.830000, 0.000000,                                 // Cu
    
    48.600000, 27.900000, 4.100000, 18.800000, 0.600000,                                // Zn (30)

    60.110000, 39.890000,                                                               // Ga 
    
    20.500000, 27.400000, 7.800000, 36.500000, 7.800000, 
    
    0.000000, 0.000000, 0.000000, 100.000000, 
    
    0.900000, 9.200000, 7.600000, 23.700000, 49.800000, 8.800000, 
    
    50.690000, 49.310000, 
    
    0.350000, 2.250000, 11.600000, 11.500000, 57.000000, 17.300000, 
    
    72.170000, 27.830000, 
    
    0.560000, 9.860000, 7.000000, 82.580000, 
    
    0.000000, 0.000000, 0.000000, 100.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    
    0.000000, 0.000000, 51.450000, 11.220000, 17.150000, 0.000000, 17.380000, 0.000000, 
    2.800000, 0.000000, 
    
    0.000000, 0.000000, 100.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    0.000000, 
    
    0.000000, 0.000000, 14.840000, 0.000000, 9.250000, 15.920000, 16.680000, 9.550000, 
    24.130000, 0.000000, 9.630000, 0.000000, 
    
    0.000000, 0.000000, 0.000000, 
    
    5.530000, 1.870000, 12.700000, 12.600000, 17.100000, 31.600000, 0.000000, 18.600000, 
    
    100.000000, 0.000000, 
    
    1.020000, 11.140000, 22.330000, 27.330000, 0.000000, 26.460000, 11.720000, 
    
    0.000000, 51.840000, 0.000000, 48.160000, 0.000000, 
    
    1.250000, 0.890000, 12.490000, 12.800000, 24.130000, 12.220000, 28.730000, 7.490000, 
    
    4.300000, 95.700000, 
    
    0.970000, 0.650000, 0.360000, 14.530000, 7.680000, 24.220000, 8.580000, 32.590000, 
    4.630000, 5.790000, 
    
    57.300000, 42.700000, 
    
    0.090000, 2.570000, 0.890000, 4.760000, 7.100000, 18.890000, 31.730000, 33.970000, 
    
    100.000000, 0.000000, 
    
    0.100000, 0.090000, 1.910000, 26.400000, 4.100000, 21.200000, 26.900000, 10.400000, 
    0.000000, 8.900000, 
    
    100.000000, 0.000000, 0.000000, 
    
    0.110000, 0.100000, 2.420000, 6.590000, 7.850000, 11.230000, 71.700000, 
    
    0.090000, 99.910000, 
    
    0.190000, 0.250000, 88.480000, 11.080000, 
    
    100.000000, 
    
    27.130000, 12.180000, 23.800000, 8.300000, 17.190000, 0.000000, 5.760000, 5.640000, 
    
    0.000000, 0.000000, 0.000000, 0.000000, 
    
    3.100000, 15.000000, 11.300000, 13.800000, 7.400000, 0.000000, 26.700000, 22.700000, 
    
    47.800000, 0.000000, 52.200000, 0.000000, 0.000000, 
    
    0.000000, 0.000000, 0.200000, 2.180000, 14.800000, 20.470000, 15.650000, 24.840000, 
    21.860000, 
    
    100.000000, 
    
    0.060000, 0.100000, 2.340000, 18.900000, 25.500000, 24.900000, 28.200000, 
    
    0.000000, 0.000000, 100.000000, 0.000000, 
    
    0.140000, 1.610000, 33.600000, 22.950000, 26.800000, 14.900000, 
    
    0.000000, 0.000000, 0.000000, 100.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
           
    0.130000, 3.050000, 14.300000, 21.900000, 16.120000, 31.800000, 12.700000, 
    
    0.000000, 0.000000, 97.410000, 2.590000, 0.000000, 
    
    0.160000, 0.000000, 5.210000, 18.600000, 27.300000, 13.630000, 35.100000, 0.000000, 
    0.000000, 
    
    0.000000, 0.012000, 99.988000, 0.000000, 0.000000, 0.000000, 0.000000, 

    0.000000, 0.120000, 0.000000, 26.300000, 14.280000, 30.700000, 0.000000, 28.600000, 
    0.000000, 0.000000, 
    
    0.000000, 37.400000, 0.000000, 62.600000, 0.000000, 
    
    0.020000, 1.580000, 1.600000, 13.300000, 16.100000, 26.400000, 41.000000, 
    
    0.000000, 0.000000, 0.000000, 37.300000, 0.000000, 62.700000, 0.000000, 0.000000, 
           
    0.010000, 0.790000, 0.000000, 32.900000, 33.800000, 25.300000, 0.000000, 7.200000, 
    0.000000, 

    0.000000, 0.000000, 0.000000, 0.000000, 100.000000, 0.000000, 0.000000, 0.000000, 
           
    0.150000, 10.100000, 17.000000, 23.100000, 13.200000, 29.650000, 0.000000, 6.800000, 
    
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
           0.000000, 29.520000, 0.000000, 70.480000, 0.000000, 0.000000, 0.000000, 0.000000, 
    0.000000, 
    
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.400000, 
    0.000000, 24.100000, 22.100000, 52.400000, 0.000000, 0.000000, 0.000000, 0.000000, 
    0.000000, 

    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    0.000000, 100.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    0.000000, 0.000000, 0.000000, 0.000000, 
    
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 

    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    
    0.000000, 0.000000, 
    
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
           
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    100.000000, 0.000000, 0.000000, 
    
           0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    0.000000, 0.000000, 0.000000, 
    
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.005500, 0.720000, 
    0.000000, 0.000000, 99.274500, 0.000000, 0.000000, 

    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    0.000000, 
    
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 

    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    0.000000, 
    
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    0.000000, 

    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    
    0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 
    0.000000 }; 

  Isotopes.resize(100);
  size_t cnt(0);
  for(size_t i=0;i<100;i++)
    {
      Abundance A(i+1);
      for(size_t j=0;j<NIso[i];j++)
        {
          A.addIso(IsoAM[cnt],FracV[cnt]/100.0);
          cnt++;
        }

      Isotopes[i]=A;
    }
  return;
}



void
Element::populateEdge()
  /*!
    Initialiser function to help
    create the object's data (Energy Levels)
  */
{
  KEdge={
   0.140000E-01,   0.250000E-01,  0.550000E-01,
   0.112000E+00,   0.188000E+00,  0.284000E+00,
   0.402000E+00,   0.537000E+00,  0.686000E+00,
   0.867000E+00,   0.107200E+01,  0.130500E+01,
   0.156000E+01,   0.183900E+01,  0.214900E+01,
   0.247200E+01,   0.282200E+01,  0.320200E+01,
   0.360700E+01,   0.403800E+01,  0.449300E+01,
   0.496500E+01,   0.546500E+01,  0.598900E+01,
   0.654000E+01,   0.711200E+01,  0.770900E+01,
   0.833300E+01,   0.897900E+01,  0.965900E+01,
   0.103670E+02,   0.111040E+02,  0.118680E+02,
   0.126580E+02,   0.134740E+02,  0.143220E+02,
   0.152000E+02,   0.161050E+02,  0.170800E+02,
   0.179980E+02,   0.189860E+02,  0.199990E+02,
   0.210450E+02,   0.221170E+02,  0.232200E+02,
   0.243500E+02,   0.255140E+02,  0.267110E+02,
   0.279400E+02,   0.292000E+02,  0.304910E+02,
   0.318130E+02,   0.331690E+02,  0.345820E+02,
   0.359850E+02,   0.374410E+02,  0.389250E+02,
   0.404440E+02,   0.419910E+02,  0.435690E+02,
   0.451840E+02,   0.468350E+02,  0.485200E+02,
   0.502400E+02,   0.519960E+02,  0.537890E+02,
   0.556180E+02,   0.574860E+02,  0.593900E+02,
   0.613320E+02,   0.633140E+02,  0.653510E+02,
   0.674140E+02,   0.695240E+02,  0.716760E+02,
   0.738720E+02,   0.761120E+02,  0.783950E+02,
   0.807230E+02,   0.831030E+02,  0.855280E+02,
   0.880060E+02,   0.905270E+02,  0.000000E+00,
   0.000000E+00,   0.984170E+02,  0.000000E+00,
   0.000000E+00,   0.000000E+00,  0.109649E+03,
   0.000000E+00,   0.115603E+03,  0.000000E+00,
   0.121760E+03
  };
  return;
}

double
Element::mass(const size_t Z) const
  /*!
    Mean atomic mass for a natural abundance 
    element
    \param Z :: Z number
    \return atomic weight
   */
{
  return (Z<1 || Z>94) ? 0.0 : 
    Isotopes[Z-1].meanMass();
}

size_t
Element::natIsotopes(const size_t Z,std::vector<size_t>& AVec,
		     std::vector<double>& FVec) const
  /*!
    Access the set of natural isotopes
    \param Z :: Z number
    \param AVec :: Atomic masses
    \param FVec :: Atomic fractions
    \return number of isotopes
  */
{
  if (Z<1 || Z>94) 
    return 0;

  const Abundance& AB=Isotopes[Z-1];
  AVec=AB.WList;
  FVec=AB.Frac;
  return AB.WList.size(); 
}

}  // NAMESPACE MonteCarlo
