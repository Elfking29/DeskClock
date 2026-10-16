#include "screens.hpp"

#include <vector>
#include <string>

class Rss : public Screen {
private:
    std::vector<std::string> authors;
    std::vector<std::string> articles;
    int unread=-1;
    int rand=-1;

    std::string pad(std::string m, uint l, char c=' ');
    std::string padtwo(int num);
    std::string center(const std::string& s, const char c=' ', uint n=20);
    std::string aposfix(std::string s);

public:
    Rss(DS1306& rtc, PCF8574& lcd);

    bool check() override;
    void setup() override;
    void disconnect() override;
    void update(std::string& key) override;
    void display() override;
};