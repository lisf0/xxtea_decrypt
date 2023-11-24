// main.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#define _CRT_SECURE_NO_WARNINGS 1

#include <iostream>
#include <string>
#include "xxtea/xxtea.h"
#include <cassert>


int32_t readDataFromFile(const std::string& file, uint8_t* &buffer)
{
    if (file.empty())
        return -1;

    FILE* fp = fopen(file.c_str(), "rb");
    if (!fp)
        return -2;

#if defined(_MSC_VER)
    auto descriptor = _fileno(fp);
#else
    auto descriptor = fileno(fp);
#endif
    struct stat statBuf;
    if (fstat(descriptor, &statBuf) == -1) {
        fclose(fp);
        return -3;
    }
    int32_t size = statBuf.st_size;

    if (size<=0)
    {
        return -4;
    }

    buffer = (uint8_t*)malloc(size);
    if (buffer == NULL) {
        // 内存分配失败的处理代码
        return -5;
    }
    memset(buffer, 0, size);

    size_t readsize = fread(buffer, 1, size, fp);
    fclose(fp);

    if (readsize < size) {
        return -6;
    }

    return size;
}


std::string decode(std::string path,std::string xxteaKey) {

	assert(!path.empty());

    uint8_t* bytes = nullptr;
	int32_t size = readDataFromFile(path,bytes);
    if (size<=0){
        return "";
    }

	uint32_t dataLen;
	uint8_t* data = xxtea_decrypt(bytes, size, (uint8_t*)xxteaKey.c_str(), (uint32_t)xxteaKey.size(), &dataLen);

    free(bytes);

	if (data == nullptr) {
		printf("Can't decrypt code for %s\n", path.c_str());
		return "";
	}

	std::string ret(reinterpret_cast<const char*>(data), dataLen);

	free(data);

	return ret;

}

int writeStringToFile(const std::string& file, const std::string& str){

    if (file.empty())
        return -1;

    FILE* fp = fopen(file.c_str(), "w");
    if (!fp)
        return -2;

    fprintf(fp, "%s", str.c_str());

    fclose(fp);

    printf("write to %s\n",file.c_str());
    return 0;
}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        printf("Usage: xxtea_decrypt.exe [input] [key] [out]\n");
        return -1;
    }
    auto ret = decode(argv[1], argv[2]);

    printf("read file %s\n\n%s\n\n", argv[1], ret.c_str());

    if (argc>3)
    {
        writeStringToFile(argv[3], ret);
    }

    return 0;
}
