#include "header_server.h"

/* SvConInfo class definition */
SvConInfo::SvConInfo()
{
}
SvConInfo::SvConInfo(WSADATA newWsaData, SOCKET newSock, sockaddr newServAddr)
{
	wsaData = newWsaData;
	sock = newSock;
	servAddr = newServAddr;
}

/* SvUserInfo class definition */
SvUserInfo::SvUserInfo(const std::string & id, std::shared_ptr<HandleData> hData) 
	: utk(id), 
	curRmNum(CHATTERS::NO_ROOM),
	spHData(hData)
{
	// left blank intentionally
}
SvUserInfo::SvUserInfo(const SvUserInfo & ui)
{
	*this = ui;
}
SvUserInfo::SvUserInfo(SvUserInfo && ui)
{
	*this = std::move(ui);
}
SvUserInfo & SvUserInfo::operator=(SvUserInfo && ui)
{
	if (this != &ui)
	{
		utk = std::move(ui.utk);
		curRmNum = ui.curRmNum;
		spHData = ui.spHData;
	}

	return *this;
}
SvUserInfo & SvUserInfo::operator=(const SvUserInfo & ui)
{
	if (this != &ui)
	{
		utk = ui.utk;
		curRmNum = ui.curRmNum;
		spHData = ui.spHData;
	}

	return *this;
}

/* SvRoomInfo definition */
SvRoomInfo::SvRoomInfo(const std::string & title) : rtk(title)
{
	userList.reserve(CHATTERS::MAX_PARTICIPANT);
}
bool SvRoomInfo::addUser(UserKey uKey)
{
	if (userList.size() < CHATTERS::MAX_PARTICIPANT) {
		userList.push_back(uKey);
		return true;
	}
	return false;
}
bool SvRoomInfo::removeUser(UserKey uKey)
{
	for (auto & it = userList.begin(); it != userList.cend(); ++it)
	{
		if (*it == uKey) {
			userList.erase(it);
			return true;
		}
	}

	return false;
}
SvRoomInfo::SvRoomInfo()
{
	std::cout << "SvRoomInfo() called." << std::endl;
}



