#pragma once

#include <thread>

namespace Server{
	class Threader{
	public:
		Threader();
		~Threader();

		template<class Processor, typename Data>
		static void new_thread(Processor processor, Data data);
	};
}