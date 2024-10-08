/*
 *	StatAddict for Telegram
 *	
 *	Copyright (C) 2024 Nazar Fedorenko
 *	
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU Lesser General Public License as published
 *	by the Free Software Foundation; either version 2.1 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU Lesser General Public License for more details.
 *
 *	You should have received a copy of the GNU Lesser General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>

#ifdef _WIN32
	#include <Windows.h>
#endif

#include "Stats.h"

// Global variables
bool showIDs = false;

void UsageInfo(const char* progName)
{
	std::cout << "StatAddict for Telegram\n"
		<< "Version 1.0\n"
		<< "(C) 2024 Nazar Fedorenko\n\n"
		<< "Generates statistics based on an exported Telegram group chat history in a JSON file.\n\n"
		<< "Usage:\n"
		<< "stataddict [chat_history.json] (optional switch)\n\n"
		<< "Optional switch:\n"
		<< "-id - show user IDs\n";
}

void DisplayList(std::string header, const UserCounts list, 
	const std::unordered_map<UserID, UserName>& users)
{
	std::cout << '\n' << header << "\n\n";
	for (size_t i = 0; i < list.size(); i++)
	{
		std::cout << i + 1 << ". " << users.at(list[i].first);

		if (showIDs)
			std::cout << " (" << list[i].first << ')';

		std::cout << ": " << list[i].second << '\n';
	}	
}

int ParseOptionalCmdLineArgs(int argc, char** argv)
{
	if (argc <= 2)
		return 0;

	// More switches will be handled here in the future
	// but currently there's only one
	if (!strcmp(argv[2], "-id"))
	{
		showIDs = true;
	}
	else
	{
		std::cerr << '\"' << argv[2] << "\" - unknown argument.\n";
		return -1;
	}

	return 0;
}

int main(int argc, char** argv)
{
	#ifdef _WIN32
		SetConsoleOutputCP(CP_UTF8);
	#endif

	std::setlocale(LC_ALL, "en_US.utf8");

	if (argc <= 1)
	{
		UsageInfo(argv[0]);
		return 0;
	}

	if (ParseOptionalCmdLineArgs(argc, argv) == -1)
		return -1;

	Stats* stats;

	try
	{
		stats = new Stats(argv[1]);
	}
	catch (std::exception& e)
	{
		std::cerr << "Failed to preprocess the JSON file.\n";
		return -1;
	}

	try
	{
		stats->GenerateStats();
	}
	catch (std::exception& e)
	{
		std::cerr << "Failed to generate statistics.\n";
		delete stats;
		return -1;
	}

	auto& users = stats->GetUsers();

	DisplayList("Messages", stats->GetSortedMessageCounts(), users);
	DisplayList("Characters", stats->GetSortedCharacterCounts(), users);
	DisplayList("Replies", stats->GetSortedReplyCounts(), users);
	DisplayList("Edited messages", stats->GetSortedEditCounts(), users);
	DisplayList("GIFs", stats->GetSortedGIFCounts(), users);
	DisplayList("Stickers", stats->GetSortedStickerCounts(), users);
	DisplayList("Images", stats->GetSortedImageCounts(), users);
	DisplayList("Videos", stats->GetSortedVideoCounts(), users);
	DisplayList("Audio", stats->GetSortedAudioCounts(), users);

	std::cout << "\nGenerated by FedNazar's StatAddict for Telegram\n";

	delete stats;
	return 0;
}