/* SvMach class definitions */
bool SvMach::addUser(const std::string& id, std::shared_ptr<HandleData> hData)
{
	SvUserInfo user(id, hData);

	return (_uList.insert(std::make_pair(user.utk.get_key(), std::move(user)))).second;
}
bool SvMach::removeUser(UserKey uKey)
{
	size_t result = _uList.erase(uKey);

	if (result == 0)
		return false;

	return true;
}
bool SvMach::joinRoom(RoomKey rKey, UserKey uKey)
{
	auto rmIter = findRoom(rKey);

	if (rmIter == _rList.end()) {
		std::cout << "Cannot find Room(#" << rKey << ")" << endl;
		return false;
	}

	bool joinRoomResult = rmIter->second.addUser(uKey);
	bool updateUserInfoResult;
	
	if (!joinRoomResult)// fail to join room
		return false;
	else				// succeed to join room
		updateUserInfoResult = updateUserInfo(uKey, rKey);	// update user informaion(room#)
		
	if (!updateUserInfoResult)	// fail to update user information(room#)
	{
		rmIter->second.removeUser(uKey);// rewind changes
		return false;
	}
	else
		return true;
}
bool SvMach::leaveRoom(RoomKey rKey, UserKey uKey)
{
	auto rmIter = findRoom(rKey);

	if (rmIter == _rList.end()) {
		std::cout << "Cannot find Room(#" << rKey << ")" << endl;
		return false;
	}
	
	bool leaveRoomResult = rmIter->second.removeUser(uKey);
	bool updateUserInfoResult;

	if (!leaveRoomResult)	// fail to leave room
		return false;
	else					// succeed to leave room
		updateUserInfoResult = updateUserInfo(uKey, CHATTERS::NO_ROOM);	// update user informaion(room#)
		
	if (!updateUserInfoResult)	// fail to update user information(room#)
	{
		rmIter->second.addUser(uKey);// rewind changes
		return false;
	}
	else
		return true;
}
bool SvMach::openRoom(UserKey uKey, const std::string & title)
{
	SvRoomInfo rm(title);

	auto rmIter = _rList.insert(std::make_pair(rm.rtk.get_key(), std::move(rm)));
	bool opResult = rmIter.second;

	if (!opResult)	// fail to add room to room list
	{
		std::cout << "Create room failed." << std::endl;
		return false;
	}

	auto rmKey = rmIter.first->second.rtk.get_key();
	bool joinResult = joinRoom(rmKey, uKey);

	if (!joinResult)	// fail to enter the room
	{
		_rList.erase(rmKey);	// rewind changes
		return false;
	}
	
	return true;
}
//std::vector<UserKey> SvMach::closeRoom(RoomKey rKey)
//{
//	auto it = _rList.find(rKey);
//	auto target = _rList.erase(it);
//
//	//target	// check
//	std::vector<UserKey> rtnVec;
//	std::swap(it->second.userList, rtnVec);
//
//	return rtnVec;
//}
bool SvMach::removeRoom(RoomKey rKey)
{
	auto removed = _rList.erase(rKey);
	if (removed == 0) {
		return false;
	}

	return true;
}
bool SvMach::updateUserInfo(UserKey uKey, RoomKey rmKey)
{
	auto uiIter = findUser(uKey);

	if (uiIter == _uList.end())	// fail to update user information operation
	{
		std::cout << "Cannot find user(user#" << uKey << ")" << std::endl;
		return false;
	}

	uiIter->second.curRmNum = rmKey;
	return true;
}
std::unordered_map<UserKey, SvUserInfo>::const_iterator SvMach::findUser(UserKey uKey) const
{
	return _uList.find(uKey);
}
std::unordered_map<RoomKey, SvRoomInfo>::const_iterator SvMach::findRoom(RoomKey rKey) const
{
	return _rList.find(rKey);
}
std::unordered_map<UserKey, SvUserInfo>::iterator SvMach::findUser(UserKey uKey)
{
	return _uList.find(uKey);
}
std::unordered_map<RoomKey, SvRoomInfo>::iterator SvMach::findRoom(RoomKey rKey)
{
	return _rList.find(rKey);
}
const std::unordered_map<UserKey, SvUserInfo>& SvMach::get_userList() const
{
	return _uList;
}
const std::unordered_map<RoomKey, SvRoomInfo>& SvMach::get_roomList() const
{
	return _rList;
}



/*********************************************************************
* SvPacketManager class implementation

*********************************************************************/
SvPacketManager & SvPacketManager::Instance()
{
	static SvPacketManager _instance;

	return _instance;
}
void SvPacketManager::sendPacket(std::shared_ptr<Packet_Base> spPk)
{
	SOCKET hClntSock;
	size_t pkSz;
	DWORD flags = 0;
	LPPER_IO_DATA ioInfo;

	hClntSock = spPk->sock;
	pkSz = spPk->get_packetSize();

	ioInfo = new PerIoData(pkSz);
	memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
	memcpy_s(ioInfo->get_buffer(), ioInfo->get_bufferLen(), spPk->get_bufAddr(), pkSz);
	ioInfo->rwMode = PerIoData::WRITE;

	WSASend(hClntSock, &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);
}
std::shared_ptr<Packet_Base> SvPacketManager::recvPacket()
{
	auto pm = SvPacketManager::Instance();

	while (pm._msgQueue.empty())	// queue에 packet이 없으면 대기
		;

	//rev lock..? pop 한줄에만 걸어도 될 것 같은데?
	auto shPk = pm._msgQueue.front();
	pm._msgQueue.pop();

	return shPk;
}



