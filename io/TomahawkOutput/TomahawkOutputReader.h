#ifndef TOMAHAWKOUTPUTREADER_H_
#define TOMAHAWKOUTPUTREADER_H_

#include <vector>
#include <fstream>
#include <string>
#include <stddef.h>
#include <regex>

#include "../../TypeDefinitions.h"
#include "../../tomahawk/MagicConstants.h"
#include "TomahawkOutputEntry.h"
#include "../../io/PackedEntryReader.h"
#include "TomahawkOutputFilterController.h"
#include "../../io/BasicBuffer.h"
#include "../../io/GZController.h"
#include "../../io/BasicWriters.h"
#include "../../io/totempole/TotempoleMagic.h"


namespace Tomahawk {
namespace IO {
// Todo: TomahawkOutputIndexReader

class TomahawkOutputReader {
	typedef TomahawkOutputEntry entry_type;
	typedef TomahawkOutputFilterController filter_type;
	typedef PackedEntryReader<entry_type, sizeof(entry_type)> reader_type;

public:
	TomahawkOutputReader();
	~TomahawkOutputReader(){ }

	// Streaming functions
	bool nextBlock(void);
	bool nextVariant(void);
	bool getBlock(const U32 blockID);
	bool getBlock(std::vector< std::pair<U32, U32> >& pairs);
	bool getBlocks(void);

	bool Open(const std::string input){
		this->stream.open(input, std::ios::binary | std::ios::in | std::ios::ate);
		if(!this->stream.good()){
			std::cerr << "failed to open file " << input << std::endl;
			return false;
		}

		this->filesize = this->stream.tellg();
		this->stream.seekg(0);

		if(!this->stream.good()){
			std::cerr << "bad stream" << std::endl;
			return false;
		}

		this->stream >> this->header;
		if(!this->header.validate(Tomahawk::Constants::WRITE_HEADER_LD_MAGIC)){
			std::cerr << "failed to validate header" << std::endl;
			return false;
		}

		if(!this->ParseHeader()){
			std::cerr << "failed to parse header" << std::endl;
			return false;
		}

		return true;
	}

	bool ParseHeader(void){
		Totempole::TotempoleContigBase base;
		for(U32 i = 0; i < this->header.n_contig; ++i){

		}

		return true;
	}

	// Other
	bool view(const std::string& filename);
	bool index(const std::string& filename);
	bool summary(const std::string& input);

	// Read entire file into memory
	filter_type& getFilter(void){ return this->filter; }

private:
	bool __viewOnly(void);
	bool __viewFilter(void);

public:
	U64 samples; 	// has to match header
	float version;	// has to match header
	U64 filesize;	// input file size
	std::ifstream stream; // reader stream
	TomahawkOutputHeader<Tomahawk::Constants::WRITE_HEADER_LD_MAGIC_LENGTH> header; // header

	IO::BasicBuffer buffer; // internal buffer
	IO::GZController gzip_controller; // TGZF controller
	filter_type filter;	// filter parameters
	IO::GenericWriterInterace* writer; // writer interface
	// Todo: PackedEntryIterator taking as input char* and length or IO::BasicBuffer

	//temp
	reader_type reader;
};

}
} /* namespace Tomahawk */

#endif /* TOMAHAWKOUTPUTREADER_H_ */
