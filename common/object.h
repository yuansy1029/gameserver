#ifndef __OBJECT_H__
#define __OBJECT_H__

//对象的混合ID
#define MAKE_OBJECT_MIXID(objTag,objID)	(((unsigned int)(objTag) << 24) | (unsigned int)(objID))

//单独取出混合ID
#define RECOVER_OBJECT_MIXID(objTag,objID,mixID)	\
	do	\
	{	\
		(objTag) = (unsigned int)(mixID) >> 24;	\
		(objID) = (unsigned int)(mixID) & 0x00FFFFFF;	\
	} while (0)

typedef unsigned int TObjMixID;
#define INVALID_OBJMIXID	0


#endif