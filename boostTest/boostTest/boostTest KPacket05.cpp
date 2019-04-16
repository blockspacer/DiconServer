#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

enum EPacketType
{
	ECLGS_VERIFY_ACCOUNT_REQ,
	ECLGS_LOGIN,
};

struct KPacketVerifyAccount
{
	std::string         m_login;
	int                 m_id;
};

template <typename Archive>
void serialize(Archive& ar, KPacketVerifyAccount& a, const unsigned int version)
{
	ar & a.m_login;
	ar & a.m_id;
}

struct KPacketLogin
{
	std::string         m_login;
	std::string         m_password;
	int                 m_id;
	int                 m_age;
};

template <typename Archive>
void serialize(Archive& ar, KPacketLogin& a, const unsigned int version)
{
	ar & a.m_login;
	ar & a.m_password;
	ar & a.m_id;
	ar & a.m_age;
}

#pragma pack( push, 1 )
class KPacket;
typedef boost::shared_ptr<KPacket>  KPacketPtr;
class KPacket
{
public:
	template <class T>
	void SetData(unsigned int nSenderUID, unsigned short usPacketId, const T& data);

	unsigned int        m_nSenderUid;
	unsigned short      m_usPacketId;
	std::vector<char>   m_buffer;
};//class KPacket
#pragma pack( pop )

template <class T>
void KPacket::SetData(unsigned int nSenderUID, unsigned short usPacketId, const T& data_)
{
	m_nSenderUid = nSenderUID;
	m_usPacketId = usPacketId;

	std::stringstream   ss;
	boost::archive::text_oarchive oa{ ss };
	oa << data_;

	std::string& str = ss.str();
	m_buffer.reserve(str.size());
	m_buffer.assign(str.begin(), str.end());
}//KPacket::SetData()


template <typename Archive>
void serialize(Archive& ar, KPacket& a, const unsigned int version)
{
	ar & a.m_nSenderUid;
	ar & a.m_usPacketId;
	ar & a.m_buffer;
}//serialize()

template <typename T>
void BufferToPacket(IN std::stringstream& ss_, OUT T& packet_)
{
	boost::archive::text_iarchive ia{ ss_ };
	ia >> packet_;
}//BufferToPacket()

template<typename T>
void PacketToBuffer(IN T& packet_, OUT std::stringstream& ss_)
{
	boost::archive::text_oarchive oa{ ss_ };
	oa << packet_;
}//PacketToBuffer()


void main()
{
	KPacket     packets[2];

	KPacketVerifyAccount    verifyAccount;
	{
		verifyAccount.m_login = "jintaeks";
		verifyAccount.m_id = 48;
	}
	packets[0].SetData(0, ECLGS_VERIFY_ACCOUNT_REQ, verifyAccount);

	KPacketLogin        login;
	{
		login.m_login = "jintaeks\0hello";
		login.m_password = "hello world";
		login.m_id = 99;
		login.m_age = 48;
	}
	packets[1].SetData(0, ECLGS_LOGIN, login);

	// write to archive
	//
	std::stringstream ss;
	PacketToBuffer(IN packets[1], OUT ss);

	// read from archive
	//
	KPacket     packetNew;
	BufferToPacket(IN ss, OUT packetNew);
}//main()
