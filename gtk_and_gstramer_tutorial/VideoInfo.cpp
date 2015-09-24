#include "VideoInfo.h"


VideoInfo::VideoInfo()
{
	// sirka a vyska videa;
	this->img_width = 0;
	this->img_height = 0;

	//info to connect
	this->actual_path = "";
	this->username = "";
	this->password = "";

	//info
	this->video_length = 0;
}

VideoInfo::VideoInfo(int img_width, int img_height, std::string path, std::string username, std::string password, int video_length){
	// sirka a vyska videa;
	this->img_width = img_width;
	this->img_height = img_height;

	//info to connect
	this->actual_path = path;
	this->username = username;
	this->password = password;

	//info
	this->video_length = video_length;
}

VideoInfo::~VideoInfo()
{
}

std::vector<std::string> VideoInfo::GetPath_list() const
{
	return path_list;
}

void VideoInfo::SetPath_list(std::vector<std::string> val)
{
	path_list = val;
}

int VideoInfo::SetCredential(std::string user, std::string pass) {

	username = user;
	password = pass;

	return 0;
}

void VideoInfo::AddPathToList(std::string path)
{
	path_list.push_back(path);
}

void VideoInfo::RemovePathFromList()
{
	path_list.erase(path_list.begin());
}

bool VideoInfo::IsLive() const
{
	return isLive;
}

void VideoInfo::IsLive(bool val)
{
	isLive = val;
}

gint64 VideoInfo::GetDuration() const
{
	return duration;
}

void VideoInfo::SetDuration(gint64 val)
{
	duration = val;
}

gint64 VideoInfo::GetPosition() const
{
	return position;
}

void VideoInfo::SetPosition(gint64 val)
{
	position = val;
}

std::string VideoInfo::Username() const
{
	return username;
}

void VideoInfo::Username(std::string val)
{
	username = val;
}

std::string VideoInfo::Password() const
{
	return password;
}

void VideoInfo::Password(std::string val)
{
	password = val;
}

int VideoInfo::Video_length() const
{
	return video_length;
}

void VideoInfo::Video_length(int val)
{
	video_length = val;
}

int VideoInfo::Img_width() const
{
	return img_width;
}

void VideoInfo::Img_width(int val)
{
	img_width = val;
}

int VideoInfo::Img_height() const
{
	return img_height;
}

void VideoInfo::Img_height(int val)
{
	img_height = val;
}

std::string VideoInfo::ActualPath() const
{
	return actual_path;
}

void VideoInfo::ActualPath(std::string val)
{
	actual_path = val;
}

