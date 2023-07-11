#include <stdio.h>
#include "PresentazioneEsame.h"
//
int main(void) {

    printf("\r\n\n\nStart Exam\r\n\n\n");

    ESAME_SCRITTORE();

    ESAME_LETTORE();

    printf("\r\n\n\nEnd Exam\r\n\n\n");

    return 1;
}

