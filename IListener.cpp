#include "EventHubInterfaces.hpp"
#include "EventHub.hpp"

IListener::IListener(std::vector<EventType> types) : types(types) {
    for (EventType type : types) {
        EventHub::getInstance().addListener(this, type);
    }
}

IListener::~IListener() {
    for (EventType type : types) {
        EventHub::getInstance().removeListener(this, type);
    }
}