/* soapStub.h
   Generated by gSOAP 2.8.2 from Init.h
   Copyright(C) 2000-2011, Robert van Engelen, Genivia Inc. All Rights Reserved.
   The generated code is released under one of the following licenses:
   GPL OR Genivia's license for commercial use.
*/
#if defined(_SG)
#ifndef soapStub_H
#define soapStub_H
#include <vector>
#define SOAP_NAMESPACE_OF_ns2	"http://tempuri.org/Imports"
#define SOAP_NAMESPACE_OF_ns1	"http://tempuri.org/"
#define SOAP_NAMESPACE_OF_ns3	"http://schemas.microsoft.com/2003/10/Serialization/"
#define SOAP_NAMESPACE_OF_ns4	"http://schemas.datacontract.org/2004/07/CouponService"
#include "../stdsoap2.h"

/******************************************************************************\
 *                                                                            *
 * Enumerations                                                               *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Types with Custom Serializers                                              *
 *                                                                            *
\******************************************************************************/

#ifndef SOAP_TYPE_xsd__QName
#define SOAP_TYPE_xsd__QName (15)
typedef std::string xsd__QName;
#endif


/******************************************************************************\
 *                                                                            *
 * Classes and Structs                                                        *
 *                                                                            *
\******************************************************************************/


#if 0 /* volatile type: do not declare here, declared elsewhere */

#endif

#ifndef SOAP_TYPE_xsd__anyType
#define SOAP_TYPE_xsd__anyType (8)
/* Primitive xsd:anyType schema type: */
class SOAP_CMAC xsd__anyType
{
public:
	char *__item;
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 8; } /* = unique id SOAP_TYPE_xsd__anyType */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         xsd__anyType() { xsd__anyType::soap_default(NULL); }
	virtual ~xsd__anyType() { }
};
#endif

#if 0 /* volatile type: do not declare here, declared elsewhere */

#endif

#ifndef SOAP_TYPE_xsd__ID_
#define SOAP_TYPE_xsd__ID_ (12)
/* Primitive xsd:ID schema type: */
class SOAP_CMAC xsd__ID_ : public xsd__anyType
{
public:
	std::string __item;
public:
	virtual int soap_type() const { return 12; } /* = unique id SOAP_TYPE_xsd__ID_ */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         xsd__ID_() { xsd__ID_::soap_default(NULL); }
	virtual ~xsd__ID_() { }
};
#endif

#ifndef SOAP_TYPE_xsd__IDREF_
#define SOAP_TYPE_xsd__IDREF_ (14)
/* Primitive xsd:IDREF schema type: */
class SOAP_CMAC xsd__IDREF_ : public xsd__anyType
{
public:
	std::string __item;
public:
	virtual int soap_type() const { return 14; } /* = unique id SOAP_TYPE_xsd__IDREF_ */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         xsd__IDREF_() { xsd__IDREF_::soap_default(NULL); }
	virtual ~xsd__IDREF_() { }
};
#endif

#ifndef SOAP_TYPE_xsd__QName_
#define SOAP_TYPE_xsd__QName_ (16)
/* Primitive xsd:QName schema type: */
class SOAP_CMAC xsd__QName_ : public xsd__anyType
{
public:
	xsd__QName __item;
public:
	virtual int soap_type() const { return 16; } /* = unique id SOAP_TYPE_xsd__QName_ */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         xsd__QName_() { xsd__QName_::soap_default(NULL); }
	virtual ~xsd__QName_() { }
};
#endif

#ifndef SOAP_TYPE_xsd__anyURI_
#define SOAP_TYPE_xsd__anyURI_ (18)
/* Primitive xsd:anyURI schema type: */
class SOAP_CMAC xsd__anyURI_ : public xsd__anyType
{
public:
	std::string __item;
public:
	virtual int soap_type() const { return 18; } /* = unique id SOAP_TYPE_xsd__anyURI_ */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         xsd__anyURI_() { xsd__anyURI_::soap_default(NULL); }
	virtual ~xsd__anyURI_() { }
};
#endif

