#pragma once

#ifndef _HEADER_CLNT_H_
#define _HEADER_CLNT_H_

#include <WinSock2.h>
#include "header_common.h"
#include "packet.h"
#include "PT_CS_Data.h"
#include "PT_SC_Data.h"



/************************************************************************
 * class pre-declarations
 *
 ************************************************************************/
class StateContext;



/************************************************************************
 * ConnectInfo class
 *
 ************************************************************************/
class ConnectInfo
{
public:
	static ConnectInfo& Instance();	//rev
	
	// accessor
	const WSADATA& get_wsaData() const;
	const SOCKET& get_sock() const;
	const sockaddr& get_servaddr() const;
	
	WSADATA& get_wsaData();
	SOCKET& get_sock();
	sockaddr& get_servaddr();
public:

private:
	ConnectInfo();
	ConnectInfo& operator=(const ConnectInfo&);

private:
	static ConnectInfo _instance;
	WSADATA _wsaData;
	SOCKET _sock;
	sockaddr _servAddr;
};



/************************************************************************
 * StateMachine class
	- abstract base class of all other 'state' class of state machine.
 ************************************************************************/
class State : public MachObject
{
public:
	virtual void run() = 0;
	
public:

protected:
	State();

	void _changeState(State * pNextState);
	virtual void _clear() = 0;	// State 초기화. 초기값, 데이터 등.

protected:
	bool _terminateFlag;	
};



/************************************************************************
 * InitState class
	- State loop 들어가기 전에 TCP 연결 및 자료구조 등 초기화하는 state
 *
 ************************************************************************/
class InitState : public State
{
public:
	static InitState& Instance();

	virtual void run();
public:

protected:
	InitState();
	InitState& operator=(const InitState&);	// prohibit object copying

	virtual void _clear();
protected:
	static InitState _instance;
};



/************************************************************************
 * LoginState class
	- Login 과 관련된 프로세스를 담당하는 state
 *
 ************************************************************************/
class LoginState : public State
{
public:	
	static LoginState& Instance();

	virtual void run();	//rev
public:

protected:
	LoginState();
	LoginState& operator=(const LoginState&);	// prohibit object copying.

	virtual void _clear();
protected:
	static LoginState _instance;
};



/************************************************************************
 * LobbyState class
	- 
 *
 ************************************************************************/
class LobbyState : public State
{
public:
	static LobbyState& Instance();

	virtual void run();	//rev
public:

protected:
	LobbyState();
	LobbyState& operator=(const LobbyState&);	// prohibit object copying.

	virtual void _clear();
protected:
	static LobbyState _instance;
	std::vector<RoomInfoToken> _roomList;
};



/************************************************************************
 * CreateRoomState class
	- 
 *
 ************************************************************************/
class CreateRoomState : public State
{
public:
	static CreateRoomState& Instance();

	virtual void run();	//rev
public:

protected:
	CreateRoomState();
	CreateRoomState& operator=(const CreateRoomState&);

	virtual void _clear();
protected:
	static CreateRoomState _instance;

};



/************************************************************************
 * //class name
 *
 ************************************************************************/
class ChatState : public State
{
public:
	static ChatState& Instance();
	
	virtual void run();	//rev
public:

protected:
	ChatState();	//rev
	ChatState& operator=(const ChatState&);

	virtual void _clear();
protected:
	static ChatState _instance;
	static std::vector<UserInfoToken> _peerList;

};



/************************************************************************
 * ExitState class
	- 프로그램 종료 전 TCP 연결 해제 등 각종 wrap up process를 진행하는 state
 *
 ************************************************************************/
class ExitState :State
{
public:
	static ExitState& Instance();	//rev

	virtual void run();
public:

protected:
	ExitState();
	ExitState& operator=(const ExitState&);	// prohibit object copying.

	virtual void _clear();
protected:
	static ExitState _instance;
};



/************************************************************************
 * StateContext
	- context of state pattern
 ************************************************************************/
class StateContext : public MachObject
{
public:
	static StateContext& Instance();

	void run();
	
	// accessor
	const UserInfoToken& get_myInfo() const;
	const State * get_pCurState() const;
	UserInfoToken& get_myInfo();
	// mutator
	void set_myInfo(UserInfoToken&&);
	void set_pCurState(State * pCurState);
public:
protected:
	StateContext();

	void changeState(State&);
protected:
	UserInfoToken _myInfo;	// client's inform
	State * _pCurState;		// state context (current state)
private:
	static StateContext _instance;
	friend State;
};



/************************************************************************
* ClntPacketManager class
- Singleton pattern.
*
************************************************************************/
class ClntPacketManager : public PacketManager_Base
{
public:
	/* Member method */
	static ClntPacketManager& Instance();

	void sendPacket(std::shared_ptr<Packet_Base> spPk);// send packet via network.
	std::shared_ptr<Packet_Base> recvPacket();	// receive packet in outgoing queue via network.
public:
	/* Member field */
protected:
	/* Member method */
protected:
	/* Member field */
	static ClntPacketManager _instance;
};



/************************************************************************
 * etc functions declarations
 * 
 ************************************************************************/

void init();	//rev
void close();
std::string& stringCheck(std::string& str);



#endif // !_HEADER_CLNT_H_