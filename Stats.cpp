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

#include "Stats.h"

#include <algorithm>

#define MBLEN_CHAR_LIMIT 8

std::vector<std::pair<UserID, unsigned long long>> Stats::SortStats(
	std::unordered_map<UserID, unsigned long long> counts)
{
	std::vector<std::pair<UserID, unsigned long long>> sortedStats(
		counts.begin(), counts.end());
	std::sort(sortedStats.begin(), sortedStats.end(), 
		[](std::pair<UserID, unsigned long long> a, 
			std::pair<UserID, unsigned long long> b)
		{
			return a.second > b.second;
		});

	return sortedStats;
}

int Stats::UTF8StringLength(const std::string& text)
{
	const char* str = text.c_str();

	int length = 0, curStrPos = 0;
	while (curStrPos < text.size())
	{
		int charLength = std::mblen(&str[curStrPos], MBLEN_CHAR_LIMIT);
		
		if (charLength == -1) // Invalid character
			break;

		curStrPos += charLength;
		length++;
	}

	return length;
}

Stats::Stats(std::string chatHistoryFileName)
{
	chatHistoryFile.open(chatHistoryFileName);
	chatHistory = json::parse(chatHistoryFile);
}

void Stats::ClearStats()
{
	users.clear();
	msgCount.clear();
	charCount.clear();
	stickerCount.clear();
	gifCount.clear();
}

void Stats::GenerateStats()
{
	ClearStats();

	for (const auto& message : chatHistory["messages"])
	{
		if (message["type"] != "message")
			continue;

		users[message["from_id"]] = message["from"];

		// Message counter
		msgCount[message["from_id"]]++;

		// Reply counter
		if (message.contains("reply_to_message_id"))
			replyCount[message["from_id"]]++;
		
		// Edit counter
		if (message.contains("edited"))
			editCount[message["from_id"]]++;

		// Character counter
		if (message["text"].is_string())
			charCount[message["from_id"]] += UTF8StringLength(message["text"]);
		else if (message["text"].is_array())
		{
			for (auto& textSlice : message["text"])
			{
				if (textSlice.is_string())
					charCount[message["from_id"]] += UTF8StringLength(textSlice);
				else if (textSlice.is_object())
					charCount[message["from_id"]] += UTF8StringLength(textSlice["text"]);
			}
		}
		
		if (message.contains("media_type"))
		{
			// GIF counter
			if (message["media_type"] == "animation")
				gifCount[message["from_id"]]++;
			// Sticker counter
			else if (message["media_type"] == "sticker")
				stickerCount[message["from_id"]]++;
			// Video counter
			else if (message["media_type"] == "video_file" ||
				message["media_type"] == "video_message")
				vidCount[message["from_id"]]++;
			// Audio counter
			else if (message["media_type"] == "audio_file" ||
				message["media_type"] == "voice_message")
				audioCount[message["from_id"]]++;
		}

		// Image counter
		if (message.contains("photo"))
			imgCount[message["from_id"]]++;
	}
}

UserCounts Stats::GetSortedMessageCounts() { return SortStats(msgCount); }
UserCounts Stats::GetSortedCharacterCounts() { return SortStats(charCount); }
UserCounts Stats::GetSortedStickerCounts() { return SortStats(stickerCount); }
UserCounts Stats::GetSortedGIFCounts() { return SortStats(gifCount); }
UserCounts Stats::GetSortedImageCounts() { return SortStats(imgCount); }
UserCounts Stats::GetSortedVideoCounts() { return SortStats(vidCount); }
UserCounts Stats::GetSortedReplyCounts() { return SortStats(replyCount); }
UserCounts Stats::GetSortedAudioCounts() { return SortStats(audioCount); }
UserCounts Stats::GetSortedEditCounts() { return SortStats(editCount); }

const std::unordered_map<UserID, UserName>& Stats::GetUsers()
{
	return users;
}

Stats::~Stats()
{
	chatHistoryFile.close();
}