#ifndef SOAP_TYPE_xsd__base64Binary
#define SOAP_TYPE_xsd__base64Binary (19)
/* Base64 schema type: */
class SOAP_CMAC xsd__base64Binary
{
public:
	unsigned char *__ptr;
	int __size;
	char *id;	/* optional element of type xsd:string */
	char *type;	/* optional element of type xsd:string */
	char *options;	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 19; } /* = unique id SOAP_TYPE_xsd__base64Binary */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         xsd__base64Binary() { xsd__base64Binary::soap_default(NULL); }
	virtual ~xsd__base64Binary() { }
};
#endif

#ifndef SOAP_TYPE_xsd__base64Binary_
#define SOAP_TYPE_xsd__base64Binary_ (23)
/* Primitive xsd:base64Binary schema type: */
class SOAP_CMAC xsd__base64Binary_ : public xsd__anyType
{
public:
	xsd__base64Binary __item;
public:
	virtual int soap_type() const { return 23; } /* = unique id SOAP_TYPE_xsd__base64Binary_ */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         xsd__base64Binary_() { xsd__base64Binary_::soap_default(NULL); }
	virtual ~xsd__base64Binary_() { }
};
#endif

#ifndef SOAP_TYPE_xsd__boolean
#define SOAP_TYPE_xsd__boolean (24)
/* Primitive xsd:boolean schema type: */
class SOAP_CMAC xsd__boolean : public xsd__anyType
{
public:
	bool __item;
public:
	virtual int soap_type() const { return 24; } /* = unique id SOAP_TYPE_xsd__boolean */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         xsd__boolean() { xsd__boolean::soap_default(NULL); }
	virtual ~xsd__boolean() { }
};
#endif

#ifndef SOAP_TYPE_xsd__byte_
#define SOAP_TYPE_xsd__byte_ (27)
/* Primitive xsd:byte schema type: */
class SOAP_CMAC xsd__byte_ : public xsd__anyType
{
public:
	char __item;
public:
	virtual int soap_type() const { return 27; } /* = unique id SOAP_TYPE_xsd__byte_ */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         xsd__byte_() { xsd__byte_::soap_default(NULL); }
	virtual ~xsd__byte_() { }
};
#endif

#ifndef SOAP_TYPE_xsd__dateTime
#define SOAP_TYPE_xsd__dateTime (28)
/* Primitive xsd:dateTime schema type: */
class SOAP_CMAC xsd__dateTime : public xsd__anyType
{
public:
	time_t __item;
public:
	virtual int soap_type() const { return 28; } /* = unique id SOAP_TYPE_xsd__dateTime */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         xsd__dateTime() { xsd__dateTime::soap_default(NULL); }
	virtual ~xsd__dateTime() { }
};
#endif

#ifndef SOAP_TYPE_xsd__decimal_
#define SOAP_TYPE_xsd__decimal_ (31)
/* Primitive xsd:decimal schema type: */
class SOAP_CMAC xsd__decimal_ : public xsd__anyType
{
public:
	std::string __item;
public:
	virtual int soap_type() const { return 31; } /* = unique id SOAP_TYPE_xsd__decimal_ */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         xsd__decimal_() { xsd__decimal_::soap_default(NULL); }
	virtual ~xsd__decimal_() { }
};
#endif

#ifndef SOAP_TYPE_xsd__double
#define SOAP_TYPE_xsd__double (32)
/* Primitive xsd:double schema type: */
class SOAP_CMAC xsd__double : public xsd__anyType
{
public:
	double __item;
public:
	virtual int soap_type() const { return 32; } /* = unique id SOAP_TYPE_xsd__double */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         xsd__double() { xsd__double::soap_default(NULL); }
	virtual ~xsd__double() { }
};
#endif

