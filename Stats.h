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

#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <fstream>

#include "lib/nlohmann/json/json.hpp"
using json = nlohmann::json;

typedef std::string UserName, UserID;
typedef std::vector<std::pair<UserID, unsigned long long>> UserCounts;

class Stats
{
private:
	std::unordered_map<UserID, UserName> users;
	std::unordered_map<UserID, unsigned long long> msgCount, replyCount,
		charCount, stickerCount, gifCount, imgCount, vidCount, audioCount,
		editCount;

	json chatHistory;
	std::ifstream chatHistoryFile;

	std::vector<std::pair<UserID, unsigned long long>> SortStats(
		std::unordered_map<UserID, unsigned long long> counts);
	int UTF8StringLength(const std::string& text);

public:
	Stats(std::string chatHistoryFileName);

	void ClearStats();

	void GenerateStats();

	UserCounts GetSortedMessageCounts();
	UserCounts GetSortedCharacterCounts();
	UserCounts GetSortedStickerCounts();
	UserCounts GetSortedGIFCounts();
	UserCounts GetSortedImageCounts();
	UserCounts GetSortedVideoCounts();
	UserCounts GetSortedAudioCounts();
	UserCounts GetSortedReplyCounts();
	UserCounts GetSortedEditCounts();

	const std::unordered_map<UserID, UserName>& GetUsers();

	~Stats();
};