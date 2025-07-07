#ifndef PLAYABLEENTRY_H
#define PLAYABLEENTRY_H

#include <string>

class PlayableEntry {
public:
    PlayableEntry();
    virtual ~PlayableEntry() = default;
    virtual void play() = 0;
    std::string getName() const { return name; }

private:
    bool exists = false;
    unsigned int weight = 0;
    std::string name;
    std::string path;
};

#endif // PLAYABLEENTRY_H
