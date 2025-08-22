/*
 * Copyright (c) 2014, Rhea Rinaldo
 * Copyright (c) 2014-2017, Fabian Greif
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

int globalVar = 0;

int globalFunction() {
    class Test  // local to fun
      {
        /* members of Test class */
      };
    int x = 1+1;
    return x;
}


class MyClass {
    float myFloat;
    static int myInt;
    public:
    int doSomething(char c) {
        int a = 5;
        int b = 10;
        while (b != 15)
        {
            b++;
        }
        return b;
    }
};

int MyClass::myInt = 10;
