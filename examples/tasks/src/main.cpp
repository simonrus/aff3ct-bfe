#include <vector>
#include <memory>
#include <iostream>

#include <aff3ct.hpp>
using namespace aff3ct;

int main(int argc, char** argv)
{
	// get the AFF3CT version
	const std::string v = "v" + std::to_string(version_major()) + "." +
	                            std::to_string(version_minor()) + "." +
	                            std::to_string(version_release());

	std::cout << "#----------------------------------------------------------"      << std::endl;
	std::cout << "# This is a basic program using the AFF3CT library (" << v << ")" << std::endl;
	std::cout << "# Feel free to improve it as you want to fit your needs."         << std::endl;
	std::cout << "#----------------------------------------------------------"      << std::endl;
	std::cout << "#"                                                                << std::endl;

	const int   fe       = 100;
	const int   seed     = argc >= 2 ? std::atoi(argv[1]) : 0;
	const int   K        = 32;
	const int   N        = 128;
	const float R        = (float)K / (float)N;
	const float ebn0_min = 0.00f;
	const float ebn0_max = 10.1f;

	std::cout << "# * Simulation parameters: "           << std::endl;
	std::cout << "#    ** Frame errors   = " << fe       << std::endl;
	std::cout << "#    ** Noise seed     = " << seed     << std::endl;
	std::cout << "#    ** Info. bits (K) = " << K        << std::endl;
	std::cout << "#    ** Frame size (N) = " << N        << std::endl;
	std::cout << "#    ** Code rate  (R) = " << R        << std::endl;
	std::cout << "#    ** SNR min   (dB) = " << ebn0_min << std::endl;
	std::cout << "#    ** SNR max   (dB) = " << ebn0_max << std::endl;
	std::cout << "#"                                     << std::endl;

	// create the AFF3CT modules
	module::Source_random<>          source (K      );
	module::Encoder_repetition_sys<> encoder(K, N   );
	module::Modem_BPSK<>             modem  (N      );
	module::Channel_AWGN_LLR<>       channel(N, seed);
	module::Decoder_repetition_std<> decoder(K, N   );
	module::Monitor_BFER<>           monitor(K, fe  );

	// create a sigma noise type
	tools::Sigma<> noise;

	// create reporters to display results in the terminal
	std::vector<tools::Reporter*> reporters =
	{
		new tools::Reporter_noise     <>(noise  ), // report the noise values (Es/N0 and Eb/N0)
		new tools::Reporter_BFER      <>(monitor), // report the bit/frame error rates
		new tools::Reporter_throughput<>(monitor)  // report the simulation throughputs
	};
	// convert the vector of reporter pointers into a vector of smart pointers
	std::vector<std::unique_ptr<tools::Reporter>> reporters_uptr;
	for (auto rep : reporters) reporters_uptr.push_back(std::unique_ptr<tools::Reporter>(rep));

	// create a terminal that will display the collected data from the reporters
	tools::Terminal_std terminal(reporters_uptr);

	// display the legend in the terminal
	terminal.legend();

	// configuration of the module tasks
	std::vector<const module::Module*> modules = {&source, &encoder, &modem, &channel, &decoder, &monitor};
	for (auto& m : modules)
		for (auto& t : m->tasks)
		{
			t->set_autoalloc  (true ); // enable the automatic allocation of the data in the tasks
			t->set_autoexec   (false); // disable the auto execution mode of the tasks
			t->set_debug      (false); // disable the debug mode
			t->set_debug_limit(16   ); // display only the 16 first bits if the debug mode is enabled
			t->set_stats      (true ); // enable the statistics

			// enable the fast mode (= disable the useless verifs in the tasks) if there is no debug and stats modes
			if (!t->is_debug() && !t->is_stats())
				t->set_fast(true);
		}

	// sockets binding (connect the sockets of the tasks = fill the input sockets with the output sockets)
	using namespace module;
	encoder[enc::sck::encode      ::U_K ].bind(source [src::sck::generate   ::U_K ]);
	modem  [mdm::sck::modulate    ::X_N1].bind(encoder[enc::sck::encode     ::X_N ]);
	channel[chn::sck::add_noise   ::X_N ].bind(modem  [mdm::sck::modulate   ::X_N2]);
	modem  [mdm::sck::demodulate  ::Y_N1].bind(channel[chn::sck::add_noise  ::Y_N ]);
	decoder[dec::sck::decode_siho ::Y_N ].bind(modem  [mdm::sck::demodulate ::Y_N2]);
	monitor[mnt::sck::check_errors::U   ].bind(encoder[enc::sck::encode     ::U_K ]);
	monitor[mnt::sck::check_errors::V   ].bind(decoder[dec::sck::decode_siho::V_K ]);

	// a loop over the various SNRs
	for (auto ebn0 = ebn0_min; ebn0 < ebn0_max; ebn0 += 1.f)
	{
		// compute the current sigma for the channel noise
		const auto esn0  = tools::ebn0_to_esn0 (ebn0, R);
		const auto sigma = tools::esn0_to_sigma(esn0   );

		noise.set_noise(sigma, ebn0, esn0);

		// update the sigma of the modem and the channel
		modem  .set_noise(noise);
		channel.set_noise(noise);

		// display the performance (BER and FER) in real time (in a separate thread)
		terminal.start_temp_report();

		// run the simulation chain
		while (!monitor.fe_limit_achieved() && !terminal.is_interrupt())
		{
			source [src::tsk::generate    ].exec();
			encoder[enc::tsk::encode      ].exec();
			modem  [mdm::tsk::modulate    ].exec();
			channel[chn::tsk::add_noise   ].exec();
			modem  [mdm::tsk::demodulate  ].exec();
			decoder[dec::tsk::decode_siho ].exec();
			monitor[mnt::tsk::check_errors].exec();
		}

		// display the performance (BER and FER) in the terminal
		terminal.final_report();

		// if user pressed Ctrl+c twice, exit the SNRs loop
		if (terminal.is_over()) break;

		// reset the monitor for the next SNR
		monitor.reset();
		terminal.reset();
	}
	std::cout << "#" << std::endl;

	// display the statistics of the tasks (if enabled)
	auto ordered = true;
	tools::Stats::show(modules, ordered);

	std::cout << "# End of the simulation" << std::endl;

	return 0;
}
