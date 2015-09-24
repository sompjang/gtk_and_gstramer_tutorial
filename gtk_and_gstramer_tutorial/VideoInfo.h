#include <string>
#pragma once
#include <glib.h>
#include <vector>

class VideoInfo
{

private:

	bool isLive;
	gint64 duration = 0;
	gint64 position = 0;

	// sirka a vyska videa;
	int img_width;
	int img_height;

	//info to connect
	std::string actual_path;
	std::string username;
	std::string password;

	std::vector <std::string> path_list;
	//info
	int video_length;
public:
	VideoInfo();
	VideoInfo(int img_width, int img_height, std::string path, std::string username, std::string password, int video_length);
	~VideoInfo();

	std::vector<std::string> GetPath_list() const;
	void SetPath_list(std::vector<std::string> val);

	int SetCredential(std::string user, std::string pass);

	void AddPathToList(std::string path);
	void RemovePathFromList();
	bool IsLive() const;
	void IsLive(bool val);
	gint64 GetDuration() const;
	void SetDuration(gint64 val);
	gint64 GetPosition() const;
	void SetPosition(gint64 val);
	int Img_width() const;
	void Img_width(int val);
	int Img_height() const;
	void Img_height(int val);
	std::string ActualPath() const;
	void ActualPath(std::string val);
	int Video_length() const;
	void Video_length(int val);
	std::string Password() const;
	void Password(std::string val);
	std::string Username() const;
	void Username(std::string val);
};

