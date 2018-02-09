#ifndef TOMAHAWK_TOMAHAWKOUTPUTMANAGER_H_
#define TOMAHAWK_TOMAHAWKOUTPUTMANAGER_H_

#include "../../io/BasicWriters.h"
#include "../../io/compression/TGZFController.h"
#include "../../support/MagicConstants.h"
#include "../../totempole/TotempoleContig.h"
#include "../../totempole/TotempoleMagic.h"
#include "../../totempole/TotempoleOutputEntry.h"
#include "../../totempole/TotempoleReader.h"
#include "../../tomahawk/meta_entry.h"
#include "../two/output_entry.h"
#include "../two/output_entry_support.h"

#define SLAVE_FLUSH_LIMIT 2000000 // 2 MB

namespace Tomahawk{
namespace IO {

/**<
 * Internal use only
 * This writer class provides lock-based writing to a target disk destination
 * when multiple slaves share the same file-handle. Each of the computational
 * slaves gets a reference of this object.
 */
template <class T>
struct OutputSlaveWriter{
	typedef OutputSlaveWriter                         self_type;
	typedef IO::WriterFile                            writer_type;
	typedef IO::BasicBuffer                           buffer_type;
	typedef IO::OutputEntry                           entry_type;
	typedef TGZFController                            tgzf_controller;
	typedef Totempole::TotempoleReader                index_reader_type;
	typedef Totempole::TotempoleOutputEntry           totempole_entry;
	typedef Totempole::TotempoleOutputEntryController totempole_controller_byte;
	typedef Totempole::TotempoleEntry                 header_entry;
	typedef MetaEntry<T>                              meta_type;
	typedef Support::OutputEntrySupport               helper_type;
	typedef Algorithm::SpinLock                       spin_lock_type;

public:
	OutputSlaveWriter() :
		n_entries(0),
		progressCount(0),
		n_blocks(0),
		//writer(nullptr),
		//writer_index(nullptr),
		buffer(2*SLAVE_FLUSH_LIMIT)
	{

	}

	~OutputSlaveWriter(){
		this->flushBlock();
		this->buffer.deleteAll();
	}

	OutputSlaveWriter(const self_type& other) :
		n_entries(0),
		progressCount(0),
		n_blocks(0),
		//writer(other.writer),
		//writer_index(other.writer_index),
		buffer(2*SLAVE_FLUSH_LIMIT)
	{
	}

	inline self_type& operator+=(const self_type& other){
		this->n_entries += other.n_entries;
		this->n_blocks += other.n_blocks;
		return(*this);
	}

	inline self_type& operator=(const U32 totempole_blocks){
		this->n_blocks = totempole_blocks;
		return(*this);
	}

	inline const U64& GetCounts(void) const{ return this->n_entries; }
	inline void ResetProgress(void){ this->progressCount = 0; }
	inline const U32& GetProgressCounts(void) const{ return this->progressCount; }
	inline const U32& getTotempoleBlocks(void) const{ return(this->n_blocks); }

	bool Open(const std::string output, index_reader_type& totempole){
		if(output.size() == 0)
			return false;

		//this->writer       = new writer_type;
		//this->writer_index = new writer_type;

		this->CheckOutputNames(output);
		this->filename = output;
		//std::cerr << "here before using pointer: " << (this->basePath + this->baseName + '.' + Tomahawk::Constants::OUTPUT_LD_SUFFIX) << std::endl;
		//std::cerr << (void*)&this->writer << '\t' << (void*)&this->writer_index << std::endl;
		//if(!this->writer.open(this->basePath + this->baseName + '.' + Tomahawk::Constants::OUTPUT_LD_SUFFIX)){
		//	std::cerr << Helpers::timestamp("ERROR", "TWO") << "Failed to open..." << std::endl;
		//	return false;
		//}
		//std::cerr << "after open" << std::endl;
		// write setuff
		this->stream.open(this->basePath + this->baseName + '.' + Tomahawk::Constants::OUTPUT_LD_SUFFIX, std::ios::binary | std::ios::out);
		if(!this->stream.good()){
			std::cerr << Helpers::timestamp("ERROR", "TWO") << "Failed to open: " << this->basePath + this->baseName + '.' + Tomahawk::Constants::OUTPUT_LD_SUFFIX << "..." << std::endl;
			return false;
		}

		if(!this->WriteHeader(totempole)){
			std::cerr << Helpers::timestamp("ERROR", "TWO") << "Failed to write header" << std::endl;
			return false;
		}

		return true;
	}

