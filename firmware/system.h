#ifndef _SYSTEM_H_
#define _SYSTEM_H_

enum mode_t
{
    MANUAL
};

class CSystem
{
public:

    void init();

    int manual_index;

    mode_t mode;
};

#endif