#ifndef SOAP_TYPE_xsd__duration_
#define SOAP_TYPE_xsd__duration_ (35)
/* Primitive xsd:duration schema type: */
class SOAP_CMAC xsd__duration_ : public xsd__anyType
{
public:
	std::string __item;
public:
	virtual int soap_type() const { return 35; } /* = unique id SOAP_TYPE_xsd__duration_ */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         xsd__duration_() { xsd__duration_::soap_default(NULL); }
	virtual ~xsd__duration_() { }
};
#endif

#ifndef SOAP_TYPE_xsd__float
#define SOAP_TYPE_xsd__float (36)
/* Primitive xsd:float schema type: */
class SOAP_CMAC xsd__float : public xsd__anyType
{
public:
	float __item;
public:
	virtual int soap_type() const { return 36; } /* = unique id SOAP_TYPE_xsd__float */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         xsd__float() { xsd__float::soap_default(NULL); }
	virtual ~xsd__float() { }
};
#endif

#ifndef SOAP_TYPE_xsd__int
#define SOAP_TYPE_xsd__int (38)
/* Primitive xsd:int schema type: */
class SOAP_CMAC xsd__int : public xsd__anyType
{
public:
	int __item;
public:
	virtual int soap_type() const { return 38; } /* = unique id SOAP_TYPE_xsd__int */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         xsd__int() { xsd__int::soap_default(NULL); }
	virtual ~xsd__int() { }
};
#endif

#ifndef SOAP_TYPE_xsd__long
#define SOAP_TYPE_xsd__long (39)
/* Primitive xsd:long schema type: */
class SOAP_CMAC xsd__long : public xsd__anyType
{
public:
	LONG64 __item;
public:
	virtual int soap_type() const { return 39; } /* = unique id SOAP_TYPE_xsd__long */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         xsd__long() { xsd__long::soap_default(NULL); }
	virtual ~xsd__long() { }
};
#endif

#ifndef SOAP_TYPE_xsd__short
#define SOAP_TYPE_xsd__short (41)
/* Primitive xsd:short schema type: */
class SOAP_CMAC xsd__short : public xsd__anyType
{
public:
	short __item;
public:
	virtual int soap_type() const { return 41; } /* = unique id SOAP_TYPE_xsd__short */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         xsd__short() { xsd__short::soap_default(NULL); }
	virtual ~xsd__short() { }
};
#endif

#ifndef SOAP_TYPE_xsd__string
#define SOAP_TYPE_xsd__string (43)
/* Primitive xsd:string schema type: */
class SOAP_CMAC xsd__string : public xsd__anyType
{
public:
	std::string __item;
public:
	virtual int soap_type() const { return 43; } /* = unique id SOAP_TYPE_xsd__string */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         xsd__string() { xsd__string::soap_default(NULL); }
	virtual ~xsd__string() { }
};
#endif

#ifndef SOAP_TYPE_xsd__unsignedByte_
#define SOAP_TYPE_xsd__unsignedByte_ (45)
/* Primitive xsd:unsignedByte schema type: */
class SOAP_CMAC xsd__unsignedByte_ : public xsd__anyType
{
public:
	unsigned char __item;
public:
	virtual int soap_type() const { return 45; } /* = unique id SOAP_TYPE_xsd__unsignedByte_ */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         xsd__unsignedByte_() { xsd__unsignedByte_::soap_default(NULL); }
	virtual ~xsd__unsignedByte_() { }
};
#endif

#ifndef SOAP_TYPE_xsd__unsignedInt
#define SOAP_TYPE_xsd__unsignedInt (46)
/* Primitive xsd:unsignedInt schema type: */
class SOAP_CMAC xsd__unsignedInt : public xsd__anyType
{
public:
	unsigned int __item;
public:
	virtual int soap_type() const { return 46; } /* = unique id SOAP_TYPE_xsd__unsignedInt */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         xsd__unsignedInt() { xsd__unsignedInt::soap_default(NULL); }
	virtual ~xsd__unsignedInt() { }
};
#endif

