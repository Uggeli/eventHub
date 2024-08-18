#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "EventHubInterfaces.hpp"
#include "Event.hpp"

class Logger : public IListener {
public:
    Logger(EventType minLevelLog);
    virtual ~Logger() = default;
    void HandleEvent(const Event* event) override;

private:
    const std::string RESET = "\033[0m";
    const std::string WARNING = "\033[31m"; // red
    const std::string INFO = "\033[32m"; // green
    const std::string DEBUG = "\033[34m"; // blue
    
protected:
    EventType minLevelLog;
};

#endif // LOGGER_HPP