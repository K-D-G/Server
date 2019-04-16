#pragma once
#include <thread>

namespace Server{
	class Threader{
	public:
		Threader();
		~Threader();

		template<class T, typename Data>
		static void new_thread(T* processor, Data data){
			std::thread temp_thread(processor->main, data);
			temp_thread.join();
		}
	};
}