#include "MimeTypes.hpp"

MimeTypes::MimeTypes()
{
	_bin = "application/octet-stream";
	_type["html"] = "text/html";
	_type["htm"] = "text/html";
	_type["shtml"] = "text/html";
	_type["css"] = "text/css";
	_type["xml"] = "text/xml";
	_type["gif"] = "image/gif";
	_type["jpeg"] = "image/jpeg";
	_type["jpg"] = "image/jpeg";
	_type["js"] = "application/javascript";
	_type["atom"] = "application/atom+xml";
	_type["rss"] = "application/rss+xml";

	_type["mml"] = "text/mathml";
	_type["txt"] = "text/plain";
	_type["jad"] = "text/vnd.sun.j2me.app-descriptor";
	_type["wml"] = "text/vnd.wap.wml";
	_type["htc"] = "text/x-component";

	_type["png"] = "image/png";
	_type["svg"] = "image/svg+xml";
	_type["svgz"] = "image/svg+xml";
	_type["tif"] = "image/tiff";
	_type["tiff"] = "image/tiff";
	_type["wbmp"] = "image/vnd.wap.wbmp";
	_type["webp"] = "image/webp";
	_type["ico"] = "image/x-icon";
	_type["jng"] = "image/x-jng";
	_type["bmp"] = "image/x-ms-bmp";

	_type["woff"] = "font/woff";
	_type["woff2"] = "font/woff2";

	_type["jar"] = "application/java-archive";
	_type["war"] = "application/java-archive";
	_type["ear"] = "application/java-archive";
	_type["json"] = "application/json";
	_type["hqx"] = "application/mac-binhex40";
	_type["doc"] = "application/msword";
	_type["pdf"] = "application/pdf";
	_type["ps"] = "application/postscript";
	_type["eps"] = "application/postscript";
	_type["ai"] = "application/postscript";
	_type["rtf"] = "application/rtf";
	_type["m3u8"] = "application/vnd.apple.mpegurl";
	_type["kml"] = "application/vnd.google-earth.kml+xml";
	_type["kmz"] = "application/vnd.google-earth.kmz";
	_type["xls"] = "application/vnd.ms-excel";
	_type["eot"] = "application/vnd.ms-fontobject";
	_type["ppt"] = "application/vnd.ms-powerpoint";
	_type["odg"] = "application/vnd.oasis.opendocument.graphics";
	_type["odp"] = "application/vnd.oasis.opendocument.presentation";
	_type["ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	_type["odt"] = "application/vnd.oasis.opendocument.text";
	
	_type["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	_type["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	_type["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";

	_type["wmlc"] = "application/vnd.wap.wmlc";
	_type["7z"] = "application/x-7z-compressed";
	_type["cco"] = "application/x-cocoa";
	_type["jardiff"] = "application/x-java-archive-diff";
	_type["jnlp"] = "application/x-java-jnlp-file";
	_type["run"] = "application/x-makeself";
	_type["pl"] = "application/x-perl";
	_type["pm"] = "application/x-perl";
	_type["prc"] = "application/x-pilot";
	_type["pdb"] = "application/x-pilot";
	_type["rar"] = "application/x-rar-compressed";
	_type["rpm"] = "application/x-redhat-package-manager";
	_type["sea"] = "application/x-sea";
	_type["swf"] = "application/x-shockwave-flash";
	_type["sit"] = "application/x-stuffit";
	_type["tcl"] = "application/x-tcl";
	_type["tk"] = "application/x-tcl";
	_type["der"] = "application/x-x509-ca-cert";
	_type["pem"] = "application/x-x509-ca-cert";
	_type["crt"] = "application/x-x509-ca-cert";
	_type["xpi"] = "application/x-xpinstall";
	_type["xhtml"] = "application/xhtml+xml";
	_type["xspf"] = "application/xspf+xml";
	_type["zip"] = "application/zip";

	_type["bin"] = "application/octet-stream";
	_type["exe"] = "application/octet-stream";
	_type["dll"] = "application/octet-stream";
	_type["deb"] = "application/octet-stream";
	_type["dmg"] = "application/octet-stream";
	_type["iso"] = "application/octet-stream";
	_type["img"] = "application/octet-stream";
	_type["msi"] = "application/octet-stream";
	_type["msp"] = "application/octet-stream";
	_type["msm"] = "application/octet-stream";

	_type["mid"] = "audio/midi";
	_type["midi"] = "audio/midi";
	_type["kar"] = "audio/midi";
	_type["mp3"] = "audio/mpeg";
	_type["ogg"] = "audio/ogg";
	_type["m4a"] = "audio/x-m4a";
	_type["ra"] = "audio/x-realaudio";

	_type["3gpp"] = "video/3gpp";
	_type["3gp"] = "video/3gpp";
	_type["ts"] = "video/mp2t";
	_type["mp4"] = "video/mp4";
	_type["mpeg"] = "video/mpeg";
	_type["mpg"] = "video/mpeg";
	_type["mov"] = "video/quicktime";
	_type["webm"] = "video/webm";
	_type["flv"] = "video/x-flv";
	_type["m4v"] = "video/x-m4v";
	_type["mng"] = "video/x-mng";
	_type["asx"] = "video/x-ms-asf";
	_type["asf"] = "video/x-ms-asf";
	_type["wmv"] = "video/x-ms-wmv";
	_type["avi"] = "video/x-msvideo";
}

std::string &MimeTypes::operator[](std::string type)
{
	try
	{
		return _type.at(type);
	}
	catch (const std::exception &e)
	{
		return _bin;
	}
}
