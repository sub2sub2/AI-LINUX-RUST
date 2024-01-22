#include <stdio.h>
#include "api.h"

int main() {
    MCA mca = mca_handle_create();


    const char* new_sender = "Testtest";
    mca_set_sender(&mca, new_sender);
    mca_set_id(&mca, 123123);
    printf("After Setting Sender: %s, State %d, ID: %d\n", mca_get_sender(&mca), static_cast<int> (mca_check_state(&mca)), mca_get_id(&mca));

    return 0;
}