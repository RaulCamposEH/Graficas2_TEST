#include "Saver.h"
#include <iostream>
#include <ios>
#include <fstream>

bool saveinFile(std::string File, ObjectPositions position)
{
	std::string buffer = std::to_string(position.x) + " " + std::to_string(position.y) + " " + std::to_string(position.z) + "\n";

	/*std::ofstream file(File.c_str(), std::ios::app);
	file.open(File.c_str(), std::ios::app);
	file.write(buffer.c_str(), buffer.size());*/

	errno_t err;
	FILE* tmpfile;
	err = fopen_s(&tmpfile, File.c_str(), "a");
	if (tmpfile == NULL) {
		printf("Impossible to open the file !\n");
		return false;
	}
	else {
		err = fputs(buffer.c_str(), tmpfile);
		if (err == EOF)
			perror("fputs()"); // POSIX requires that errno is set
	}
	fclose(tmpfile);
	return true;
}

bool openandRead(std::string File, std::vector<ObjectPositions>& positions)
{
	//std::ifstream file(File.c_str(), std::ios::in);
	errno_t err;
	FILE* tmpfile;
	err = fopen_s(&tmpfile, (const char*)File.c_str(), "r");
	if (tmpfile == NULL) {
		printf("Impossible to open the file !\n");
		return false;
	}
	while (1)
	{
		ObjectPositions obj;
		float x,y,z;
		int res = fscanf_s(tmpfile, "%f %f %f\n", &x, &y, &z);
		if (res == EOF)
			break;
		obj.x = x;
		obj.y = y;
		obj.z = z;
		positions.push_back(obj);

	}
	fclose(tmpfile);
	if (positions.empty())
	{
		return false;
	}
	return true;
}