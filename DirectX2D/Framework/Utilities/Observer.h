#pragma once

class Observer : public Singleton<Observer>
{
private:
    friend class Singleton;

    Observer() = default;
    ~Observer() = default;

public:
    void AddEvent(string key, Event event);
    void AddParamEvent(string key, ParamEvent paramEvent);
    void AddIntParamEvent(string key, IntParamEvent paramEvent);

    void ExecuteEvent(string key);
    void ExecuteParamEvent(string key, void* object);
    void ExecuteIntParamEvent(string key, int value);

private:
    unordered_map<string, vector<Event>> totalEvent;
    unordered_map<string, vector<ParamEvent>> totalParamEvent;
    unordered_map<string, vector<IntParamEvent>> totalIntParamEvent;
};