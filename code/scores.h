#ifndef __SCORES__H_
#define __SCORES__H_

std::string URL_encode(const std::string text) {
	std::ostringstream ss;
	unsigned int i;
	for (i = 0; i < text.length(); ++i) {
		if (text[i] == ' ') {
			ss << "+";
			continue;
		} else if (isalnum(text[i]) || text[i] == '-' || text[i] == '_'
				|| text[i] == '.') {
			ss << text[i];
			continue;
		} else {
			char dat[256];
			snprintf(dat, 255, "%%%02X", static_cast<unsigned int>(text[i]));
			ss << dat;
		}
	}
	return ss.str();
}

struct Score {
	char username[25];
	unsigned int score;

	Score() {
		score = 0;
	}

	Score(int x) {
		score = x;
	}

	Score(const Score &s) {
		score = 0;
		this->operator=(s);
	}
	void operator=(const Score &s) {
		score = s.score;
		snprintf(username, 25, "%s", s.username);
	}
	Score(const char *userName, unsigned int score) {
		this->score = score;
		snprintf(username, 25, "%s", userName);
	}

	friend bool operator<(const Score &s1, const Score &s2);
	friend std::ostream &operator<<(std::ostream &out, const Score &s);
};


class TempSocketObject {
protected:
	mx::mxSocket sock_obj;
	bool connected;
public:
	TempSocketObject() {
		if(sock_obj.createSocket() == false) {
			connected = false;
		} else connected = true;
		std::cout << "Socket Created? : " << ((connected == true) ? "True" : "False");
	}
	bool initConnection(const std::string ip, const unsigned int port) {
		if(connected == false) return false;
		bool op;

		if (ip.length() > 0 && isdigit(ip[0])) {
			op = sock_obj.connectTo(ip, port);
		} else
			op = sock_obj.connectTo(mx::getHost(ip), port);

		// send data about post
		if (op == false) {
			std::cout << "Failed connection to: " << ip << " ["
					<< mx::getHost(ip) << "] " << ":" << port << std::endl;
			return false;
		} else {
			std::cout << "Successful connection to: " << ip << " ["
					<< mx::getHost(ip) << "]\n";
		}
		return true;
	}
	unsigned int Write(void *buf, const unsigned int s) {
		std::cout << "Writing...  " << s << "\n";
		return sock_obj.Write(buf, s);
	}
	unsigned int Read(void *buf, const unsigned int s) {
		return sock_obj.Read(buf, s);
	}
	void Close() {
		sock_obj.closeSocket();
		std::cout << "socket closed..\n";
	}
	void SendString(std::string text) {
		const char *src = text.c_str();
		unsigned int len = text.length();
		Write(const_cast<char*>(src), len);
	}
};


#endif
