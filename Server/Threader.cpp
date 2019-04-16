#include "Threader.h"

using namespace Server;

Threader::Threader(){

}

Threader::~Threader(){

}

template<class T, typename Data>
void Threader::new_thread(T* processor, Data data){
	std::thread temp_thread(processor.main, data);
	temp_thread.join();
}