	void flushBlock(void){
		if(this->buffer.size() > 0){
			if(!this->compressor.Deflate(this->buffer)){
				std::cerr << Helpers::timestamp("ERROR","TGZF") << "Failed deflate DATA..." << std::endl;
				exit(1);
			}

			this->spin_lock.lock();
			this->index_entry.byte_offset = (U64)this->stream.tellp();
			this->index_entry.uncompressed_size = this->buffer.size();
			this->stream.write(this->compressor.buffer.data(), this->compressor.buffer.size());
			this->index_entry.byte_offset_end = (U64)this->stream.tellp();
			this->stream << this->index_entry;
			++this->n_blocks;
			//std::cerr << this->entry << std::endl;
			this->spin_lock.unlock();

			this->buffer.reset();
			this->compressor.Clear();
			this->index_entry.reset();
		}
	}

	bool finalise(void){
		// Make sure data is flushed
		this->stream.flush();

		// Update blocks written
		/*
		std::fstream re(this->basePath + this->baseName + '.' + Tomahawk::Constants::OUTPUT_LD_SUFFIX + '.' + Tomahawk::Constants::OUTPUT_LD_SORT_INDEX_SUFFIX, std::ios::in | std::ios::out | std::ios::binary);
		if(!re.good()){
			std::cerr << Helpers::timestamp("ERROR", "TWO") << "Failed to reopen index..." << std::endl;
			return false;
		}

		re.seekg(Tomahawk::Constants::WRITE_HEADER_LD_SORT_MAGIC_LENGTH + sizeof(float) + sizeof(U64) + sizeof(U32));
		if(!re.good()){
			std::cerr << Helpers::timestamp("ERROR", "TWO") << "Failed to seek in index..." << std::endl;
			return false;
		}

		re.write((char*)&this->n_blocks, sizeof(U32));
		if(!re.good()){
			std::cerr << Helpers::timestamp("ERROR", "TWO") << "Failed to update counts in index..." << std::endl;
			return false;
		}
		re.flush();``
		re.close();
		*/

		return true;
	}

	void Add(const meta_type& meta_a, const meta_type& meta_b, const header_entry& header_a, const header_entry& header_b, const helper_type& helper){
		const U32 writePosA = meta_a.position << 2 | meta_a.phased << 1 | meta_a.missing;
		const U32 writePosB = meta_b.position << 2 | meta_b.phased << 1 | meta_b.missing;
		this->buffer += helper.controller;
		this->buffer += header_a.contigID;
		this->buffer += writePosA;
		this->buffer += header_b.contigID;
		this->buffer += writePosB;
		this->buffer << helper;
		++this->n_entries;
		++this->progressCount;
		++this->index_entry.n_entries;

		if(this->buffer.size() > SLAVE_FLUSH_LIMIT)
			this->flushBlock();
	}

	void close(void){
		this->stream.flush();
		this->stream.close();
	}

private:
	bool WriteHeader(index_reader_type& totempole){
		TomahawkOutputHeader<Tomahawk::Constants::WRITE_HEADER_LD_MAGIC_LENGTH> head(Tomahawk::Constants::WRITE_HEADER_LD_MAGIC, totempole.getSamples(), totempole.getContigs());
		TomahawkOutputSortHeader<Tomahawk::Constants::WRITE_HEADER_LD_SORT_MAGIC_LENGTH> headIndex(Tomahawk::Constants::WRITE_HEADER_LD_SORT_MAGIC, totempole.getSamples(), totempole.getContigs());

		this->stream << head;

		// Write contig data to TWO
		// length | n_char | chars[0 .. n_char - 1]
		for(U32 i = 0; i < totempole.getContigs(); ++i)
			this->stream << *totempole.getContigBase(i);

		/*
		if(!totempole.writeLiterals(stream)){
			std::cerr << Helpers::timestamp("ERROR", "TGZF") << "Failed to write literals..." << std::endl;
			return false;
		}
		*/

		return(stream.good());
	}

	void CheckOutputNames(const std::string& input){
		std::vector<std::string> paths = Helpers::filePathBaseExtension(input);
		this->basePath = paths[0];
		if(this->basePath.size() > 0)
			this->basePath += '/';

		if(paths[3].size() == Tomahawk::Constants::OUTPUT_LD_SUFFIX.size() && strncasecmp(&paths[3][0], &Tomahawk::Constants::OUTPUT_LD_SUFFIX[0], Tomahawk::Constants::OUTPUT_LD_SUFFIX.size()) == 0)
			this->baseName = paths[2];
		else this->baseName = paths[1];
	}


private:
	std::string     filename;
	std::string     basePath;
	std::string     baseName;

	U64             n_entries;      // number of entries written
	U32             progressCount;  // lines added since last flush
	U32             n_blocks;       // number of index blocks written
	totempole_entry index_entry;    // keep track of sort order
	std::ofstream   stream;         // output stream
	buffer_type     buffer;         // internal buffer
	tgzf_controller compressor;     // compressor
	spin_lock_type  spin_lock;      // spin-lock for parallel writing
};

}
}

#endif /* TOMAHAWK_TOMAHAWKOUTPUTMANAGER_H_ */