/* etc. functions definitions */
DWORD WINAPI recvThreadMain(LPVOID pComPort)
{
	HANDLE hComPort = (HANDLE)pComPort;
	SOCKET sock;
	DWORD bytesTrans;
	LPPER_IO_DATA ioInfo;
	DWORD flags = 0;

	while (1)
	{
		GetQueuedCompletionStatus(hComPort, &bytesTrans, (LPDWORD)&sock, (LPOVERLAPPED *)&ioInfo, INFINITE);
		cout << "GetQueuedComplitionStatus(mode: " << ioInfo->rwMode << ") bytesTrans = " << bytesTrans << endl;

		if (ioInfo->rwMode == PerIoData::READ_HEADER)
		{
			cout << "|READ_HEADER| sock: " << sock << endl;
			if (bytesTrans == 0)	// EOF 전송시
			{
				std::cout << "|READ_HEADER| closing socket(bytesTrans == 0) socket: " << sock << std::endl;
				closesocket(sock);
				delete(ioInfo);
				continue;
			}

			// 받은 바이트수 체크
			ioInfo->wsaBuf.len -= bytesTrans;
			ioInfo->wsaBuf.buf += bytesTrans;
			if (ioInfo->wsaBuf.len != 0)
			{
				cout << "|READ_HEADER| fragmented packet. WSARecv() called again." << endl;
				WSARecv(sock, &(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);
			}
			else {
				// ioInfo reset to wait for packet
				size_t pkSize = static_cast<int>(*(ioInfo->get_buffer()));
				cout << "|READ_HEADER| Packet size: " << pkSize << ". rwMode changed to READ_PACKET." << endl;
				memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
				ioInfo->allocBuffer(pkSize);
				ioInfo->rwMode = PerIoData::READ_PACKET;

				// wsarecv call
				WSARecv(sock, &(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);
			}
		}
		else if (ioInfo->rwMode == PerIoData::READ_PACKET)
		{
			cout << "|READ_PACKET| sock: " << sock << endl;

			if (bytesTrans == 0)
			{
				std::cout << "|READ_PACKET| closing socket(bytesTrans == 0)" << std::endl;
				closesocket(sock);
				delete(ioInfo);
				continue;
			}

			// 받은 바이트수 체크
			ioInfo->wsaBuf.len -= bytesTrans;
			ioInfo->wsaBuf.buf += bytesTrans;
			if (ioInfo->wsaBuf.len != 0)
			{	// incomplete receive
				cout << "|READ_PACKET| fragmented packet. WSARecv() called again." << endl;
				WSARecv(sock, &(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);
			}
			else {	// complete receive
				auto shPk = extractPacketFromBuffer(ioInfo->get_buffer);
				shPk->sock = sock;
				
				SvPacketManager::Instance()._msgQueue.push(std::move(shPk));	//rev lock 걸어야할까..?
			}
		}
		else
		{
			puts("|WRITE| message sent");
			delete(ioInfo);
		}
	}
	return 0;
}

//DWORD WINAPI packetProcessWorkerThreadMain(LPVOID pComPort)
//{
//	while (1) {
//		auto pm = SvPacketManager::Instance();
//
//		auto shRecvPk = pm.recvPacket();
//
//		shRecvPk->deserialize();
//		auto shSendPk = shRecvPk->processPacket(pm.getAgent());
//
//		if (shSendPk != nullptr)
//		{
//			// packet sending process
//			shSendPk->serialize();
//			pm.sendPacket(shSendPk);
//		}
//	}
//
//	return 0;
//}

DWORD WINAPI packetProcessWorkerThreadMain(LPVOID pComPort)
{
	while (1) {
		auto pm = SvPacketManager::Instance();

		auto shRecvPk = pm.recvPacket();

		std::cout << "Packet::_buf: " << shRecvPk->get_bufAddr() << std::endl;
	}

	return 0;
}

void ErrorHandling(char * message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}