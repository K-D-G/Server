#include "Threader.h"

using namespace Server;

Threader::Threader(){

}

Threader::~Threader(){

}

template<class Processor, typename Data>
void Threader::new_thread(Processor processor, Data data){
	std::thread temp_thread(processor.main, Data);

	temp_thread.join();
}
