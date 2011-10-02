/* Copyright (c) <2009> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

#ifndef __DRTTI_H__
#define __DRTTI_H__

#include "dCRC.h"

class dRtti
{
	public:
	dRtti(const char* typeName);
	dCRCTYPE GetTypeId() const;
	bool IsTypeID(dCRCTYPE id) const;

	private:
	dCRCTYPE m_typeId;
};

inline dRtti::dRtti(const char* typeName)
{
	m_typeId = dCRC64 (typeName);
}

inline dCRCTYPE dRtti::GetTypeId() const
{
	return m_typeId;
}

inline bool dRtti::IsTypeID (dCRCTYPE id) const
{
	return m_typeId == id;
}



#define dAddRtti(baseClass)							\
private:											\
static dRtti m_rtti; 								\
public:												\
virtual bool IsType (dCRCTYPE  typeId) const		\
{													\
	if (m_rtti.IsTypeID (typeId)) {					\
		return true;								\
	}												\
	return baseClass::IsType (typeId);				\
}													\
													\
virtual dCRCTYPE GetTypeId () const					\
{													\
	return m_rtti.GetTypeId ();						\
}													\
													\
static dCRCTYPE GetRttiType()						\
{													\
	return m_rtti.GetTypeId();						\
}


#define dInitRtti(className)						\
	dRtti className::m_rtti (#className)


#endif