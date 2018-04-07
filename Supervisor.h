//
// Created by mdl150330 on 4/6/2018.
//

#ifndef TEAM555_SUPERVISOR_H
#define TEAM555_SUPERVISOR_H


class Supervisor {

public:
    Supervisor();
    ~Supervisor() = default;
    void run(bool idle);
    void defense();
    void offense();
};


#endif //TEAM555_SUPERVISOR_H
