#include <iostream>

int main()
{
    std::string startTime = "2016/05/18 13:10:00";

    time_t tStart;
    int yy=0, month=0, dd=0, hh=0, mm=0, ss=0;
    struct tm whenStart;
    const char *zStart = startTime.c_str();

    sscanf(zStart, "%d/%d/%d %d:%d:%d", &yy, &month, &dd, &hh, &mm, &ss);
    whenStart.tm_year = yy - 1900;
    whenStart.tm_mon = month - 1;
    whenStart.tm_mday = dd;
    whenStart.tm_hour = hh;
    whenStart.tm_min = mm;
    whenStart.tm_sec = ss;
    whenStart.tm_isdst = -1;

    tStart = mktime(&whenStart);

    std::cout << tStart << std::endl;
}
