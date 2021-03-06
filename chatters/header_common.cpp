#include "header_common.h"



/* InfoToken class definition */
InfoToken::InfoToken() : _key(-1)
{
	//std::cout << "InfoToken() called" << std::endl;
}
InfoToken::InfoToken(Key key) : _key(key)
{
	// left intentionally blank
}
InfoToken::~InfoToken()
{
	//std::cout << "~InfoToken() called" << std::endl;
}



/* UserInfoToken class definition */
UserKey UserInfoToken::_getUniqueKey()
{
	return _UniqueKey++;
}
UserInfoToken::UserInfoToken() : _id("DEFAULT")
{
	_key = -1;
	//_id = "DEFAULT";
}
UserInfoToken::UserInfoToken(UserInfoToken&& tk)
{
	*this = std::move(tk);
}
UserInfoToken::~UserInfoToken()
{
	// left blank intentionally
}
UserInfoToken & UserInfoToken::operator=(UserInfoToken&& tk)
{
	if (this == &tk) 
		return *this;

	this->_key = tk._key;
	tk._key = -1;
	std::swap(this->_id, tk._id);

	return *this;
}
UserInfoToken & UserInfoToken::operator=(const UserInfoToken & utk)
{
	_key = utk._key;
	_id = utk._id;

	return *this;
}
UserKey UserInfoToken::get_key() const
{
	return _key;
}
std::string UserInfoToken::get_id() const
{
	return _id;
}
void UserInfoToken::operator<<(std::stringstream & sstream)
{
	sstream >> _key;
	sstream.ignore(std::numeric_limits<std::streamsize>::max(), '|');
	sstream >> _id;
	sstream.ignore(std::numeric_limits<std::streamsize>::max(), '|');
}
UserInfoToken::UserInfoToken(const std::string& id)
{
	_id = id;
	_key = _getUniqueKey();
}
UserInfoToken::UserInfoToken(const UserInfoToken & tk) 
	: InfoToken(tk._key),
	_id(tk._id)
{
	// left intentionally blank
}
std::ostream & operator<<(std::ostream & os, const UserInfoToken &utk)
{
	os << utk.get_key() << '|' 
		<< utk.get_id() << '|';
	return os;
}



/* RoomInfoToken class definition */
UserKey RoomInfoToken::_getUniqueKey()
{
	return _UniqueKey++;
}
RoomInfoToken::RoomInfoToken() : _numOfPeer(0), _title("")
{
	_key = _getUniqueKey();
}
RoomInfoToken::RoomInfoToken(const std::string& title)
{
	_title = title;
	_numOfPeer = 0;
	_key = _getUniqueKey();
}
RoomInfoToken::RoomInfoToken(const RoomInfoToken & tk)
{
	_title = tk._title;
	_numOfPeer = tk._numOfPeer;
	_key = tk._key;
}
RoomInfoToken::RoomInfoToken(RoomInfoToken && tk)
{
	*this = std::move(tk);
}
RoomInfoToken & RoomInfoToken::operator=(RoomInfoToken && tk)
{
	if (this == &tk)
		return *this;

	this->_key = tk._key;
	tk._key = -1;
	std::swap(this->_title, tk._title);
	_numOfPeer = tk._numOfPeer;

	return *this;
}
RoomInfoToken & RoomInfoToken::operator=(const RoomInfoToken & rtk)
{
	_key = rtk._key;
	_title = rtk._title;
	_numOfPeer = rtk._numOfPeer;

	return *this;
}
RoomInfoToken::~RoomInfoToken()
{
	// left blank intentionally
}
RoomKey RoomInfoToken::get_key() const
{
	return _key;
}
std::string RoomInfoToken::get_title() const
{
	return _title;
}
int RoomInfoToken::get_numOfPeer() const
{
	return _numOfPeer;
}
void RoomInfoToken::set_numOfPeer(int num)
{
	_numOfPeer = num;
}
void RoomInfoToken::operator<<(std::stringstream & sstream)
{
	std::string token;

	// extract room key
	std::getline(sstream, token, '|');
	_key = static_cast<Key>(stoi(token));
	
	// extract room title
	std::getline(sstream, token, '|');
	_title = std::move(token);

	// extract the number of participants in the room
	std::getline(sstream, token, '|');
	_numOfPeer = stoi(token);
}
std::ostream & operator<<(std::ostream & sstream, const RoomInfoToken & rtk)
{
	sstream << rtk.get_key() << '|'
		<< rtk.get_title() << '|'
		<< rtk.get_numOfPeer() << '|';

	return sstream;
}



/* MachObject class definition */
MachObject::~MachObject()
{
	// left blank intentionally
}
MachObject::MachObject()
{
	// left blank intentionally
}
void MachObject::_dcastEnableFunc()
{
	// left blank intentionally
}




/* Etc. functions */
std::string takePassword()
{
	char input;
	std::string pw;

	// take char in range [33, 126], except 124(=vertical bar '|')
	while ((input = _getch()) != '\r')
	{
		if (input == 124)		// ignore vertical bar(|)
			;
		else if (input == '\b')	// erase one character from console output line
		{
			if (pw.empty())
				;
			else {
				pw.pop_back();
				std::cout << '\b' << ' ' << '\b';
			}
		}
		else if ((input >= 33) && (input <= 126))
		{
			pw += input;
			std::cout << '*';
		}
		else	// ignore rest character
			;
	}
	std::cout << std::endl;

	return pw;
}