// Copyright 2015 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#pragma once

#include <map>
#include <vector>
#ifndef _WIN32
#include <sys/socket.h>
#include <sys/un.h>
#endif

#include "Common/CommonTypes.h"

// MemoryWatcher reads a file containing in-game memory addresses and outputs
// changes to those memory addresses to a unix domain socket as the game runs.
//
// The input file is a newline-separated list of hex memory addresses, without
// the "0x". To follow pointers, separate addresses with a space. For example,
// "ABCD EF" will watch the address at (*0xABCD) + 0xEF.
// The output to the socket is two lines. The first is the address from the
// input file, and the second is the new value in hex.
class MemoryWatcher final
{
public:
	MemoryWatcher();
	~MemoryWatcher();
	void Step();

	static void Init();
	static void Shutdown();

private:
	bool LoadAddresses(const std::string& path);
	bool OpenSocket(const std::string& path);

	void ParseLine(const std::string& line);
	u32 ChasePointer(const std::string& line);
	std::string ComposeMessage(const std::string& line, u32 value);

	bool m_running;

#ifdef _WIN32
	void* m_pipe;
	unsigned long m_written;
#else
	int m_fd;
	sockaddr_un m_addr;
#endif

	// Address as stored in the file -> list of offsets to follow
	std::map<std::string, std::vector<u32>> m_addresses;
	// Address as stored in the file -> current value
	std::map<std::string, u32> m_values;
};
