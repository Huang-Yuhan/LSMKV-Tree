#include<fstream>
#include<vector>
#include<cstring>
using namespace std;

void read_from_file(std::string filePath)
{
	std::ifstream fin;
	fin.open(filePath,std::ios::in|std::ios::binary);
	
	// copy the file content to buffer
	
	auto beginpos=fin.tellg();
	fin.seekg(0,std::ios::end);
	auto endpos=fin.tellg();
	auto file_length=endpos-beginpos;
	fin.seekg(0,std::ios::beg);
	uint32_t totalMemSize=file_length;
	if(fin.fail())
	{
		throw "open file "+filePath+" in read_from_file error";
	}

	char *buffer=new char[file_length+1];
	buffer[file_length]='\0';
	fin.read(buffer,file_length);

	fin.close();

	// read data from buffer

	char * pos=buffer;
	char *str_buffer=new char[file_length+1];
	uint64_t time_stamp,key_num,key_min,key_max;
	memcpy(&time_stamp,pos,sizeof(uint64_t));
	pos+=sizeof(uint64_t);

	memcpy(&key_num,pos,sizeof(uint64_t));
	pos+=sizeof(uint64_t);

	memcpy(&key_min,pos,sizeof(uint64_t));
	pos+=sizeof(uint64_t);

	memcpy(&key_max,pos,sizeof(uint64_t));
	pos+=sizeof(uint64_t);

	pos=&buffer[10240+32];		//key start
	uint32_t offset_tmp;
	uint64_t key_tmp;
	std::vector<uint64_t> keys;
	std::vector<uint32_t> offsets;
	std::vector<std::string> values;
	for(uint64_t i=1;i<=key_num;i++)
	{
		memcpy(&key_tmp,pos,sizeof(uint64_t));
		pos+=sizeof(uint64_t);

		memcpy(&offset_tmp,pos,sizeof(uint32_t));
		pos+=sizeof(uint32_t);

		keys.push_back(key_tmp);
		offsets.push_back(offset_tmp);
	}

	// value start

	for(uint64_t i=0;i<key_num;i++)
	{
		uint32_t value_length=file_length;
		if(i+1<key_num)value_length=offsets[i+1];
		value_length-=offsets[i];
		memcpy(str_buffer,pos,value_length);
		pos+=value_length;
		values.push_back(std::string(str_buffer,value_length));
	}

	{
		std::ofstream os("255content.txt");
		os<<"key_num:"<<key_num<<"\ntime_stamp:"<<time_stamp<<"\nkey_min:"<<key_min<<"\nkey_max"<<key_max<<"\n";
		for(int i=0;i<key_num;i++)
		{
			os<<keys[i]<<" "<<"offset:"<<offsets[i]<<" "<<values[i]<<"\n";
		}
	}
}

int main()
{
    read_from_file("data/level-1/260_14487_4061_32767.sst");
    return 0;
}