#include "event_engine.h"
#include "processor_udp.h"

int main()
{
    tiny::EventEngine engine(5);
    tiny::ProcessorUdp *svr = new tiny::ProcessorUdp();

    svr->Init(8866);
    engine.AddEvent(tiny::EventType_Read, svr);
    engine.RunLoop();
    return 0;
}