#ifndef SOAP_TYPE_xsd__unsignedLong
#define SOAP_TYPE_xsd__unsignedLong (47)
/* Primitive xsd:unsignedLong schema type: */
class SOAP_CMAC xsd__unsignedLong : public xsd__anyType
{
public:
	ULONG64 __item;
public:
	virtual int soap_type() const { return 47; } /* = unique id SOAP_TYPE_xsd__unsignedLong */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         xsd__unsignedLong() { xsd__unsignedLong::soap_default(NULL); }
	virtual ~xsd__unsignedLong() { }
};
#endif

#ifndef SOAP_TYPE_xsd__unsignedShort
#define SOAP_TYPE_xsd__unsignedShort (49)
/* Primitive xsd:unsignedShort schema type: */
class SOAP_CMAC xsd__unsignedShort : public xsd__anyType
{
public:
	unsigned short __item;
public:
	virtual int soap_type() const { return 49; } /* = unique id SOAP_TYPE_xsd__unsignedShort */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         xsd__unsignedShort() { xsd__unsignedShort::soap_default(NULL); }
	virtual ~xsd__unsignedShort() { }
};
#endif

#ifndef SOAP_TYPE_ns3__char__
#define SOAP_TYPE_ns3__char__ (60)
/* Primitive ns3:char schema type: */
class SOAP_CMAC ns3__char__ : public xsd__anyType
{
public:
	int __item;
public:
	virtual int soap_type() const { return 60; } /* = unique id SOAP_TYPE_ns3__char__ */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns3__char__() { ns3__char__::soap_default(NULL); }
	virtual ~ns3__char__() { }
};
#endif

#ifndef SOAP_TYPE_ns3__duration__
#define SOAP_TYPE_ns3__duration__ (62)
/* Primitive ns3:duration schema type: */
class SOAP_CMAC ns3__duration__ : public xsd__anyType
{
public:
	std::string __item;
public:
	virtual int soap_type() const { return 62; } /* = unique id SOAP_TYPE_ns3__duration__ */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns3__duration__() { ns3__duration__::soap_default(NULL); }
	virtual ~ns3__duration__() { }
};
#endif

#ifndef SOAP_TYPE_ns3__guid__
#define SOAP_TYPE_ns3__guid__ (64)
/* Primitive ns3:guid schema type: */
class SOAP_CMAC ns3__guid__ : public xsd__anyType
{
public:
	std::string __item;
public:
	virtual int soap_type() const { return 64; } /* = unique id SOAP_TYPE_ns3__guid__ */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns3__guid__() { ns3__guid__::soap_default(NULL); }
	virtual ~ns3__guid__() { }
};
#endif

#ifndef SOAP_TYPE__ns1__ScreenCoupon
#define SOAP_TYPE__ns1__ScreenCoupon (51)
/* ns1:ScreenCoupon */
class SOAP_CMAC _ns1__ScreenCoupon
{
public:
	int *gameID;	/* optional element of type xsd:int */
	std::string *code;	/* optional element of type xsd:string */
	LONG64 *charID;	/* optional element of type xsd:long */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 51; } /* = unique id SOAP_TYPE__ns1__ScreenCoupon */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _ns1__ScreenCoupon() { _ns1__ScreenCoupon::soap_default(NULL); }
	virtual ~_ns1__ScreenCoupon() { }
};
#endif

#ifndef SOAP_TYPE__ns1__ScreenCouponResponse
#define SOAP_TYPE__ns1__ScreenCouponResponse (52)
/* ns1:ScreenCouponResponse */
class SOAP_CMAC _ns1__ScreenCouponResponse
{
public:
	class ns4__CouponInfo *ScreenCouponResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type ns4:CouponInfo */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 52; } /* = unique id SOAP_TYPE__ns1__ScreenCouponResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _ns1__ScreenCouponResponse() { _ns1__ScreenCouponResponse::soap_default(NULL); }
	virtual ~_ns1__ScreenCouponResponse() { }
};
#endif

#ifndef SOAP_TYPE__ns1__UpdateCouponStatus
#define SOAP_TYPE__ns1__UpdateCouponStatus (53)
/* ns1:UpdateCouponStatus */
class SOAP_CMAC _ns1__UpdateCouponStatus
{
public:
	int *gameID;	/* optional element of type xsd:int */
	std::string *code;	/* optional element of type xsd:string */
	std::string *cherryID;	/* optional element of type xsd:string */
	LONG64 *characterID;	/* optional element of type xsd:long */
	std::string *characterName;	/* optional element of type xsd:string */
	std::string *IP;	/* optional element of type xsd:string */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 53; } /* = unique id SOAP_TYPE__ns1__UpdateCouponStatus */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _ns1__UpdateCouponStatus() { _ns1__UpdateCouponStatus::soap_default(NULL); }
	virtual ~_ns1__UpdateCouponStatus() { }
};
#endif

