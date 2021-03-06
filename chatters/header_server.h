#pragma once

#include <memory>
#include <WinSock2.h>
#include <process.h>
#include <atomic>

//#include "packet.h"
//#include "header_common.h"
#include "DBConnector.h"
#include "PT_CS_Data.h"
#include "PT_SC_Data.h"


using std::cin;
using std::cout;
using std::endl;



class UserInfoToken;



/*********************************************************************
 * PerIoData class
	- TCP IOCP 전송에서 I/O 버퍼와 관련된 클래스
 *********************************************************************/
// do not allocate object of PerIoData type using malloc(). Use only new op.
typedef struct PerIoData
{
public:
	enum : int {
		READ_HEADER = 0,
		READ_PACKET,
		WRITE
	};
public:
	PerIoData();
	PerIoData(size_t bufSz);
	~PerIoData();

	void set_refCount(int newVal);
	int get_refCount() const;
	void allocBuffer(size_t bufSz);
	void set_Buffer(char * bufPtr, int bufSz);
	char * get_buffer() const;
	size_t get_bufferLen() const;

	void operator delete(void * p);
public:
	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	int rwMode;		// read mode / write mode distinguisher
private:
	void _releaseBuffer();
private:
	char * _buffer;
	size_t _bufferLen;
	std::atomic<int> _refCount;
} PER_IO_DATA, *LPPER_IO_DATA;



/*********************************************************************
 * SvConInfo class
	- Server의 연결관련 정보가 저장하는 클래스
 *********************************************************************/
class SvConInfo
{
public:
	/* Member method */
	SvConInfo();	//rev 그외 constructor 구현
	SvConInfo(WSADATA newWsaData, SOCKET newSock, sockaddr newServAddr);
public:
	/* Member field */
	WSADATA wsaData;
	SOCKET sock;
	sockaddr servAddr;
private:
	/* Member method */
private:
	/* Member field */
};

/*********************************************************************
 * SvUserInfo class
	- server에 접속 중인 client의 user information을 저장하는  클래스
 *********************************************************************/
class SvUserInfo
{
public:
	/* Member method */
	SvUserInfo(const std::string& id, SOCKET socket);
	SvUserInfo(const SvUserInfo& ui);
	SvUserInfo(SvUserInfo&& ui);
	~SvUserInfo();

	SvUserInfo& operator= (SvUserInfo&& ui);
	SvUserInfo& operator= (const SvUserInfo& ui);

	// accessor
	SOCKET get_socket() const;
	
	// mutator
	void set_socket(SOCKET socket);
public:
	/* Member field */
	UserInfoToken utk;	// user 정보를 담고 있는 token
	int curRmNum;		// 현재 참여 중인 채팅방. 없을 시 -1
private:
	/* Member method */
	SvUserInfo();
private:
	/* Member field */
	SOCKET _socket;
};

class SvUserInfoManager
{
public:
	/* Member method */
	SvUserInfoManager();
	~SvUserInfoManager();

	std::shared_ptr<SvUserInfo> add(std::string id, SOCKET sock);
	std::shared_ptr<SvUserInfo> remove(UserKey uKey);
	std::shared_ptr<SvUserInfo> find(UserKey uKey);

	// accessor
	const std::unordered_map<UserKey, std::shared_ptr<SvUserInfo>> & get() const;
public:
	/* Member field */

protected:
	/* Member method */
	SvUserInfoManager(const SvUserInfoManager&);
	SvUserInfoManager(SvUserInfoManager&&);
	SvUserInfoManager& operator=(const SvUserInfoManager&);
	SvUserInfoManager& operator=(SvUserInfoManager&&);

	void clear();
protected:
	/* Member field */
	std::unordered_map<UserKey, std::shared_ptr<SvUserInfo>> _userList;
};

/*********************************************************************
 * SvRoomInfo class
	- 현재 생성되어 있는 채팅방 관련 정보를 저장하는 클래스

 *********************************************************************/
class SvRoomInfo
{
public:
	/* Member method */
	SvRoomInfo(const std::string& title);

	bool addUser(UserKey uKey);
	bool removeUser(UserKey uKey);
public:
	/* Member field */
	RoomInfoToken rtk;
	std::vector<UserKey> userList;

private:
	/* Member method */
	SvRoomInfo();
private:
	/* Member field */
};

class SvRoomInfoManager
{
public:
	/* Member method */
	SvRoomInfoManager();
	~SvRoomInfoManager();

