#include <algorithm>
#include <iostream>
#include <string>
#include "EventHub.hpp"
#include "Event.hpp"
#include "EventHubInterfaces.hpp"

void EventHub::addListener(IListener* listener, EventType type) {
    listeners[type].push_back(listener);
}

void EventHub::removeListener(IListener* listener, EventType type) {
    auto& listenerList = listeners[type];
    listenerList.erase(std::remove(listenerList.begin(), listenerList.end(), listener), listenerList.end());
}

void EventHub::removeListener(IListener* listener) {
    for (auto& [type, listenerList] : listeners) {
        listenerList.erase(std::remove(listenerList.begin(), listenerList.end(), listener), listenerList.end());
    }
}

void EventHub::dispatch(const Event& event) const {
    if (auto it = listeners.find(event.getType()); it != listeners.end()) {
        for (auto listener : it->second) {
            listener->onEvent(event);
        }
    }
}

void EventHub::update() {
    std::vector<std::shared_ptr<Event>> expiredEvents;
    for (auto& event : events) {
        event->decrementExpirationTime();
        if (event->getExpirationTime() == 0) {
            expiredEvents.push_back(event);
        }
    }
    for (auto& event : expiredEvents) {
        events.erase(std::remove(events.begin(), events.end(), event), events.end());
    }
}

