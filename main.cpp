#include <iostream>
#include "include/sgfplib.h"

using namespace std;

int fail(DWORD err, const char* message) {
    cerr << "ERROR" << " (" << err << "): " << message << endl;
    return err;
}

int main (int argc, char **argv)
{
    if (argc < 2) {
        return fail(1, "Inufficient paramenters");
    }

    return 0;
}