#ifndef _DPROGRESS_HEADER_
#define _DPROGRESS_HEADER_


#include "..\include\DTimer.h"

namespace DUtility {

	///@brief Manages a console-based progress bar to keep the user entertained.
	///
	///Defining the global `NOPROGRESS` will
	///disable all progress operations, potentially speeding up a program. The look
	///of the progress bar is shown in ProgressBar.hpp.
	/// The progress bar looks like this:
	/// 
	/// [===================================](70 % -0.2s - 1 threads)
	class DU_DLL_API DProgress {
	private:
		uint32_t total_work;    ///< Total work to be accomplished
		uint32_t next_update;   ///< Next point to update the visible progress bar
		uint32_t call_diff;     ///< Interval between updates in work units
		uint32_t work_done;
		uint16_t old_percent;   ///< Old percentage value (aka: should we update the progress bar) TODO: Maybe that we do not need this
		DTimer    timer;         ///< Used for generating ETA

								///Clear current line on console so a new progress bar can be written
		void clearConsoleLine() const {
			std::cerr << "\r" <<std::endl<< std::flush;
		}

	public:
		///@brief Start/reset the progress bar.
		///@param total_work  The amount of work to be completed, usually specified in cells.
		void start(uint32_t total_work) {
			timer = DTimer();
			timer.start();
			this->total_work = total_work;
			next_update = 0;
			call_diff = total_work / 200;
			old_percent = 0;
			work_done = 0;
			clearConsoleLine();
		}

		///@brief Update the visible progress bar, but only if enough work has been done.
		///
		///Define the global `NOPROGRESS` flag to prevent this from having an
		///effect. Doing so may speed up the program's execution.
		void update(uint32_t work_done0) {
			work_done = work_done0;

			if (work_done < next_update)
				return;

			next_update += call_diff;

			uint16_t percent = (uint8_t)(work_done * 100 / total_work);
			if (percent>100)
				percent = 100;
			if (percent == old_percent)
				return;
			old_percent = percent;

			std::cerr << "\r["
				<< std::string(percent / 2, '=') << std::string(50 - percent / 2, ' ')
				<< "] ("
				<< percent << "% - "
				<< std::fixed << std::setprecision(1) << timer.lap() / percent * (100 - percent)
				<< "s "<< std::flush;
		}

		///Increment by one the work done and update the progress bar
		DProgress& operator++() {
			work_done++;
			update(work_done);
			return *this;
		}

		///Stop the progress bar. Throws an exception if it wasn't started.
		///@return The number of seconds the progress bar was running.
		double stop() {
			clearConsoleLine();
			timer.stop();
			return timer.accumulated();
		}

		///@return Return the time the progress bar ran for.
		double time_it_took() {
			return timer.accumulated();
		}

		uint32_t cellsProcessed() const {
			return work_done;
		}
	};

}

#endif
