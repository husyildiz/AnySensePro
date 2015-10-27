/*
 * TelemetryController.h
 *
 *  Created on: 15.10.2015
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_TELEMETRYCONTROLLER_H_
#define APPLICATION_INCLUDE_TELEMETRYCONTROLLER_H_

#include "Configuration.h"
#include "TelemetryAdapter.h"
#include "TelemetryMAVLink.h"

namespace App
{

class TelemetryController
{
private:
	Configuration& m_configuration;
	TelemetryMAVLink m_MAVLinkAdapter;
	//TelemetryAdapter* adapters[] = { NULL, NULL, &m_MAVLinkAdapter };
	TelemetryAdapter* m_active;
	uint8_t m_workspace[TELEMETRY_WORKSPACE];

	TelemetryAdapter* GetAdapter(TelemetryProtocol protocol);
public:
	TelemetryController(Configuration& configuration);
	void Run();
};

} /* namespace Application */

#endif /* APPLICATION_INCLUDE_TELEMETRYCONTROLLER_H_ */