	std::shared_ptr<SvRoomInfo> add(std::string title);
	std::shared_ptr<SvRoomInfo> remove(RoomKey rmKey);
	std::shared_ptr<SvRoomInfo> find(RoomKey rmKey);

	// assessor
	const std::unordered_map<RoomKey, std::shared_ptr<SvRoomInfo>>& get() const;
public:
	/* Member field */

protected:
	/* Member method */
	SvRoomInfoManager(const SvRoomInfoManager&);
	SvRoomInfoManager(SvRoomInfoManager&&);
	SvRoomInfoManager& operator=(const SvRoomInfoManager&);
	SvRoomInfoManager& operator=(SvRoomInfoManager&&);

	void clear();
protected:
	/* Member field */
	std::unordered_map<RoomKey, std::shared_ptr<SvRoomInfo>> _roomList;
};

class SvSocketManager
{
public:
	/* Member method */
	SvSocketManager();
	~SvSocketManager();
	
	bool entry(SOCKET sock, UserKey uKey);
	UserKey remove(SOCKET sock);
	UserKey find(SOCKET sock);
public:
	/* Member field */

protected:
	/* Member method */
	SvSocketManager(const SvSocketManager&);
	SvSocketManager(SvSocketManager&&);
	SvSocketManager& operator=(const SvSocketManager&);
	SvSocketManager& operator=(SvSocketManager&&);

	void clear();
protected:
	/* Member field */
	std::unordered_map<SOCKET, UserKey> _socketList;
};

/*********************************************************************
 * SvRoomInfo class
	- 현재 생성되어 있는 채팅방 관련 정보를 저장하는 클래스

 *********************************************************************/
class SvMach : public MachObject
{
public:
	/* Member method */
	SvMach();
	~SvMach();

	bool db_signin(const std::string& id, const std::string& pw);
	bool db_signup(const std::string& id, const std::string& pw);
	UserKey addUser(const std::string& id, SOCKET socket);	// add new user to user info list
	bool removeUser(UserKey uKey);				// remove user from user info list
	bool removeUser(SOCKET socket);
	bool joinRoom(RoomKey rmKey, UserKey uKey);	// user join in the chatting room
	bool leaveRoom(RoomKey rmKey, UserKey uKey);	// user leaves the chatting room
	RoomKey openRoom(UserKey uKey, const std::string& title);		// add new chatting room in chatting room list
	//std::vector<UserKey> dismissRoom(RoomKey rmKey);	// owner of a chatting room dismiss the room
	
	const std::shared_ptr<SvUserInfo> findUser(UserKey uKey);
	const std::shared_ptr<SvRoomInfo> findRoom(RoomKey rmKey);

	// accessor
	const std::unordered_map<UserKey, std::shared_ptr<SvUserInfo>>& get_users() const;
	const std::unordered_map<RoomKey, std::shared_ptr<SvRoomInfo>>& get_rooms() const;
public:
	/* Member field */

private:
	/* Member method */
	SvMach(const SvMach & mach);
private:
	/* Member field */
	DBConnector _dbc;
	SvUserInfoManager _users;
	SvRoomInfoManager _rooms;
	SvSocketManager _sockets;
};



/************************************************************************
 * SvPacketManager class
	- Singleton pattern.
 *
************************************************************************/
class SvPacketManager : public PacketManager_Base
{
public:
	/* Member method */
	static SvPacketManager& Instance();

	void sendPacket(std::shared_ptr<Packet_Base> spPk);// transmit packet via network.
	std::shared_ptr<Packet_Base> recvPacket();	// get packet from incoming packet queue.
public:
	/* Member field */
protected:
	/* Member method */
	SvPacketManager();
	SvPacketManager(const SvPacketManager&);
	SvPacketManager(SvPacketManager&&);

	SvPacketManager& operator=(const SvPacketManager&);
protected:
	/* Member field */
	static SvPacketManager _instance;
	std::queue<std::shared_ptr<Packet_Base>> _msgQueue;	// incoming packet queue

	friend DWORD WINAPI recvThreadMain(LPVOID);
	friend DWORD WINAPI packetProcessWorkerThreadMain(LPVOID);
};



/*********************************************************************
 * etc. functions
	- 기타 함수 선언

*********************************************************************/
DWORD WINAPI recvThreadMain(LPVOID pComPort);
DWORD WINAPI packetProcessWorkerThreadMain(LPVOID pComPort);
void ErrorHandling(char * mesaage);

