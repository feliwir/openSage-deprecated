#pragma once
#include <SFML/System.hpp>
#include <stdint.h>
#include <map>
#include <fstream>
#include <vector>
#include <memory>

class BigStream : public sf::InputStream
{
private:
	struct BigFile;

	struct BigHeader
	{
		uint8_t magic[4];
		uint8_t unused[4];
		uint32_t num_files;
		uint8_t unused2[4];
	};

	struct BigEntry
	{
		uint32_t offset;
		uint32_t size;
		std::string name;
		std::shared_ptr<BigFile> container;
	};

	struct BigFile
	{
		BigHeader header;
		std::vector<std::shared_ptr<BigEntry>> entries;
		std::ifstream fin;
	};

public:
    BigStream();
    ~BigStream();

    bool open(const std::string& filename);
	inline std::string readAll()
	{
		char* buf = new char[entry->size];
		read(buf, entry->size);
		return buf;
	}
	virtual sf::Int64 read(void* data, sf::Int64 size);
    virtual sf::Int64 seek(sf::Int64 position);
    virtual sf::Int64 tell();
    virtual sf::Int64 getSize();

	static void AddFile(const std::string& name);

private:
	std::shared_ptr<BigEntry> entry;
	uint32_t pos;
	static std::map<std::string, std::shared_ptr<BigEntry>> entries;
	static std::vector<std::shared_ptr<BigFile>> archives;
};

