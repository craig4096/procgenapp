
#ifndef PROGRESS_H
#define PROGRESS_H

class Progress {
public:
    // sets the progress (in percent) of a certain operation
    virtual void setPercent(float) = 0;

    // when the generator moves onto the next operation
    virtual void nextOperation(int i) = 0;
};


#endif
