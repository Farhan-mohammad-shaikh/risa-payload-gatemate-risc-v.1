#ifndef EXPERIMENTMANAGER_H_
#define EXPERIMENTMANAGER_H_

#include "experiment.h"
#include "logging.h"

class ExperimentManager
{
public:
	ExperimentManager();

	/**
	 * @brief Initializes the given Experiment and sets it to be
	 * run when runCurrentExperiment() gets executed
	 * 
	 * @param experiment the experiment thats to be run
	 */
	void startExperiment(Experiment *experiment);

	/**
	 * @brief runs the current experiment
	 * 
	 * @retval false if the experiment isnt done yet, true if the experiment is done
	 */
	bool runCurrentExperiment();

	Experiment *current_experiment;
	ExperimentState cur_state;

private:
	
};

#endif // EXPERIMENTMANAGER_H_