#ifndef SOAP_TYPE__ns1__UpdateCouponStatusResponse
#define SOAP_TYPE__ns1__UpdateCouponStatusResponse (54)
/* ns1:UpdateCouponStatusResponse */
class SOAP_CMAC _ns1__UpdateCouponStatusResponse
{
public:
	bool *UpdateCouponStatusResult;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:boolean */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 54; } /* = unique id SOAP_TYPE__ns1__UpdateCouponStatusResponse */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         _ns1__UpdateCouponStatusResponse() { _ns1__UpdateCouponStatusResponse::soap_default(NULL); }
	virtual ~_ns1__UpdateCouponStatusResponse() { }
};
#endif

#ifndef SOAP_TYPE_ns4__CouponInfo
#define SOAP_TYPE_ns4__CouponInfo (57)
/* ns4:CouponInfo */
class SOAP_CMAC ns4__CouponInfo : public xsd__anyType
{
public:
	std::string *ItemDescription;	/* optional element of type xsd:string */
	std::string *ItemID;	/* optional element of type xsd:string */
	int *ItemQty;	/* optional element of type xsd:int */
	int *ItemType;	/* optional element of type xsd:int */
	int *StatusCode;	/* optional element of type xsd:int */
	std::string *StatusDescription;	/* optional element of type xsd:string */
public:
	virtual int soap_type() const { return 57; } /* = unique id SOAP_TYPE_ns4__CouponInfo */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	         ns4__CouponInfo() { ns4__CouponInfo::soap_default(NULL); }
	virtual ~ns4__CouponInfo() { }
};
#endif


#ifndef SOAP_TYPE___ns1__ScreenCoupon
#define SOAP_TYPE___ns1__ScreenCoupon (74)
/* Operation wrapper: */
struct __ns1__ScreenCoupon
{
public:
	_ns1__ScreenCoupon *ns1__ScreenCoupon;	/* optional element of type ns1:ScreenCoupon */
};
#endif

