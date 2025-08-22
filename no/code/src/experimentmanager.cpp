#include "experimentmanager.h"

ExperimentManager::ExperimentManager()
{
}

void ExperimentManager::startExperiment(Experiment *experiment)
{
	if(!experiment)
	{
		LOGWARN("Started experiment doesnt exist %d", experiment);
		return;
	}

	current_experiment = experiment;
	current_experiment->init();
	cur_state = ExperimentState::TEST_INITIALIZED;

	LOGINFO("Experiment has been initialized");
}

bool ExperimentManager::runCurrentExperiment()
{
	if(!current_experiment)
	{
		//LOGWARN("no experiment was started");
		return false;
	}

	cur_state = current_experiment->run();

	switch(cur_state)
	{	
		case ExperimentState::TEST_INITIALIZED:

			return false;
		case ExperimentState::STILL_RUNNING:
			/* Do nothing */
			return false;
		case ExperimentState::TEST_FINISHED:
			current_experiment->cleanUp();
			current_experiment = nullptr;
			return true;
	}

	return false;
}
