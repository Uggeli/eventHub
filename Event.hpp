#ifndef EVENT_HPP
#define EVENT_HPP

#include <iostream>
#include <string>
#include <chrono>
#include <unordered_map>
#include <sstream>

enum class EventType {
    UNKNOWN,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING
};

struct Event {
    virtual ~Event() = default;
    virtual EventType getType() const { return type; }
    u_int8_t getExpirationTime() const { return expirationTime; }
    void setExpirationTime(u_int8_t expirationTime) { this->expirationTime = expirationTime; }
    void decrementExpirationTime() { if (expirationTime > 0) expirationTime--; }
protected:
    EventType type;
    u_int8_t expirationTime;
};

class EventTypeManager {
public:
    static EventTypeManager& getInstance() {
        static EventTypeManager instance;
        return instance;
    }

    virtual ~EventTypeManager() = default;
    EventType getEventType(const std::string& type) {
        if (auto it = eventTypes.find(type); it != eventTypes.end()) {
            return it->second;
        }
        return EventType::UNKNOWN;
    }

    EventType addEventType(const std::string& type) {
        if (auto it = eventTypes.find(type); it != eventTypes.end()) {
            return it->second;
        }
        EventType newType = static_cast<EventType>(nextType++);
        eventTypes[type] = newType;
        return newType;
    }

private:
    EventTypeManager() : nextType(static_cast<int>(EventType::LOG_WARNING) + 1) {}
    std::unordered_map<std::string, EventType> eventTypes;
    int nextType;
};

class LogEvent : public Event {
public:
    LogEvent(const std::string& message, EventType type) : message(message) {
        this->type = type;
    }
    std::string message;
};

// Forward declare IListener
class IListener;

#endif // EVENT_HPP