#ifndef SOAP_TYPE___ns1__UpdateCouponStatus
#define SOAP_TYPE___ns1__UpdateCouponStatus (78)
/* Operation wrapper: */
struct __ns1__UpdateCouponStatus
{
public:
	_ns1__UpdateCouponStatus *ns1__UpdateCouponStatus;	/* optional element of type ns1:UpdateCouponStatus */
};
#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Header
#define SOAP_TYPE_SOAP_ENV__Header (83)
/* SOAP Header: */
struct SOAP_ENV__Header
{
#ifdef WITH_NOEMPTYSTRUCT
private:
	char dummy;	/* dummy member to enable compilation */
#endif
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Code
#define SOAP_TYPE_SOAP_ENV__Code (84)
/* SOAP Fault Code: */
struct SOAP_ENV__Code
{
public:
	char *SOAP_ENV__Value;	/* optional element of type xsd:QName */
	struct SOAP_ENV__Code *SOAP_ENV__Subcode;	/* optional element of type SOAP-ENV:Code */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Detail
#define SOAP_TYPE_SOAP_ENV__Detail (86)
/* SOAP-ENV:Detail */
struct SOAP_ENV__Detail
{
public:
	char *__any;
	int __type;	/* any type of element <fault> (defined below) */
	void *fault;	/* transient */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Reason
#define SOAP_TYPE_SOAP_ENV__Reason (89)
/* SOAP-ENV:Reason */
struct SOAP_ENV__Reason
{
public:
	char *SOAP_ENV__Text;	/* optional element of type xsd:string */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Fault
#define SOAP_TYPE_SOAP_ENV__Fault (90)
/* SOAP Fault: */
struct SOAP_ENV__Fault
{
public:
	char *faultcode;	/* optional element of type xsd:QName */
	char *faultstring;	/* optional element of type xsd:string */
	char *faultactor;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *detail;	/* optional element of type SOAP-ENV:Detail */
	struct SOAP_ENV__Code *SOAP_ENV__Code;	/* optional element of type SOAP-ENV:Code */
	struct SOAP_ENV__Reason *SOAP_ENV__Reason;	/* optional element of type SOAP-ENV:Reason */
	char *SOAP_ENV__Node;	/* optional element of type xsd:string */
	char *SOAP_ENV__Role;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *SOAP_ENV__Detail;	/* optional element of type SOAP-ENV:Detail */
};
#endif

#endif

/******************************************************************************\
 *                                                                            *
 * Typedefs                                                                   *
 *                                                                            *
\******************************************************************************/

#ifndef SOAP_TYPE__QName
#define SOAP_TYPE__QName (5)
typedef char *_QName;
#endif

#ifndef SOAP_TYPE__XML
#define SOAP_TYPE__XML (6)
typedef char *_XML;
#endif

#ifndef SOAP_TYPE_xsd__ID
#define SOAP_TYPE_xsd__ID (11)
typedef std::string xsd__ID;
#endif

#ifndef SOAP_TYPE_xsd__IDREF
#define SOAP_TYPE_xsd__IDREF (13)
typedef std::string xsd__IDREF;
#endif

#ifndef SOAP_TYPE_xsd__anyURI
#define SOAP_TYPE_xsd__anyURI (17)
typedef std::string xsd__anyURI;
#endif

#ifndef SOAP_TYPE_xsd__byte
#define SOAP_TYPE_xsd__byte (26)
typedef char xsd__byte;
#endif

#ifndef SOAP_TYPE_xsd__decimal
#define SOAP_TYPE_xsd__decimal (30)
typedef std::string xsd__decimal;
#endif

#ifndef SOAP_TYPE_xsd__duration
#define SOAP_TYPE_xsd__duration (34)
typedef std::string xsd__duration;
#endif

#ifndef SOAP_TYPE_xsd__unsignedByte
#define SOAP_TYPE_xsd__unsignedByte (44)
typedef unsigned char xsd__unsignedByte;
#endif

#ifndef SOAP_TYPE_ns3__char
#define SOAP_TYPE_ns3__char (59)
typedef int ns3__char;
#endif

#ifndef SOAP_TYPE_ns3__duration
#define SOAP_TYPE_ns3__duration (61)
typedef std::string ns3__duration;
#endif

#ifndef SOAP_TYPE_ns3__guid
#define SOAP_TYPE_ns3__guid (63)
typedef std::string ns3__guid;
#endif


/******************************************************************************\
 *                                                                            *
 * Externals                                                                  *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Client-Side Call Stubs                                                     *
 *                                                                            *
\******************************************************************************/


SOAP_FMAC5 int SOAP_FMAC6 soap_call___ns1__ScreenCoupon(struct soap *soap, const char *soap_endpoint, const char *soap_action, _ns1__ScreenCoupon *ns1__ScreenCoupon, _ns1__ScreenCouponResponse *ns1__ScreenCouponResponse);

SOAP_FMAC5 int SOAP_FMAC6 soap_call___ns1__UpdateCouponStatus(struct soap *soap, const char *soap_endpoint, const char *soap_action, _ns1__UpdateCouponStatus *ns1__UpdateCouponStatus, _ns1__UpdateCouponStatusResponse *ns1__UpdateCouponStatusResponse);

#endif

#endif		//#if defined(_SG)
/* End of soapStub.h */
